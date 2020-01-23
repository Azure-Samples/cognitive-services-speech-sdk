//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using Microsoft.CognitiveServices.Speech.Transcription;
using System;
using System.Threading.Tasks;

namespace helloworld
{
    class Program
    {
        public static void Main(string[] args)
        {
            CreateConversationAsync().Wait();

            // Comment out the previous line, and uncomment the next line to join an existing conversation.
            // Remember to replace YourConversationId with the ID of the conversation you want to join
            // JoinConversationAsync("YourConversationId").Wait();
        }

        static async Task CreateConversationAsync()
        {
            // Replace with your own subscription key and service region (e.g., "westus").
            string subscriptionKey = "YourSubscriptionKey";
            string region = "YourServiceRegion";

            // Change this to match the language you are speaking. You can find the full list of supported
            // speech language codes here: https://docs.microsoft.com/azure/cognitive-services/speech-service/language-support
            string fromLanguage = "en-US";

            // Change this to the language you would like to translate the transcriptions to. You can find
            // the full list of supported translation languages here: https://aka.ms/speech/sttt-languages
            string toLanguage = "de";

            // Set this to the display name you want for the conversation host
            string displayName = "The host";

            // Creates an instance of a speech config with specified subscription key and service region.
            var config = SpeechTranslationConfig.FromSubscription(subscriptionKey, region);
            config.SpeechRecognitionLanguage = fromLanguage;
            config.AddTargetLanguage(toLanguage);

            // Create the conversation object you'll need to manage the conversation
            using (var conversation = await Conversation.CreateConversationAsync(config).ConfigureAwait(false))
            {
                // Start the conversation so you and others can join
                await conversation.StartConversationAsync().ConfigureAwait(false);

                // Get the conversation ID. It will be up to your scenario to determine how this is shared
                // with other participants.
                string conversationId = conversation.ConversationId;
                Console.WriteLine($"CONVERSATION: Created a new conversation with ID '{conversationId}'");

                // At this point, you can use the conversation object to manage the conversation. For example,
                // to mute everyone else in the room you can call this method:
                await conversation.MuteAllParticipantsAsync().ConfigureAwait(false);

                // Configure which audio source you want to use. In this case we will use your default microphone
                var audioConfig = AudioConfig.FromDefaultMicrophoneInput();

                // Create the conversation translator you'll need to send audio, send IMs, and receive conversation events
                using (var conversationTranslator = new ConversationTranslator(audioConfig))
                {
                    // You should connect all the event handlers you need at this point
                    conversationTranslator.SessionStarted += (s, e) =>
                    {
                        Console.WriteLine($"SESSION STARTED: {e.SessionId}");
                    };
                    conversationTranslator.SessionStopped += (s, e) =>
                    {
                        Console.WriteLine($"SESSION STOPPED: {e.SessionId}");
                    };
                    conversationTranslator.Canceled += (s, e) =>
                    {
                        Console.WriteLine($"CANCELED: Reason={e.Reason}");
                        switch (e.Reason)
                        {
                            case CancellationReason.EndOfStream:
                                Console.WriteLine($"CANCELED: End of audio reached");
                                break;

                            case CancellationReason.Error:
                                Console.WriteLine($"CANCELED: ErrorCode= {e.ErrorCode}");
                                Console.WriteLine($"CANCELED: ErrorDetails= {e.ErrorDetails}");
                                break;
                        }
                    };
                    conversationTranslator.ConversationExpiration += (s, e) =>
                    {
                        Console.WriteLine($"CONVERSATION: Will expire in {e.ExpirationTime.TotalMinutes} minutes");
                    };
                    conversationTranslator.ParticipantsChanged += (s, e) =>
                    {
                        Console.Write("PARTICIPANTS: The following participant(s) have ");
                        switch (e.Reason)
                        {
                            case ParticipantChangedReason.JoinedConversation:
                                Console.Write("joined");
                                break;

                            case ParticipantChangedReason.LeftConversation:
                                Console.Write("left");
                                break;

                            case ParticipantChangedReason.Updated:
                                Console.Write("been updated");
                                break;
                        }

                        Console.WriteLine(":");

                        foreach (var participant in e.Participants)
                        {
                            Console.WriteLine($"\tPARTICIPANT: {participant.DisplayName}");
                        }
                    };
                    conversationTranslator.TextMessageReceived += (s, e) =>
                    {
                        Console.WriteLine($"TEXT MESSAGE: From '{e.Result.ParticipantId}': '{e.Result.Text}'");
                        foreach (var entry in e.Result.Translations)
                        {
                            Console.WriteLine($"\tTRANSLATED: '{entry.Key}': '{entry.Value}'");
                        }
                    };
                    conversationTranslator.Transcribed += (s, e) =>
                    {
                        Console.WriteLine($"TRANSCRIBED: From '{e.Result.ParticipantId}': '{e.Result.Text}'");
                        foreach (var entry in e.Result.Translations)
                        {
                            Console.WriteLine($"\tTRANSLATED: '{entry.Key}': '{entry.Value}'");
                        }
                    };
                    conversationTranslator.Transcribing += (s, e) =>
                    {
                        Console.WriteLine($"TRANSCRIBING: From '{e.Result.ParticipantId}': '{e.Result.Text}'");
                        foreach (var entry in e.Result.Translations)
                        {
                            Console.WriteLine($"\tTRANSLATED: '{entry.Key}': '{entry.Value}'");
                        }
                    };

                    // Join the conversation so you can start receiving events
                    await conversationTranslator.JoinConversationAsync(conversation, displayName).ConfigureAwait(false);

                    // You can now send an instant message to all other participants in the room
                    await conversationTranslator.SendTextMessageAsync("The instant message to send").ConfigureAwait(false);

                    // Start sending audio
                    await conversationTranslator.StartTranscribingAsync().ConfigureAwait(false);

                    // At this point, you should start receiving transcriptions for what you are saying using
                    // the default microphone. Press enter to stop audio capture
                    Console.WriteLine("Started transcribing. Press enter to stop");
                    while (Console.ReadKey(true).Key != ConsoleKey.Enter) { }
                    
                    // Stop audio capture
                    await conversationTranslator.StopTranscribingAsync().ConfigureAwait(false);

                    // Leave the conversation. After this you will no longer receive events
                    await conversationTranslator.LeaveConversationAsync().ConfigureAwait(false);
                }

                // End the conversation
                await conversation.EndConversationAsync().ConfigureAwait(false);

                // Delete the conversation. Any other participants that are still in the conversation will be removed
                await conversation.DeleteConversationAsync().ConfigureAwait(false);
            }
        }

