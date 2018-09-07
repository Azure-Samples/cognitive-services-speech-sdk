//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.IO;

namespace Microsoft.CognitiveServices.Speech.Tests.EndToEnd
{
    static class TestData
    {
        public static string AudioDir { get; set; }

        public sealed class German
        {
            public sealed class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Fähigkeiten und Fähigkeiten Batman hat keine inhärenten Superkräfte. Er setzt auf seine eigenen wissenschaftlichen Kenntnisse Detektivfähigkeiten und athletisches Geschick.",
                    "In den Geschichten gilt Batman als einer der größten Detektiv der Welt, wenn nicht gar der größte Verbrecher Löser der Welt.",
                    "Batman wurde immer wieder als genialer Level-Intellekt beschrieben. Einer der größten Kampfkünstler im DC-Universum.",
                    "Und mit dem Spitzenwert der menschlichen körperlichen Konditionierung reiste er durch die Welt und erwarb die Fähigkeiten, die nötig waren, um seinem Kreuzzug zu helfen.",
                    "Sein Wissen und seine Expertise in fast jeder Disziplin, die dem Menschen bekannt ist, ist durch jeden anderen Charakter im Universum fast beispiellos.",
                    "Bat man es unerschöpflichen Reichtum ermöglicht es ihm, auf fortschrittliche Technologie zuzugreifen, da ein kompetenter Wissenschaftler in der Lage ist, diese Technologien zu seinem Vorteil zu nutzen und zu modifizieren.",
                    "Batman beschreibt Superman als den gefährlichsten Mann der Erde, der in der Lage ist, ein Team von Super betriebenen Außerirdischen zu besiegen.",
                    "Von sich aus, um seine inhaftierten Teamkollegen zu retten.",
                    "In der ersten Storyline hält Superman auch Batman für einen der brillantesten Köpfe auf dem Planeten.",
                    "Batman hat die Fähigkeit, unter großen körperlichen Schmerzen zu funktionieren und der Gedankenkontrolle zu widerstehen. Er ist ein Meister der Verkleidung.",
                    "Mehrsprachig und ein Experte für Spionage sammelt oft Informationen unter verschiedenen Identitäts Gruppen.",
                    "FAT man es Karate Judo und Jujitsu Training hat ihn nach Master of Stealth zu einer Flucht gemacht, die es ihm erlaubt, zu erscheinen und zu verschwinden.",
                    "Nach Willen und zur Pause frei."
                };
            }

            public sealed class Weather
            {
                public static readonly string Utterance = "Wie ist das Wetter?";
            }

            public sealed class FirstOne
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "CallTheFirstOne.wav");
                public static readonly string Utterance = "Rufe die erste an.";
            }
        }

        public sealed class Chinese
        {
            public sealed class Weather
            {
                public static readonly string Utterance = "天气怎么样？";
            }
        }

        public sealed class English
        {
            public sealed class Batman
            {
                public static readonly string AudioFile = Path.Combine(AudioDir,"batman.wav");
                // This is expected results using StartContinuousRecognitionAsync() which uses converstation mode.
                // The result using RecognizeAsync() might be slightly different, since it uses interactive mode.
                public static readonly string[] Utterances =
                {
                    "Skills and abilities Batman has no inherent super powers, he relies on his own scientific knowledge detective skills and athletic prowess.",
                    "In the stories Batman is regarded as one of the world's greatest detective if not the world's greatest crime solver.",
                    "Batman has been repeatedly described as having genius level intellect. One of the greatest martial artists in the DC universe.",
                    "And having peak human physical conditioning his traveled the world, acquiring the skills needed to aid his crusade grounds crime.",
                    "His knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe.",
                    "Bat man's inexhaustible wealth allows him to access advanced technology as a proficient scientists use able to use and modify those technologies to his advantage.",
                    "Batman describes Superman as the most dangerous man on Earth able to defeat a team of super powered extraterrestrials.",
                    "By himself in order to rescue his imprisoned teammates.",
                    "In the first storyline, Superman also considers Batman to be one of the most brilliant minds on the planet.",
                    "Batman has the ability to function under great physical pain and withstand mind control. He is a master of disguise.",
                    "Multilingual and an expert in espionage, often gathering information under different identity's.",
                    "Fat man's Karate Judo and jujitsu training has made him after Master of stealth an escape, allowing tumor, allowing him to appear and disappear at will.",
                    "And to break free."
                };
            }

            public sealed class Weather
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.wav");
                public static readonly string Utterance = "What's the weather like?";
            }

            public sealed class Silence
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "silenceshort.wav");
            }
            public sealed class HomeAutomation
            {
                public sealed class TurnOn
                {
                    public static readonly string AudioFile = Path.Combine(AudioDir, "TurnOnTheLamp.wav");
                    public static readonly string Utterance = "Turn on the lamp.";
                }
                public sealed class TurnOff
                {
                    public static readonly string AudioFile = Path.Combine(AudioDir, "ShutDownTheComputer.wav");
                    public static readonly string Utterance = "Shut down the computer.";
                }
            }
        }

        public sealed class French
        {
            public sealed class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Compétences et aptitudes Batman n'a pas de super pouvoirs inhérents. Il s'appuie sur ses propres connaissances scientifiques des compétences de détective et des prouesses athlétiques."
                };
            }
        
            public sealed class FirstOne
            {
                public static readonly string Utterance = "Appelle le premier.";
            }

            public sealed class Weather
            {
                public static readonly string Utterance = "Quel temps fait-il?";
            }
        }

        public sealed class Spanish
        {
            public sealed class Batman
            {
                public static readonly string[] Utterances =
                {
                    "Habilidades y habilidades Batman no tiene súper poderes inherentes. Se basa en su propio conocimiento científico habilidades de detective y destreza atlética."
                };
            }

            public sealed class FirstOne
            {
                public static readonly string Utterance = "Llama al primero.";
            }

            public sealed class Weather
            {
                public static readonly string Utterance = "¿Cómo es el clima?";
            }
        }
    }
}
