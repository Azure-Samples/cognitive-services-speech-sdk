package tests.unit;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

@RunWith(Suite.class)
@SuiteClasses({
    SpeechFactoryTests.class,
    SpeechRecognizerTests.class,
    IntentRecognizerTests.class,
    TranslationRecognizerTests.class,
    KeywordRecognitionModelTests.class,
    LanguageUnderstandingModelTests.class
    })
public class AllUnitTests {

}
