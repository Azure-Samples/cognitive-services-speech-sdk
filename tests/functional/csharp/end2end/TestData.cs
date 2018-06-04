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
                    "In den Geschichten Batman gilt als einer der weltweit größten Detektiv, wenn nicht die weltweit größten Verbrechen.",
                    "Sind.",
                    "Batman wurde wiederholt als Genie Level Intellekt beschrieben. Einer der größten Kampfkünstler im DCS-Universum.",
                    "Und mit Peak menschlichen körperlichen Konditionierung ist die Welt bereist Erwerb der Fähigkeiten benötigt, um seine Kreuzfahrer gewährt Verbrechen Hilfe.",
                    "Sein Wissen und Know-how in fast jeder Disziplin, die dem Menschen bekannt ist, ist fast beispiellos von jedem anderen Charakter im Universum.",
                    "Der unerschöpfliche Reichtum des schlechten Mannes erlaubt ihm, vorgerückte Technologie als tüchtige Wissenschaftler zuzugreifen, die fähig sind, diese Technologien zu seinem Vorteil zu verwenden und zu ändern.",
                    "Batman beschreibt Superman als den gefährlichsten Mann auf Erden in der Lage, ein Team von Super powered Außerirdischen zu besiegen.",
                    "Von sich selbst, um seine inhaftierten Teamkameraden zu retten.",
                    "In der ersten Storyline Superman hält Batman auch als einer der brillantesten Köpfe auf dem Planeten.",
                    "Batman hat die Fähigkeit, unter großen körperlichen Schmerzen Funktion kann mit Stand-Mind-Control. Er ist ein Meister der Verkleidung.",
                    "Mehrsprachig und ein Experte in der Spionage oft sammeln Informationen unter verschiedenen Identität.",
                    "FAT Mans Karate Judo und Jujitsu Ausbildung hat ihn zu einem Mathe sind Meister der selbst-und Flucht allein zu viel.",
                    "Erlaubt ihm zu erscheinen und verschwinden nach bewillen und zu brechen frei."
                };
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
                public static readonly string[] Utterances =
                {
                    "Skills and abilities Batman has no inherent super powers. He relies on his own scientific knowledge detective skills and athletic prowess.",
                    "In the stories batman is regarded as one of the world's greatest detective if not the world's greatest crimes.",
                    "Are.",
                    "Batman has been repeatedly described as having genius level intellect. One of the greatest martial artists in the DCS universe.",
                    "And having peak human physical conditioning is traveled the world acquiring the skills needed to aid his crusader grants crime.",
                    "His knowledge and expertise in almost every discipline known to man is nearly unparalleled by any other character in the universe.",
                    "Bad man's inexhaustible wealth allows him to access advanced technology as a proficient scientists use able to use and modify those technologies to his advantage.",
                    "Batman describes superman as the most dangerous man on earth able to defeat a team of super powered extraterrestrials.",
                    "By himself in order to rescue his imprison teammates.",
                    "In the first storyline superman also considers batman to be one of the most brilliant minds on the planet.",
                    "Batman has the ability to function under great physical pain can with stand mind control. He is a master of disguise.",
                    "Multilingual and an expert in espionage often gathering information under different identity's.",
                    "Fat Mans karate judo and jujitsu training has made him a math are master of self and escape alone too much.",
                    "Allowing him to appear and disappear at will and to break free.",
                };
            }

            public sealed class Weather
            {
                public static readonly string AudioFile = Path.Combine(AudioDir, "whatstheweatherlike.wav");
                public static readonly string Utterance = "What's the weather like?";
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
        }
    }
}