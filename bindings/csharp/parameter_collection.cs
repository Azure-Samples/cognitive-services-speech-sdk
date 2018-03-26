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
    public class ParameterCollection<T>
    {
        internal ParameterCollection(T owner)
        {
            if (typeof(T) == typeof(Carbon.Recognition.RecognizerFactory))
            {
                isFactoryParameter = true;
                speechParameters = null;
            }
            if (typeof(T) == typeof(Carbon.Recognition.Speech.SpeechRecognizer))
            {
                isFactoryParameter = false;
                var speechRecognizer = owner as Carbon.Recognition.Speech.SpeechRecognizer;
                speechParameters = speechRecognizer.recoImpl.Parameters;
            }
            else
            {
                throw new NotImplementedException("ParameterCollection: Unsupported type: " + typeof(T));
            }
        }

        /// <summary>
        /// Checks whether the parameter specified by name has a string value. 
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <returns>true if the parameter has a string value, and false otherwise.</returns>
        public bool IsString(string name)
        {
            return isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.IsString(name) : speechParameters.HasString(name);
        }

        /// <summary>
        /// Returns the value of parameter specified by name as string. If the name is not available, the default value is returned.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="defaultValue">The default value which is returned if the parameter is not available in the collection.</param>
        /// <returns>value of the parameter.</returns>
        public string GetString(string name, string defaultValue = null)
        {
            return isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.GetString(name, defaultValue) : speechParameters.GetString(name, defaultValue);
        }

        /// <summary>
        /// Sets the string value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void SetString(string name, string value)
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
        /// Checks whether the parameter specified by name has a integer value. 
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <returns>true if the parameter has an int value, and false otherwise.</returns>
        public bool IsNumber(string name)
        {
            return isFactoryParameter? Carbon.Internal.RecognizerFactory.Parameters.IsNumber(name) : speechParameters.HasNumber(name);
        }

        /// <summary>
        /// Returns the value of parameter specified by name as integer. If the name is not available, the default value is returned.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="defaultValue">The default value which is returned if the parameter is not available in the collection.</param>
        /// <returns>value of the parameter.</returns>
        public int GetNumber(string name, int defaultValue = 0)
        {
            return isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.GetNumber(name, defaultValue) : speechParameters.GetNumber(name, defaultValue);
        }

        /// <summary>
        /// Sets the integer value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void SetNumber(string name, int value)
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
        /// Checks whether the parameter specified by name has a boolean value. 
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <returns>true if the parameter has an boolean value, and false otherwise.</returns>
        public bool IsBool(string name)
        {
            return isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.IsBool(name) : speechParameters.HasBool(name);
        }

        /// <summary>
        /// Returns the value of parameter specified by name as boolean. If the name is not available, the default value is returned.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="defaultValue">The default value which is returned if the parameter is not available in the collection.</param>
        /// <returns>value of the parameter.</returns>
        public bool GetBool(string name, bool defaultValue = false)
        {
            return isFactoryParameter ? Carbon.Internal.RecognizerFactory.Parameters.GetBool(name, defaultValue) : speechParameters.GetBool(name, defaultValue);
        }

        /// <summary>
        /// Sets the boolean value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void SetBool(string name, bool value)
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

        /// <summary>
        /// Indexer operator to get and set parameter value.
        /// </summary>
        /// <param name="name"></param>
        /// <returns></returns>
        public Value this[string name]
        {
            get
            {
                var count = 0;
                count += IsString(name) ? 1 : 0;
                count += IsBool(name) ? 1 : 0;
                count += IsNumber(name) ? 1 : 0;
                if (count == 0)
                {
                    throw new KeyNotFoundException("Name: " + name + "not found in the collection.");
                }
                else if (count > 1)
                {
                    throw new InvalidOperationException("Name " + name + "duplicated in the collection.");
                }
                
                return IsString(name) ? new Value(GetString(name)) : IsNumber(name) ? new Value(GetNumber(name)) : new Value(GetBool(name));
            }

            set
            {
                if (value.ValueType == typeof(int))
                {
                    SetNumber(name, value.AsInt());
                }
                else if (value.ValueType == typeof(string))
                {
                    SetString(name, value.AsString());
                }
                else if (value.ValueType == typeof(bool))
                {
                    SetBool(name, value.AsBoolean());
                }

            }
        }

        private bool isFactoryParameter = false;
        private Carbon.Internal.RecognizerParameterCollection speechParameters;
    }
}
