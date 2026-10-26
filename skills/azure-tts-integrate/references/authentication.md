# Identity and routing

Reuse the app's secret provider, configuration names and full endpoint. Inspect presence/shape, not secret values. Missing access blocks live calls, not offline implementation.

## Identity choice

| Existing flow | Preserve / verify |
|---|---|
| Resource key | Trusted backend only; matching resource/region and key-auth policy |
| Speech STS token | Trusted issuer; matching route; valid 10 minutes, refresh at about 9 |
| Entra / managed identity | Officially preferred. Custom-domain endpoint, *Cognitive Services Speech User* role; management access is insufficient. SDK: `token_credential` / `TokenCredential` overloads; REST: `Authorization: Bearer aad#<resourceId>#<token>` |
| Browser | Backend audio or approved short-lived token; never a resource key |

Do not switch a working bearer/Entra flow to key authentication. The Python adapter accepts a synchronous `TokenCredential` with the custom-domain endpoint; it does not handle raw STS bearer tokens. STS can use the official SDK authorization-token API or REST and does not by itself require HTTP. Entra uses actual token expiry, not STS lifetime. Refresh using the active client's supported API; changing the original `SpeechConfig` may not update a synthesizer. Long-lived Entra refresh has not been verified live here. Protect token issuer and keep tokens out of logs, URLs and persistent frontend storage.

## Endpoint shapes — public cloud examples

| Operation | Route |
|---|---|
| Regional REST | `https://<region>.tts.speech.microsoft.com/cognitiveservices/v1` |
| Resource REST | `https://<resource>.cognitiveservices.azure.com/tts/cognitiveservices/v1` |
| Text input stream | `wss://<region>.tts.speech.microsoft.com/cognitiveservices/websocket/v2` |
| Batch | `https://<resource>.cognitiveservices.azure.com/texttospeech/batchsyntheses` |

Region is a label; resource endpoint is a base URI; operation URL includes path. A Foundry project URL is not a universal Speech endpoint. Verify private/sovereign routing and the selected token/SDK combination before live use.

**401/403:** inspect redacted method, issuer/host alignment, expiry, role and network/key policy. No environment dumps, endless retries or silent fallback. Resource/IAM/subdomain/network changes require missing authorization; honor approvals already given.

Sources: [Entra](https://learn.microsoft.com/azure/ai-services/speech-service/how-to-configure-azure-ad-auth), [REST](https://learn.microsoft.com/azure/ai-services/speech-service/rest-text-to-speech). Docs checked 2026-10-19.
