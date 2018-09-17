Pod::Spec.new do |s|

  s.name         = "Microsoft-CognitiveServices-Speech-for-OSX"
  s.version      = "1.0.0"
  s.summary      = "The Microsoft Cognitive Services Speech SDK for OSX."
  s.description  = "Via the Speech SDK, the Speech Service helps developers add speech-enabled features to their apps."
  s.homepage     = "https://azure.microsoft.com/en-us/services/cognitive-services/directory/speech"

  s.license      = { :type => "MIT", :file => "license.md" }

  s.author       = { "Name" => "Microsoft" }

  s.platform              = :osx, "10.13"
  s.osx.deployment_target = "10.13"

  s.osx.vendored_frameworks = "MicrosoftCognitiveServicesSpeech.framework"
  s.osx.framework           = "CFNetwork"
  
# TODO: VSTS:1408414 - Update URL to point to a .zip stored in Azure Blobs
  s.source              = { :http => "http://localhost:8000/MicrosoftCognitiveServicesSpeech-OSX.zip"  }

  s.source_files        = "MicrosoftCognitiveServicesSpeech.framework", "MicrosoftCognitiveServicesSpeech.framework/Headers/*.{h}"
  s.public_header_files = "MicrosoftCognitiveServicesSpeech.framework/Headers/*.h"

end
