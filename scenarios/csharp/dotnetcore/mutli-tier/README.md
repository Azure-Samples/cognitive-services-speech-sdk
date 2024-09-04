# Multi-tier Speech SDK Sample

This sample covers teh components needed to use the Azure Speech SDK in a multi-tier environment. In this sample, audio data is received by an end user device and sent to a middle tier using robust transport where it is give to the Azure Speech SDK for recognition. 

```mermaid
graph TD
    A[SpeechSDKRemoteClientApp]
    B[SpeechSDKRemoteClientAppCore]
    C[SpeechSDKRemoteClientGRPC]
    D[SpeechSDKRemoteClientWebSocket]
    E[SpeechSDKRemoteServiceCore]
    F[SpeechSDKRemoteServiceGRPC]
    G[SpeechSDKRemoteServiceWebSocket]
    K[Speech SDK]
    L[Speech Service]
    A <--> B
    B <--> C
    B <--> D
    C <--> F
    D <--> G
    G <--> E
    F <--> E
    E <--> K
    K <--> L
```