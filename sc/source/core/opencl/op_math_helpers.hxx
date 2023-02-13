/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

const char Math_IntgDecl[] ="double Intg(double n);\n";
const char Math_Intg[] =
"double Intg(double n)\n"
"{\n"
"    if(trunc(n)==n )\n"
"        return n;\n"
"    else if(n<0)\n"
"        return trunc(n)-1;\n"
"    else\n"
"        return trunc(n)+1;\n"
"}\n";

const char nCorrValDecl[] ="double constant nCorrVal[]"
"= {0, 9e-1, 9e-2, 9e-3, 9e-4, 9e-5, 9e-6, 9e-7, "
"9e-8,9e-9, 9e-10, 9e-11, 9e-12, 9e-13, 9e-14, 9e-15};\n";

const char RoundDecl[] = "double  Round(double fValue);\n";

const char Round[] =
"double  Round(double fValue)\n"
"{\n"
"    if ( fValue == 0.0  )\n"
"        return fValue;\n"
"\n"
"    double fFac = 0;\n"
"    int nExp;\n"
"    if ( fValue > 0.0 )\n"
"        nExp = ( floor( log10( fValue ) ) );\n"
"    else\n"
"        nExp = 0;\n"
"    int nIndex = 15 - nExp;\n"
"    if ( nIndex > 15 )\n"
"        nIndex = 15;\n"
"    else if ( nIndex <= 1 )\n"
"        nIndex = 0;\n"
"    fValue = floor( fValue + 0.5 + nCorrVal[nIndex] );\n"
"    return fValue;\n"
"}\n";

const char bikDecl[] = "double bik(double n,double k);\n";
const char bik[] =
"double bik(double n,double k)\n"
"{\n"
"    double nVal1 = n;\n"
"    double nVal2 = k;\n"
"    n = n - 1;\n"
"    k = k - 1;\n"
"    while (k > 0)\n"
"    {\n"
"        nVal1 = nVal1 * n;\n"
"        nVal2 = nVal2 * k;\n"
"        k = k - 1;\n"
"        n = n - 1;\n"
"    }\n"
"    return (nVal1 / nVal2);\n"
"}\n";

const char local_cothDecl[] = "double local_coth(double n);\n";
const char local_coth[] =
"double local_coth(double n)\n"
"{\n"
"    double a = exp(n);\n"
"    double b = exp(-n);\n"
"    double nVal = (a + b) / (a - b);\n"
"    return nVal;\n"
"}\n";

const char local_coshDecl[] = "double local_cosh(double n);\n";
const char local_cosh[] =
"double local_cosh(double n)\n"
"{\n"
"    double nVal = (exp(n) + exp(-n)) / 2;\n"
"    return nVal;\n"
"}\n";
const char atan2Decl[] = "double arctan2(double y, double x);\n";
const char atan2Content[] =
"double arctan2(double y, double x)\n"
"{\n"
"    if(y==0.0)\n"
"        return x >= 0 ? 0.0 : M_PI;\n"
"    double a,num,den,tmpPi;\n"
"    int flag;\n"
"    tmpPi = 0;\n"
"    if (fabs(x) >= fabs(y))\n"
"    {\n"
"        num = y;\n"
"        den = x;\n"
"        flag = 1;\n"
"        if (x < 0.0)\n"
"            tmpPi = M_PI;\n"
"    }\n"
"    if(fabs(x) < fabs(y))\n"
"    {\n"
"        num = x;\n"
"        den = y;\n"
"        flag = -1;\n"
"        tmpPi = M_PI_2;\n"
"    }\n"
"    a = atan(num/den);\n"
"    a = flag==1?a:-a;\n"
"    a = a + (y >= 0.0 ? tmpPi : -tmpPi);\n"
"    return a;\n"
"}\n";

const char is_representable_integerDecl[] =  "int is_representable_integer(double a);\n";
const char is_representable_integer[] =
"int is_representable_integer(double a) {\n"
"    long kMaxInt = (1L << 53) - 1;\n"
"    if (a <= (double)kMaxInt)\n"
"    {\n"
"        long nInt = (long)a;\n"
"        double fInt;\n"
"        return (nInt <= kMaxInt &&\n"
"                (!((fInt = (double)nInt) < a) && !(fInt > a)));\n"
"    }\n"
"    return 0;\n"
"}\n";

