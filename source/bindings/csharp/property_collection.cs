//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Interface to retrieve a property value from a property collection.
    /// </summary>
    public interface IPropertyCollection
    {
        /// <summary>
        /// Returns value of the property in string.
        /// If the property value is not defined, an empty string is returned
        /// </summary>
        /// <param name="id">The id of property. see <see cref="PropertyId"/></param>
        /// <returns>value of the property.</returns>
        string Get(PropertyId id);

        /// <summary>
        /// Returns value of the property in string.
        /// If the property value is not defined, an empty string is returned,
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <returns>value of the property.</returns>
        string Get(string propertyName);

        /// <summary>
        /// Returns value of the property in string.
        /// If the property value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <param name="id">The id of property. see <see cref="PropertyId"/></param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        string Get(PropertyId id, string defaultValue);

        /// <summary>
        /// Returns value of the property in string.
        /// If the property value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        string Get(string propertyName, string defaultValue);
    }
}
