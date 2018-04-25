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
    /// Represents collection of parameters and their values.
    /// </summary>
    public sealed class ParameterCollection<Owner> : IDisposable
    {
        internal ParameterCollection(Owner owner)
        {
            if (typeof(Owner) == typeof(Recognition.SpeechFactory))
            {
                var speechFactory = owner as Recognition.SpeechFactory;
                factoryParameters = speechFactory.factoryImpl.Parameters;
            }
            else if (typeof(Owner) == typeof(Recognition.Speech.SpeechRecognizer))
            {
                var recognizer = owner as Recognition.Speech.SpeechRecognizer;
                recognizerParameters = recognizer.recoImpl.Parameters;
            }
            else if (typeof(Owner) == typeof(Recognition.Intent.IntentRecognizer))
            {
                var recognizer = owner as Recognition.Intent.IntentRecognizer;
                recognizerParameters = recognizer.recoImpl.Parameters;
            }
            else if (typeof(Owner) == typeof(Recognition.Translation.TranslationRecognizer))
            {
                var recognizer = owner as Recognition.Translation.TranslationRecognizer;
                recognizerParameters = recognizer.recoImpl.Parameters;
            }
            else
            {
                throw new NotImplementedException("ParameterCollection: Unsupported type: " + typeof(Owner));
            }
        }

        /// <summary>
        /// Checks whether the parameter specified by name has a <typeparamref name="T"/> value.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <returns>true if the parameter has a <typeparamref name="T"/> value, and false otherwise.</returns>
        public bool Is<T>(string name)
        {
            if (factoryParameters != null)
            {
                if (typeof(T) == typeof(string))
                {
                    return factoryParameters.ContainsString(name);
                }
                else if (typeof(T) == typeof(int))
                {
                    return factoryParameters.ContainsNumber(name);
                }
                else if (typeof(T) == typeof(bool))
                {
                    return factoryParameters.ContainsBool(name);
                }
                else
                {
                    throw new NotImplementedException("ParameterCollection: Unsupported value type: " + typeof(T));
                }
            }
            else
            {
                if (typeof(T) == typeof(string))
                {
                    return recognizerParameters.ContainsString(name);
                }
                else if (typeof(T) == typeof(int))
                {
                    return recognizerParameters.ContainsNumber(name);
                }
                else if (typeof(T) == typeof(bool))
                {
                    return recognizerParameters.ContainsBool(name);
                }
                else
                {
                    throw new NotImplementedException("ParameterCollection: Unsupported value type: " + typeof(T));
                }
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
        /// <returns>value of the parameter.</returns>
        public T Get<T>(string name)
        {
            T defaultT;
            if (typeof(T) == typeof(string))
            {
                defaultT = (T)Convert.ChangeType(string.Empty, typeof(T), CultureInfo.InvariantCulture);
            }
            else if (typeof(T) == typeof(int))
            {
                defaultT = (T)Convert.ChangeType(0, typeof(T), CultureInfo.InvariantCulture);
            }
            else if (typeof(T) == typeof(bool))
            {
                defaultT = (T)Convert.ChangeType(false, typeof(T), CultureInfo.InvariantCulture);
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
            if (factoryParameters != null)
            {
                if (typeof(T) == typeof(string))
                {
                    var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string), CultureInfo.InvariantCulture);
                    var ret = factoryParameters.GetString(name, defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int), CultureInfo.InvariantCulture);
                    var ret = factoryParameters.GetNumber(name, defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool), CultureInfo.InvariantCulture);
                    var ret = factoryParameters.GetBool(name, defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("ParameterCollection: Unsupported value type: " + typeof(T));
                }
            }
            else
            {
                if (typeof(T) == typeof(string))
                {
                    var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string), CultureInfo.InvariantCulture);
                    var ret = recognizerParameters.GetString(name, defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int), CultureInfo.InvariantCulture);
                    var ret = recognizerParameters.GetNumber(name, defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool), CultureInfo.InvariantCulture);
                    var ret = recognizerParameters.GetBool(name, defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("ParameterCollection: Unsupported value type: " + typeof(T));
                }
            }
        }

        /// <summary>
        /// Sets the string value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void Set(string name, string value)
        {
            if (factoryParameters != null)
            {
                factoryParameters.SetString(name, value);
            }
            else
            {
                recognizerParameters.SetString(name, value);
            }
        }

        /// <summary>
        /// Sets the integer value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void Set(string name, int value)
        {
            if (factoryParameters != null)
            {
                factoryParameters.SetNumber(name, value);
            }
            else
            {
                recognizerParameters.SetNumber(name, value);
            }
        }

        /// <summary>
        /// Sets the boolean value of the parameter specified by name.
        /// </summary>
        /// <param name="name">The parameter name.</param>
        /// <param name="value">The value of the parameter.</param>
        public void Set(string name, bool value)
        {
            if (factoryParameters != null)
            {
                factoryParameters.SetBool(name, value);
            }
            else
            {
                recognizerParameters.SetBool(name, value);
            }
        }

        /// <summary>
        /// Dispose of the associated parameter value collection.
        /// </summary>
        public void Dispose()
        {
            if (disposed)
            {
                return;
            }

            recognizerParameters?.Dispose();
            factoryParameters?.Dispose();
            disposed = true;
        }

        private bool disposed = false;
        private Microsoft.CognitiveServices.Speech.Internal.RecognizerParameterValueCollection recognizerParameters;
        private Microsoft.CognitiveServices.Speech.Internal.FactoryParameterCollection factoryParameters;
    }
}
