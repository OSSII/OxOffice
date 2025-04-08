#!/usr/bin/env bash
# Deploy development environment
#
# Copyright 林博仁(Buo-ren Lin) <buo.ren.lin@gmail.com>
# SPDX-License-Identifier: MPL-2.0+

init(){
    printf \
        'Info: Loading common functions...\n'
    functions_file="${script_dir}/functions.sh"
    if ! test -e "${functions_file}"; then
        printf \
            'Error: Unable to locate the required functions.sh file.\n' \
            1>&2
        exit 1
    fi

    # shellcheck source=dev-assets/functions.sh
    if ! source "${functions_file}"; then
        printf \
            'Error: Unable to load the functions.sh file.\n' \
            1>&2
        exit 1
    fi

    if ! check_running_user; then
        printf \
            'Error: Running user check failed.\n' \
            1>&2
        exit 1
    fi

    if ! refresh_package_manager_local_cache; then
        printf \
            'Error: Unable to refresh the package manager local cache.\n' \
            1>&2
        exit 2
    fi

    build_dependency_pkgs=(
        # Official product build dependencies
        # https://wiki.documentfoundation.org/Development/BuildingOnLinux#Build_dependencies
        ant
        ant-optional
        autoconf
        bison
        build-essential
        ccache
        doxygen
        flex
        git
        gperf
        graphviz
        junit4
        libavahi-client-dev
        libcups2-dev
        libfontconfig1-dev
        libgstreamer1.0-dev
        libgstreamer-plugins-base1.0-dev
        libgtk-3-dev
        libkf5config-dev
        libkf5coreaddons-dev
        libkf5i18n-dev
        libkf5kio-dev
        libkf5windowsystem-dev
        libkrb5-dev
        libnss3-dev
        libqt5x11extras5-dev
        libx11-dev
        libxml2-utils
        libxrandr-dev
        libxslt1-dev
        libxt-dev
        nasm
        openjdk-17-jdk
        python3
        python3-dev
        python3-setuptools
        qtbase5-dev
        xsltproc
        zip

        # For elevating permission from a regular user when following the upstream instructions
        sudo

        # For detecting whether the program is run in a container environment
        virt-what
    )
    if ! check_distro_packages_installed "${build_dependency_pkgs[@]}"; then
        print_progress 'Installing build dependencies...'
        if ! install_distro_packages "${build_dependency_pkgs[@]}"; then
            printf \
                'Error: Unable to install build dependency packages.\n' \
                1>&2
            exit 2
        fi
    fi

    if ! virt_type="$(virt-what)"; then
        printf \
            'Error: Unable to query the virtualization type.\n' \
            1>&2
        exit 2
    fi

    # NOTE: Avoid managing build user outside of docker as it'll remove existing user account to allow the build user to have the same UID as the bind-mounted project files
    if test "${virt_type}" == docker \
        && ! getent passwd builduser >/dev/null; then
        local sudoers_file="${script_dir}/allow-builduser-to-run-sudo-commands-without-authentication.sudoers"
        if ! setup_build_user "${sudoers_file}"; then
            printf \
                'Error: Unable to setup the build user.\n' \
                1>&2
            exit 2
        fi
    fi

    # This is not an command substitution syntax
    # shellcheck disable=SC2016
    print_progress \
        'Development environment successfully deployed, please run `sudo -u builduser -s` before building the product.' \
        =
}

printf \
    'Info: Configuring the defensive interpreter behaviors...\n'
set_opts=(
    # Terminate script execution when an unhandled error occurs
    -o errexit
    -o errtrace

    # Terminate script execution when an unset parameter variable is
    # referenced
    -o nounset
)
if ! set "${set_opts[@]}"; then
    printf \
        'Error: Unable to configure the defensive interpreter behaviors.\n' \
        1>&2
    exit 1
fi

printf \
    'Info: Checking the existence of the required commands...\n'
required_commands=(
    realpath
)
flag_required_command_check_failed=false
for command in "${required_commands[@]}"; do
    if ! command -v "${command}" >/dev/null; then
        flag_required_command_check_failed=true
        printf \
            'Error: This program requires the "%s" command to be available in your command search PATHs.\n' \
            "${command}" \
            1>&2
    fi