const char approx_equalDecl[] = "int approx_equal(double a, double b);\n";
const char approx_equal[] =
"int approx_equal(double a, double b) {\n"
"    double e48 = 1.0 / (16777216.0 * 16777216.0);\n"
"    double e44 = e48 * 16.0;\n"
"    if (a == b)\n"
"        return 1;\n"
"    if (a == 0.0 || b == 0.0)\n"
"        return 0;\n"
"    double d = fabs(a - b);\n"
"    if (!isfinite(d))\n"
"        return 0;   // Nan or Inf involved\n"
"    if (d > ((a = fabs(a)) * e44) || d > ((b = fabs(b)) * e44))\n"
"        return 0;\n"
"    if (is_representable_integer(d) && is_representable_integer(a) && is_representable_integer(b))\n"
"        return 0;   // special case for representable integers.\n"
"    return (d < a * e48 && d < b * e48);\n"
"}\n";

const char fsum_approxDecl[] = "double fsum_approx(double a, double b);\n";
const char fsum_approx[] =
"double fsum_approx(double a, double b) {\n"
"    if ( ((a < 0.0 && b > 0.0) || (b < 0.0 && a > 0.0))\n"
"         && approx_equal( a, -b ) )\n"
"        return 0.0;\n"
"    return a + b;\n"
"}\n";

const char fsub_approxDecl[] = "double fsub_approx(double a, double b);\n";
const char fsub_approx[] =
"double fsub_approx(double a, double b) {\n"
"    if ( ((a < 0.0 && b < 0.0) || (a > 0.0 && b > 0.0)) && approx_equal( a, b ) )\n"
"        return 0.0;\n"
"    return a - b;\n"
"}\n";

const char value_approxDecl[] = "double value_approx( double fValue );\n";
const char value_approx[] =
"double value_approx( double fValue )\n"
"{\n"
"    const double fBigInt = 2199023255552.0;\n"
"    if (fValue == 0.0 || fValue == HUGE_VAL || !isfinite(fValue))\n"
"        return fValue;\n"
"    double fOrigValue = fValue;\n"
"    fValue = fabs(fValue);\n"
"    if (fValue > fBigInt)\n"
"        return fOrigValue;\n"
"    if (is_representable_integer(fValue))\n" // TODO? || getBitsInFracPart(fValue) <= 11)\n"
"        return fOrigValue;\n"
"    int nExp = (int)(floor(log10(fValue)));\n"
"    nExp = 14 - nExp;\n"
"    double fExpValue = pow(10.0,nExp);\n"
"    fValue *= fExpValue;\n"
"    if (!isfinite(fValue))\n"
"        return fOrigValue;\n"
"    fValue = round(fValue);\n"
"    fValue /= fExpValue;\n"
"    if (!isfinite(fValue))\n"
"        return fOrigValue;\n"
"    return copysign(fValue, fOrigValue);\n"
"}\n";

// This compares two cell contents, including possible string equalities (based on string ids
// coming from DynamicKernelArgument::GetStringId()).
// The EmptyIsZero conversion must not be have been done for the arguments.
const char cell_equalDecl[] = "bool cell_equal(double a, double b, bool a_is_string, bool b_is_string);\n";
const char cell_equal[] =
"bool cell_equal(double a, double b, bool a_is_string, bool b_is_string) {\n"
"    if( !a_is_string && !b_is_string )\n"
"        return approx_equal( isnan(a) ? 0 : a, isnan(b) ? 0 : b );\n"
"    if( a_is_string && b_is_string )\n"
"        return a == b;\n"
// empty strings and empty cells compare equal
"    if(( a_is_string && a == 0 && isnan(b)) || ( b_is_string && b == 0 && isnan(a)))\n"
"        return true;\n"
"    return false;\n"
"}\n";

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
