//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <string>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace IntegrationTests {

    /// <summary>
    /// Base class for translator languages
    /// </summary>
    class TranslatorLanguageBase
    {
    public:
        /// <summary>
        /// Default copy constructor
        /// </summary>
        TranslatorLanguageBase(const TranslatorLanguageBase&) = default;

        /// <summary>
        /// Default move constructor
        /// </summary>
        TranslatorLanguageBase(TranslatorLanguageBase&&) = default;

        /// <summary>
        /// Gets the full code for this language (e.g. zh-Hans-CN)
        /// </summary>
        /// <returns>Reference to string</returns>
        const std::string& Code() const;

        /// <summary>
        /// Gets just the code for this language (e.g. zh)
        /// </summary>
        /// <returns>Reference to string</returns>
        const std::string& Lang() const;

        /// <summary>
        /// Determines if this language is equal to the other language
        /// </summary>
        /// <param name="other">The other language to compare to</param>
        /// <returns>True if they are equal, false otherwise</returns>
        bool virtual Equals(const TranslatorLanguageBase& other) const = 0;

    protected:
        static bool TryParseLanguageCode(const std::string& rawLangCode, std::string& lang, std::string& script, std::string& region);
        TranslatorLanguageBase(const std::string& fullCode, const std::string& lang);

    private:
        std::string fullCode;
        const std::string lang;
    };

    /// <summary>
    /// A text language for translation
    /// </summary>
    class TranslatorTextLanguage : public TranslatorLanguageBase
    {
    public:
        /// <summary>
        /// Default copy constructor
        /// </summary>
        TranslatorTextLanguage(const TranslatorTextLanguage&) = default;

        /// <summary>
        /// Default move constructor
        /// </summary>
        TranslatorTextLanguage(TranslatorTextLanguage&&) = default;

        /// <summary>
        /// Gets the script for this language (e.g. Hans)
        /// </summary>
        /// <returns>Reference to string</returns>
        const std::string& Script() const;

        /// <summary>
        /// Parses the raw language code (e.g. zh-Hans-CN)
        /// </summary>
        /// <param name="rawLang">The raw language code to parse</param>
        /// <returns>The parsed text language. An exception is thrown on failures</returns>
        static TranslatorTextLanguage Parse(const std::string& rawLang);

        /// <summary>
        /// Determines if this language is equal to the other language
        /// </summary>
        /// <param name="other">The other language to compare to</param>
        /// <returns>True if they are equal, false otherwise</returns>
        bool Equals(const TranslatorLanguageBase& other) const override;

    private:
        TranslatorTextLanguage(const std::string& lang, const std::string& script);
        const std::string script;
    };

    /// <summary>
    /// A speech language for translation
    /// </summary>
    class TranslatorSpeechLanguage : public TranslatorLanguageBase
    {
    public:
        /// <summary>
        /// Default copy constructor
        /// </summary>
        TranslatorSpeechLanguage(const TranslatorSpeechLanguage&) = default;

        /// <summary>
        /// Default move constructor
        /// </summary>
        TranslatorSpeechLanguage(TranslatorSpeechLanguage&&) = default;

        /// <summary>
        /// Gets the region for this language (e.g. CN)
        /// </summary>
        /// <returns>Reference to string</returns>
        const std::string& Region() const;

        /// <summary>
        /// Gets the corresponding text language for this speech language
        /// </summary>
        /// <returns>Reference to the text language</returns>
        const TranslatorTextLanguage& TextLanguage() const;

        /// <summary>
        /// Parses the raw language code (e.g. zh-Hans-CN)
        /// </summary>
        /// <param name="rawLang">The raw language code to parse</param>
        /// <returns>The parsed speech language. An exception is thrown on failures</returns>
        static TranslatorSpeechLanguage Parse(const std::string& rawLang);

        /// <summary>
        /// Determines if this language is equal to the other language
        /// </summary>
        /// <param name="other">The other language to compare to</param>
        /// <returns>True if they are equal, false otherwise</returns>
        bool Equals(const TranslatorLanguageBase& other) const override;

        /// <summary>
        /// Determines if this speech language has the same text language as other
        /// </summary>
        /// <param name="other">The other language to compare to</param>
        /// <returns>True if they are equal, false otherwise</returns>
        bool virtual Equals(const TranslatorTextLanguage& other) const;

    private:
        TranslatorSpeechLanguage(const std::string& lang, const std::string& region, const TranslatorTextLanguage& textLang);
        const std::string region;
        const TranslatorTextLanguage textLang;
    };

}}}}
