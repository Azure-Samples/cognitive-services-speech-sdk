//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Diagnostics;
using System.Globalization;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Defines result of speech recognition.
    /// </summary>
    public class SpeechRecognitionResult
    {
        internal SpeechRecognitionResult(Internal.RecognitionResult result)
        {
            Trace.Assert((int)ResultPropertyKind.Json == (int)Internal.ResultProperty.Json);
            Trace.Assert((int)ResultPropertyKind.LanguageUnderstandingJson == (int)Internal.ResultProperty.LanguageUnderstandingJson);
            Trace.Assert((int)ResultPropertyKind.ErrorDetails == (int)Internal.ResultProperty.ErrorDetails);

            Trace.Assert((int)RecognitionStatus.Recognized == (int)Internal.Reason.Recognized);
            Trace.Assert((int)RecognitionStatus.IntermediateResult == (int)Internal.Reason.IntermediateResult);
            Trace.Assert((int)RecognitionStatus.NoMatch == (int)Internal.Reason.NoMatch);
            Trace.Assert((int)RecognitionStatus.Canceled == (int)Internal.Reason.Canceled);
            Trace.Assert((int)RecognitionStatus.OtherRecognizer == (int)Internal.Reason.OtherRecognizer);
            
            resultImpl = result;
            this.ResultId = result.ResultId;
            this.Text = result.Text;
            this.RecognitionStatus = (RecognitionStatus)((int)result.Reason);
            Properties = new ResultPropertiesImpl(result.Properties);
        }

        /// <summary>
        /// Specifies the result identifier.
        /// </summary>
        public string ResultId { get; }

        /// <summary>
        /// Specifies status of speech recognition result.
        /// </summary>
        public RecognitionStatus RecognitionStatus { get; }

        /// <summary>
        /// Presents the recognized text in the result.
        /// </summary>
        public string Text { get; }

        /// <summary>
        /// Duration of the recognized speech.
        /// </summary>
        public TimeSpan Duration => TimeSpan.FromTicks((long)this.resultImpl.Duration());

        /// <summary>
        /// Offset of the recognized speech in ticks.
        /// </summary>
        public DateTimeOffset Offset => new DateTimeOffset((long)this.resultImpl.Offset(), TimeSpan.Zero);

        /// <summary>
        /// In case of an unsuccessful recognition, provides a brief description of an occurred error.
        /// This field is only filled-out if the recognition status (<see cref="RecognitionStatus"/>) is set to Canceled.
        /// </summary>
        public string RecognitionFailureReason { get { return Properties.Get<string>(ResultPropertyKind.ErrorDetails); } }

        /// <summary>
        /// Contains properties of the results.
        /// </summary>
        public IResultProperties Properties;

        /// <summary>
        /// Returns a string that represents the speech recognition result.
        /// </summary>
        /// <returns>A string that represents the speech recognition result.</returns>
        public override string ToString()
        {
            return string.Format(CultureInfo.InvariantCulture,"ResultId:{0} Status:{1} Recognized text:<{2}>. Json:{3}", 
                ResultId, RecognitionStatus, Text, Properties.Get<string>(ResultPropertyKind.Json));
        }

        // Hold the reference.
        private Internal.RecognitionResult resultImpl;

        private class ResultPropertiesImpl : IResultProperties
        {
            private Internal.ResultPropertyValueCollection resultPropertyImpl;

            public ResultPropertiesImpl(Internal.ResultPropertyValueCollection internalProperty)
            {
                resultPropertyImpl = internalProperty; 
            }

            public bool Is<T>(ResultPropertyKind propertyKind)
            {
                if (typeof(T) == typeof(string))
                {
                    return resultPropertyImpl.Get((Internal.ResultProperty)propertyKind).IsString();
                }
                else if (typeof(T) == typeof(int))
                {
                    return resultPropertyImpl.Get((Internal.ResultProperty)propertyKind).IsNumber();
                }
                else if (typeof(T) == typeof(bool))
                {
                    return resultPropertyImpl.Get((Internal.ResultProperty)propertyKind).IsBool();
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public bool Is<T>(string propertyName)
            {
                if (typeof(T) == typeof(string))
                {
                    return resultPropertyImpl.Get(propertyName).IsString();
                }
                else if (typeof(T) == typeof(int))
                {
                    return resultPropertyImpl.Get(propertyName).IsNumber();
                }
                else if (typeof(T) == typeof(bool))
                {
                    return resultPropertyImpl.Get(propertyName).IsBool();
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public T Get<T>(ResultPropertyKind propertyKind)
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
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }

                return Get<T>(propertyKind, defaultT);
            }

            public T Get<T>(string propertyName)
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
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }

                return Get<T>(propertyName, defaultT);
            }

            public T Get<T>(ResultPropertyKind propertyKind, T defaultValue)
            {
                if (typeof(T) == typeof(string))
                {
                    var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string), CultureInfo.InvariantCulture);
                    var ret = resultPropertyImpl.Get((Internal.ResultProperty)propertyKind).GetString(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int), CultureInfo.InvariantCulture);
                    var ret = resultPropertyImpl.Get((Internal.ResultProperty)propertyKind).GetNumber(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool), CultureInfo.InvariantCulture);
                    var ret = resultPropertyImpl.Get((Internal.ResultProperty)propertyKind).GetBool(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

            public T Get<T>(string propertyName, T defaultValue)
            {
                if (typeof(T) == typeof(string))
                {
                    var defaultInT = (string)Convert.ChangeType(defaultValue, typeof(string), CultureInfo.InvariantCulture);
                    var ret = resultPropertyImpl.Get(propertyName).GetString(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(int))
                {
                    var defaultInT = (int)Convert.ChangeType(defaultValue, typeof(int), CultureInfo.InvariantCulture);
                    var ret = resultPropertyImpl.Get(propertyName).GetNumber(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else if (typeof(T) == typeof(bool))
                {
                    var defaultInT = (bool)Convert.ChangeType(defaultValue, typeof(bool), CultureInfo.InvariantCulture);
                    var ret = resultPropertyImpl.Get(propertyName).GetBool(defaultInT);
                    return (T)Convert.ChangeType(ret, typeof(T), CultureInfo.InvariantCulture);
                }
                else
                {
                    throw new NotImplementedException("Property type: Unsupported value type: " + typeof(T));
                }
            }

        }
    }
}
