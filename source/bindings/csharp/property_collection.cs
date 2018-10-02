//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Class to retrieve or set a property value from a property collection.
    /// </summary>
    public class PropertyCollection
    {
        private Internal.PropertyCollection impl;

        internal PropertyCollection(Internal.PropertyCollection collection)
        {
            impl = collection;
        }

        /// <summary>
        /// Returns value of a property.
        /// If the property value is not defined, an empty string is returned.
        /// </summary>
        /// <param name="id">The ID of property. See <see cref="PropertyId"/></param>
        /// <returns>value of the property</returns>
        public string GetProperty(PropertyId id)
        {
            return GetProperty(id, string.Empty);
        }

        /// <summary>
        /// Returns value of a property.
        /// If the property value is not defined, an empty string is returned.
        /// </summary>
        /// <param name="propertyName">The name of property</param>
        /// <returns>value of the property</returns>
        public string GetProperty(string propertyName)
        {
            return GetProperty(propertyName, string.Empty);
        }

        /// <summary>
        /// Returns value of a property.
        /// If the property value is not defined, the specified default value is returned.
        /// </summary>
        /// <param name="id">The id of property. See <see cref="PropertyId"/></param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        public string GetProperty(PropertyId id, string defaultValue)
        {
            return impl.GetProperty((Internal.PropertyId)id, defaultValue);
        }

        /// <summary>
        /// Returns value of a property.
        /// If the property value is not defined, the specified default value is returned.
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        public string GetProperty(string propertyName, string defaultValue)
        {
            return impl.GetProperty(propertyName, defaultValue);
        }

        /// <summary>
        /// Set value of a property.
        /// </summary>
        /// <param name="id">The id of property. See <see cref="PropertyId"/></param>
        /// <param name="value">value to set</param>
        public void SetProperty(PropertyId id, string value)
        {
            impl.SetProperty((Internal.PropertyId)id, value);
        }

        /// <summary>
        /// Set value of a property.
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <param name="value">value to set</param>
        public void SetProperty(string propertyName, string value)
        {
            impl.SetProperty(propertyName, value);
        }
    }
}