done
if test "${flag_required_command_check_failed}" == true; then
    printf \
        'Error: Required command check failed, please check your installation.\n' \
        1>&2
    exit 1
fi

printf \
    'Info: Configuring the convenience variables...\n'
if test -v BASH_SOURCE; then
    # Convenience variables may not need to be referenced
    # shellcheck disable=SC2034
    {
        printf \
            'Info: Determining the absolute path of the program...\n'
        if ! script="$(
            realpath \
                --strip \
                "${BASH_SOURCE[0]}"
            )"; then
            printf \
                'Error: Unable to determine the absolute path of the program.\n' \
                1>&2
            exit 1
        fi
        script_dir="${script%/*}"
        script_filename="${script##*/}"
        script_name="${script_filename%%.*}"
    }
fi
# Convenience variables may not need to be referenced
# shellcheck disable=SC2034
{
    script_basecommand="${0}"
    script_args=("${@}")
}

printf \
    'Info: Setting the ERR trap...\n'
trap_err(){
    printf \
        'Error: The program has encountered an unhandled error and is prematurely aborted.\n' \
        1>&2
}
if ! trap trap_err ERR; then
    printf \
        'Error: Unable to set the ERR trap.\n' \
        1>&2
    exit 1
fi

setup_build_user(){
    local sudoers_file="${1}"; shift

    print_progress \
        'Setting up the regular user for running the build.'

    printf \
        'Info: Querying the ownership ID of the bind-mounted project files...\n'
    if ! project_files_uid="$(
        stat --format=%u "${sudoers_file}"
        )"; then
        printf \
            '%s: Error: Unable to query the ownership ID of the bind-mounted project files.\n' \
            "${FUNCNAME[0]}" \
            1>&2
        return 2
    fi

    if getent passwd "${project_files_uid}" >/dev/null; then
        printf \
            'Info: Removing the user that occupies the project files ownership UID(%s)...\n' \
            "${project_files_uid}"
        id_opts=(
            --user
            --name
        )
        if ! occupying_username="$(
            id "${id_opts[@]}" "${project_files_uid}"
            )"; then
            printf \
                'Error: Unable to query the occupying username of "%s" UID.\n' \
                "${project_files_uid}" \
                1>&2
            return 2
        fi

        if ! userdel --force "${occupying_username}"; then
            printf \
                "%s: Error: Unable to remove the occupying username(%s).\n" \
                "${FUNCNAME[0]}" \
                "${occupying_username}" \
                1>&2
            return 2
        fi
    fi

    printf \
        'Info: Creating the build user...\n'
    useradd_opts=(
        --comment 'User for running the build'
        --uid "${project_files_uid}"
        --user-group
        --create-home
    )
    if ! useradd "${useradd_opts[@]}" builduser; then
        printf \
            'Error: Unable to create the build user using the useradd command.\n' \
            1>&2
        return 2
    fi

    printf \
        'Info: Allowing the build user to run sudo commands without authentication...\n'
    if ! test -e "${sudoers_file}"; then
        printf \
            '%s: Error: Unable to locate the sudoers file.\n' \
            "${FUNCNAME[0]}" \
            1>&2
        return 2
    fi

    local installed_sudoers_file=/etc/sudoers.d/90-allow-builduser-to-run-sudo-commands-without-authentication
    if ! test -e "${installed_sudoers_file}"; then
        printf \
            'Info: Checking the validity of the drop-in sudoers file...\n'
        visudo_opts=(
            -c
            -f "${sudoers_file}"
        )
        if ! visudo "${visudo_opts[@]}"; then
            printf \
                '%s: Error: Drop-in sudoers file validity check has failed.\n' \
                "${FUNCNAME[0]}" \
                1>&2
            return 2
        fi

        printf \
            'Info: Installing the drop-in sudoers file...\n'
        install_opts=(
            --owner root
            --group root
            --mode 0644
            --verbose
        )
        if ! install "${install_opts[@]}" \
            "${sudoers_file}" \
            "${installed_sudoers_file}"; then
            printf \
                '%s: Error: Unable to install the drop-in sudoers file.\n' \
                "${FUNCNAME[0]}" \
                1>&2
            return 2
        fi
    fi

    return 0
}

init
