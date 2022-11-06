// <copyright file="TranscriptionDefinition.cs" company="Microsoft Corporation">
// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
// </copyright>

namespace Connector
{
    using System.Collections.Generic;
    using System.Linq;

    public sealed class TranscriptionDefinition
    {
        private TranscriptionDefinition(
            string name,
            string description,
            string locale,
            IEnumerable<string> contentUrls,
            Dictionary<string, string> properties,
            ModelIdentity model)
        {
            this.DisplayName = name;
            this.Description = description;
            this.ContentUrls = contentUrls;
            this.Locale = locale;
            this.Model = model;
            this.Properties = properties;
        }

        public string DisplayName { get; set; }

        public string Description { get; set; }

        public IEnumerable<string> ContentUrls { get; }

        public string Locale { get; set; }

        public ModelIdentity Model { get; set; }

        public IDictionary<string, string> Properties { get; }

        public static TranscriptionDefinition Create(
            string name,
            string description,
            string locale,
            string contentUrl,
            Dictionary<string, string> properties,
            ModelIdentity model)
        {
            return new TranscriptionDefinition(name, description, locale, new[] { contentUrl }.ToList(), properties, model);
        }

        public static TranscriptionDefinition Create(
            string name,
            string description,
            string locale,
            IEnumerable<string> contentUrls,
            Dictionary<string, string> properties,
            ModelIdentity model)
        {
            return new TranscriptionDefinition(name, description, locale, contentUrls, properties, model);
        }
    }
}
