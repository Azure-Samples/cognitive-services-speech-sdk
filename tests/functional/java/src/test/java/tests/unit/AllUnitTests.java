//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

package tests.unit;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

@RunWith(Suite.class)
@SuiteClasses({
    IntentRecognizerTests.class,
    SpeechConfigTests.class,
    SpeechRecognizerTests.class,
    SpeechSynthesizerTests.class,
    TranslationRecognizerTests.class,
    KeywordRecognitionModelTests.class,
    LanguageUnderstandingModelTests.class,
    RecognizerTests.class,
    SpeechPropertyIdTests.class,
    AudioInputStreamTests.class,
    PropertyCollectionTests.class,
    PhraseListGrammarTests.class,
    GrammarTests.class,
    GrammarListTests.class,
    ClassLanguageModelTests.class,
    ConversationTranscriberTests.class,
    ConversationTranslatorTests.class,
    CloseGuardTests.class,
    KeywordRecognizerTests.class,
    CompressedInputStreamTests.class,
    PronunciationAssessmentTests.class
    })
public class AllUnitTests {

}
