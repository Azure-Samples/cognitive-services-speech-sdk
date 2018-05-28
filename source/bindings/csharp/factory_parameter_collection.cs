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
        /// Checks whether the parameter has a <typeparamref name="T"/> value.
        /// </summary>
        /// <param name="parameterKind">The kind of parameter. see <see cref="FactoryParameterKind"/></param>
        /// <returns>true if the parameter has a <typeparamref name="T"/> value, and false otherwise.</returns>
        bool Is<T>(FactoryParameterKind parameterKind);

        /// <summary>
        /// Checks whether the parameter has a <typeparamref name="T"/> value.
        /// </summary>
        /// <param name="parameterName">The name of parameter.</param>
        /// <returns>true if the parameter has a <typeparamref name="T"/> value, and false otherwise.</returns>
        bool Is<T>(string parameterName);

        /// <summary>
        /// Returns value of the parameter in type <typeparamref name="T"/>. 
        /// The parameter must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the parameter value is not defined, a default value is returned: an empty string if <typeparamref name="T"/> is string,
        /// 0 if <typeparamref name="T"/> is int, and false if <typeparamref name="T"/> is bool.
        /// </summary>
        /// <typeparam name="T">The type of parameter. only string, int and bool are supported.</typeparam>
        /// <param name="parameterKind">The kind of parameter. see <see cref="FactoryParameterKind"/></param>
        /// <returns>value of the parameter.</returns>
        T Get<T>(FactoryParameterKind parameterKind);

        /// <summary>
        /// Returns value of the parameter in type <typeparamref name="T"/>. 
        /// The parameter must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the parameter value is not defined, a default value is returned: an empty string if <typeparamref name="T"/> is string,
        /// 0 if <typeparamref name="T"/> is int, and false if <typeparamref name="T"/> is bool.
        /// </summary>
        /// <typeparam name="T">The type of parameter. only string, int and bool are supported.</typeparam>
        /// <param name="parameterName">The name of parameter.</param>
        /// <returns>value of the parameter.</returns>
        T Get<T>(string parameterName);

        /// <summary>
        /// Returns value of the parameter in type <typeparamref name="T"/>. 
        /// The parameter must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the parameter value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <typeparam name="T">The type of parameter. only string, int and bool are supported.</typeparam>
        /// <param name="parameterKind">The kind of parameter. see <see cref="FactoryParameterKind"/></param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the parameter.</param>
        /// <returns>value of the parameter.</returns>
        T Get<T>(FactoryParameterKind parameterKind, T defaultValue);

        /// <summary>
        /// Returns value of the parameter in type <typeparamref name="T"/>. 
        /// The parameter must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the parameter value is not defined, the specified defaultValue is returned.
        /// </summary>
        /// <typeparam name="T">The type of parameter. only string, int and bool are supported.</typeparam>
        /// <param name="parameterName">The name of parameter.</param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the parameter.</param>
        /// <returns>value of the parameter.</returns>
        T Get<T>(string parameterName, T defaultValue);

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

        /// <summary>
        /// Sets the integer value of the parameter specified by name.
        /// </summary>
        /// <param name="parameterKind">The kind of parameter. see <see cref="FactoryParameterKind"/></param>
        /// <param name="value">The value of the parameter.</param>
        void Set(FactoryParameterKind parameterKind, int value);

        /// <summary>
        /// Sets the integer value of the parameter specified by name.
        /// </summary>
        /// <param name="parameterName">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        void Set(string parameterName, int value);

        /// <summary>
        /// Sets the boolean value of the parameter specified by name.
        /// </summary>
        /// <param name="parameterKind">The kind of parameter. see <see cref="FactoryParameterKind"/></param>
        /// <param name="value">The value of the parameter.</param>
        void Set(FactoryParameterKind parameterKind, bool value);

        /// <summary>
        /// Sets the boolean value of the parameter specified by name.
        /// </summary>
        /// <param name="parameterName">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        void Set(string parameterName, bool value);
    }
}



