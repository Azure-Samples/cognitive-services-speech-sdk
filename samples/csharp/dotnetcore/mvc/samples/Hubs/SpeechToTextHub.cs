using Concentus;
using Concentus.Oggfile;
using Microsoft.AspNetCore.SignalR;
using Microsoft.CognitiveServices.Speech;
using Microsoft.CognitiveServices.Speech.Audio;
using System.Text;

namespace samples.Hubs;

public sealed class SpeechToTextHub : Hub<ISpeechToTextHub>
{
    public const string InvalidConfigErrorMessage = "Please provide your service key and region in the configuration provider (e.g., in appsettings.json).";

    private const uint _samplesPerSecond = 16000;
    private const byte _channels = 1;

    private readonly ILogger _logger;
    private readonly IConfiguration _configuration;

    public SpeechToTextHub(
        ILogger<SpeechToTextHub> logger,
        IConfiguration configuration)
    {
        _logger = logger;
        _configuration = configuration;
    }

    public async Task ContinuousRecognition(IAsyncEnumerable<string> stream, uint samplePerSecond)
    {
        var speechConfig = _configuration.GetSpeechConfig();

        if (speechConfig == null)
        {
            await Clients.Caller.ReceiveMessage(InvalidConfigErrorMessage, true);

            return;
        }

        using var format = AudioStreamFormat.GetWaveFormatPCM(_samplesPerSecond, 16, _channels);
        using var audioInputStream = AudioInputStream.CreatePushStream(format);
        using var audioConfig = AudioConfig.FromStreamInput(audioInputStream);
        using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);

        recognizer.Recognizing += RecognizerRecognizing;
        recognizer.Recognized += RecognizerRecognized;
        recognizer.SessionStarted += RecognizerStarted;
        recognizer.SessionStopped += RecognizerStopped;
        recognizer.Canceled += RecognizerCanceled;

        await recognizer.StartContinuousRecognitionAsync();

        await PushPCMAudioAsync(audioInputStream, stream);

        await recognizer.StopContinuousRecognitionAsync();

        recognizer.Recognizing -= RecognizerRecognizing;
        recognizer.Recognized -= RecognizerRecognized;
        recognizer.SessionStarted -= RecognizerStarted;
        recognizer.SessionStopped -= RecognizerStopped;
        recognizer.Canceled -= RecognizerCanceled;
    }

    public async Task RecognizeOnce(IAsyncEnumerable<string> stream)
    {
        var speechConfig = _configuration.GetSpeechConfig();

        if (speechConfig == null)
        {
            await Clients.Caller.ReceiveMessage(InvalidConfigErrorMessage, true);

            return;
        }

        await Clients.Caller.SessionStarted();

        var format = AudioStreamFormat.GetCompressedFormat(AudioStreamContainerFormat.OGG_OPUS);
        var audioInputStream = AudioInputStream.CreatePushStream(format);

        await PopulateAudioAsync(audioInputStream, stream);

        try
        {
            using var audioConfig = AudioConfig.FromStreamInput(audioInputStream);
            using var recognizer = new SpeechRecognizer(speechConfig, audioConfig);

            var result = await recognizer.RecognizeOnceAsync();

            if (result.Reason == ResultReason.RecognizedSpeech)
            {
                await Clients.Caller.ReceiveMessage(result.Text);
            }
            else if (result.Reason == ResultReason.NoMatch)
            {
                await Clients.Caller.ReceiveMessage("Speech could not be recognized. Please speak clearly into the microphone.");
            }
            else
            {
                var cancellation = CancellationDetails.FromResult(result);

                await Clients.Caller.ReceiveMessage("Speech service is not available. Please try again later.", true);

                if (cancellation.Reason == CancellationReason.Error)
                {
                    await Clients.Caller.ReceiveMessage($"Failed to recognize the incoming speech. Reason: {cancellation.Reason}. Error code: {cancellation.ErrorCode}. Error details: {cancellation.ErrorDetails}.", true);

                    return;
                }

                await Clients.Caller.ReceiveMessage($"Failed to recognize the incoming speech. Reason {cancellation.Reason}.", true);
            }
        }
        finally
        {
            audioInputStream.Dispose();
            await Clients.Caller.SessionStopped();
        }
    }

    private static async Task PushPCMAudioAsync(PushAudioInputStream audioInputStream, IAsyncEnumerable<string> stream)
    {
        using var decoder = OpusCodecFactory.CreateDecoder((int)_samplesPerSecond, _channels);

        await foreach (var base64String in stream)
        {
            using var memoryStream = new MemoryStream();

            var chunk = Convert.FromBase64String(base64String);
            memoryStream.Write(chunk, 0, chunk.Length);

            // Reset the position of the stream.
            memoryStream.Seek(0, SeekOrigin.Begin);

            var oggStream = new OpusOggReadStream(decoder, memoryStream);

            while (oggStream.HasNextPacket)
            {
                // Decode the next packet.
                var packet = oggStream.DecodeNextPacket();
                if (packet == null)
                {
                    continue;
                }

                for (int i = 0; i < packet.Length; i++)
                {
                    var bytes = BitConverter.GetBytes(packet[i]);
                    audioInputStream.Write(bytes, bytes.Length);
                }
            }
        }
    }

    private static async Task PopulateAudioAsync(PushAudioInputStream audioInputStream, IAsyncEnumerable<string> stream)
    {
        using var memoryStream = new MemoryStream();

        await foreach (var base64String in stream)
        {
            var chunk = Convert.FromBase64String(base64String);
            memoryStream.Write(chunk, 0, chunk.Length);
        }

        memoryStream.Seek(0, SeekOrigin.Begin);

        using var binaryReader = new BinaryReader(memoryStream, Encoding.UTF8, leaveOpen: true);
        byte[] readBytes;
        do
        {
            readBytes = binaryReader.ReadBytes(1024);
            audioInputStream.Write(readBytes);
        } while (readBytes.Length > 0);
    }

    private void RecognizerStopped(object sender, SessionEventArgs e)
    {
        Clients.Caller.SessionStopped();
    }

    private void RecognizerStarted(object sender, SessionEventArgs e)
    {
        Clients.Caller.SessionStarted();
    }

    private void RecognizerCanceled(object sender, SpeechRecognitionCanceledEventArgs e)
    {
        var cancellation = CancellationDetails.FromResult(e.Result);

        if (cancellation.Reason == CancellationReason.Error)
        {
            Clients.Caller.ReceiveMessage($"The request to recognize the incoming speech was canceled. Reason: {cancellation.Reason}. Error code: {cancellation.ErrorCode}. Error details: {cancellation.ErrorDetails}.", true).GetAwaiter().GetResult(); ;

            return;
        }

        Clients.Caller.ReceiveMessage($"The request to recognize the incoming speech was canceled. Reason {cancellation.Reason}.", true).GetAwaiter().GetResult(); ;
    }

    private void RecognizerRecognizing(object sender, SpeechRecognitionEventArgs e)
    {
        _logger.LogDebug("RecognizerRecognizing. Result: {Result}. Reason {Reason}", e.Result?.Text, e.Result?.Reason);

        if (e.Result.Reason == ResultReason.RecognizingSpeech)
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
            Clients.Caller.ReceiveMessage(e.Result.Text).GetAwaiter().GetResult();
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

    Task SessionStopped();

    Task SessionStarted();
}
