// <copyright file="Words.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector.Database.Models
{
    using System;
    using System.ComponentModel.DataAnnotations;

    public class Words : DbModelBase
    {
        [Key]
        public Guid Id { get; set; }

        [StringLength(MaxDefaultStringLength)]
        public string Word { get; private set; }

        [StringLength(MaxTimeSpanColumnLength)]
        public string Offset { get; private set; }

        [StringLength(MaxTimeSpanColumnLength)]
        public string Duration { get; private set; }

        public float Confidence { get; private set; }
    }
}
