//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Newtonsoft.Json;
using System.Net.Http.Formatting;

public class BatchSynthesisClient
{
    private const string OcpApimSubscriptionKey = "Ocp-Apim-Subscription-Key";

    private readonly string hostName;
    private readonly string baseUri;
    private readonly string subscriptionKey;

    private readonly HttpClient client;

    public BatchSynthesisClient(string hostName, string key)
    {
        this.hostName = hostName;
        this.subscriptionKey = key;
        this.baseUri = $"{this.hostName}/api/texttospeech/3.1-preview1/batchsynthesis";

        this.client = new HttpClient();
        client.DefaultRequestHeaders.Add(OcpApimSubscriptionKey, this.subscriptionKey);
    }

    public async Task<IEnumerable<BatchSynthesis>> GetAllSynthesesAsync()
    {
        var syntheses = new List<BatchSynthesis>();
        var uri = new Uri(this.baseUri);
        do
        {
            var response = await this.client.GetAsync(uri).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                await HandleErrorResponse(response);
                return syntheses;
            }

            var pagedSyntheses = await response.Content.ReadAsAsync<PaginatedResults<BatchSynthesis>>().ConfigureAwait(false);
            syntheses.AddRange(pagedSyntheses.Values);
            uri = pagedSyntheses.NextLink;
        }
        while (uri != null);

        return syntheses;
    }

    public async Task<BatchSynthesis> GetSynthesisAsync(Guid id)
    {
        var uri = new Uri(this.baseUri + $"/{id}");
        var response = await this.client.GetAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
            return null;
        }

        return await response.Content.ReadAsAsync<BatchSynthesis>().ConfigureAwait(false);
    }

    public async Task DeleteSynthesisAsync(Guid id)
    {
        var uri = new Uri(this.baseUri + $"/{id}");
        var response = await this.client.DeleteAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    public async Task<Uri> CreateSynthesisAsync(
        string voiceName,
        string displayName,
        string description,
        string script,
        bool isSsml)
    {
        var uri = new Uri(this.baseUri);

        var batchSynthesis = new BatchSynthesis
        {
            DisplayName = displayName,
            Description = description,
            TextType = isSsml ? "Ssml" : "PlainText",
            SynthesisConfig = new BatchSynthesisConfig
            {
                Voice = voiceName
            },
            Inputs = new List<BatchSynthesisInputDefinition> { new BatchSynthesisInputDefinition { Text = script } }
        };

        StringContent content = new StringContent(JsonConvert.SerializeObject(batchSynthesis));
        content.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;
        var response = await this.client.PostAsync(uri, content).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
            return null;
        }

        var location = response.Headers.GetValues("Location").FirstOrDefault();
        return new Uri(location);

    }

    private static async Task HandleErrorResponse(HttpResponseMessage response)
    {
        var content = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
        Console.WriteLine(content);
    }
}
