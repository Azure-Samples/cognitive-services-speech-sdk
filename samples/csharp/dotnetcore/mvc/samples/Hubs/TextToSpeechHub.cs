using Microsoft.AspNetCore.SignalR;
using Microsoft.CognitiveServices.Speech;

namespace samples.Hubs;

public sealed class TextToSpeechHub : Hub<ITextToSpeechHub>
{
    private readonly IConfiguration _configuration;

    public TextToSpeechHub(IConfiguration configuration)
    {
        _configuration = configuration;
    }

    public async Task Send(string text, string voiceName)
    {
        if (string.IsNullOrWhiteSpace(text))
        {
            return;
        }

        var speechConfig = _configuration.GetSpeechConfig();

        if (speechConfig == null)
        {
            await Clients.Caller.ReceiveMessage(SpeechToTextHub.InvalidConfigErrorMessage, true);

            return;
        }

        if (!string.IsNullOrWhiteSpace(voiceName))
        {
            speechConfig.SpeechSynthesisVoiceName = voiceName;
        }

        using var synth = new SpeechSynthesizer(speechConfig, null);
        using var voiceStream = new MemoryStream();
        using var result = await synth.SpeakTextAsync(text);

        if (result.Reason == ResultReason.SynthesizingAudioCompleted)
        {
            await Clients.Caller.ReceiveData(Convert.ToBase64String(result.AudioData), text);

            return;
        }

        if (result.Reason == ResultReason.Canceled)
        {
            var cancellation = SpeechSynthesisCancellationDetails.FromResult(result);

            if (cancellation.Reason == CancellationReason.Error)
            {
                await Clients.Caller.ReceiveMessage($"Failed to speak an incoming text. Reason: {cancellation.Reason}. Error code: {cancellation.ErrorCode}. Error details: {cancellation.ErrorDetails}.", true);

                return;
            }

            await Clients.Caller.ReceiveMessage($"Failed to speak an incoming text. Reason {cancellation.Reason}.", true);
        }
    }
}

public interface ITextToSpeechHub
{
    Task ReceiveMessage(string message, bool isError = false);

    Task ReceiveData(string base64String, string plainText);
}
