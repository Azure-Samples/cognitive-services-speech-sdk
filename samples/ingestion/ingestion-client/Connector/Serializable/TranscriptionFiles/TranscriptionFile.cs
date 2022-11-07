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
            this.Name = name;
            this.KindString = kind;
            this.Links = links;
        }

        public string Name { get; set; }

        public TranscriptionFileKind Kind
        {
            get
            {
                if (Enum.TryParse(this.KindString, out TranscriptionFileKind outputKind))
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
