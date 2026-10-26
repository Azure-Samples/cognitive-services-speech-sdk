# Other runtime choices

| Existing runtime | Supported direction |
|---|---|
| Java | Official Speech SDK for complete text/SSML and output streaming; verify identity overloads. Current official TextStream list does not include Java |
| C++ | Official SDK; listed for TextStream, but verify installed signatures and available examples before implementation |
| Other HTTP backend | REST SSML to bytes/progressive output or durable Batch; neither is an arbitrary incremental-text API |

No reusable modules or language build evidence are bundled for these paths. Preserve runtime/identity and report unsupported combinations before proposing a backend or input change. Read only the operation guide and the installed SDK's language reference. Do not port method names from another language by analogy.

Sources: [SDK samples by language](https://github.com/Azure-Samples/cognitive-services-speech-sdk), [TextStream support](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-lower-speech-synthesis-latency), [REST](https://learn.microsoft.com/azure/ai-services/speech-service/rest-text-to-speech).
