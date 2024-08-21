//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System.Net.Http.Formatting;
using System.Net.Http.Headers;
using Newtonsoft.Json;
using Newtonsoft.Json.Converters;

public class CustomVoiceClient
{
    private const string OcpApimSubscriptionKey = "Ocp-Apim-Subscription-Key";
    private const string ApiVersion = "api-version=2023-12-01-preview";

    private readonly string baseUri;

    private readonly HttpClient client;

    public static JsonSerializerSettings ReaderSettings { get; } = new JsonSerializerSettings
    {
        ConstructorHandling = ConstructorHandling.AllowNonPublicDefaultConstructor,
        Converters = new List<JsonConverter> { new StringEnumConverter() { AllowIntegerValues = true } },
        Formatting = Formatting.Indented
    };

    public static JsonSerializerSettings WriterSettings { get; } = new JsonSerializerSettings
    {
        ConstructorHandling = ConstructorHandling.AllowNonPublicDefaultConstructor,
        Converters = new List<JsonConverter> { new StringEnumConverter() { AllowIntegerValues = false } },
        DateFormatString = "yyyy-MM-ddTHH\\:mm\\:ss.fffZ",
        NullValueHandling = NullValueHandling.Ignore,
        Formatting = Formatting.Indented,
        ReferenceLoopHandling = ReferenceLoopHandling.Ignore
    };

    public CustomVoiceClient(string region, string key)
    {
        this.baseUri = $"https://{region}.api.cognitive.microsoft.com/customvoice";

        this.client = new HttpClient();
        client.DefaultRequestHeaders.Add(OcpApimSubscriptionKey, key);
    }


    #region Project Operations

    public async Task<IEnumerable<Project>> GetAllProjectsAsync()
    {
        var projects = new List<Project>();
        var uri = new Uri($"{this.baseUri}/projects?{ApiVersion}");
        do
        {
            var response = await this.client.GetAsync(uri).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                await HandleErrorResponse(response);
            }

            var pagedProjects = await response.Content.ReadAsAsync<PaginatedResources<Project>>().ConfigureAwait(false);
            projects.AddRange(pagedProjects.Value);
            uri = pagedProjects.NextLink;
        }
        while (uri != null);

