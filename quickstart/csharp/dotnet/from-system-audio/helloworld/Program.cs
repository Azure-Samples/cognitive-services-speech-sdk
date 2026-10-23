//
// Author : Naveen Gopalakrishna
//

using System;
using System.Threading.Tasks;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using NAudio.Wave;

namespace speechtotext_naudio
{
    class Program
    {
        public static async Task RecognitionWithPushAudioStreamAsync()
        {
           
            //create an instace of WasApiLoopBackcapture 
            var wasIn = new WasapiLoopbackCapture();
            wasIn.WaveFormat = new WaveFormat(16000, 16, 1);

            var speechKey = "<AZURE_SPEECH_KEY>";
            var speechRegion = "<AZURE_REGION>";


            // Creates an instance of a speech config with specified subscription key and service region.
            // Replace with your own subscription key and service region (e.g., "westus").            
            var config = SpeechConfig.FromSubscription(speechKey, speechRegion);

            var stopRecognition = new TaskCompletionSource<int>(TaskCreationOptions.RunContinuationsAsynchronously);

            // Create a push stream
            using (var pushStream = AudioInputStream.CreatePushStream())
            {
                using (var audioInput = AudioConfig.FromStreamInput(pushStream))
                {
                    // Creates a speech recognizer using audio stream input.
                    using (var recognizer = new SpeechRecognizer(config, audioInput))
                    {
                        Console.WriteLine("Say something...");

                        // Subscribes to events.
                        recognizer.Recognizing += (s, e) =>
                        {
                            Console.WriteLine($"RECOGNIZING: Text={e.Result.Text}");
                        };

                        recognizer.Recognized += (s, e) =>
                        {
                            if (e.Result.Reason == ResultReason.RecognizedSpeech)
                            {
                                Console.WriteLine($"RECOGNIZED: Text={e.Result.Text}");
                            }
                            else if (e.Result.Reason == ResultReason.NoMatch)
                            {
                                Console.WriteLine($"NOMATCH: Speech could not be recognized.");
                            }
                        };

                        recognizer.Canceled += (s, e) =>
                        {
                            Console.WriteLine($"CANCELED: Reason={e.Reason}");

                            if (e.Reason == CancellationReason.Error)
                            {
                                Console.WriteLine($"CANCELED: ErrorCode={e.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails={e.ErrorDetails}");
                                Console.WriteLine($"CANCELED: Did you update the subscription info?");
                            }

                            stopRecognition.TrySetResult(0);
                        };

                        recognizer.SessionStarted += (s, e) =>
                        {
                            Console.WriteLine("\nSession started event.");
                        };

                        recognizer.SessionStopped += (s, e) =>
                        {
                            Console.WriteLine("\nSession stopped event.");
                            Console.WriteLine("\nStop recognition.");
                            stopRecognition.TrySetResult(0);
                        };

                      
                        wasIn.DataAvailable += (s, e) =>
                        {
                            if (e.BytesRecorded != 0)
                            {
                                pushStream.Write(e.Buffer);
                            }
                        };

                        // Starts continuous recognition. Uses StopContinuousRecognitionAsync() to stop recognition.
                        await recognizer.StartContinuousRecognitionAsync().ConfigureAwait(false);

                        // waveIn.StartRecording();
                        wasIn.StartRecording();

                        // Waits for completion.
                        // Use Task.WaitAny to keep the task rooted.
                        Task.WaitAny(new[] { stopRecognition.Task });

                        // Stops recognition.
                        await recognizer.StopContinuousRecognitionAsync().ConfigureAwait(false);
                        // waveIn.StopRecording();
                        wasIn.StopRecording();
                    }
                }
            }
        }

        static void Main(string[] args)
        {

            Task.Run(() => RecognitionWithPushAudioStreamAsync());

            Console.WriteLine("Press any key to stop transcription...");
            while (!Console.KeyAvailable)
            {
                // Keep the program running until a key is pressed
                RecognitionWithPushAudioStreamAsync().Wait();
            }
        }
    }
}