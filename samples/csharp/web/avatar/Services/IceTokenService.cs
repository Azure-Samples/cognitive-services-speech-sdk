using Avatar.Models;
using Microsoft.Extensions.Options;

namespace Avatar.Services
{
    public class IceTokenService
    {
        private readonly HttpClient _httpClient;
        private readonly ClientSettings _clientSettings;

        public IceTokenService(HttpClient httpClient, IOptions<ClientSettings> clientSettings)
        {
            _httpClient = httpClient;
            _clientSettings = clientSettings.Value;
        }

        public async Task RefreshIceTokenAsync()
        {
            var url = !string.IsNullOrEmpty(_clientSettings.SpeechPrivateEndpoint)
                ? $"{_clientSettings.SpeechPrivateEndpoint}/tts/cognitiveservices/avatar/relay/token/v1"
                : $"https://{_clientSettings.SpeechRegion}.tts.speech.microsoft.com/cognitiveservices/avatar/relay/token/v1";

            var request = new HttpRequestMessage(HttpMethod.Get, url);
            request.Headers.Add("Ocp-Apim-Subscription-Key", _clientSettings.SpeechKey);

            var response = await _httpClient.SendAsync(request);
            response.EnsureSuccessStatusCode();

            GlobalVariables.IceToken = await response.Content.ReadAsStringAsync();
        }
    }

}
