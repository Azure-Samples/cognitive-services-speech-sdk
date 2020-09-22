// <copyright file="TranscriptionFile.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Serializable.TranscriptionFiles
{
    using System;
    using Connector.Enums;

    public class TranscriptionFile
    {
        public TranscriptionFile(string name, string kind, TranscriptionFileLink links)
        {
            Name = name;
            KindString = kind;
            Links = links;
        }

        public string Name { get; set; }

        public TranscriptionFileKind Kind
        {
            get
            {
                if (Enum.TryParse(KindString, out TranscriptionFileKind outputKind))
                {
                    return outputKind;
                }

                return TranscriptionFileKind.Unknown;
            }
        }

        public TranscriptionFileLink Links { get; set; }

        private string KindString { get; set; }
    }
}
