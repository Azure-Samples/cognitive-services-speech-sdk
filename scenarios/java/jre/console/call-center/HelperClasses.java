//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.*;

import com.google.gson.JsonArray;
import com.google.gson.JsonObject;

// This describes the body of our request to the Create Transcription REST API.
// Create Transcription REST API request and response JSON sample and schema:
// https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
class TranscriptionRequestProperties
{
    public boolean diarizationEnabled;
    public String timeToLive;
    public TranscriptionRequestProperties(boolean diarizationEnabled, String timeToLive)
    {
        this.diarizationEnabled = diarizationEnabled;
        this.timeToLive = timeToLive;
    }
}

class TranscriptionRequest
{
    public String[] contentUrls;
    public TranscriptionRequestProperties properties;
    public String locale;
    public String displayName;
    public TranscriptionRequest(String[] contentUrls, TranscriptionRequestProperties properties, String locale, String displayName)
    {
        this.contentUrls = contentUrls;
        this.properties = properties;
        this.locale = locale;
        this.displayName = displayName;
    }
}

// This contains the data we want from a transcription.
class TranscriptionPhrase
{
    public int id;
    public String text;
    public String itn;
    public String lexical;
    public int speakerNumber;
    public String offset;
    public double offsetInTicks;
    public TranscriptionPhrase(int id, String text, String itn, String lexical, int speakerNumber, String offset, double offsetInTicks)
    {
        this.id = id;
        this.text = text;
        this.itn = itn;
        this.lexical = lexical;
        this.speakerNumber = speakerNumber;
        this.offset = offset;
        this.offsetInTicks = offsetInTicks;
    }
}

// Part of the body of our request to the Analyze Text REST API.
// Analyze Text REST API request and response JSON sample and schema:
// https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeText
class SentimentRequestDocument
{
    public int id;
    public String language;
    public String text;
    public SentimentRequestDocument(int id, String language, String text)
    {
        this.id = id;
        this.language = language;
        this.text = text;
    }
}

// Part of the body of our request to the Analyze Text API.
class SentimentRequestDocuments
{
    public List<SentimentRequestDocument> documents;
    public SentimentRequestDocuments(List<SentimentRequestDocument> documents)
    {
        this.documents = documents;
    }
}

// This describes the body of our request to the Analyze Text API.
class SentimentRequest
{
    final public String kind = "SentimentAnalysis";
    public SentimentRequestDocuments analysisInput;
    public SentimentRequest(List<SentimentRequestDocument> documents)
    {
        this.analysisInput = new SentimentRequestDocuments(documents);
    }
}

