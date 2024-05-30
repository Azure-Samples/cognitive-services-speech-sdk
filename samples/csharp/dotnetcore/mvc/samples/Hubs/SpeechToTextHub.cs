using Microsoft.AspNetCore.SignalR;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using System.Text;

namespace samples.Hubs;

public sealed class SpeechToTextHub : Hub<ISpeechToTextHub>
{
    public const string InvalidConfigErrorMessage = "Please provide your service key and region in the configuration provider (e.g., in appsettings.json).";

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
        var speechConfig = _configuration.GetSpeechConfig();

        if (speechConfig == null)
        {
            await Clients.Caller.ReceiveMessage(InvalidConfigErrorMessage, true);

            return;
        }

        using var format = AudioStreamFormat.GetCompressedFormat(AudioStreamContainerFormat.OGG_OPUS);
        using var audioInputStream = AudioInputStream.CreatePushStream(format);
        using var audioConfig = AudioConfig.FromStreamInput(audioInputStream);
        var recognizer = new SpeechRecognizer(speechConfig, audioConfig);

        recognizer.Recognizing += RecognizerRecognizing;
        recognizer.Recognized += RecognizerRecognized;
        recognizer.SessionStarted += RecognizerStarted;
        recognizer.SessionStopped += RecognizerStopped;

        await recognizer.StartContinuousRecognitionAsync();

        await PushAudioAsync(audioInputStream, stream);

        await recognizer.StopContinuousRecognitionAsync();

        recognizer.Recognizing -= RecognizerRecognizing;
        recognizer.Recognized -= RecognizerRecognized;
        recognizer.SessionStarted -= RecognizerStarted;
        recognizer.SessionStopped -= RecognizerStopped;

        recognizer.Dispose();
    }

    public async Task RecognizeOnce(IAsyncEnumerable<string> stream)
    {
        var speechConfig = _configuration.GetSpeechConfig();

        if (speechConfig == null)
        {
            await Clients.Caller.ReceiveMessage(InvalidConfigErrorMessage, true);

            return;
        }

        var format = AudioStreamFormat.GetCompressedFormat(AudioStreamContainerFormat.OGG_OPUS);
        var audioInputStream = AudioInputStream.CreatePushStream(format);

        await PushAudioAsync(audioInputStream, stream);

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
            }
            else
            {
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
            }

            await Clients.Caller.ReceiveMessage("Session Ended.");
        }
        finally
        {
            audioInputStream.Dispose();
        }
    }

    private static async Task PushAudioAsync(PushAudioInputStream audioInputStream, IAsyncEnumerable<string> stream)
    {
        await foreach (var base64String in stream)
        {
            using var memoryStream = new MemoryStream();

            var chunk = Convert.FromBase64String(base64String);
            memoryStream.Write(chunk, 0, chunk.Length);
            memoryStream.Seek(0, SeekOrigin.Begin);

            using var binaryReader = new BinaryReader(memoryStream, Encoding.UTF8, leaveOpen: true);
            byte[] readBytes;
            do
            {
                readBytes = binaryReader.ReadBytes(1024);
                audioInputStream.Write(readBytes);
            } while (readBytes.Length > 0);
        }
    }

    private void RecognizerStopped(object sender, SessionEventArgs e)
    {
        Clients.Caller.ReceiveMessage("Session Ended.").GetAwaiter().GetResult();
    }

    private void RecognizerStarted(object sender, SessionEventArgs e)
    {
        Clients.Caller.ReceiveMessage("Session Started.").GetAwaiter().GetResult();
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

    private void RecognizerRecognized(object sender, SpeechRecognitionEventArgs e)
    {
        _logger.LogDebug("RecognizedRecognizing. Result: {Result}. Reason {Reason}", e.Result?.Text, e.Result?.Reason);

        if (e.Result.Reason == ResultReason.RecognizedSpeech)
        {
            Clients.Caller.ReceiveMessage("Final Result: " + e.Result.Text).GetAwaiter().GetResult();
        }
        else if (e.Result.Reason == ResultReason.NoMatch)
        {
            Clients.Caller.ReceiveMessage("NOMATCH: Speech could not be recognized.", true).GetAwaiter().GetResult();
        }
    }
}

public class AudioInterpreterTextContext
{
    public uint SamplesPerSecond { get; set; }

    public byte BitsPerSample { get; set; } = 16;

    public byte Channel { get; set; } = 1;

    public Dictionary<string, object> Data { get; } = [];
}

public interface ISpeechToTextHub
{
    Task ReceiveMessage(string message, bool isError = false);

    Task ReceivePartialMessage(string message);
}
