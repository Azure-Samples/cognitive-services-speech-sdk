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
        /// Checks whether the property has a <typeparamref name="T"/> value.
        /// </summary>
        /// <param name="propertyKind">The kind of property. see <see cref="ResultPropertyKind"/></param>
        /// <returns>true if the property has a <typeparamref name="T"/> value, and false otherwise.</returns>
        bool Is<T>(ResultPropertyKind propertyKind);

        /// <summary>
        /// Checks whether the property has a <typeparamref name="T"/> value.
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <returns>true if the property has a <typeparamref name="T"/> value, and false otherwise.</returns>
        bool Is<T>(string propertyName);

        /// <summary>
        /// Returns value of the property in type <typeparamref name="T"/>. 
        /// The property must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the property value is not defined, a default value is returned: an empty string if <typeparamref name="T"/> is string,
        /// 0 if <typeparamref name="T"/> is int, and false if <typeparamref name="T"/> is bool.
        /// </summary>
        /// <typeparam name="T">The type of property. only string, int and bool are supported.</typeparam>
        /// <param name="propertyKind">The kind of property. see <see cref="ResultPropertyKind"/></param>
        /// <returns>value of the property.</returns>
        T Get<T>(ResultPropertyKind propertyKind);

        /// <summary>
        /// Returns value of the property in type <typeparamref name="T"/>. 
        /// The property must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the property value is not defined, a default value is returned: an empty string if <typeparamref name="T"/> is string,
        /// 0 if <typeparamref name="T"/> is int, and false if <typeparamref name="T"/> is bool.
        /// </summary>
        /// <typeparam name="T">The type of property. only string, int and bool are supported.</typeparam>
        /// <param name="propertyName">The name of property.</param>
        /// <returns>value of the property.</returns>
        T Get<T>(string propertyName);

        /// <summary>
        /// Returns value of the property in type <typeparamref name="T"/>. 
        /// The property must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the property value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <typeparam name="T">The type of property. only string, int and bool are supported.</typeparam>
        /// <param name="propertyKind">The kind of property. see <see cref="ResultPropertyKind"/></param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        T Get<T>(ResultPropertyKind propertyKind, T defaultValue);

        /// <summary>
        /// Returns value of the property in type <typeparamref name="T"/>.
        /// The property must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the property value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <typeparam name="T">The type of property. only string, int and bool are supported.</typeparam>
        /// <param name="propertyName">The name of property.</param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        T Get<T>(string propertyName, T defaultValue);
    }
}
