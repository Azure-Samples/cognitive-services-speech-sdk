//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#include <iostream>
#include <string>
#include <speechapi_cxx.h>

using namespace std::chrono_literals;
using namespace Microsoft::CognitiveServices::Speech;
using namespace Microsoft::CognitiveServices::Speech::Audio;
using namespace Microsoft::CognitiveServices::Speech::Transcription;

void StartNewConversation()
{
    // Replace with your own subscription key and service region (e.g., "westus").
    std::string subscriptionKey("YourSubscriptionKey");
    std::string region("YourServiceRegion");

    // Change this to match the language you are speaking. You can find the full list of supported
    // speech language codes here: https://aka.ms/speech/sttt-languages
    std::string speechLanguage("en-US");

    // Creates an instance of a speech config with specified subscription key and service region.
    auto speechConfig = SpeechConfig::FromSubscription(subscriptionKey, region);
    speechConfig->SetSpeechRecognitionLanguage(speechLanguage);

    // Create the conversation object you'll need to manage the conversation
    auto conversation = Conversation::CreateConversationAsync(speechConfig).get();

    // Start the conversation so you and others can join
    conversation->StartConversationAsync().get();

    // Get the conversation ID. It will be up to your scenario to determine how this is shared
    // with other participants.
    std::cout << "CONVERSATION: Created a new conversation with ID " << conversation->GetConversationId() << std::endl;

    // You can now call various commands to manage the room. For example:
    conversation->MuteAllParticipantsAsync().get();

    // Create the conversation translator you'll need to send audio, send IMs, and receive conversation events
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();
    auto conversationTranslator = ConversationTranslator::FromConfig(audioConfig);

    // add any event handlers
    conversationTranslator->SessionStarted += [](const SessionEventArgs& args)
    {
        std::cout << "SESSION STARTED: " << args.SessionId << std::endl;
    };
    conversationTranslator->SessionStopped += [](const SessionEventArgs& args)
    {
        std::cout << "SESSION STOPPED: " << args.SessionId << std::endl;
    };
    conversationTranslator->Canceled += [](const ConversationTranslationCanceledEventArgs& args)
    {
        std::cout << "CANCELED: Reason=" << (int)args.Reason << std::endl;
        switch (args.Reason)
        {
            case CancellationReason::EndOfStream:
                std::cout << "CANCELED: End of audio reached" << std::endl;
                break;

            case CancellationReason::Error:
                std::cout << "CANCELED: ErrorCode= " << (long)args.ErrorCode << std::endl;
                std::cout << "CANCELED: ErrorDetails=" << args.ErrorDetails << std::endl;
                break;
        }
    };
    conversationTranslator->ConversationExpiration += [](const ConversationExpirationEventArgs& args)
    {
        std::cout << "CONVERSATION: Will expire in " << args.ExpirationTime.count() << " minutes" << std::endl;
    };
    conversationTranslator->ParticipantsChanged += [](const ConversationParticipantsChangedEventArgs& args)
    {
        std::cout << "PARTICIPANTS: The following participant(s) have ";
        switch (args.Reason)
        {
            case ParticipantChangedReason::JoinedConversation:
                std::cout << "joined";
                break;

            case ParticipantChangedReason::LeftConversation:
                std::cout << "left";
                break;

            case ParticipantChangedReason::Updated:
                std::cout << "been updated";
                break;
        }

        std::cout << ":" << std::endl;

        for (std::shared_ptr<Participant> participant : args.Participants)
        {
            std::cout << "\tPARTICIPANT: " << participant->DisplayName << std::endl;
        }
    };
    conversationTranslator->Transcribing += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "TRANSCRIBING: From " << args.Result->ParticipantId << ": " << args.Result->Text << std::endl;
        for (const auto& entry : args.Result->Translations)
        {
            std::cout << "\tTRANSLATED: " << entry.first << ": " << entry.second << std::endl;
        }
    };
    conversationTranslator->Transcribed += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "TRANSCRIBED: From " << args.Result->ParticipantId << ": " << args.Result->Text << std::endl;
        for (const auto& entry : args.Result->Translations)
        {
            std::cout << "\tTRANSLATED: " << entry.first << ": " << entry.second << std::endl;
        }
    };
    conversationTranslator->TextMessageReceived += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "TEXT MESSAGE: From " << args.Result->ParticipantId << ": " << args.Result->Text << std::endl;
        for (const auto& entry : args.Result->Translations)
        {
            std::cout << "\tTRANSLATED: " << entry.first << ": " << entry.second << std::endl;
        }
    };

    // Join the conversation so you can start receiving events
    conversationTranslator->JoinConversationAsync(conversation, "Test Host").get();

    // Send an instant message
    conversationTranslator->SendTextMessageAsync("This is a short test message").get();

    // Start sending audio
    conversationTranslator->StartTranscribingAsync().get();

    // At this point, you should start receiving transcriptions for what you are saying using
    // the default microphone. Press enter to stop audio capture
    std::cout << "Started transcribing. Press Enter to stop" << std::endl;
    std::cin.get();

    // Stop audio capture
    conversationTranslator->StopTranscribingAsync().get();

    // Leave the conversation. You will stop receiving events
    conversationTranslator->LeaveConversationAsync().get();

    // Once you are done, remember to delete the conversation.
    conversation->EndConversationAsync().get(); // You will not be able to rejoin after this
    conversation->DeleteConversationAsync().get(); // All participants still in the room will be ejected
}

