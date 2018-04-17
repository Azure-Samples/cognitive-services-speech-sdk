package com.microsoft.cognitiveservices.speech.recognition.translation;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/// <summary>
/// Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
/// </summary>
public class TranslationSynthesisResult
{
    // BUG: this is hack for making documentation going.
    TranslationSynthesisResult(com.microsoft.cognitiveservices.speech.internal.TranslationSynthesisResult result)
    {
        _AudioData = null; // TODO result.getAudioData();
    }

    public TranslationSynthesisResult(com.microsoft.cognitiveservices.speech.recognition.translation.TranslationSynthesisResult result) {
        _AudioData = result.getAudioData();
    }

    /// <summary>
    /// translated text in the target language.
    /// </summary>
    public byte[] getAudioData()
    {
        return _AudioData;
    }// { get; }
    private byte[] _AudioData;

    /// <summary>
    /// Returns a String that represents the speech recognition result.
    /// </summary>
    /// <returns>A String that represents the speech recognition result.</returns>
    @Override
    public String toString()
    {
        return "AudioData <<" + _AudioData + ">>";
    }
}
