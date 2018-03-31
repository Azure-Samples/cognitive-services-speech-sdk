//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;
using System.Threading.Tasks;
using Carbon;

namespace Carbon.Recognition.Speech
{
    /// <summary>
    /// Defines the speech recognizer class containing both methods and events for speech recognition.
    /// </summary>
    public class SpeechRecognizer : Recognition.Recognizer
    {
        /// <summary>
        /// Defines event handler for the event when an intermediate recognition result is recevied.
        /// </summary>
        public event EventHandler<SpeechRecognitionResultEventArgs> OnIntermediateResult;

        /// <summary>
        /// Defines event handler for the event when the final recognition result is recevied.
        /// </summary>
        public event EventHandler<SpeechRecognitionResultEventArgs> OnFinalResult;

        /// <summary>
        /// Defines event handler for the event when an error is occured during recognition.
        /// </summary>
        public event EventHandler<RecognitionErrorEventArgs> OnRecognitionError;

        internal SpeechRecognizer(Internal.SpeechRecognizer recoImpl)
        {
            this.recoImpl = recoImpl;

            intermediateResultHandler = new ResultHandlerImpl(this, isFinalResultHandler: false);
            recoImpl.IntermediateResult.Connect(intermediateResultHandler);

            finalResultHandler = new ResultHandlerImpl(this, isFinalResultHandler: true);
            recoImpl.FinalResult.Connect(finalResultHandler);

            errorHandler = new ErrorHandlerImpl(this);
            recoImpl.NoMatch.Connect(errorHandler);
            recoImpl.Canceled.Connect(errorHandler);

            recoImpl.SessionStarted.Connect(sessionStartedHandler);
            recoImpl.SessionStopped.Connect(sessionStoppedHandler);
            recoImpl.SoundStarted.Connect(soundStartedHandler);
            recoImpl.SoundStopped.Connect(soundStoppedHandler);

            Parameters = new ParameterCollection<SpeechRecognizer>(this);
        }

        ~SpeechRecognizer()
        {
            // BUG: we get exception when doing Disconnect().

            //recoImpl.IntermediateResult.Disconnect(intermediateResultHandler);
            //recoImpl.FinalResult.Disconnect(finalResultHandler);
            //recoImpl.NoMatch.Disconnect(errorHandler);
            //recoImpl.Canceled.Disconnect(errorHandler);
            //recoImpl.SessionStarted.Disconnect(sessionStartedHandler);
            //recoImpl.SessionStopped.Disconnect(sessionStoppedHandler);
            //recoImpl.SoundStarted.Disconnect(soundStartedHandler);
            //recoImpl.SoundStopped.Disconnect(soundStoppedHandler);
        }

        /// <summary>
        /// The property represents the subscription key being used.
        /// </summary>
        public string SubscriptionKey
        {
            get
            {
                //return Parameters.GetString(SubscriptionKey);
                return Parameters.Get<string>(ParameterNames.SpeechSubscriptionKey);
            }
        }

        /// <summary>
        /// The property represents the region being used.
        /// </summary>
        public string Region
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.Region);
            }
        }

        /// <summary>
        /// The property represents the target language for the recognition.
        /// </summary>
        public string Language
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.SpeechRecoLanguage);
            }
        }

        /// <summary>
        /// The property represents the recognition mode.
        /// </summary>
        public string RecognitionMode
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.SpeechRecognitionMode);
            }
        }

        /// <summary>
        /// The property represents the intput file name.
        /// </summary>
        public string InputFile
        {
            get
            {
                return Parameters.Get<string>(ParameterNames.InputFile);
            }
        }

        /// <summary>
        /// The property that represents the collection of parameters and their values.
        /// </summary>
        internal ParameterCollection<SpeechRecognizer> Parameters { get; }

        /// <summary>
        /// Starts speech recognition
        /// </summary>
        /// <returns>A task representing the recognition operation.</returns>
        public Task<SpeechRecognitionResult> RecognizeAsync()
        {
            return Task.Run(() => { return new SpeechRecognitionResult(this.recoImpl.Recognize()); });
        }

        internal Internal.SpeechRecognizer recoImpl;
        private ResultHandlerImpl intermediateResultHandler;
        private ResultHandlerImpl finalResultHandler;
        private ErrorHandlerImpl errorHandler;

        /// <summary>
        /// Defines an internal class to raise a C# event for intermediate/final result when a corresponding callback is invoked by the native layer.
        /// </summary>
        private class ResultHandlerImpl : Internal.SpeechRecognitionEventListener
        {
            public ResultHandlerImpl(SpeechRecognizer recognizer, bool isFinalResultHandler)
            {
                this.recognizer = recognizer;
                this.isFinalResultHandler = isFinalResultHandler;
            }

            public override void Execute(Internal.SpeechRecognitionEventArgs eventArgs)
            {
                var resultEventArg = new SpeechRecognitionResultEventArgs(eventArgs);
                var handler = isFinalResultHandler ? recognizer.OnFinalResult : recognizer.OnIntermediateResult;
                if (handler != null)
                {
                    handler(this.recognizer, resultEventArg);
                }
            }

            private SpeechRecognizer recognizer;
            private bool isFinalResultHandler;
        }

        /// <summary>
        /// Defines an internal class to raise a C# event for error during recognition when a corresponding callback is invoked by the native layer.
        /// </summary>
        private class ErrorHandlerImpl : Internal.SpeechRecognitionEventListener
        {
            public ErrorHandlerImpl(SpeechRecognizer recognizer)
            {
                this.recognizer = recognizer;
            }

            public override void Execute(Carbon.Internal.SpeechRecognitionEventArgs eventArgs)
            {
                var resultEventArg = new RecognitionErrorEventArgs(eventArgs.SessionId, eventArgs.Result.Reason);
                var handler = this.recognizer.OnRecognitionError;

                if (handler != null)
                {
                    handler(this.recognizer, resultEventArg);
                }
            }

            private SpeechRecognizer recognizer;
        }
    }

    
}
