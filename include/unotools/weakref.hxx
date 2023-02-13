/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */

/*
 * This file is part of LibreOffice published API.
 */
#pragma once

#include <sal/config.h>

#include <com/sun/star/uno/Reference.hxx>
#include <com/sun/star/uno/XInterface.hpp>
#include <cppuhelper/weakref.hxx>
#include <cppuhelper/weak.hxx>
#include <rtl/ref.hxx>
#include <type_traits>

namespace unotools
{
/** The WeakReference<> holds a weak reference to an object.

    That object must implement the css::uno::XWeak interface.

    The WeakReference itself is *not* thread safe, just as
    Reference itself isn't, but the implementation of the listeners etc.
    behind it *is* thread-safe, so multiple threads can have their own
    WeakReferences to the same XWeak object.

    @tparam interface_type Must be a C++ implementation class type, not a UNO interface type.  (See
    the C++20 requires-clause on the get member.  That clause is not put on the class as a whole to
    avoid overly tight requirements on when interface_type needs to be complete.)
*/
template <class interface_type>
class SAL_WARN_UNUSED WeakReference : public com::sun::star::uno::WeakReferenceHelper
{
public:
    /** Default ctor.  Creates an empty weak reference.
    */
    WeakReference()
        : WeakReferenceHelper()
    {
    }

    /** Copy ctor.  Initialize this reference with a hard reference.

        @param rRef another hard ref
    */
    WeakReference(const rtl::Reference<interface_type>& rRef)
        : WeakReferenceHelper(css::uno::Reference<css::uno::XWeak>(rRef))
    {
    }

    /** Copy ctor.  Initialize this reference with a hard reference.

        @param rRef another hard ref
    */
    WeakReference(interface_type& rRef)
        : WeakReferenceHelper(&rRef)
    {
    }

    /** Copy ctor.  Initialize this reference with a hard reference.

        @param rRef another hard ref
    */
    WeakReference(interface_type* pRef)
        : WeakReferenceHelper(
              css::uno::Reference<css::uno::XWeak>(static_cast<cppu::OWeakObject*>(pRef)))
    {
    }

    /** Releases this reference and takes over hard reference xInt.
        If the implementation behind xInt does not support XWeak
        or XInt is null, then this reference is null.

        @param xInt another hard reference
    */
    WeakReference& operator=(const rtl::Reference<interface_type>& xInt)
    {
        WeakReferenceHelper::operator=(xInt);
        return *this;
    }

    WeakReference& operator=(rtl::Reference<interface_type>&& xInt)
    {
        WeakReferenceHelper::operator=(std::move(xInt));
        return *this;
    }

    WeakReference& operator=(interface_type* pInt)
    {
        WeakReferenceHelper::operator=(
            css::uno::Reference<css::uno::XWeak>(static_cast<::cppu::OWeakObject*>(pInt)));
        return *this;
    }

    /**  Gets a hard reference to the object.

         @return hard reference or null, if the weakly referenced interface has gone
    */
    rtl::Reference<interface_type> SAL_CALL get() const
#if __cplusplus >= 202002L
        requires(!cppu::detail::isUnoInterfaceType<interface_type>)
#endif
    {
        css::uno::Reference<css::uno::XInterface> xInterface = WeakReferenceHelper::get();
        // If XInterface is an ambiguous base of interface_type, we have to use dynamic_cast,
        // otherwise we can use the faster static_cast.
        if constexpr (std::is_convertible_v<interface_type*, css::uno::XInterface*>)
            return static_cast<interface_type*>(xInterface.get());
        else
            return dynamic_cast<interface_type*>(xInterface.get());
    }

    /**  Gets a hard reference to the object.

         @return hard reference or null, if the weakly referenced interface has gone
    */
    operator ::rtl::Reference<interface_type>() const { return get(); }
};
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
