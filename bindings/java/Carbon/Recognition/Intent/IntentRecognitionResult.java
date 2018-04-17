package Carbon.Recognition.Intent;
//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/// <summary>
/// Defines result of intent recognition.
/// </summary>
public class IntentRecognitionResult extends Carbon.Recognition.Speech.SpeechRecognitionResult
{
    IntentRecognitionResult(Carbon.Internal.IntentRecognitionResult result)
    {
        super(result);
        this._intentId = result.getIntentId();
    }

    /// <summary>
    /// A String that represents the intent identifier being recognized.
    /// </summary>
    public String getIntentId() // { get; }
    {
        return _intentId;
    }
    private String _intentId;

    /// <summary>
    /// Returns a String that represents the intent recognition result.
    /// </summary>
    /// <returns>A String that represents the intent recognition result.</returns>
    @Override
    public String toString()
    {
        return "ResultId:" + getResultId() + " Status:" + getReason() + " IntentId:<" + _intentId + "> Recognized text:<" + getRecognizedText() + ">.";
    }
}
