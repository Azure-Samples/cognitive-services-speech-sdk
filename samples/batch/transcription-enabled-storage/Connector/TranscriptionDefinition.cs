// <copyright file="TranscriptionDefinition.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System;
    using System.Collections.Generic;
    using System.Linq;

    public sealed class TranscriptionDefinition
    {
        private TranscriptionDefinition(
            string name,
            string description,
            string locale,
            List<Uri> contentUrls,
            Dictionary<string, string> properties,
            IEnumerable<ModelIdentity> models)
        {
            DisplayName = name;
            Description = description;
            ContentUrls = contentUrls;
            Locale = locale;
            Models = models;
            Properties = properties;
        }

        public string DisplayName { get; set; }

        public string Description { get; set; }

        public List<Uri> ContentUrls { get; }

        public string Locale { get; set; }

        public IEnumerable<ModelIdentity> Models { get; set; }

        public IDictionary<string, string> Properties { get; }

        public static TranscriptionDefinition Create(
            string name,
            string description,
            string locale,
            Uri contentUrl,
            Dictionary<string, string> properties,
            IEnumerable<ModelIdentity> models)
        {
            return new TranscriptionDefinition(name, description, locale, new[] { contentUrl }.ToList(), properties, models);
        }

        public static TranscriptionDefinition Create(
            string name,
            string description,
            string locale,
            List<Uri> contentUrls,
            Dictionary<string, string> properties,
            IEnumerable<ModelIdentity> models)
        {
            return new TranscriptionDefinition(name, description, locale, contentUrls, properties, models);
        }
    }
}
