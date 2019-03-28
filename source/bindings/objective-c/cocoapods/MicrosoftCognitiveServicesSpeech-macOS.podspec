Pod::Spec.new do |s|
  s.name         = "MicrosoftCognitiveServicesSpeech-macOS"
  s.version      = "{{{MAJOR.MINOR.PATCH}}}"
  s.summary      = "The Microsoft Cognitive Services Speech SDK for macOS."
  s.description  = "Via the Speech SDK, the Speech Service helps developers add speech-enabled features to their apps."
  s.homepage     = "https://docs.microsoft.com/azure/cognitive-services/speech-service/"

  s.license      = { :type => "Microsoft Speech SDK license" }

  s.author       = "Microsoft"

  s.platform              = :osx, "10.13"
  s.osx.deployment_target = "10.13"

  s.osx.vendored_frameworks = "MicrosoftCognitiveServicesSpeech.framework"
  s.osx.framework           = "CFNetwork"

  s.source                  = { :http => "https://csspeechstorage.blob.core.windows.net/drop/#{s.version.to_s}/MicrosoftCognitiveServicesSpeech-macOS-#{s.version.to_s}.zip",
                                :sha256 => "{{{BINARY_SHA}}}" }

  s.vendored_frameworks = 'MicrosoftCognitiveServicesSpeech.framework'

  s.documentation_url       = "https://aka.ms/csspeech/objectivecref"
end
