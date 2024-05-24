using Microsoft.AspNetCore.SignalR;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using System.Text;

namespace samples.Hubs;

public sealed class SpeechToTextHub : Hub<ISpeechToTextHub>
{
    private readonly ILogger _logger;
    private readonly IConfiguration _configuration;

    public SpeechToTextHub(
        ILogger<SpeechToTextHub> logger,
        IConfiguration configuration)
    {
        _logger = logger;
        _configuration = configuration;
    }

    public async Task ContinuousRecognition(IAsyncEnumerable<string> stream)
    {
        var speechConfig = GetSpeechConfig();

        if (speechConfig == null)
        {
            await Clients.Caller.ReceiveMessage("Please provide your service key and region in the configuration provider. (Ex., appsettings.json).", true);

            return;
        }

        using var format = AudioStreamFormat.GetCompressedFormat(AudioStreamContainerFormat.OGG_OPUS);
        using var audioInputStream = AudioInputStream.CreatePushStream(format);
        using var audioConfig = AudioConfig.FromStreamInput(audioInputStream);
        var recognizer = new SpeechRecognizer(speechConfig, audioConfig);

        recognizer.Recognizing += RecognizerRecognizing;
        recognizer.SessionStarted += RecognizerStarted;
        recognizer.SessionStopped += RecognizerStopped;
        await recognizer.StartContinuousRecognitionAsync();

        await foreach (var base64Str in stream)
        {
            var chunk = Convert.FromBase64String(base64Str);
            audioInputStream.Write(chunk);
        }

        await recognizer.StopContinuousRecognitionAsync();

        recognizer.Recognizing -= RecognizerRecognizing;
        recognizer.SessionStarted -= RecognizerStarted;
        recognizer.SessionStopped -= RecognizerStopped;

        recognizer.Dispose();
    }

    public async Task RecognizeOnce(IAsyncEnumerable<string> stream)
    {
        var speechConfig = GetSpeechConfig();

        if (speechConfig == null)
        {
            await Clients.Caller.ReceiveMessage("Please provide your service key and region in the configuration provider. (Ex., appsettings.json).", true);

            return;
        }

        var audioInputStream = await GetAudioStreamDirectAsync(stream); // await GetAudioStreamChunksAsync(stream);
        try
        {
            using var audioConfig = AudioConfig.FromStreamInput(audioInputStream);
            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);

            await Clients.Caller.ReceiveMessage("Session Started.");

            var result = await recognizer.RecognizeOnceAsync();

            if (result.Reason == ResultReason.RecognizedSpeech)
            {
                await Clients.Caller.ReceiveMessage(result.Text);
            }
            else if (result.Reason == ResultReason.NoMatch)
            {
                await Clients.Caller.ReceiveMessage("Speech could not be recognized. Please speak clearly into the microphone.");
                await Clients.Caller.ReceiveMessage("Session Ended.");

                return;
            }

            var cancellation = CancellationDetails.FromResult(result);

            await Clients.Caller.ReceiveMessage("Speech service is not available. Please try again later.", true);

            if (cancellation.Reason == CancellationReason.Error)
            {
                await Clients.Caller.ReceiveMessage($"Failed to recognize the incoming speech. Reason: {cancellation.Reason}. Error code: {cancellation.ErrorCode}. Error details: {cancellation.ErrorDetails}.", true);
            }
            else
            {
                await Clients.Caller.ReceiveMessage($"Failed to recognize the incoming speech. Reason {cancellation.Reason}.", true);
            }

            await Clients.Caller.ReceiveMessage("Session Ended.");
        }
        finally
        {
            audioInputStream.Dispose();
        }
    }

    private static async Task<PushAudioInputStream> GetAudioStreamDirectAsync(IAsyncEnumerable<string> stream)
    {
        var format = AudioStreamFormat.GetCompressedFormat(AudioStreamContainerFormat.OGG_OPUS);
        var audioInputStream = AudioInputStream.CreatePushStream(format);

        await foreach (var base64Str in stream)
        {
            var chunk = Convert.FromBase64String(base64Str);
            audioInputStream.Write(chunk);
        }

        return audioInputStream;
    }

    private static async Task<PushAudioInputStream> GetAudioStreamChunksAsync(IAsyncEnumerable<string> stream)
    {
        using var memoryStream = new MemoryStream();
        await foreach (var base64Str in stream)
        {
            var chunk = Convert.FromBase64String(base64Str);
            memoryStream.Write(chunk);
        }

        memoryStream.Seek(0, SeekOrigin.Begin);
        var format = AudioStreamFormat.GetCompressedFormat(AudioStreamContainerFormat.OGG_OPUS);
        var audioInputStream = AudioInputStream.CreatePushStream(format);
        using (var binaryReader = new BinaryReader(memoryStream, Encoding.UTF8, leaveOpen: true))
        {
            byte[] readBytes;
            do
            {
                readBytes = binaryReader.ReadBytes(1024);

                if (readBytes.Length == 0)
                {
                    break;
                }
                audioInputStream.Write(readBytes);
            } while (readBytes.Length > 0);
        }

        return audioInputStream;
    }

    private SpeechConfig GetSpeechConfig()
    {
        var section = _configuration.GetSection("SpeechService");
        var key = section.GetValue<string>("Key");
        var region = section.GetValue<string>("Region");

        if (string.IsNullOrWhiteSpace(key) || string.IsNullOrWhiteSpace(region))
        {
            return null;
        }

        return SpeechConfig.FromSubscription(key, region);
    }

    private void RecognizerStopped(object sender, SessionEventArgs e)
    {
        Clients.Caller.ReceiveMessage("Session Started.").GetAwaiter().GetResult();
    }

    private void RecognizerStarted(object sender, SessionEventArgs e)
    {
        Clients.Caller.ReceiveMessage("Session Ended.").GetAwaiter().GetResult();
    }

    private void RecognizerRecognizing(object sender, SpeechRecognitionEventArgs e)
    {
        _logger.LogDebug("RecognizerRecognizing. Result: {Result}. Reason {Reason}", e.Result?.Text, e.Result?.Reason);

        if (e.Result.Reason == ResultReason.RecognizedSpeech)
        {
            Clients.Caller.ReceivePartialMessage(e.Result.Text).GetAwaiter().GetResult();
        }
        else if (e.Result.Reason == ResultReason.NoMatch)
        {
            Clients.Caller.ReceiveMessage("NOMATCH: Speech could not be recognized.", true).GetAwaiter().GetResult();
        }
    }
}

public interface ISpeechToTextHub
{
    Task ReceiveMessage(string message, bool isError = false);

    Task ReceivePartialMessage(string message);
}
