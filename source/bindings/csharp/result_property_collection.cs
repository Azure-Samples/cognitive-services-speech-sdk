//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Interface to retrieve a property value from property collection of recognition result.
    /// </summary>
    public interface IResultProperties
    {
        /// <summary>
        /// Returns value of the property in string/>. 
        /// If the property value is not defined, an empty string is returned
        /// </summary>
        /// <param name="propertyKind">The kind of property. see <see cref="ResultPropertyKind"/></param>
        /// <returns>value of the property.</returns>
        string Get(ResultPropertyKind propertyKind);

        /// <summary>
        /// Returns value of the property in string/>. 
        /// If the property value is not defined, an empty string is returned,
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <returns>value of the property.</returns>
        string Get(string propertyName);

        /// <summary>
        /// Returns value of the property in string/>. 
        /// If the property value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <param name="propertyKind">The kind of property. see <see cref="ResultPropertyKind"/></param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        string Get(ResultPropertyKind propertyKind, string defaultValue);

        /// <summary>
        /// Returns value of the property in string/>.
        /// If the property value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        string Get(string propertyName, string defaultValue);
    }
}