void JoinExistingConversation(const std::string& conversationId)
{
    std::string speechLanguage("en-US");

    // As a participant, you don't need to specify any subscription key, or region. You can directly create
    // the conversation translator object
    auto audioConfig = AudioConfig::FromDefaultMicrophoneInput();
    auto conversationTranslator = ConversationTranslator::FromConfig(audioConfig);

    // Attach event handlers here. For example:
    conversationTranslator->Transcribing += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "TRANSCRIBING: From " << args.Result->ParticipantId << ": " << args.Result->Text << std::endl;
        for (const auto& entry : args.Result->Translations)
        {
            std::cout << "\tTRANSLATED: " << entry.first << ": " << entry.second << std::endl;
        }
    };
    conversationTranslator->Transcribed += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "TRANSCRIBED: From " << args.Result->ParticipantId << ": " << args.Result->Text << std::endl;
        for (const auto& entry : args.Result->Translations)
        {
            std::cout << "\tTRANSLATED: " << entry.first << ": " << entry.second << std::endl;
        }
    };
    conversationTranslator->TextMessageReceived += [](const ConversationTranslationEventArgs& args)
    {
        std::cout << "TEXT MESSAGE: From " << args.Result->ParticipantId << ": " << args.Result->Text << std::endl;
        for (const auto& entry : args.Result->Translations)
        {
            std::cout << "\tTRANSLATED: " << entry.first << ": " << entry.second << std::endl;
        }
    };

    // Join the conversation
    conversationTranslator->JoinConversationAsync(conversationId, "participant", speechLanguage).get();

    // Start sending audio
    conversationTranslator->StartTranscribingAsync().get();

    // At this point, you should start receiving transcriptions for what you are saying using
    // the default microphone. Press enter to stop audio capture
    std::cout << "Started transcribing. Press Enter to stop" << std::endl;
    std::cin.get();

    // Stop audio capture
    conversationTranslator->StopTranscribingAsync().get();

    // Once you are done, leave the conversation
    conversationTranslator->LeaveConversationAsync().get();
}

int main()
{
    StartNewConversation();

    // Comment out the previous line, and uncomment the next line to join an existing conversation.
    // Remember to replace YourConversationId with the ID of the conversation you want to join
    //JoinExistingConversation("YourConversationId");
}
