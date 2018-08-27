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
                    "Fähigkeiten und Fähigkeiten Batman hat keine inhärenten Supermächte. Er verlässt sich auf seine eigenen wissenschaftlichen Kenntnisse Detektiv Kenntnisse und sportliche Fähigkeiten.",
                    "In den Geschichten Batman gilt als einer der weltweit größten Detektiv, wenn nicht der weltweit größte Krimi-Löser.",
                    "Batman wurde wiederholt als Genie Level Intellekt beschrieben. Einer der größten Kampfkünstler im DC-Universum.",
                    "Und mit Peak menschlichen körperlichen Konditionierung reiste die Welt Erwerb der Fähigkeiten benötigt, um seine Kreuzzug Gründen Verbrechen zu unterstützen.",
                    "Sein Wissen und Know-how in fast jeder Disziplin, die dem Menschen bekannt ist, ist fast beispiellos von jedem anderen Charakter im Universum.",
                    "Bat man es unerschöpflichen Reichtum ermöglicht es ihm, fortschrittliche Technologie als kompetente Wissenschaftler nutzen in der Lage zu nutzen und zu modifizieren, diese Technologien zu seinem Vorteil zugreifen.",
                    "Batman beschreibt Superman als den gefährlichsten Mann auf Erden in der Lage, ein Team von Super powered Außerirdischen zu besiegen.",
                    "Von sich selbst, um seine inhaftierten Mitspieler zu retten.",
                    "In der ersten Storyline Superman hält Batman auch als einer der brillantesten Köpfe auf dem Planeten.",
                    "Batman hat die Fähigkeit, unter großen körperlichen Schmerz zu funktionieren und Geist Kontrolle zu widerstehen. Er ist ein Meister der Verkleidung.",
                    "Mehrsprachig und ein Experte in der Spionage oft sammeln Informationen unter verschiedenen Identität.",
                    "FAT man es Karate Judo und Jujitsu Training hat ihn nach dem Master of Stealth eine Flucht ermöglicht Tumor ermöglicht es ihm zu erscheinen und verschwinden nach bewillen gemacht.",
                    "Und frei zu brechen."
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