// Part of the response from the Analyze Text API.
class SentimentAnalysisResult
{
    public int speakerNumber;
    public double offsetInTicks;
    public JsonObject document;
    public SentimentAnalysisResult(int speakerNumber, double offsetInTicks, JsonObject document)
    {
        this.speakerNumber = speakerNumber;
        this.offsetInTicks = offsetInTicks;
        this.document = document;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
// Analyze Conversation REST API request and response JSON sample and schema:
// https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_SubmitJob
class ConversationItem
{
    public int id;
    public String text;
    public String itn;
    public String lexical;
    public String role;
    public int participantId;
    public ConversationItem(int id, String text, String itn, String lexical, String role, int participantId)
    {
        this.id = id;
        this.text = text;
        this.itn = itn;
        this.lexical = lexical;
        this.role = role;
        this.participantId = participantId;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class Conversation
{
    public String id;
    public String language;
    public String modality;
    public List<ConversationItem> conversationItems;
    public Conversation(String id, String language, String modality, List<ConversationItem> conversationItems)
    {
        this.id = id;
        this.language = language;
        this.modality = modality;
        this.conversationItems = conversationItems;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class Conversations
{
    public List<Conversation> conversations;
    public Conversations(List<Conversation> conversations)
    {
        this.conversations = conversations;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class ConversationAnalysisRequestTask
{
    public String taskName;
    public String kind;
    public ConversationAnalysisRequestTask(String taskName, String kind)
    {
        this.taskName = taskName;
        this.kind = kind;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class ConversationalSummarizationTaskParameters
{
    final public String modelVersion = CallCenter.conversationSummaryModelVersion;
    public List<String> summaryAspects;
    public ConversationalSummarizationTaskParameters(List<String> summaryAspects)
    {
        this.summaryAspects = summaryAspects;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class ConversationalSummarizationTask extends ConversationAnalysisRequestTask
{
    ConversationalSummarizationTaskParameters parameters;
    public ConversationalSummarizationTask(String taskName, ConversationalSummarizationTaskParameters parameters)
    {
        super(taskName, "ConversationalSummarizationTask");
        this.parameters = parameters;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class ConversationalPIITaskParameters
{
    public List<String> piiCategories;
    public boolean includeAudioRedaction;
    public String redactionSource;
    final public String modelVersion = CallCenter.conversationSummaryModelVersion;
    public boolean loggingOptOut;
    public ConversationalPIITaskParameters(List<String> piiCategories,
    boolean includeAudioRedaction, String redactionSource, boolean loggingOptOut)
    {
        this.piiCategories = piiCategories;
        this.includeAudioRedaction = includeAudioRedaction;
        this.redactionSource = redactionSource;
        this.loggingOptOut = loggingOptOut;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class ConversationalPIITask extends ConversationAnalysisRequestTask
{
    ConversationalPIITaskParameters parameters;
    public ConversationalPIITask(String taskName, ConversationalPIITaskParameters parameters)
    {
        super(taskName, "ConversationalPIITask");
        this.parameters = parameters;
    }
}

// This describes the body of our request to the Analyze Conversation REST API.
class ConversationAnalysisRequest
{
    public String displayName;
    public Conversations analysisInput;
    public List<ConversationAnalysisRequestTask> tasks;
    public ConversationAnalysisRequest(String displayName, Conversations analysisInput, List<ConversationAnalysisRequestTask> tasks)
    {
        this.displayName = displayName;
        this.analysisInput = analysisInput;
        this.tasks = tasks;
    }
}

// This contains part of the data we want from a conversation analysis (summary).
class ConversationAspectAndSummary
{
    public String aspect;
    public String summary;
    public ConversationAspectAndSummary(String aspect, String summary)
    {
        this.aspect = aspect;
        this.summary = summary;
    }
}

// This contains part of the data we want from a conversation analysis (PII analysis).
class PIICategoryAndText
{
    public String category;
    public String text;
    public PIICategoryAndText(String category, String text)
    {
        this.category = category;
        this.text = text;
    }
}

// This contains the data we want from a conversation analysis.
class ConversationAnalysisForSimpleOutput
{
    public List<ConversationAspectAndSummary> summary;
    public List<List<PIICategoryAndText>> PIIAnalysis;
    public ConversationAnalysisForSimpleOutput(List<ConversationAspectAndSummary> summary, List<List<PIICategoryAndText>> PIIAnalysis)
    {
        this.summary = summary;
        this.PIIAnalysis = PIIAnalysis;
    }
}

class CombinedRedactedContent
{
    public int channel;
    public StringBuilder lexical;
    public StringBuilder itn;
    public StringBuilder display;
    
    public CombinedRedactedContent(int channel)
    {
        this.channel = channel;
        this.lexical = new StringBuilder();
        this.itn = new StringBuilder();
        this.display = new StringBuilder();
    }
}

class CombinedResults
{
    public JsonObject conversationSummaryResults;
    public JsonObject conversationPiiResults;
    public CombinedResults(JsonObject conversationSummaryResults, JsonObject conversationPiiResults)
    {
        this.conversationSummaryResults = conversationSummaryResults;
        this.conversationPiiResults = conversationPiiResults;
    }
}

class FullOutput
{
    public JsonObject transcription;
    public CombinedResults conversationAnalyticsResults;
    public FullOutput(JsonObject transcription, CombinedResults conversationAnalyticsResults)
    {
        this.transcription = transcription;
        this.conversationAnalyticsResults = conversationAnalyticsResults;
    }
}
