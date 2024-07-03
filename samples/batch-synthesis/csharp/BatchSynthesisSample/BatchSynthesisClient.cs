//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Text;
using System.Text.Json;

public class BatchSynthesisClient
{
    private const string OcpApimSubscriptionKey = "Ocp-Apim-Subscription-Key";

    private readonly string hostName;
    private readonly string baseUri;
    private readonly string subscriptionKey;
    private readonly string apiVersion;

    private readonly HttpClient client;

    public BatchSynthesisClient(string hostName, string key, string apiVersion)
    {
        this.hostName = hostName;
        this.subscriptionKey = key;
        this.baseUri = $"{this.hostName}/texttospeech/batchsyntheses";
        this.apiVersion = apiVersion;

        this.client = new HttpClient();
        client.DefaultRequestHeaders.Add(OcpApimSubscriptionKey, this.subscriptionKey);
    }

    public async Task<IEnumerable<BatchSynthesis>> GetAllSynthesesAsync()
    {
        var syntheses = new List<BatchSynthesis>();
        var uri = new Uri($"{this.baseUri}?api-version={this.apiVersion}");

        do
        {
            var response = await this.client.GetAsync(uri).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                await HandleErrorResponse(response);
                return syntheses;
            }

            var pagedSyntheses = await response.Content.ReadAsAsync<PaginatedResults<BatchSynthesis>>().ConfigureAwait(false);
            syntheses.AddRange(pagedSyntheses.Value);
            uri = pagedSyntheses.NextLink;
        }
        while (uri != null);

        return syntheses;
    }

    public async Task<BatchSynthesis> GetSynthesisAsync(string id)
    {
        var uri = new Uri(this.baseUri + $"/{id}?api-version={this.apiVersion}");
        var response = await this.client.GetAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
            return null;
        }

        return await response.Content.ReadAsAsync<BatchSynthesis>().ConfigureAwait(false);
    }

    public async Task DeleteSynthesisAsync(string id)
    {
        var uri = new Uri(this.baseUri + $"/{id}?api-version={this.apiVersion}");
        var response = await this.client.DeleteAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    public async Task CreateSynthesisAsync(
        string id,
        string voiceName,
        string script,
        bool isSsml)
    {
        var uri = new Uri($"{this.baseUri}/{id}?api-version={this.apiVersion}");

        var batchSynthesis = new BatchSynthesis
        {
            InputKind = isSsml ? "Ssml" : "PlainText",
            SynthesisConfig = new SynthesisConfig
            {
                Voice = voiceName
            },
            Inputs = new List<BatchSynthesisInputDefinition> { new BatchSynthesisInputDefinition { Content = script } }
        };

        // Create JsonSerializer instance with configured options
        string jsonString = JsonSerializer.Serialize(batchSynthesis, new JsonSerializerOptions { IgnoreNullValues = true });
        var response = await this.client.PutAsync(uri, new StringContent(jsonString, Encoding.UTF8, "application/json")).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    private static async Task HandleErrorResponse(HttpResponseMessage response)
    {
        var content = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
        Console.WriteLine(content);
        if (response.Headers.TryGetValues("apim-request-id", out var traceRequestId))
        {
            Console.WriteLine($"Trace request ID: {traceRequestId.FirstOrDefault()}.");
        }
    }
}
