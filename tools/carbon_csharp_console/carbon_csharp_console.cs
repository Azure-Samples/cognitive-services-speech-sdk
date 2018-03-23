//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// carbon_csharp_console.cs: A console application for testing Carbon C# client library.
//

using System;
using System.Threading.Tasks;
using Carbon;
using Carbon.Recognition;
using Carbon.Recognition.Speech;

namespace CarbonSamples
{
    class Program
    {
        static void MyIntermediateResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(String.Format("IntermediateResult received. Result: {0} ", e.ToString()));
        }

        static void MyFinalResultEventHandler(object sender, SpeechRecognitionResultEventArgs e)
        {
            Console.WriteLine(String.Format("FinalResult received. Result: {0} ", e.ToString()));
        }

        static void MyErrorEventHandler(object sender, SpeechRecognitionErrorEventArgs e)
        {
            Console.WriteLine(String.Format("Error occured. SessionId: {0}, Reason: {1}", e.SessionId, e.Reason));
        }

        static void MySessionStartedEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format("SessionStarted event. SessionId: {0}.", e.SessionId));
        }
        static void MySessionStoppedEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format("SessionStopped event. SessionId: {0}.", e.SessionId));
        }

        static void MySoundStartedEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format("SoundStarted event. SessionId: {0}.", e.SessionId));
        }
        static void MySoundStoppedEventHandler(object sender, SessionEventArgs e)
        {
            Console.WriteLine(String.Format("SoundStopped event. SessionId: {0}.", e.SessionId));
        }

        static async Task RecoUsingCSharpEventHandlerAsync(string audioFile)
        {
            var reco = RecognizerFactory.CreateSpeechRecognizerWithFileInput(audioFile);
            // var reco = RecognizerFactory.CreateSpeechRecognizer();

            // Subscribes to events.
            reco.OnIntermediateResult += MyIntermediateResultEventHandler;
            reco.OnFinalResult += MyFinalResultEventHandler;
            reco.OnRecognitionError += MyErrorEventHandler;
            reco.OnSessionStarted += MySessionStartedEventHandler;
            reco.OnSessionStopped += MySessionStoppedEventHandler;
            reco.OnSoundStarted += MySoundStartedEventHandler;
            reco.OnSoundStopped += MySoundStoppedEventHandler;

            // Starts recognition.
            var result = await reco.RecognizeAsync();

            Console.WriteLine("Recognition result: " + result.Text);

            // Unsubscribe to events.
            reco.OnIntermediateResult -= MyIntermediateResultEventHandler;
            reco.OnFinalResult -= MyFinalResultEventHandler;
            reco.OnRecognitionError -= MyErrorEventHandler;
            reco.OnSessionStarted -= MySessionStartedEventHandler;
            reco.OnSessionStopped -= MySessionStoppedEventHandler;
            reco.OnSoundStarted -= MySoundStartedEventHandler;
            reco.OnSoundStopped -= MySoundStoppedEventHandler;
        }

        static void Main(string[] args)
        {
            if (args.Length <= 0)
            {
                Console.WriteLine("Usage: carbon_csharp_console wavfile");
                Environment.Exit(1);
            }

            RecoUsingCSharpEventHandlerAsync(args[0]).Wait();
        }
    }

}
