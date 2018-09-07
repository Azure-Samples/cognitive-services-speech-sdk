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
    /// Interface to retrieve a parameter value from factory parameter collection of speech factory.
    /// </summary>
    public interface IFactoryParameters
    {
        /// <summary>
        /// Returns value of the parameter in string />. 
        /// If the parameter value is not defined, a default value is returned: an empty string is returned,
        /// </summary>
        /// <param name="parameterKind">The kind of parameter. see <see cref="FactoryParameterKind"/></param>
        /// <returns>value of the parameter.</returns>
        string Get(FactoryParameterKind parameterKind);

        /// <summary>
        /// Returns value of the parameter in string/>. 
        /// If the parameter value is not defined, a default value is returned: an empty string is returned
        /// </summary>
        /// <param name="parameterName">The name of parameter.</param>
        /// <returns>value of the parameter.</returns>
        string Get(string parameterName);

        /// <summary>
        /// Returns value of the parameter in string/>. 
        /// If the parameter value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <param name="parameterKind">The kind of parameter. see <see cref="FactoryParameterKind"/></param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the parameter.</param>
        /// <returns>value of the parameter.</returns>
        string Get(FactoryParameterKind parameterKind, string defaultValue);

        /// <summary>
        /// Returns value of the parameter in string/>. 
        /// If the parameter value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <param name="parameterName">The name of parameter.</param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the parameter.</param>
        /// <returns>value of the parameter.</returns>
        string Get(string parameterName, string defaultValue);

        /// <summary>
        /// Sets the string value of the parameter specified by name.
        /// </summary>
        /// <param name="parameterKind">The kind of parameter. see <see cref="FactoryParameterKind"/></param>
        /// <param name="value">The value of the parameter.</param>
        void Set(FactoryParameterKind parameterKind, string value);

        /// <summary>
        /// Sets the string value of the parameter specified by name.
        /// </summary>
        /// <param name="parameterName">The name of parameter</param>
        /// <param name="value">The value of the parameter.</param>
        void Set(string parameterName, string value);
    }
}



