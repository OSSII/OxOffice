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

#include <iostream>

#include <svdata.hxx>

#include <i18nlangtag/mslangid.hxx>
#include <i18nlangtag/languagetag.hxx>

#include <font/FontSelectPattern.hxx>
#include <font/PhysicalFontCollection.hxx>
#include <font/PhysicalFontFaceCollection.hxx>
#include <font/PhysicalFontFace.hxx>
#include <font/fontsubstitution.hxx>
#include <font/LoFontSubstitution.hxx>

namespace vcl::font
{

struct CJKFontFeature
{
    OUString maFeatureName; // 以分號隔開的特徵名稱，大小寫視為不同，例如：u"明;宋;바탕;Ming;Mincho;Sung;Song;Batang"
    LanguageFontMap maEqualFonts = {};
    OUString maDefaultFallbackFont = "";

    LanguageFontMap maUnionFonts = {}; // 指定符合特徵的字型如果缺字，就從這些字型中找
    OUString maDefaultUnionFont = "";  // 如果沒有指定符合特徵的字型，就從預設字型中找

    OUString maPrivateUseAreaFont = ""; // 關聯到的私用區字型
};

/*
   It is recommended that CJK users install Source Han fonts,
   because Source Han fonts provides CJK font names.
   Compared with Noto CJK font which only has English name,
   it is more intuitive to use.
   Source Han Serif and Source Han Sans have been built into
   MODA ODF Application Tools and OxOffice.
   Therefore, it can directly replace Noto CJK fonts,
   as well as Ming, Song, Hei and other fonts,
   greatly reducing the problem of weird font styles.

   The only alternative font for the Kai type is TW Kai,
   so the Kai type of each language basically lists the default Kai type of each OS.
   If it cannot be found, TW Kai will be used.
*/

// TODO: implement dynamic lists
std::vector<CJKFontFeature> aCJKFeaturesMap =
{
    // 宋、明體類字型名稱特徵(大小寫視為不同)
    {
        u"*明;*宋;*바탕;*Ming;*Mincho;*Sung;*Song;*Batang",
        {
            { LANGUAGE_CHINESE_TRADITIONAL, u"新細明體;PMingLiU;蘋果儷細宋;Apple LiSung;思源宋體;Source Han Serif TC;Noto Serif CJK TC" },
            { LANGUAGE_CHINESE_HONGKONG,    u"細明體_HKSCS;MingLiU_HKSCS;蘋果儷細宋;Apple LiSung;思源宋體 香港;Source Han Serif HC;Noto Serif CJK HK" },
            { LANGUAGE_CHINESE_MACAU,       u"細明體_HKSCS;MingLiU_HKSCS;蘋果儷細宋;Apple LiSung;思源宋體 香港;Source Han Serif HC;Noto Serif CJK HK" },
            { LANGUAGE_CHINESE_SIMPLIFIED,  u"宋体;SimSun;宋体-简;Song;思源宋体;Source Han Serif SC;Noto Serif CJK SC" },
            { LANGUAGE_CHINESE_SINGAPORE,   u"宋体;SimSun;宋体-简;Song;思源宋体;Source Han Serif SC;Noto Serif CJK SC" },
            { LANGUAGE_JAPANESE,            u"ＭＳ Ｐ明朝;MS PMincho;游明朝体;YuMincho;源ノ明朝;Source Han Serif;Noto Serif CJK JP" },
            { LANGUAGE_KOREAN,              u"바탕;Batang;일반체;AppleMyungjo;본명조;Source Han Serif K;Noto Serif CJK KR" },
        },
        u"全字庫正宋體;TW-Sung",
        {
            { LANGUAGE_CHINESE_TRADITIONAL, u"新細明體-ExtB;PMingLiU-ExtB" },
            { LANGUAGE_CHINESE_HONGKONG,    u"細明體_HKSCS-ExtB;MingLiU_HKSCS-ExtB" },
            { LANGUAGE_CHINESE_MACAU,       u"細明體_HKSCS-ExtB;MingLiU_HKSCS-ExtB" },
            { LANGUAGE_CHINESE_SIMPLIFIED,  u"SimSun-ExtB" },
            { LANGUAGE_CHINESE_SINGAPORE,   u"SimSun-ExtB" },
            { LANGUAGE_JAPANESE,            u"" },
            { LANGUAGE_KOREAN,              u"" },
        },
        u"全字庫正宋體 Ext-B;TW-Sung-Ext-B;全字庫正宋體 Plus;TW-Sung-Plus"
    },
    // 黑體類字型名稱特徵(大小寫視為不同)
    {
        u"*黑;*Hei;*Gothic;*蘋方;*苹方;*PingFang",
        {
            { LANGUAGE_CHINESE_TRADITIONAL, u"微軟正黑體;Microsoft JhengHei;蘋方-繁;PingFang TC;思源黑體;Source Han Sans TC;Noto Sans CJK TC" },
            { LANGUAGE_CHINESE_HONGKONG,    u"微軟正黑體;Microsoft JhengHei;蘋方-港;PingFang HK;思源黑體 香港;Source Han Sans HC;Noto Sans CJK HK" },
            { LANGUAGE_CHINESE_MACAU,       u"微軟正黑體;Microsoft JhengHei;蘋方-港;PingFang HK;思源黑體 香港;Source Han Sans HC;Noto Sans CJK HK" },
            { LANGUAGE_CHINESE_SIMPLIFIED,  u"微软雅黑;Microsoft YaHei;苹方-简;PingFang SC;思源黑体;Source Han Sans SC;Noto Sans CJK SC" },
            { LANGUAGE_CHINESE_SINGAPORE,   u"微软雅黑;Microsoft YaHei;苹方-简;PingFang SC;思源黑体;Source Han Sans SC;Noto Sans CJK SC" },
            { LANGUAGE_JAPANESE,            u"源ノ角ゴシック;Source Han Sans;Noto Sans CJK JP" },
            { LANGUAGE_KOREAN,              u"본고딕;Source Han Sans K;Noto Sans CJK KR" },
        },
        u"微軟正黑體;Microsoft JhengHei;蘋方-繁;PingFang TC;思源黑體;Source Han Sans TC;Noto Sans CJK TC"
    },
    // 楷體類字型名稱特徵(大小寫視為不同)
    {
        u"*楷;*Kai",
        {
            { LANGUAGE_CHINESE_TRADITIONAL, u"標楷體;DFKai-SB;標楷體-繁;BiauKai" },
            { LANGUAGE_CHINESE_HONGKONG,    u"標楷體;DFKai-SB;標楷體-繁;BiauKai" },
            { LANGUAGE_CHINESE_MACAU,       u"標楷體;DFKai-SB;標楷體-繁;BiauKai" },
            { LANGUAGE_CHINESE_SIMPLIFIED,  u"楷体;SimKai;楷体-简;Kai" },
            { LANGUAGE_CHINESE_SINGAPORE,   u"楷体;SimKai;楷体-简;Kai" },
        },
        u"全字庫正楷體;TW-Kai",
        {
            // empty map. fallback to Default Union Font(全字庫正楷體 Ext-B;TW-Kai-Ext-B)
        },
        u"全字庫正楷體 Ext-B;TW-Kai-Ext-B;全字庫正楷體 Plus;TW-Kai-Plus"
    },
    // 無襯線字型相當於黑體字型
    {
        u"Sans;sans;Sans-Serif",
        {
            { LANGUAGE_CHINESE_TRADITIONAL, u"微軟正黑體;Microsoft JhengHei;蘋方-繁;PingFang TC;思源黑體;Source Han Sans TC;Noto Sans CJK TC" },
            { LANGUAGE_CHINESE_HONGKONG,    u"微軟正黑體;Microsoft JhengHei;蘋方-港;PingFang HK;思源黑體 香港;Source Han Sans HC;Noto Sans CJK HK" },
            { LANGUAGE_CHINESE_MACAU,       u"微軟正黑體;Microsoft JhengHei;蘋方-港;PingFang HK;思源黑體 香港;Source Han Sans HC;Noto Sans CJK HK" },
            { LANGUAGE_CHINESE_SIMPLIFIED,  u"微软雅黑;Microsoft YaHei;苹方-简;PingFang SC;思源黑体;Source Han Sans SC;Noto Sans CJK SC" },
            { LANGUAGE_CHINESE_SINGAPORE,   u"微软雅黑;Microsoft YaHei;苹方-简;PingFang SC;思源黑体;Source Han Sans SC;Noto Sans CJK SC" },
            { LANGUAGE_JAPANESE,            u"源ノ角ゴシック;Source Han Sans;Noto Sans CJK JP" },
            { LANGUAGE_KOREAN,              u"본고딕;Source Han Sans K;Noto Sans CJK KR" },
        }
    },
    // 有襯線字型相當於明、宋體字型
    {
        u"Serif;serif",
        {
            { LANGUAGE_CHINESE_TRADITIONAL, u"新細明體;PMingLiU;蘋果儷細宋;Apple LiSung;思源宋體;Source Han Serif TC;Noto Serif CJK TC" },
            { LANGUAGE_CHINESE_HONGKONG,    u"細明體_HKSCS;MingLiU_HKSCS;蘋果儷細宋;Apple LiSung;思源宋體 香港;Source Han Serif HC;Noto Serif CJK HK" },
            { LANGUAGE_CHINESE_MACAU,       u"細明體_HKSCS;MingLiU_HKSCS;蘋果儷細宋;Apple LiSung;思源宋體 香港;Source Han Serif HC;Noto Serif CJK HK" },
            { LANGUAGE_CHINESE_SIMPLIFIED,  u"宋体;SimSun;宋体-简;Song;思源宋体;Source Han Serif SC;Noto Serif CJK SC" },
            { LANGUAGE_CHINESE_SINGAPORE,   u"宋体;SimSun;宋体-简;Song;思源宋体;Source Han Serif SC;Noto Serif CJK SC" },
            { LANGUAGE_JAPANESE,            u"ＭＳ Ｐ明朝;MS PMincho;游明朝体;YuMincho;源ノ明朝;Source Han Serif;Noto Serif CJK JP" },
            { LANGUAGE_KOREAN,              u"바탕;Batang;일반체;AppleMyungjo;본명조;Source Han Serif K;Noto Serif CJK KR" },
        }
    }
};

/*----------------- FontFeature -----------------*/
FontFeature::FontFeature()
{}

FontFeature::~FontFeature()
{}

ImplFontAttrs FontFeature::IsCJKFont(PhysicalFontFace* pFace)
{
    ImplFontAttrs aAttrs = ImplFontAttrs::None;
    vcl::FontCapabilities aFontCapabilities;

    if (pFace->GetFontCapabilities(aFontCapabilities) && aFontCapabilities.oCodePageRange)
    {
        const std::bitset<vcl::CodePageCoverage::MAX_CP_ENUM> aCodePageRange =
            *aFontCapabilities.oCodePageRange;

        if (aCodePageRange[vcl::CodePageCoverage::CP932])
            aAttrs |= ImplFontAttrs::CJK_JP;
        if (aCodePageRange[vcl::CodePageCoverage::CP936])
            aAttrs |= ImplFontAttrs::CJK_SC;
        if (aCodePageRange[vcl::CodePageCoverage::CP949] ||
            aCodePageRange[vcl::CodePageCoverage::CP1361] /* fold Korean */)
            aAttrs |= ImplFontAttrs::CJK_KR;
        if (aCodePageRange[vcl::CodePageCoverage::CP950])
            aAttrs |= ImplFontAttrs::CJK_TC;

        if (aAttrs != ImplFontAttrs::None)
            aAttrs |= ImplFontAttrs::CJK;
    }

    return aAttrs;
}

const OUString FontFeature::GetEqualFontsByLanguage(const LanguageType& rLanguage) const
{
    OUString aFontLists;
    auto it = maEqualFonts.find(rLanguage);
    if (it != maEqualFonts.end())
    {
        aFontLists = it->second;
    }

    if (!maDefaultFallbackFont.isEmpty())
    {
        if (!aFontLists.isEmpty())
            aFontLists += ";";
        aFontLists += maDefaultFallbackFont;
    }

    return aFontLists;
}

const OUString FontFeature::GetUnionFontsByLanguage(const LanguageType& rLanguage) const
{
    OUString aFontLists;
    auto it = maUnionFonts.find(rLanguage);
    if (it != maUnionFonts.end())
    {
        aFontLists = it->second;
    }

    if (aFontLists.isEmpty())
        aFontLists = maDefaultUnionFont;

    return aFontLists;
}

bool FontFeature::IsMatch(const OUString& rFontName) const
{
    sal_Int32 nTokenPos = 0;
    while (nTokenPos != -1)
    {
        const OUString aTokenName(GetNextFontToken(maFeatureName, nTokenPos));
        // start with aTokenName
        if (aTokenName[0] == '^')
        {
            if (rFontName.indexOf(aTokenName.copy(1)) == 0)
                return true;
        }
        // end with aTokenName
        else if (aTokenName[0] == '$')
        {
            if (rFontName.indexOf(aTokenName.copy(1)) == rFontName.getLength() - aTokenName.getLength() + 1)
                return true;
        }
        // contains aTokenName
        else if (aTokenName[0] == '*')
        {
            if (rFontName.indexOf(aTokenName.copy(1)) != -1)
                return true;
        }
        // otherwise, find the font name that exactly matches aTokenName
        else
        {
            if (rFontName == aTokenName)
                return true;
        }
    }

    return false;
}

/*--------------------- LoFontSubstitution ---------------------*/

LoFontSubstitution::LoFontSubstitution()
    : mpDevPreMatchHook(nullptr)
    , mpDevFallbackHook(nullptr)
{
    // 取得系統語言
    meSystemLanguage = MsLangId::getConfiguredSystemUILanguage();

    // Noto CJK fonts are compatible with Source Han fonts.
    // This map is used to convert the font name of Noto CJK to the font name of Source Han.
    // or convert the font name of Source Han to the font name of Noto CJK.
    maCompatibleFonts =
    {
        // Corresponds Google Noto Serif CJK font to Source Han Serif font.
        { "Noto Serif CJK TC", {u"思源宋體", "Source Han Serif TC"} },
        { "Noto Serif CJK HK", {u"思源宋體 香港", "Source Han Serif HC"} },
        { "Noto Serif CJK SC", {u"思源宋体", "Source Han Serif SC"} },
        { "Noto Serif CJK JP", {u"源ノ明朝", "Source Han Serif"} },
        { "Noto Serif CJK KR", {u"본명조", "Source Han Serif K"} },
        // Corresponds Google Noto Sans CJK font to Source Han Sans font.
        { "Noto Sans CJK TC", {u"思源黑體", "Source Han Sans TC"} },
        { "Noto Sans CJK HK", {u"思源黑體 香港", "Source Han Sans HC"} },
        { "Noto Sans CJK SC", {u"思源黑体", "Source Han Sans SC"} },
        { "Noto Sans CJK JP", {u"源ノ角ゴシック", "Source Han Sans"} },
        { "Noto Sans CJK KR", {u"본고딕", "Source Han Sans K"} },
        // Corresponds Google Noto Sans Mono CJK font to Source Han Sans HW font.
        { "Noto Sans Mono CJK TC", {u"思源黑體 HW", "Source Han Sans HW TC"} },
        { "Noto Sans Mono CJK HK", {u"思源黑體 香港 HW", "Source Han Sans HW HC"} },
        { "Noto Sans Mono CJK SC", {u"思源黑体 HW", "Source Han Sans HW SC"} },
        { "Noto Sans Mono CJK JP", {u"源ノ角ゴシック HW", "Source Han Sans HW"} },
        { "Noto Sans Mono CJK KR", {u"본고딕 HW", "Source Han Sans HW K"} }
    };

    for (auto& aCJKFeature : aCJKFeaturesMap)
    {
        // Add to the font feature collection
        AddFeature(aCJKFeature.maFeatureName, aCJKFeature.maEqualFonts,
                  aCJKFeature.maDefaultFallbackFont, aCJKFeature.maUnionFonts,
                  aCJKFeature.maDefaultUnionFont, aCJKFeature.maPrivateUseAreaFont);
    }

    SetFinalCJKFallbackFonts(u"全字庫正宋體;TW-Sung;全字庫正宋體 Ext-B;TW-Sung-Ext-B;全字庫正宋體 Plus;TW-Sung-Plus");
    // TODO: set final fallback fonts
    // SetFinalFallbackFonts();
}

LoFontSubstitution::~LoFontSubstitution()
{
    maFontFamilySubstituteCache.clear();
}

void LoFontSubstitution::AddCompatibleFont(const OUString& rFontName, const OUString& rCompatibleFontName)
{
    // find the font name in the compatible font map
    auto it = maCompatibleFonts.find(rFontName);
    if (it != maCompatibleFonts.end())
    {
        // 檢查相容列表中是否已經有相容字型名稱
        auto it2 = std::find(it->second.begin(), it->second.end(), rCompatibleFontName);
        if (it2 == it->second.end())
            // If the compatible font name is not found, append the compatible font name to the compatible font map.
            it->second.emplace_back(rCompatibleFontName);
    }
    else
    {
        // If the font name is not found, add the font name and the compatible font name to the compatible font map.
        maCompatibleFonts.insert({ rFontName, {rCompatibleFontName} });
    }
}

void LoFontSubstitution::AddFeature(const OUString& rFeatureName, const LanguageFontMap& rEqualFonts,
                                    const OUString& rDefaultFallbackFont, const LanguageFontMap& rUnionFonts,
                                    const OUString& rDefaultUnionFont, const OUString& rPrivateUseAreaFont)
{
    std::unique_ptr<FontFeature> aFeature = std::make_unique<FontFeature>();
    aFeature->SetFeatureName(rFeatureName);
    aFeature->SetEqualFonts(rEqualFonts);
    aFeature->SetDefaultFallbackFont(rDefaultFallbackFont);
    aFeature->SetUnionFonts(rUnionFonts);
    aFeature->SetDefaultUnionFont(rDefaultUnionFont);
    aFeature->SetPrivateUseAreaFont(rPrivateUseAreaFont);
    maFontFeatures.emplace_back(std::move(aFeature));
}

FontFeature* LoFontSubstitution::FindFeature(const OUString& rFontName)
{
    for (const auto& aFeature : maFontFeatures)
    {
        if (aFeature->IsMatch(rFontName))
            return aFeature.get();
    }

    return nullptr;
}

const FontFeature* LoFontSubstitution::FindFeature(const OUString& rFontName) const
{
    return const_cast<LoFontSubstitution*>(this)->FindFeature(rFontName);
}

void LoFontSubstitution::SetEUDCRelation(const OUString& rMasterFamilyName, const OUString& rEUDCFamilyName)
{
    if (rMasterFamilyName.equalsIgnoreAsciiCase("SystemDefaultEUDCFont"))
    {
        maSystemDefaultEUDCFont = rEUDCFamilyName;
    }
    else
    {
        maEUDCRelations[rMasterFamilyName] = rEUDCFamilyName; // Add to the EUDC relation map
        // Set the private use area font for the specified font feature
        FontFeature* pFeature = FindFeature(rMasterFamilyName);
        if (pFeature)
            pFeature->SetPrivateUseAreaFont(rEUDCFamilyName);
    }
}

PhysicalFontFamily* LoFontSubstitution::GetEUDCRelation(const OUString& rMasterFamilyName) const
{
    const PhysicalFontCollection* pFontCollection = ImplGetSVData()->maGDIData.mxScreenFontList.get();
    PhysicalFontFamily* pFoundData = nullptr;

    // 有指定要匹配的字型名稱
    if (!rMasterFamilyName.isEmpty())
    {
        auto it = maEUDCRelations.find( rMasterFamilyName );
        // 沒有找到，找對應的字型特徵
        if (it == maEUDCRelations.end())
        {
            const FontFeature* pFeature = FindFeature(rMasterFamilyName);
            if (pFeature && !pFeature->GetPrivateUseAreaFont().isEmpty())
                pFoundData = pFontCollection->FindFontFamily(pFeature->GetPrivateUseAreaFont());
        }
        else
            pFoundData = pFontCollection->FindFontFamily(it->second);
    }

    //
    if (!pFoundData && !maSystemDefaultEUDCFont.isEmpty())
        pFoundData = pFontCollection->FindFontFamily(maSystemDefaultEUDCFont);

    return pFoundData;
}

/**
 * @brief FindFontSubstitute function searches for a font substitute for the given FontSelectPattern.
 *
 * @param rFSD The FontSelectPattern object for which a font substitute is to be found.
 * @return bool Returns true if a font substitute is found, false otherwise.
 */
bool LoFontSubstitution::FindFontSubstitute(FontSelectPattern& rFSD) const
{
    // Ignore Microsoft Symbol Encoded font and OpenSymbol font.
    if (rFSD.IsMicrosoftSymbolEncoded() || IsOpenSymbol(rFSD.maSearchName))
        return false;

    // Check cache first
    if (GetCache(rFSD))
        return true;

    if (GetCompatible(rFSD))
        return true;

    const PhysicalFontCollection *pFontCollection = ImplGetSVData()->maGDIData.mxScreenFontList.get();
    PhysicalFontFamily* pFoundData = nullptr;

    // 尋找字型名稱符合 FontFeature
    const FontFeature* pFeature = FindFeature(rFSD.maTargetName);
    if (pFeature)
    {
        const OUString aFontLists = pFeature->GetEqualFontsByLanguage(rFSD.meLanguage);
        pFoundData = pFontCollection->FindFontFamilyByTokenNames(aFontLists);
    }
    // 字型名稱不符合 Feature，但指定了 CJK 語言
    else if (MsLangId::isCJK(rFSD.meLanguage))
    {
        const utl::DefaultFontConfiguration& rDefaults = utl::DefaultFontConfiguration::get();
        const LanguageTag aLanguageTag(rFSD.meLanguage);
        OUString aFontname = rDefaults.getDefaultFont(aLanguageTag, DefaultFontType::CJK_TEXT);
        pFoundData = pFontCollection->FindFontFamilyByTokenNames(aFontname);

        if (!pFoundData)
        {
            aFontname = rDefaults.getDefaultFont(aLanguageTag, DefaultFontType::UI_SANS);
            pFoundData = pFontCollection->FindFontFamilyByTokenNames(aFontname);
        }
    }

    if (pFoundData)
    {
        AddToCache(rFSD, pFoundData->GetFamilyName());
        return true;
    }

    // Call device specific prematch substitution
    if (mpDevPreMatchHook)
    {
        const OUString aSavedSearchName = rFSD.maSearchName; // Save the original font name
        if (mpDevPreMatchHook->FindFontSubstitute(rFSD) && rFSD.maSearchName != aSavedSearchName)
        {
            pFoundData = pFontCollection->FindFontFamily(rFSD.maSearchName);
            if (pFoundData)
            {
                AddToCache(rFSD, pFoundData->GetFamilyName());
                return true;
            }
        }
    }

    return false;
}

/**
 * @brief Finds a font substitute for the given font select pattern and logical font instance.
 *
 * This function searches for font substitutes in the following order:
 * 1. Symbol font fallback
 * 2. CJK font fallback
 * 3. EUDC font fallback
 * 4. Find fonts one by one
 *
 * If no font substitute is found, the function uses the original font to display the missing characters.
 *
 * @param rFSD The font select pattern.
 * @param pLogicalFont The logical font instance.
 * @param rMissingChars The missing characters.
 * @return True if a font substitute is found, false otherwise.
 */
bool LoFontSubstitution::FindFontSubstitute(FontSelectPattern& rFSD,
                                              LogicalFontInstance* pLogicalFont,
                                              OUString& rMissingChars) const
{
    // 1. Symbol font fallback
    if (ImplFindSymbolFontSubstitute(rFSD, pLogicalFont, rMissingChars))
        return true;

    // 2. CJK font fallback
    if (ImplFindFontFeatureSubstitute(rFSD, pLogicalFont, rMissingChars))
        return true;

    // 3. EUDC font fallback
    if (ImplFindEUDCFontSubstitute(rFSD, pLogicalFont, rMissingChars))
        return true;

    // 4. Find fonts one by one
    if (ImplFindFontsOneByOne(rFSD, pLogicalFont, rMissingChars))
        return true;

    {
        // Still no font found containing missing characters.
        // Just use the original font to display the missing characters.
        // Usually the characters defined by the .notdef of the font are displayed (square-shaped).
        rFSD.maSearchName = pLogicalFont->GetFontFace()->GetFamilyName();
        // Clear the missing characters to display the square box
        rMissingChars.clear();
        return true;
    }

    return true;

    /* // 如果有指定的 GlyphFallbackHook，則呼叫它
    if (mpDevFallbackHook)
        return mpDevFallbackHook->FindFontSubstitute(rFSD, pLogicalFont, rMissingChars);

    return false; */
}

bool LoFontSubstitution::GetCache(FontSelectPattern& rFSD) const
{
    // 如果沒有指定語言，則使用系統預設語言
    LanguageType aLang = (rFSD.meLanguage == LANGUAGE_NONE
                       || rFSD.meLanguage == LANGUAGE_SYSTEM
                       || rFSD.meLanguage == LANGUAGE_DONTKNOW)
                       ? meSystemLanguage : rFSD.meLanguage;

    // 快取中是否有指定的字型名稱
    auto it = maFontFamilySubstituteCache.find(rFSD.maTargetName);
    // 找到字體名稱
    if (it != maFontFamilySubstituteCache.end())
    {
        // 找指定的語言
        auto it2 = it->second.find(aLang);
        if (it2 != it->second.end())
        {
            rFSD.maSearchName = it2->second;
            return true;
        }
    }
    return false;
}

bool LoFontSubstitution::GetCompatible(FontSelectPattern& rFSD) const
{
    const PhysicalFontCollection* pFontCollection = ImplGetSVData()->maGDIData.mxScreenFontList.get();
    PhysicalFontFamily* pFoundData = nullptr;

    // Check if there are compatible fonts
    auto it = maCompatibleFonts.find(rFSD.maTargetName);
    if (it != maCompatibleFonts.end())
    {
        // Check if the font name is in the compatible font list
        for (auto &it2 : it->second)
        {
            pFoundData = pFontCollection->FindFontFamily(it2);
            if (pFoundData)
            {
                AddToCache(rFSD, pFoundData->GetFamilyName());
                return true;
            }
        }
    }

    // Reverse check to see if there is a compatible font
    for (auto& revIt : maCompatibleFonts)
    {
        // Check if the font name is in the compatible font list
        for (auto &it2 : revIt.second)
        {
            // If the font name is in the compatible font list
            if (it2 == rFSD.maTargetName)
            {
                // Find the font name in the font collection
                pFoundData = pFontCollection->FindFontFamily(revIt.first);
                if (pFoundData)
                {
                    AddToCache(rFSD, pFoundData->GetFamilyName());
                    return true;
                }
            }
        }
    }
    return false;
}

void LoFontSubstitution::AddToCache(FontSelectPattern& rFSD, const OUString& rFontName) const
{
    // 如果沒有指定語言，則使用系統預設語言
    LanguageType aLang = (rFSD.meLanguage == LANGUAGE_NONE
                       || rFSD.meLanguage == LANGUAGE_SYSTEM
                       || rFSD.meLanguage == LANGUAGE_DONTKNOW)
                       ? meSystemLanguage : rFSD.meLanguage;

    maFontFamilySubstituteCache[rFSD.maTargetName][aLang] = rFontName;
    rFSD.maSearchName = rFontName;
}

void LoFontSubstitution::DumpCache() const
{
    for (const auto& rPair : maFontFamilySubstituteCache)
    {
        std::cout << "Font: " << rPair.first << std::endl;
        for (const auto& rPair2 : rPair.second)
        {
            // rPair2.first: LanguageType
            // rPair2.second: FontName
            LanguageTag aLocale(rPair2.first);
            std::cout << "  " << aLocale.getBcp47()
                      << "(0x" << std::hex << std::uppercase << rPair2.first << std::dec << "): "
                      << rPair2.second << std::endl;
        }
    }
}

/**
 * Check if the given PhysicalFontFace has any missing characters.
 *
 * @param pFace The PhysicalFontFace to check.
 * @param rMissingChars The string of missing characters.
 * @return True if there are missing characters, false otherwise.
 */
bool LoFontSubstitution::HasMissingChars(PhysicalFontFace* pFace, OUString& rMissingChars) const
{
    FontCharMapRef xFontCharMap = pFace->GetFontCharMap();

    // avoid fonts with unknown CMAP subtables for glyph fallback
    if (!xFontCharMap.is() || xFontCharMap->IsDefaultMap())
        return false;

    int nMatchCount = 0;
    std::vector<sal_UCS4> rRemainingCodes;
    const sal_Int32 nStrLen = rMissingChars.getLength();
    sal_Int32 nStrIdx = 0;
    while (nStrIdx < nStrLen)
    {
        const sal_UCS4 uChar = rMissingChars.iterateCodePoints(&nStrIdx);
        if (xFontCharMap->HasChar(uChar))
            nMatchCount++;
        else
            rRemainingCodes.emplace_back(uChar);
    }

    xFontCharMap = nullptr;

    if (nMatchCount > 0)
        rMissingChars = OUString(rRemainingCodes.data(), rRemainingCodes.size());

    return nMatchCount > 0;
}

/**
 * @brief Finds a substitute font for symbol characters in the given font selection pattern.
 *
 * This function checks if there is a Wingdings font available in the system font collection.
 * If Wingdings font is found and there are missing symbol characters in the range 0xF020 ~ 0xF0FF,
 * it replaces the missing characters with the Wingdings font.
 *
 * @param rFSD The font selection pattern to search for a substitute font.
 * @param pLogicalFont The logical font instance associated with the font selection pattern.
 * @param rMissingChars The string of missing characters to be replaced.
 *
 * @return True if a substitute font is found and symbol characters are replaced, false otherwise.
 */
bool LoFontSubstitution::ImplFindSymbolFontSubstitute(FontSelectPattern& rFSD,
                                                        LogicalFontInstance* pLogicalFont,
                                                        OUString& rMissingChars) const
{
    const PhysicalFontCollection* pFontCollection = ImplGetSVData()->maGDIData.mxScreenFontList.get();
    // Whether there is Wingdings font.
    PhysicalFontFamily* pWingdings = pFontCollection->FindFontFamily(u"Wingdings");
    if (!pWingdings)
        return false;

    int nMatchCount = 0; // Number of missing characters in the range 0xF020 ~ 0xF0FF
    std::vector<sal_UCS4> rRemainingCodes;
    const sal_Int32 nStrLen = rMissingChars.getLength();
    sal_Int32 nStrIdx = 0;
    while (nStrIdx < nStrLen)
    {
        const sal_UCS4 uChar = rMissingChars.iterateCodePoints(&nStrIdx);
        if (uChar >= 0xF020 && uChar <= 0xF0FF)
            nMatchCount++;
        else
            rRemainingCodes.emplace_back(uChar);
    }

    // If there are missing characters in the range 0xF020 ~ 0xF0FF, replace them with Wingdings font.
    if (nMatchCount > 0)
    {
        rMissingChars = OUString(rRemainingCodes.data(), rRemainingCodes.size());
        PhysicalFontFace* pFace = pLogicalFont->GetFontFace();
        // 如果是 CJK 字型，就 fallback 到 Wingdings
        if ((FontFeature::IsCJKFont(pFace) & ImplFontAttrs::CJK) != ImplFontAttrs::None)
            rFSD.maSearchName = pWingdings->GetFamilyName();
        else
            // 非 CJK 字型，又有包含 Microsoft symbol 字元，不能再進入後續的 fallback 程序
            // 在這裡就要終止，顯示目前實體字型缺字方塊
            rFSD.maSearchName = pFace->GetFamilyName();
    }

    return nMatchCount > 0;
}

bool LoFontSubstitution::ImplFindFontFeatureSubstitute(FontSelectPattern& rFSD,
                                                         LogicalFontInstance* /*pLogicalFont*/,
                                                         OUString& rMissingChars) const
{
    const PhysicalFontCollection* pFontCollection = ImplGetSVData()->maGDIData.mxScreenFontList.get();

    std::vector<OUString> aFallbackFontLists;

    // 依據字型名稱，找出對應的字型特徵
    const FontFeature* pFeature = FindFeature(rFSD.maTargetName);
    // 字型名稱包含字型特徵
    if (pFeature)
    {
        // 1.依語言取得等效字型名稱
        const OUString aEqualFontList = pFeature->GetEqualFontsByLanguage(rFSD.meLanguage);
        if (!aEqualFontList.isEmpty())
            aFallbackFontLists.emplace_back(aEqualFontList);

        // 2.依語言取得聯合字型名稱
        const OUString aUnionFontList = pFeature->GetUnionFontsByLanguage(rFSD.meLanguage);
        if (!aUnionFontList.isEmpty())
            aFallbackFontLists.emplace_back(aUnionFontList);
    }
    else // 否則取得該語系的預設字型名稱列表
    {
        // guess a locale matching to the missing chars
        LanguageType eLang = rFSD.meLanguage;
        LanguageTag aLanguageTag(eLang);

        // fall back to default UI locale if the font language is inconclusive
        if (eLang == LANGUAGE_DONTKNOW)
            aLanguageTag = Application::GetSettings().GetUILanguageTag();

        // get the default font for a specified locale
        const utl::DefaultFontConfiguration& rDefaults = utl::DefaultFontConfiguration::get();
        const OUString aDefault = rDefaults.getUserInterfaceFont(aLanguageTag);
        if (!aDefault.isEmpty())
            aFallbackFontLists.emplace_back(aDefault);
    }

    // 3.加入最終的回退字型名稱
    const OUString aFinalFallbackFonts = MsLangId::isCJK(rFSD.meLanguage)
        ? GetFinalCJKFallbackFonts() : GetFinalFallbackFonts();
    if (!aFinalFallbackFonts.isEmpty())
        aFallbackFontLists.emplace_back(aFinalFallbackFonts);

    // 依序從 aFallbackFontLists 中找出第一個有包含缺少字元的字型
    PhysicalFontFamily* pFoundData = nullptr;
    for (const OUString& rFallbackFontList : aFallbackFontLists)
    {
        for(sal_Int32 nTokenPos = 0; !rFallbackFontList.isEmpty() && nTokenPos != -1;)
        {
            std::u16string_view aFamilyName = GetNextFontToken(rFallbackFontList, nTokenPos);
            if (aFamilyName.empty())
                continue;

            pFoundData = pFontCollection->FindFontFamily(aFamilyName);

            // 有實體字體
            if (pFoundData)
            {
                // 這個字體有包含缺少的字元
                if (HasMissingChars(pFoundData->FindBestFontFace(rFSD), rMissingChars))
                {
                    rFSD.maSearchName = pFoundData->GetFamilyName(); // 設定實體的字型名稱
                    return true;
                }
            }
        }
    }
    return false;
}

bool LoFontSubstitution::ImplFindEUDCFontSubstitute(FontSelectPattern& rFSD,
                                                      LogicalFontInstance* /*pLogicalFont*/,
                                                      OUString& rMissingChars) const
{
    PhysicalFontFamily* pFoundData = GetEUDCRelation(rFSD.maTargetName);

    if (pFoundData && HasMissingChars(pFoundData->FindBestFontFace(rFSD), rMissingChars))
    {
        rFSD.maSearchName = pFoundData->GetFamilyName(); // 設定實體的字型名稱
        return true;
    }

    return false;
}

bool LoFontSubstitution::ImplFindFontsOneByOne(FontSelectPattern& rFSD,
                                                 LogicalFontInstance* /* pLogicalFont */,
                                                 OUString& rMissingChars) const
{
    const PhysicalFontCollection *pFontCollection = ImplGetSVData()->maGDIData.mxScreenFontList.get();
    std::unique_ptr<PhysicalFontFaceCollection> pTestFontList = pFontCollection->GetFontFaceCollection();
    bool bFound = false;
    sal_Int32 nTestFontCount = pTestFontList->Count();
    for(int i = 0; i < nTestFontCount; ++i)
    {
        PhysicalFontFace* pFace = pTestFontList->Get(i);
        // Skip Microsoft symbol encoded fonts and OpenSymbol font
        if (pFace->IsMicrosoftSymbolEncoded() || IsOpenSymbol(pFace->GetFamilyName()))
            continue;

        bFound = HasMissingChars(pFace, rMissingChars);
        if (!bFound)
            continue;

        rFSD.maSearchName = pFace->GetFamilyName();
        break;
    }

    return bFound;
}

} // namespace vcl::font

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
