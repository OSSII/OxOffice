/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4; fill-column: 100 -*- */
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

#pragma once

#include <i18nlangtag/mslangid.hxx>
#include <font/fontsubstitution.hxx>

#include <memory>
#include <map>
#include <vector>

namespace vcl::font
{

class FontSelectPattern;
class PhysicalFontFace;

typedef std::map<LanguageType, OUString> LanguageFontMap;

class FontFeature final
{
public:
    explicit FontFeature();
             ~FontFeature();

    static ImplFontAttrs IsCJKFont(PhysicalFontFace* pFace);

    const OUString& GetFeatureName() const { return maFeatureName; }
    const LanguageFontMap& GetEqualFonts() const { return maEqualFonts; }
    const OUString& GetDefaultFallbackFont() const { return maDefaultFallbackFont; }
    const LanguageFontMap& GetUnionFonts() const { return maUnionFonts; }
    const OUString& GetDefaultUnionFont() const { return maDefaultUnionFont; }
    const OUString& GetPrivateUseAreaFont() const { return maPrivateUseAreaFont; }

    const OUString GetEqualFontsByLanguage(const LanguageType& rLanguage) const;
    const OUString GetUnionFontsByLanguage(const LanguageType& rLanguage) const;

    void SetFeatureName(const OUString& rFeatureName) { maFeatureName = rFeatureName; }
    void SetEqualFonts(const LanguageFontMap& rEqualFonts) { maEqualFonts = rEqualFonts; }
    void SetDefaultFallbackFont(const OUString& rDefaultFallbackFont) { maDefaultFallbackFont = rDefaultFallbackFont; }
    void SetUnionFonts(const LanguageFontMap& rUnionFonts) { maUnionFonts = rUnionFonts; }
    void SetDefaultUnionFont(const OUString& rDefaultUnionFont) { maDefaultUnionFont = rDefaultUnionFont; }
    void SetPrivateUseAreaFont(const OUString& rPrivateUseAreaFont) { maPrivateUseAreaFont = rPrivateUseAreaFont; }

    bool IsMatch(const OUString& rFontName) const;

private:
    OUString maFeatureName; // feature name

    // 這兩個是給 pre match 使用的
    LanguageFontMap maEqualFonts = {};
    OUString maDefaultFallbackFont = "";

    // 以下是給 glyph fallback 使用的
    LanguageFontMap maUnionFonts = {};
    OUString maDefaultUnionFont = "";
    OUString maPrivateUseAreaFont = "";
};

class LoFontSubstitution : public PreMatchFontSubstitution,
                           public GlyphFallbackFontSubstitution
{
public:
    static LoFontSubstitution& instance()
    {
        static LoFontSubstitution aFontSubstitution;
        return aFontSubstitution;
    }

    bool FindFontSubstitute(FontSelectPattern& rFSD) const override;
    bool FindFontSubstitute(FontSelectPattern& rFSD,
                            LogicalFontInstance* pLogicalFont,
                            OUString& rMissingChars) const override;

    void SetPreMatchHook(PreMatchFontSubstitution* pHook) { mpDevPreMatchHook = pHook; }
    void SetFallbackHook(GlyphFallbackFontSubstitution* pHook) { mpDevFallbackHook = pHook; }

    /// @brief 新增相容字型到字型替代表
    /// @param rFontName 字型名稱
    /// @param rCompatibleFontName 相容字型名稱
    void AddCompatibleFont(const OUString& rFontName, const OUString& rCompatibleFontName);

    void AddFeature(const OUString& rFeatureName, const LanguageFontMap& rEqualFonts,
                    const OUString& rDefaultFallbackFont, const LanguageFontMap& rUnionFonts,
                    const OUString& rDefaultUnionFont, const OUString& rPrivateUseAreaFont);

    FontFeature* FindFeature(const OUString& rFontName);
    const FontFeature* FindFeature(const OUString& rFontName) const;

    const OUString& GetFinalFallbackFonts() const { return maFinalFallbackFonts; }
    const OUString& GetFinalCJKFallbackFonts() const { return maFinalCJKFallbackFonts; }

    void SetFinalFallbackFonts(const OUString& rFinalFallbackFonts) { maFinalFallbackFonts = rFinalFallbackFonts; }
    void SetFinalCJKFallbackFonts(const OUString& rFinalCJKFallbackFonts) { maFinalCJKFallbackFonts = rFinalCJKFallbackFonts; }

    void SetEUDCRelation( const OUString& rMasterFamilyName, const OUString& rEUDCFamilyName );
    PhysicalFontFamily* GetEUDCRelation( const OUString& rMasterFamilyName = OUString() ) const;

private:
    LoFontSubstitution();
    ~LoFontSubstitution();
    LoFontSubstitution(const LoFontSubstitution&) = delete;
    LoFontSubstitution& operator=(const LoFontSubstitution&) = delete;

    /// @brief device specific prematch substitution
    PreMatchFontSubstitution* mpDevPreMatchHook;
    /// @brief device specific glyph fallback substitution
    GlyphFallbackFontSubstitution* mpDevFallbackHook;

    LanguageType meSystemLanguage; // system language type

    /// @brief font family substitute cache
    mutable std::map<OUString, LanguageFontMap> maFontFamilySubstituteCache;

    std::map<OUString, std::vector<OUString>> maCompatibleFonts;

    std::vector<std::unique_ptr<FontFeature>> maFontFeatures;

    OUString maFinalFallbackFonts; // Final fallback font (except CJK)
    OUString maFinalCJKFallbackFonts; // final CJK fallback fonts

    std::map<OUString, OUString> maEUDCRelations;
    OUString maSystemDefaultEUDCFont;

    /// @brief Get the font family substitute for the given pattern in cache
    /// @param rFSD The pattern to get the font substitute for.
    bool GetCache(FontSelectPattern& rFSD) const;

    /// @brief Add a font family substitute to the cache
    /// @param rFSD The pattern to add the font substitute for.
    /// @param rFontName The font name to add to the cache.
    void AddToCache(FontSelectPattern& rFSD, const OUString& rFontName) const;

    bool GetCompatible(FontSelectPattern& rFSD) const;

    /// @brief Dump the font family cache.
    void DumpCache() const;

    /// @brief Check if the given font face has missing characters.
    /// @param pFace The font face to check for missing characters.
    /// @param rMissingChars The missing characters to check for.
    bool HasMissingChars(PhysicalFontFace* pFace, OUString& rMissingChars) const;

    bool ImplFindSymbolFontSubstitute(FontSelectPattern& rFSD,
                                      LogicalFontInstance* pLogicalFont,
                                      OUString& rMissingChars) const;

    bool ImplFindFontFeatureSubstitute(FontSelectPattern& rFSD,
                                       LogicalFontInstance* pLogicalFont,
                                       OUString& rMissingChars) const;

    bool ImplFindEUDCFontSubstitute(FontSelectPattern& rFSD,
                                    LogicalFontInstance* pLogicalFont,
                                    OUString& rMissingChars) const;

    bool ImplFindFontsOneByOne(FontSelectPattern& rFSD,
                               LogicalFontInstance* pLogicalFont,
                               OUString& rMissingChars) const;

};

} // namespace vcl::font

/* vim:set shiftwidth=4 softtabstop=4 expandtab cinoptions=b1,g0,N-s cinkeys+=0=break: */
