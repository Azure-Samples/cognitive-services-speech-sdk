Pod::Spec.new do |s|
  s.name         = "MicrosoftCognitiveServicesSpeech-iOS"
  s.version      = "{{{MAJOR.MINOR.PATCH}}}"
  s.summary      = "The Microsoft Cognitive Services Speech SDK for iOS."
  s.description  = "Via the Speech SDK, the Speech Service helps developers add speech-enabled features to their apps."
  s.homepage     = "https://docs.microsoft.com/azure/cognitive-services/speech-service/"

  s.license      = { :type => "Microsoft Speech SDK license" }

  s.author       = "Microsoft"

  s.platform                = :ios, "9.2"
  s.ios.deployment_target   = "9.2"

  s.ios.vendored_frameworks = "MicrosoftCognitiveServicesSpeech.framework"

  s.source                  = { :http => "https://csspeechstorage.blob.core.windows.net/drop/#{s.version.to_s}/MicrosoftCognitiveServicesSpeech-iOS-#{s.version.to_s}.zip",
                                :sha256 => "{{{BINARY_SHA}}}" }

  s.vendored_frameworks = 'MicrosoftCognitiveServicesSpeech.framework'

  s.documentation_url       = "https://aka.ms/csspeech/objectivecref"
end
