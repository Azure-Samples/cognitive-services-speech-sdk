package Carbon.Recognition.Speech;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import Carbon.Recognition.RecognitionStatus;

/// <summary>
/// Defines result of speech recognition.
/// </summary>
public class SpeechRecognitionResult 
{
    protected SpeechRecognitionResult(Carbon.Internal.RecognitionResult result)
    {
        this._ResultId = result.getResultId();
        this._RecognizedText = result.getText();
        this._Reason = RecognitionStatus.values()[result.getReason().swigValue()];
    }

    /// <summary>
    /// Specifies the result identifier.
    /// </summary>
    public String getResultId()
    {
        return _ResultId;
    }// { get; }
    private String _ResultId;

    /// <summary>
    /// Specifies status of the result.
    /// </summary>
    public RecognitionStatus getReason() // { get; }
    {
        return _Reason;
    }
    private RecognitionStatus _Reason;

    /// <summary>
    /// Presents the recognized text in the result.
    /// </summary>
    public String getRecognizedText() {
        return _RecognizedText;
    } // { get; }
    private String _RecognizedText;

    /// <summary>
    /// Returns a String that represents the speech recognition result.
    /// </summary>
    /// <returns>A String that represents the speech recognition result.</returns>
    @Override
    public String toString()
    {
        return "ResultId:" + _ResultId + " Status:" + _Reason + " Recognized text:<" + _RecognizedText + ">.";
    }
}
