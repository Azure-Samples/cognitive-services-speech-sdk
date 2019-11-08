//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.IO;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    static class TestData
    {
        public static string AudioDir { get; set; }

        public static string KwsDir { get; set; }

        public static class Kws
        {
            public static class Computer
            {
                public static readonly string ModelFile = Path.Combine(KwsDir, Path.Combine("Computer", "kws.table"));
                public static readonly string ModelKeyword = "computer";

                public static readonly string AudioFile = Path.Combine(KwsDir, "kws_whatstheweatherlike.wav");
                public static readonly string AudioFile2x = Path.Combine(KwsDir, "kws_whatstheweatherlike_x2.wav");
                public static readonly string Utterance = "Computer what's the weather like?";
            }
            public static class Computer2
            {
                public static readonly string ModelFile = Path.Combine(KwsDir, Path.Combine("Computer", "kws.table"));
                public static readonly string ModelKeyword = "computer";

                public static readonly string AudioFile1 = Path.Combine(KwsDir, "kws_whatstheweatherlike.wav");
                public static readonly string AudioFile2 = Path.Combine(KwsDir, "kws_howoldareyou.wav");
                public static readonly string Utterance1 = "Computer what's the weather like?";
                public static readonly string Utterance2 = "Computer how old are you?";
            }

            public static class Secret
            {
                public static readonly string ModelFile = Path.Combine(KwsDir, Path.Combine("Secret", "kws.table"));
                public static readonly string ModelKeyword = "secret";

                public static readonly string AudioFile = Path.Combine(KwsDir, "kws_secret.wav");
                public static readonly string AudioFile2x = Path.Combine(KwsDir, "kws_secret_x2.wav");
                public static readonly string Utterance = "Secret what's the weather like?";
            }
        }

        public static class German
        {
            public static class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Fähigkeiten und Fähigkeiten Batman hat keine inhärenten Superkräfte. Er verlässt sich auf seine eigenen wissenschaftlichen Fähigkeiten Detektiv Fähigkeiten und athletische Fähigkeiten in den Geschichten Batman gilt als einer der größten Detektiv der Welt, wenn nicht die größte Verbrechen Solver Batman wurde wiederholt als genial beschrieben. Level-Intellekt. Einer der größten Kampfkünstler im DC-Universum.",
                    "Und mit Spitzengebrauch durch menschliche körperliche Konditionierung reiste die Welt und erwarb die Fähigkeiten, die nötig waren, um seine Kreuzzugs-Grundkriminalität zu unterstützen.",
                    "Sein Wissen und seine Expertise in fast jeder Disziplin, die dem Menschen bekannt ist, ist fast unvergleichlich für jeden anderen Charakter im Universum.",
                    "Batmans unerschöpflicher Reichtum ermöglicht es ihm, auf fortschrittliche Technologie zuzugreifen, da ein kompetenter Wissenschaftler in der Lage ist, diese Technologien zu seinem Vorteil zu nutzen und zu modifizieren.",
                    "Batman beschreibt Superman als den gefährlichsten Mann der Welt, der in der Lage ist, ein Team von supermotorisierten Außerirdischen selbst zu besiegen, um seine eingesperrten Teamkollegen zu retten.",
                    "Batman beschreibt Superman als den gefährlichsten Mann der Welt, der in der Lage ist, ein Team von supermotorisierten Außerirdischen selbst zu besiegen, um seine inhaftierten Teamkollegen zu retten.",
                    "In der ersten Handlung betrachtet Superman auch Batman als einen der brillantesten Köpfe auf dem Planeten.",
                    "Batman hat die Fähigkeit, unter großen körperlichen Schmerzen zu funktionieren und der Kontrolle des Geistes standzuhalten. Er ist ein Meister der mehrsprachigen Verkleidung und ein Experte für Spionage, der oft Informationen unter verschiedenen Identitäten sammelt.",
                    "Batmans Karate-Judo- und Jujitsu-Training hat ihn zu einem Mathe-Meister der Tarnung gemacht und erlaubt ihm zu viel, so dass er nach Belieben erscheinen und verschwinden und sich befreien kann.",
                    ""
                };
            }

            public static class HowIsYourWork
            {
                public const string Utterance = "Wie Sie heute mit Arbeitsaufgaben umgehen";
            }

            public static class Weather
            {
                public static readonly string Utterance = "Wie ist das Wetter";
            }

            public static class FirstOne
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "CallTheFirstOne.wav");
                public static readonly string Utterance = "Rufe die erste an.";
            }

            public static class Profanity
            {
                public static readonly string MaskedUtterancePattern = "****";
                public static readonly string MaskedUtteranceTranslation = "Was bedeutet das Wort ****?";
                public static readonly string RemovedUtteranceTranslation = "Was bedeutet das Wort";
                public static readonly string RawUtteranceTranslation = "fick";
                public static readonly string TaggedUtteranceTranslation = "<profanity>fick</profanity>";
            }
        }

        public static class Chinese
        {
            public static class Weather
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "weatherInChinese.wav");
                public static readonly string Utterance = "天气怎么样";
                public static readonly string TranslationUtterance = "天气.";
            }
        }

        public static class English
        {
            public static class Margarita
            {
                // Manual transcription:
                // <music at the beginning, continues in the background once
                // speaker starts around second 7.25> we all know and love a
                // margarita and if you're anything like me you've probably had
                // your fair share and if you have you've probably noticed that
                // margaritas can vary from incredible to <ugh> so I'm gonna
                // show you how to make one of those incredibly fresh and
                // delicious margaritas there are 3 key steps you need to
                // follow first
                public static readonly string AudioFile = Path.Combine(AudioDir, "Margarita-44100.wav");
                public static readonly string[] Utterance =
                {
                    "we all know and Love, a margarita and if you're anything like me, you probably had your fair share and if you have youve probably noticed that margaritas can vary from incredible too",
                    "so I'm going to show you how to make one of those incredibly fresh and Delicious Margaritas. There are 3 key steps you need to follow 1st."

                };
            }

            public static class Batman
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "batman.wav");
                // This is expected results using StartContinuousRecognitionAsync() which uses conversation mode.
                // The result using RecognizeOnceAsync() might be slightly different, since it uses interactive mode.
                public static readonly string[] Utterances =
                {
                    "Skills and abilities Batman has no inherent super powers, he relies on his own scientific knowledge detective skills and athletic prowess in the stories. Batman is regarded as one of the world's greatest detective if not the world's greatest crime solver. Batman has been repeatedly described as having genius level intellect. One of the greatest martial artists in the DC universe.",
                    "And having peak human physical conditioning use traveled the world, acquiring the skills needed to aid his crusade grounds crime.",
                    "His knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe.",
                    "Batman's inexhaustible wealth allows him to access advanced technology as a proficient scientists is able to use and modify those technologies to his advantage.",
                    "Batman describes Superman as the most dangerous man on Earth able to defeat a team of super powered extraterrestrials by himself in order to rescue his imprison teammates.",
                    "In the 1st storyline, Superman also considers Batman to be one of the most brilliant minds on the planet.",
                    "Batman has the ability to function under great physical pain and withstand mind control. He is a master of disguise multilingual and an expert in espionage, often gathering information under different identity's.",
                    "Batman's Karate Judo and jujitsu training has made him a math are master of stealth an escape, allowing too much allowing him to appear and disappear at Will and to break free."
                };

                public static readonly string[] UtterancesInteractive =
                {
                    "Skills and abilities Batman has no inherent super powers, he relies on his own scientific knowledge detective skills and athletic prowess.",
                    "In the stories Batman is regarded as one of the world's greatest detective if not the world's greatest crime solver. Batman has been repeatedly described as having genius level intellect. One of the greatest martial artists in the DC universe.",
                    "And having peak human physical conditioning he has traveled the world, acquiring the skills needed to aid his crusade grounds crime.",
                    "His knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe.",
                    "Batman's inexhaustible wealth allows him to access advanced technology as a proficient scientists use able to use and modify those technologies to his advantage.",
                    "Batman describes Superman as the most dangerous man on Earth able to defeat a team of super powered extraterrestrials by himself in order to rescue his imprison teammates.",
                    "In the 1st storyline, Superman also considers Batman to be one of the most brilliant minds on the planet.",
                    "Batman has the ability to function under great physical pain and withstand mind control. He is a master of disguise multilingual and an expert in espionage, often gathering information under different identity's.",
                    "Batman's Karate Judo and jujitsu training has made him a math are master of stealth an escape, allowing allowing him to appear and disappear at Will and to break free.",
                };

                // Normalization for translation may be different.
                public static readonly string[] UtterancesTranslation =
                {
                    "Skills and abilities Batman has no inherent super powers. He relies on his own scientific knowledge detective skills and athletic prowess in the stories Batman is regarded as one of the world's greatest detective if not the world's Greatest Crime Solver Batman has been repeatedly described as having genius. Level intellect. One of the greatest martial artists in the DC universe.",
                    "And having peak human physical conditioning use traveled the world acquiring the skills needed to aid his crusade grounds crime.",
                    "His knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe.",
                    "Batman's inexhaustible wealth allows him to access advanced technology as a proficient scientists is able to use and modify those technologies to his advantage.",
                    "Batman describes Superman as the most dangerous man on earth able to defeat a team of super powered extraterrestrials by himself in order to rescue his imprison teammates.",
                    "In the first storyline Superman also considers Batman to be one of the most brilliant minds on the planet.",
                    "Batman has the ability to function under great physical pain and withstand mind control. He is a master of disguise multilingual and an expert in espionage often gathering information under different identity's.",
                    "Batman's karate judo and jujitsu training has made him a math are master of stealth an escape allowing too much allowing him to appear and disappear at will and to break free.",
                };
            }

            public static class Weather
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.wav");
                public static readonly string Utterance = "What's the weather like?";
            }

            public static class WeatherMP3
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.mp3");
                public static readonly string Utterance = "What's the weather like?";
            }

            public static class WeatherOPUS
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.opus");
                public static readonly string Utterance = "What's the weather like?";
            }

            public static class WeatherFLAC
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.flac");
                public static readonly string Utterance = "What's the weather like?";
            }

            public static class WeatherALAW
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.alaw");
                public static readonly string Utterance = "What's the weather like?";
            }

            public static class WeatherMULAW
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.mulaw");
                public static readonly string Utterance = "What's the weather like?";
            }

            public static class TranscriberAudioData
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "RecordedAudioMessages.json");
                public static readonly string TwoSpeakersAudio = Path.Combine(AudioDir, "katiesteve.wav");
                public static readonly string KatieVoice = Path.Combine(AudioDir, "enrollment_audio_katie.wav");
                public static readonly string SteveVoice = Path.Combine(AudioDir, "enrollment_audio_steve.wav");
                public static readonly string Utterance = "Good morning Steve.";
            }

            public static class WeatherMultiTurns
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike-5secsilences.wav");
                public static readonly string[] Utterances =
                {
                    "What's the weather like?",
                    "What's the weather like?",
                    "What's the weather like?"
                };
            }

            public static class Silence
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "silenceshort.wav");
            }

            public static class HomeAutomation
            {
                public static class TurnOn
                {
                    public static readonly string AudioFile = Path.Combine(AudioDir, "TurnOnTheLamp.wav");
                    public static readonly string Utterance = "Turn on the lamp.";
                }
                public static class TurnOff
                {
                    public static readonly string AudioFile = Path.Combine(AudioDir, "ShutDownTheComputer.wav");
                    public static readonly string Utterance = "Shut down the computer.";
                }
            }

            public static class TStockValue
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "TStockValue.wav");
                public static readonly string Utterance = "What's the AT&T stock value today?";
            }

            public static class DgiWreckANiceBeach
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "wreck-a-nice-beach.wav");
                public static readonly string CorrectRecoText = "Wreck a nice beach.";
                public static readonly string DefaultRecoText = "Recognize speech.";
            }

            public static class Profanity
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "WTFProfanity.wav");
                public static readonly string EnglishWord = "fuck";
                public static readonly string MaskedPattern = "****";
                public static readonly string MaskedUtterance = "What does the word **** mean?";
                public static readonly string RemovedUtterance = "What does the word  mean?";
                public static readonly string RawUtterance = "What does the word fuck mean?";
                public static readonly string MaskedUtteranceTranslation = "What does the word **** mean?";
                public static readonly string RemovedUtteranceTranslation = "";
                public static readonly string RawUtteranceTranslation = "What does the word fuck mean?";
                public static readonly string TaggedUtteranceTranslation = "<profanity>fuck</profanity>";
                
            }

            public static class Punctuation
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "explicitpunc1.wav");
                public static readonly string Utterance = "If it rains, sent me an email.";
            }
        }

        public static class French
        {
            public static class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Compétences et aptitudes Batman n'a pas de super pouvoirs inhérents. Il s'appuie sur ses propres connaissances scientifiques des compétences de détective et des prouesses athlétiques."
                };
            }

            public static class FirstOne
            {
                public static readonly string Utterance = "Appelle le premier";
            }

            public static class Weather
            {
                public static readonly string Utterance = "Quel temps fait-il";
            }
        }

        public static class Spanish
        {
            public static class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Habilidades y habilidades Batman no tiene súper poderes inherentes. Se basa en su propio conocimiento científico habilidades de detective y destreza atlética."
                };
            }

            public static class FirstOne
            {
                public static readonly string[] PossibleUtterances =
                {
                    "Llama al primero.",
                    "Llama a la primera."
                };
            }

            public static class Weather
            {
                public static readonly string[] PossibleUtterances =
                {
                    "¿Cómo es el clima?",
                    "¿Cómo es el tiempo?",
                    "¿cómo está el tiempo?"
                };
            }
        }

        public static class Catalan
        {
            public static readonly string AudioFile = Path.Combine(AudioDir, "ca-es.wav");

            public static class HowIsYourWork
            {
                public static readonly string Utterance = @"Com estàs fent amb els elements de treball avui";
            }

            public static class Weather
            {
                public static readonly string Utterance = "Wie ist das Wetter?";
            }

            public static class FirstOne
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "CallTheFirstOne.wav");
                public static readonly string Utterance = "Rufe die 1 an.";
            }
        }

        public static class ExpectedErrorDetails
        {
            public static string InvalidTargetLanaguageErrorMessage = @"Translation call failed: Response status code does not indicate success: 400 (Bad Request).";
            public static string InvalidVoiceNameErrorMessage = @"Synthesis service failed with code:  - Could not identify the voice 'InvalidVoice' for the text to speech service ";
        }

        public static class OfflineUnidec
        {
            public static string LocalModelPathRoot = Path.Combine(AudioDir, "../../../../../external/unidec/Unidec.Model/model");
            public static string LocalModelLanguage = "en-US";
        }
    }
}
