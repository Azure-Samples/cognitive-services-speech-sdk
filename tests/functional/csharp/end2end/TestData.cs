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

        public static class German
        {
            public static class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Fähigkeiten und Fähigkeiten Batman hat keine inhärenten Superkräfte. Er setzt auf seine eigenen wissenschaftlichen Kenntnisse Detektiv-Fähigkeiten und sportliche Geschicklichkeit in den Geschichten Batman gilt als einer der größten Detektiv der Welt, wenn nicht der größte Verbrecher Löser der Welt wurde immer wieder als Genialität beschrieben. Ebene.",
                    "Einer der größten Kampfkünstler im DC-Universum und mit höchster menschlicher körperlicher Konditionierung. Er hat die Welt bereist, um die Fähigkeiten zu erwerben, die nötig sind, um seinem Kreuzzug zu helfen. sein Wissen und seine Expertise in fast jeder Disziplin, die dem Menschen bekannt ist, ist von keinem anderen Charakter im Universum, den der unerschöpfliche Reichtum des Menschen zulässt Ihm den Zugang zu fortschrittlicher Technologie, wie ein kompetenter Wissenschaftler in der Lage, zu verwenden und zu modifizieren.",
                    "Es ist zu seinem Vorteil Batman beschreibt Superman als den gefährlichsten Mann auf der Erde in der Lage, ein Team von Super angetriebenen Außerirdischen von sich selbst zu besiegen, um seine inhaftierten Teamkollegen zu retten.",
                    "In der ersten Storyline hält Superman auch Batman für einen der brillantesten Köpfe auf dem Planeten.",
                    "Batman hat die Fähigkeit, unter großen körperlichen Schmerzen zu funktionieren und der Gedankenkontrolle zu widerstehen. Er ist ein Meister der Verkleidung mehrsprachig und ein Experte für Spionage oft sammeln Informationen unter verschiedenen Identität Fledermausmann Karate Judo und Jujitsu Training hat ihn nach Meister der Verstohlenheit eine Flucht, die es ihm erlaubt, zu erscheinen. Und verschwinden.",
                    "Und sich zu befreien.",
                };
            }

            public static class HowIsYourWork
            {
                public const string Utterance = "Wie geht es Ihnen heute mit den arbeitsgegenständen";
            }

            public static class Weather
            {
                public static readonly string Utterance = "Wie ist das Wetter?";
            }

            public static class FirstOne
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "CallTheFirstOne.wav");
                public static readonly string Utterance = "Rufe die erste an.";
            }
        }

        public static class Chinese
        {
            public static class Weather
            {
                public static readonly string Utterance = "天气怎么样？";
            }
        }

        public static class English
        {
            public static class Margarita
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "Margarita-44100.wav");
                public static readonly string Utterance =
                    "Second now and Love, a margarita and if you're anything like me, you probably had your fair share and if you have. You've probably noticed that margaritas? Can vary from incredible too. So I'm going to show you? How to make one of those incredibly fresh and Delicious Margaritas. There are 3 key steps you need to follow.";
            }

            public static class Batman
            {
                public static readonly string AudioFile = Path.Combine(AudioDir,"batman.wav");
                // This is expected results using StartContinuousRecognitionAsync() which uses converstation mode.
                // The result using RecognizeOnceAsync() might be slightly different, since it uses interactive mode.
                public static readonly string[] Utterances =
                {
                    "Skills and abilities Batman has no inherent super powers, he relies on his own scientific knowledge detective skills and athletic prowess in the stories. Batman is regarded as one of the world's greatest detective if not the world's greatest crime solver. Batman has been repeatedly described as having genius level.",
                    "One of the greatest martial artists in the DC universe and having peak human physical conditioning. He has traveled the world, acquiring the skills needed to aid his crusade grounds crime, his knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe. That man's inexhaustible wealth allows him to access advanced technology as a proficient scientists use able to use and modify.",
                    "It's to his advantage Batman describes Superman as the most dangerous man on Earth able to defeat a team of super powered extraterrestrials by himself in order to rescue his imprisoned teammates.",
                    "In the first storyline, Superman also considers Batman to be one of the most brilliant minds on the planet.",
                    "Batman has the ability to function under great physical pain and withstand mind control. He is a master of disguise multilingual and an expert in espionage, often gathering information under different identity's bat man's Karate Judo and jujitsu. Training has made him after Master of stealth an escape, allowing tumor allowing him to appear and disappear at.",
                    "And to break free.",
                };
            }

            public static class Weather
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.wav");
                public static readonly string Utterance = "What's the weather like?";
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
                public static readonly string Utterance = "Appelle le premier.";
            }

            public static class Weather
            {
                public static readonly string Utterance = "Quel temps fait-il?";
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
                public static readonly string Utterance = "Llama al primero.";
            }

            public static class Weather
            {
                public static readonly string Utterance = "¿Cómo es el clima?";
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
                public static readonly string Utterance = "Rufe die erste an.";
            }
        }
    }
}