        return projects;
    }

    public async Task<Project> GetProjectAsync(string projectId)
    {
        var uri = new Uri($"{this.baseUri}/projects/{projectId}?{ApiVersion}");
        var response = await this.client.GetAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        return await response.Content.ReadAsAsync<Project>().ConfigureAwait(false);
    }

    public async Task<Project> CreateProjectAsync(
        string projectId,
        ProjectKind projectKind,
        string description)
    {
        var uri = new Uri($"{this.baseUri}/projects/{projectId}?{ApiVersion}");

        var projectDefinition = new Project
        {
            Description = description,
            Kind = projectKind,
        };

        var content = new StringContent(JsonConvert.SerializeObject(projectDefinition, WriterSettings));
        content.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;
        var response = await this.client.PutAsync(uri, content).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var project = await response.Content.ReadAsAsync<Project>().ConfigureAwait(false);

        return project;
    }

    public async Task DeleteProjectAsync(string projectId)
    {
        var uri = new Uri($"{this.baseUri}/projects/{projectId}?{ApiVersion}");
        var response = await this.client.DeleteAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    #endregion

    #region Consent Operations

    public async Task<IEnumerable<Consent>> GetAllConsentsAsync()
    {
        var consents = new List<Consent>();
        var uri = new Uri($"{this.baseUri}/consents?{ApiVersion}");
        do
        {
            var response = await this.client.GetAsync(uri).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                await HandleErrorResponse(response);
            }

            var pagedConsents = await response.Content.ReadAsAsync<PaginatedResources<Consent>>().ConfigureAwait(false);
            consents.AddRange(pagedConsents.Value);
            uri = pagedConsents.NextLink;
        }
        while (uri != null);

        return consents;
    }

    public async Task<Consent> GetConsentAsync(string consentId)
    {
        var uri = new Uri($"{this.baseUri}/consents/{consentId}?{ApiVersion}");
        var response = await this.client.GetAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var consent = await response.Content.ReadAsAsync<Consent>().ConfigureAwait(false);

        return consent;
    }

    public async Task<Consent> CreateConsentAsync(
        string consentId,
        string projectId,
        string voiceTalentName,
        string companyName,
        string locale,
        Uri audioUrl)
    {
        var uri = new Uri($"{this.baseUri}/consents/{consentId}?{ApiVersion}");

        var consentDefinition = new Consent
        {
            ProjectId = projectId,
            VoiceTalentName = voiceTalentName,
            CompanyName = companyName,
            Locale = locale,
            AudioUrl = audioUrl
        };

        var content = new StringContent(JsonConvert.SerializeObject(consentDefinition, WriterSettings));
        content.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;
        var response = await this.client.PutAsync(uri, content).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var consent = await response.Content.ReadAsAsync<Consent>().ConfigureAwait(false);

        // Wait for consent ready. It takes 2-3 seconds.
        while (consent.Status != Status.Succeeded && consent.Status != Status.Failed)
        {
            await Task.Delay(1000).ConfigureAwait(false);
            consent = await this.GetConsentAsync(consentId).ConfigureAwait(false);
        }

        return consent;
    }

    public async Task<Consent> UploadConsentAsync(
        string consentId,
        string projectId,
        string voiceTalentName,
        string companyName,
        string locale,
        string audioFilePath)
    {
        var uri = new Uri($"{this.baseUri}/consents/{consentId}?{ApiVersion}");

        var audioDataConent = new StreamContent(File.OpenRead(audioFilePath));
        audioDataConent.Headers.ContentType = new MediaTypeHeaderValue("audio/wav");

        var multipartContent = new MultipartFormDataContent
        {
            { new StringContent(projectId), "projectId" },
            { new StringContent(voiceTalentName), "voiceTalentName" },
            { new StringContent(companyName), "companyName" },
            { new StringContent(locale), "locale" },
            { audioDataConent, "audioData", Path.GetFileName(audioFilePath) }
        };

        var response = await this.client.PostAsync(uri, multipartContent).ConfigureAwait(false);

        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var consent = await response.Content.ReadAsAsync<Consent>().ConfigureAwait(false);

        // Wait for consent ready. It takes 2-3 seconds.
        while (consent.Status != Status.Succeeded && consent.Status != Status.Failed)
        {
            await Task.Delay(1000).ConfigureAwait(false);
            consent = await this.GetConsentAsync(consentId).ConfigureAwait(false);
        }

        return consent;
    }

    public async Task DeleteConsentAsync(string consentId)
    {
        var uri = new Uri($"{this.baseUri}/consents/{consentId}?{ApiVersion}");
        var response = await this.client.DeleteAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    #endregion

    #region TrainingSet Operations

    public async Task<IEnumerable<TrainingSet>> GetAllTrainingSetsAsync()
    {
        var trainingSets = new List<TrainingSet>();
        var uri = new Uri($"{this.baseUri}/trainingsets?{ApiVersion}");
        do
        {
            var response = await this.client.GetAsync(uri).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                await HandleErrorResponse(response);
            }

            var pagedTrainingSets = await response.Content.ReadAsAsync<PaginatedResources<TrainingSet>>().ConfigureAwait(false);
            trainingSets.AddRange(pagedTrainingSets.Value);
            uri = pagedTrainingSets.NextLink;
        }
        while (uri != null);

        return trainingSets;
    }

    public async Task<TrainingSet> GetTrainingSetAsync(string trainingSetId)
    {
        var uri = new Uri($"{this.baseUri}/trainingsets/{trainingSetId}?{ApiVersion}");
        var response = await this.client.GetAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var trainingSet = await response.Content.ReadAsAsync<TrainingSet>().ConfigureAwait(false);

        return trainingSet;
    }

    public async Task<TrainingSet> CreateTrainingSetAsync(
        string trainingSetId,
        string projectId,
        string description,
        string locale)
    {
        var uri = new Uri($"{this.baseUri}/trainingsets/{trainingSetId}?{ApiVersion}");

        var trainingSetDefinition = new TrainingSet
        {
            ProjectId = projectId,
            Description = description,
            Locale = locale
        };

        var content = new StringContent(JsonConvert.SerializeObject(trainingSetDefinition, WriterSettings));
        content.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;
        var response = await this.client.PutAsync(uri, content).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var trainingSet = await response.Content.ReadAsAsync<TrainingSet>().ConfigureAwait(false);

        return trainingSet;
    }

    public async Task DeleteTrainingSetAsync(string trainingSetId)
    {
        var uri = new Uri($"{this.baseUri}/trainingsets/{trainingSetId}?{ApiVersion}");
        var response = await this.client.DeleteAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    public async Task UploadDataThroughAzureBlobAsync(
        string trainingSetId,
        DatasetKind kind,
        AzureBlobContentSource audios,
        AzureBlobContentSource scripts)
    {
        var uri = new Uri($"{this.baseUri}/trainingsets/{trainingSetId}:upload?{ApiVersion}");

        var datasetDefinition = new Dataset
        {
            Kind = kind,
            Audios = audios,
            Scripts = scripts
        };

        var content = new StringContent(JsonConvert.SerializeObject(datasetDefinition, WriterSettings));
        content.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;
        var response = await this.client.PostAsync(uri, content).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    #endregion

    #region Model Operations

    public async Task<IEnumerable<Model>> GetAllModelsAsync()
    {
        var models = new List<Model>();
        var uri = new Uri($"{this.baseUri}/models?{ApiVersion}");
        do
        {
            var response = await this.client.GetAsync(uri).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                await HandleErrorResponse(response);
            }

            var pagedModels = await response.Content.ReadAsAsync<PaginatedResources<Model>>().ConfigureAwait(false);
            models.AddRange(pagedModels.Value);
            uri = pagedModels.NextLink;
        }
        while (uri != null);

        return models;
    }

    public async Task<Model> GetModelAsync(string modelId)
    {
        var uri = new Uri($"{this.baseUri}/models/{modelId}?{ApiVersion}");
        var response = await this.client.GetAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var model = await response.Content.ReadAsAsync<Model>().ConfigureAwait(false);

        return model;
    }

    public async Task<Model> CreateModelAsync(
        string modelId,
        string projectId,
        string description,
        string consentId,
        string trainingSetId,
        string voiceName,
        RecipeKind recipeKind,
        string locale,
        ModelProperties properties)
    {
        if (recipeKind == RecipeKind.Default && !string.IsNullOrEmpty(locale))
        {
            throw new ArgumentException("Do not need 'locale' parameter for Default recipe.");
        }

        if (recipeKind == RecipeKind.CrossLingual && string.IsNullOrEmpty(locale))
        {
            throw new ArgumentException("Need 'locale' parameter to specify the locale of voice model for CrossLingual recipe.");
        }

        if (recipeKind == RecipeKind.MultiStyle && properties == null)
        {
            throw new ArgumentException("Need 'properties' parameter to specify style for MultiStyle recipe.");
        }

        var uri = new Uri($"{this.baseUri}/models/{modelId}?{ApiVersion}");

        var modelDefinition = new Model
        {
            ProjectId = projectId,
            Description = description,
            ConsentId = consentId,
            TrainingSetId = trainingSetId,
            VoiceName = voiceName,
            Recipe = new Recipe { Kind = recipeKind },
            Locale = locale,
            Properties = properties
        };

        var content = new StringContent(JsonConvert.SerializeObject(modelDefinition, WriterSettings));
        content.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;
        var response = await this.client.PutAsync(uri, content).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var model = await response.Content.ReadAsAsync<Model>().ConfigureAwait(false);

        return model;
    }

    public async Task DeleteModelAsync(string modelId)
    {
        var uri = new Uri($"{this.baseUri}/models/{modelId}?{ApiVersion}");
        var response = await this.client.DeleteAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    #endregion

    #region PersonalVoice Operations

    public async Task<IEnumerable<PersonalVoice>> GetAllPersonalVoicesAsync()
    {
        var personalVoices = new List<PersonalVoice>();
        var uri = new Uri($"{this.baseUri}/personalvoices?{ApiVersion}");
        do
        {
            var response = await this.client.GetAsync(uri).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                await HandleErrorResponse(response);
            }

            var pagedPersonalVoices = await response.Content.ReadAsAsync<PaginatedResources<PersonalVoice>>().ConfigureAwait(false);
            personalVoices.AddRange(pagedPersonalVoices.Value);
            uri = pagedPersonalVoices.NextLink;
        }
        while (uri != null);

        return personalVoices;
    }

    public async Task<PersonalVoice> GetPersonalVoiceAsync(string personalVoiceId)
    {
        var uri = new Uri($"{this.baseUri}/personalvoices/{personalVoiceId}?{ApiVersion}");
        var response = await this.client.GetAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var personalVoice = await response.Content.ReadAsAsync<PersonalVoice>().ConfigureAwait(false);

        return personalVoice;
    }

    public async Task<PersonalVoice> CreatePersonalVoiceAsync(
        string personalVoiceId,
        string projectId,
        string description,
        string consentId,
        string audiosFolder)
    {
        if (!Directory.Exists(audiosFolder))
        {
            throw new ArgumentException($"Can't find '{audiosFolder}'.");
        }

        var uri = new Uri($"{this.baseUri}/personalvoices/{personalVoiceId}?{ApiVersion}");

        var multipartContent = new MultipartFormDataContent
        {
            { new StringContent(projectId), "projectId" },
            { new StringContent(consentId), "consentId" },
            { new StringContent(description), "description" }
        };

        foreach (var file in Directory.EnumerateFiles(audiosFolder))
        {
            multipartContent.Add(new StreamContent(File.OpenRead(file)), "audioData", Path.GetFileName(file));
        }

        var response = await this.client.PostAsync(uri, multipartContent).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var personalVoice = await response.Content.ReadAsAsync<PersonalVoice>().ConfigureAwait(false);

        // Wait for consent ready. It takes 2-3 seconds.
        while (personalVoice.Status != Status.Succeeded && personalVoice.Status != Status.Failed)
        {
            await Task.Delay(1000).ConfigureAwait(false);
            personalVoice = await this.GetPersonalVoiceAsync(personalVoiceId).ConfigureAwait(false);
        }

        return personalVoice;
    }

    public async Task DeletePersonalVoiceAsync(string personalVoiceId)
    {
        var uri = new Uri($"{this.baseUri}/personalvoices/{personalVoiceId}?{ApiVersion}");
        var response = await this.client.DeleteAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    #endregion

    #region Endpoint Operations

    public async Task<IEnumerable<Endpoint>> GetAllEndpointsAsync()
    {
        var endpoints = new List<Endpoint>();
        var uri = new Uri($"{this.baseUri}/endpoints?{ApiVersion}");
        do
        {
            var response = await this.client.GetAsync(uri).ConfigureAwait(false);

            if (!response.IsSuccessStatusCode)
            {
                await HandleErrorResponse(response);
            }

            var pagedEndpoints = await response.Content.ReadAsAsync<PaginatedResources<Endpoint>>().ConfigureAwait(false);
            endpoints.AddRange(pagedEndpoints.Value);
            uri = pagedEndpoints.NextLink;
        }
        while (uri != null);

        return endpoints;
    }

    public async Task<Endpoint> GetEndpointAsync(Guid endpointId)
    {
        var uri = new Uri($"{this.baseUri}/endpoints/{endpointId}?{ApiVersion}");
        var response = await this.client.GetAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var endpoint = await response.Content.ReadAsAsync<Endpoint>().ConfigureAwait(false);

        return endpoint;
    }

    public async Task<Endpoint> CreateEndpointAsync(
        Guid endpointId,
        string projectId,
        string description,
        string modelId)
    {
        var uri = new Uri($"{this.baseUri}/endpoints/{endpointId}?{ApiVersion}");

        var endpointDefinition = new Endpoint
        {
            ProjectId = projectId,
            Description = description,
            ModelId = modelId
        };

        var content = new StringContent(JsonConvert.SerializeObject(endpointDefinition, WriterSettings));
        content.Headers.ContentType = JsonMediaTypeFormatter.DefaultMediaType;
        var response = await this.client.PutAsync(uri, content).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }

        var endpoint = await response.Content.ReadAsAsync<Endpoint>().ConfigureAwait(false);

        return endpoint;
    }

    public async Task DeleteEndpointAsync(Guid endpointId)
    {
        var uri = new Uri($"{this.baseUri}/endpoints/{endpointId}?{ApiVersion}");
        var response = await this.client.DeleteAsync(uri).ConfigureAwait(false);
        if (!response.IsSuccessStatusCode)
        {
            await HandleErrorResponse(response);
        }
    }

    #endregion

    private static async Task HandleErrorResponse(HttpResponseMessage response)
    {
        var content = await response.Content.ReadAsStringAsync().ConfigureAwait(false);
        throw new HttpRequestException($"Request failed with status code {response.StatusCode}. {content}");
    }
}