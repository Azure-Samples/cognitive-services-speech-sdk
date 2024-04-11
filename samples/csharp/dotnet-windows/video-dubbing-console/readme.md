# Video Dubbing

Video dubbing client tool and API sample code

# Solution:
   [VideoTranslationApiSampleCode.sln](VideoTranslationApiSampleCode\VideoTranslationApiSampleCode.sln)


# API sample:

## Video file API:
   Metadata API: [VideoFileClient.cs](VideoTranslationApiSampleCode\VideoTranslationLib\VideoTranslationMetadataClient.cs)

   Video file API: [VideoFileClient.cs](VideoTranslationApiSampleCode\VideoTranslationLib\VideoFileClient.cs)

   Translation API: [VideoTranslationClient.cs](VideoTranslationApiSampleCode\VideoTranslationLib\VideoTranslationClient.cs)

   Target locale API: [TargetLocaleClient.cs](VideoTranslationApiSampleCode\VideoTranslationLib\TargetLocaleClient.cs)

# For project CommonLib
   Not upgrade Flurl to 4.0 due to 4.0 doesn't support NewtonJson for ReceiveJson.