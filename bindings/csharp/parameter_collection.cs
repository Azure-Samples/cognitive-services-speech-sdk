//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;

namespace Carbon
{
    /// <summary>
    /// Represents collection of parameters and their values.
    /// </summary>
    public class ParameterCollection<OwnerType>
    {
        internal ParameterCollection(OwnerType owner)
        {
            if (typeof(OwnerType) == typeof(Carbon.Recognition.RecognizerFactory))
            {
                isFactoryParameter = true;
                speechParameters = null;
            }
            if (typeof(OwnerType) == typeof(Carbon.Recognition.Speech.SpeechRecognizer))
            {
                isFactoryParameter = false;
                var speechRecognizer = owner as Carbon.Recognition.Speech.SpeechRecognizer;
                speechParameters = speechRecognizer.recoImpl.Parameters;
            }
            else
            {
                throw new NotImplementedException("ParameterCollection: Unsupported type: " + typeof(OwnerType));
            }
        }

        /// <summary>
        /// Checks whether the parameter specified by name has a string value. 
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <returns>true if the parameter has a string value, and false otherwise.</returns>
        public bool Is<T>(string name)
        {
            if (typeof(T) == typeof(string))
            {
                return isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.IsString(name) : speechParameters.IsString(name);
            }
            else if (typeof(T) == typeof(int))
            {
                return isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.IsNumber(name) : speechParameters.IsNumber(name);
            }
            else if (typeof(T) == typeof(bool))
            {
                return isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.IsBool(name) : speechParameters.IsBool(name);
            }
            else
            {
                throw new NotImplementedException("ParameterCollection: Unsupported value type: " + typeof(T));
            }
        }

        /// <summary>
        /// Returns the parameter value in type <typeparamref name="T"/>. The parameter must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the name is not available, it returns an empty string if the parameter is a string, 0 if the parameter is int, 
        /// and false if the parameter is bool.
        /// </summary>
        /// <typeparam name="T">The type of parameter. only string, int and bool are supported.</typeparam>
        /// <param name="name">The parameter name.</param>
        /// <param name="defaultValue">The default value which is returned if the parameter is not available in the collection.</param>
        /// <returns>value of the parameter.</returns>
        public T Get<T>(string name)
        {
            T defaultT;
            if (typeof(T) == typeof(string))
            {
                defaultT = (T)Convert.ChangeType(string.Empty, typeof(T));
            }
            else if (typeof(T) == typeof(int))
            {
                defaultT = (T)Convert.ChangeType(0, typeof(T));
            }
            else if (typeof(T) == typeof(bool))
            {
                defaultT = (T)Convert.ChangeType(false, typeof(T));
            }
            else
            {
                throw new NotImplementedException("ParameterCollection: Unsupported value type: " + typeof(T));
            }

            return Get<T>(name, defaultT);
        }

        /// <summary>
        /// Returns the parameter value in type <typeparamref name="T"/>. The parameter must have the same type as <typeparamref name="T"/>.
        /// Currently only string, int and bool are allowed.
        /// If the name is not available, the specified defaultValue is returned.
        /// </summary>
        /// <typeparam name="T">The type of parameter. only string, int and bool are supported.</typeparam>
        /// <param name="name">The parameter name.</param>
        /// <param name="defaultValue">The default value which is returned if the parameter is not available in the collection.</param>
        /// <returns>value of the parameter.</returns>
        public T Get<T>(string name, T defaultValue)
        {
            if (typeof(T) == typeof(string))
            {
                var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string));
                var ret = isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.GetString(name, defaultInT) : speechParameters.GetString(name, defaultInT);
                return (T)Convert.ChangeType(ret, typeof(T));
            }
            else if (typeof(T) == typeof(int))
            {
                var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int));
                var ret = isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.GetNumber(name, defaultInT) : speechParameters.GetNumber(name, defaultInT);
                return (T)Convert.ChangeType(ret, typeof(T));
            }
            else if (typeof(T) == typeof(bool))
            {
                var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool));
                var ret = isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.GetBool(name, defaultInT) : speechParameters.GetBool(name, defaultInT);
                return (T)Convert.ChangeType(ret, typeof(T));
            }
            else
            {
                throw new NotImplementedException("ParameterCollection: Unsupported value type: " + typeof(T));
            }
        }

        /// <summary>
        /// Sets the string value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void Set(string name, string value)
        {
            if (isFactoryParameter)
            {
                Carbon.Internal.RecognizerFactory.Parameters.SetString(name, value);
            }
            else
            {
                speechParameters.SetString(name, value);
            }
        }

        /// <summary>
        /// Sets the integer value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void Set(string name, int value)
        {
            if (isFactoryParameter)
            {
                Carbon.Internal.RecognizerFactory.Parameters.SetNumber(name, value);
            }
            else
            {
                speechParameters.SetNumber(name, value);
            }
        }

        /// <summary>
        /// Sets the boolean value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void Set(string name, bool value)
        {
            if (isFactoryParameter)
            {
                Carbon.Internal.RecognizerFactory.Parameters.SetBool(name, value);
            }
            else
            {
                speechParameters.SetBool(name, value);
            }
        }

        private bool isFactoryParameter = false;
        private Carbon.Internal.RecognizerParameterCollection speechParameters;
    }
}