        static async Task JoinConversationAsync(string conversationId)
        {
            // Set this to the display name you want for the participant
            string displayName = "participant";

            // Change this to the language you would like to translate the transcriptions to. You can find
            // the full list of supported translation languages here: https://aka.ms/speech/sttt-languages
            string language = "en-US";

            // As a participant, you don't need to specify any subscription key, or region. You can directly create
            // the conversation translator object
            var audioConfig = AudioConfig.FromDefaultMicrophoneInput();
            using (var conversationTranslator = new ConversationTranslator(audioConfig))
            {
                // You should add all the event handlers you need at this point. For now we will only add
                // handlers for the transcription, and instant message events
                conversationTranslator.TextMessageReceived += (s, e) =>
                {
                    Console.WriteLine($"TEXT MESSAGE: From '{e.Result.ParticipantId}': '{e.Result.Text}'");
                    foreach (var entry in e.Result.Translations)
                    {
                        Console.WriteLine($"\tTranslated into '{entry.Key}': '{entry.Value}'");
                    }
                };
                conversationTranslator.Transcribed += (s, e) =>
                {
                    Console.WriteLine($"TRANSCRIBED: From '{e.Result.ParticipantId}': '{e.Result.Text}'");
                    foreach (var entry in e.Result.Translations)
                    {
                        Console.WriteLine($"\tTRANSLATED: '{entry.Key}': '{entry.Value}'");
                    }
                };
                conversationTranslator.Transcribing += (s, e) =>
                {
                    Console.WriteLine($"TRANSCRIBING: From '{e.Result.ParticipantId}': '{e.Result.Text}'");
                    foreach (var entry in e.Result.Translations)
                    {
                        Console.WriteLine($"\tTRANSLATED: '{entry.Key}': '{entry.Value}'");
                    }
                };

                // Join the conversation so you can start receiving events
                await conversationTranslator.JoinConversationAsync(conversationId, displayName, language).ConfigureAwait(false);

                // You can now send an instant message to all other participants in the room
                await conversationTranslator.SendTextMessageAsync("Message from participant").ConfigureAwait(false);

                // Start sending audio
                await conversationTranslator.StartTranscribingAsync().ConfigureAwait(false);

                // At this point, you should start receiving transcriptions for what you are saying using
                // the default microphone. Press enter to stop audio capture
                Console.WriteLine("Started transcribing. Press enter to stop");
                while (Console.ReadKey(true).Key != ConsoleKey.Enter) { }

                // Stop audio capture
                await conversationTranslator.StopTranscribingAsync().ConfigureAwait(false);

                // Leave the conversation. You will stop receiving events after this
                await conversationTranslator.LeaveConversationAsync().ConfigureAwait(false);
            }
        }
    }
}
