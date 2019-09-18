#! /usr/bin/env perl
use warnings;
use strict;

use JSON::PP;
use 5.22.0;
use Getopt::Long qw(GetOptions);

my $OUTPUT_DIRECTORY;
my $SPEECHSDK_SPEECH_KEY;
my $SPEECHSDK_SPEECH_REGION;
my $SPEECHSDK_SPEECH_ENDPOINTID;
my $SPEECHSDK_LUIS_KEY;
my $SPEECHSDK_LUIS_REGION;
my $SPEECHSDK_LUIS_HOMEAUTOMATION_APPID;
my $SPEECHSDK_BOT_REGION;
my $SPEECHSDK_BOT_SUBSCRIPTION;
my $SPEECHSDK_BOT_FUNCTIONALTESTBOT;
my $SPEECHSDK_PRINCETON_INROOM_ENDPOINT;
my $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PPE_KEY;
my $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY;
my $SPEECHSDK_PRINCETON_REGION;
my $SPEECHSDK_PRINCETON_ONLINE_ENDPOINT;

GetOptions('OutputDirectory=s' => \$OUTPUT_DIRECTORY,
          'SPEECHSDK_SPEECH_KEY=s' => \$SPEECHSDK_SPEECH_KEY,
          'SPEECHSDK_SPEECH_REGION=s' => \$SPEECHSDK_SPEECH_REGION,
          'SPEECHSDK_SPEECH_ENDPOINTID=s' => \$SPEECHSDK_SPEECH_ENDPOINTID,
          'LanguageUnderstandingSubscriptionKey=s' => \$SPEECHSDK_LUIS_KEY,
          'LanguageUnderstandingServiceRegion=s' => \$SPEECHSDK_LUIS_REGION,
          'LanguageUnderstandingHomeAutomationAppId=s' => \$SPEECHSDK_LUIS_HOMEAUTOMATION_APPID,
          'DialogRegion=s' => \$SPEECHSDK_BOT_REGION,
          'DialogSubscriptionKey=s' => \$SPEECHSDK_BOT_SUBSCRIPTION,
          'DialogFunctionalTestBot=s' => \$SPEECHSDK_BOT_FUNCTIONALTESTBOT,
          'InRoomAudioEndpoint=s' => \$SPEECHSDK_PRINCETON_INROOM_ENDPOINT,
          'OnlineAudioEndpoint=s' => \$SPEECHSDK_PRINCETON_ONLINE_ENDPOINT,
          'ConversationTranscriptionEndpoint=s' => \$SPEECHSDK_PRINCETON_INROOM_ENDPOINT,
          'ConversationTranscriptionPPEKey=s' => \$SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PPE_KEY,
          'ConversationTranscriptionPRODKey=s' => \$SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY,
          'SpeechRegionForConversationTranscription=s' => \$SPEECHSDK_PRINCETON_REGION
) or die "Usage: $0 --OutputDirectory <outputDirectory>
          --SPEECHSDK_SPEECH_KEY <speechSdkSpeechKey>
          --SPEECHSDK_SPEECH_REGION <speechSdkRegion>
          --SPEECHSDK_SPEECH_ENDPOINTID <speechSdkEndpointId>
          --LanguageUnderstandingSubscriptionKey <languageUnderstandingSubscriptionKey>
          --LanguageUnderstandingServiceRegion <languageUnderstandingServiceRegion>
          --LanguageUnderstandingHomeAutomationAppId <languageUnderstandingHomeAutomationAppId>
          --DialogRegion <dialogRegion>
          --DialogSubscriptionKey <dialogSubscriptionKey>
          --DialogFunctionalTestBot <dialogFunctionalTestBot>
          --InRoomAudioEndpoint <inRoomAudioEndpoint>
          --OnlineAudioEndpoint <onlineAudioEndpoint>
          --ConversationTranscriptionEndpoint <conversationTranscriptionEndpoint>
          --ConversationTranscriptionPPEKey <conversationTranscriptionPPEKey>
          --ConversationTranscriptionPRODKey <conversationTranscriptionPRODKey>
          --SpeechRegionForConversationTranscription <speechRegionForConversationTranscription>";

my %jsonData = ("UnifiedSpeechSubscriptionKey" => $SPEECHSDK_SPEECH_KEY,
            "Region" => $SPEECHSDK_SPEECH_REGION,
            "DeploymentId" => $SPEECHSDK_SPEECH_ENDPOINTID,
            "InputDir" => './input',
            "LanguageUnderstandingSubscriptionKey" => $SPEECHSDK_LUIS_KEY,
            "LanguageUnderstandingServiceRegion" => $SPEECHSDK_LUIS_REGION,
            "LanguageUnderstandingHomeAutomationAppId" => $SPEECHSDK_LUIS_HOMEAUTOMATION_APPID,
            "DialogRegion" => $SPEECHSDK_BOT_REGION,
            "DialogSubscriptionKey" => $SPEECHSDK_BOT_SUBSCRIPTION,
            "DialogFunctionalTestBot" => $SPEECHSDK_BOT_FUNCTIONALTESTBOT,
            "ConversationTranscriptionEndpoint" => $SPEECHSDK_PRINCETON_INROOM_ENDPOINT,
            "ConversationTranscriptionPPEKey" => $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PPE_KEY,
            "ConversationTranscriptionPRODKey" => $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY,
            "SpeechRegionForConversationTranscription" => $SPEECHSDK_PRINCETON_REGION,
            "InRoomAudioEndpoint" => $SPEECHSDK_PRINCETON_INROOM_ENDPOINT,
            "OnlineAudioEndpoint" => $SPEECHSDK_PRINCETON_ONLINE_ENDPOINT,
            "Endpoint" => $SPEECHSDK_SPEECH_ENDPOINTID,
            "ConversationTranscriptionSubscriptionKey" => $SPEECHSDK_PRINCETON_CONVERSATIONTRANSCRIBER_PROD_KEY);

open my $fh, ">", "$OUTPUT_DIRECTORY/test.settings.json" or die "Can't open the file $OUTPUT_DIRECTORY/test.settings.json";
print $fh encode_json(\%jsonData);
close $fh;