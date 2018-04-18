//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Generic;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Represents collection of parameters and their values.
    /// </summary>
    public sealed class ParameterCollection<Owner> : IDisposable
    {
        internal ParameterCollection(Owner owner)
        {
            if (typeof(Owner) == typeof(Recognition.RecognizerFactory))
            {
                isFactoryParameter = true;
                recognizerParameters = null;
            }
            else if (typeof(Owner) == typeof(Recognition.Speech.SpeechRecognizer))
            {
                isFactoryParameter = false;
                var recognizer = owner as Recognition.Speech.SpeechRecognizer;
                recognizerParameters = recognizer.recoImpl.Parameters;
            }
            else if (typeof(Owner) == typeof(Recognition.Translation.TranslationRecognizer))
            {
                isFactoryParameter = false;
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
            if (typeof(T) == typeof(string))
            {
                return isFactoryParameter ? Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.ContainsString(name) : recognizerParameters.ContainsString(name);
            }
            else if (typeof(T) == typeof(int))
            {
                return isFactoryParameter ? Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.ContainsNumber(name) : recognizerParameters.ContainsNumber(name);
            }
            else if (typeof(T) == typeof(bool))
            {
                return isFactoryParameter ? Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.ContainsBool(name) : recognizerParameters.ContainsBool(name);
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
                var ret = isFactoryParameter ? Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.GetString(name, defaultInT) : recognizerParameters.GetString(name, defaultInT);
                return (T)Convert.ChangeType(ret, typeof(T));
            }
            else if (typeof(T) == typeof(int))
            {
                var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int));
                var ret = isFactoryParameter ? Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.GetNumber(name, defaultInT) : recognizerParameters.GetNumber(name, defaultInT);
                return (T)Convert.ChangeType(ret, typeof(T));
            }
            else if (typeof(T) == typeof(bool))
            {
                var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool));
                var ret = isFactoryParameter ? Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.GetBool(name, defaultInT) : recognizerParameters.GetBool(name, defaultInT);
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
                Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.SetString(name, value);
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
            if (isFactoryParameter)
            {
                Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.SetNumber(name, value);
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
            if (isFactoryParameter)
            {
                Microsoft.CognitiveServices.Speech.Internal.DefaultRecognizerFactory.Parameters.SetBool(name, value);
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
            disposed = true;
        }

        private bool isFactoryParameter = false;
        private bool disposed = false;
        private Microsoft.CognitiveServices.Speech.Internal.RecognizerParameterValueCollection recognizerParameters;
    }
}
