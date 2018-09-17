Pod::Spec.new do |s|

  s.name         = "Microsoft-CognitiveServices-Speech-for-iOS"
  s.version      = "1.0.0"
  s.summary      = "The Microsoft Cognitive Services Speech SDK for iOS."
  s.description  = "Via the Speech SDK, the Speech Service helps developers add speech-enabled features to their apps."
  s.homepage     = "https://azure.microsoft.com/en-us/services/cognitive-services/directory/speech"

  s.license      = { :type => "MIT", :file => "license.md" }

  s.author       = { "Name" => "Microsoft" }

  s.platform              = :ios, "11.4"
  s.ios.deployment_target = "11.4"

  s.ios.vendored_frameworks = "MicrosoftCognitiveServicesSpeech.framework"
  s.ios.framework           = "CFNetwork"
  
# TODO: VSTS:1408414 - Update URL to point to a .zip stored in Azure Blobs
  s.source              = { :http => "http://localhost:8000/MicrosoftCognitiveServicesSpeech-iOS.zip"  }

  s.source_files        = "MicrosoftCognitiveServicesSpeech.framework", "MicrosoftCognitiveServicesSpeech.framework/Headers/*.{h}"
  s.public_header_files = "MicrosoftCognitiveServicesSpeech.framework/Headers/*.h"

end
