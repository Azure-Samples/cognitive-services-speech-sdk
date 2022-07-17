//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

// This describes the body of our request to the Create Transcription REST API.
// Create Transcription REST API request and response JSON sample and schema:
// https://westus.dev.cognitive.microsoft.com/docs/services/speech-to-text-api-v3-0/operations/CreateTranscription
public class TranscriptionRequest {
    public String[] contentUrls;
    public JsonObject properties;
    public String locale;
    public String displayName;
    public TranscriptionRequest(String[] contentUrls, JsonObject properties, String locale, String displayName) {
        this.contentUrls = contentUrls;
        this.properties = properties;
        this.locale = locale;
        this.displayName = displayName;
    }
}

// This contains the data we want from a transcription.
public class PhraseAndSpeaker {
    public String phrase;
    public int speakerID;
    public PhraseAndSpeaker(String phrase, int speakerID) {
        this.phrase = phrase;
        this.speakerID = speakerID;
    }
}

// Part of the body of our request to the Analyze Text REST API.
// Analyze Text REST API request and response JSON sample and schema:
// https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeText
public class Document {
    public int id;
    public String language;
    public String text;
    public Document(int id, String language, String text) {
        this.id = id;
        this.language = language;
        this.text = text;
    }
}

// Part of the body of our request to the Analyze Text API.
public class SentimentRequestDocuments {
    public List<Document> documents;
    public SentimentRequestDocuments(List<Document> documents) {
        this.documents = documents;
    }
}

// This describes the body of our request to the Analyze Text API.
public class SentimentRequest {
    final public String kind = "SentimentAnalysis";
    public SentimentRequestDocuments analysisInput;
    public SentimentRequest(List<Document> documents) {
        this.analysisInput = new SentimentRequestDocuments(documents);
    }
}

// This contains the data we want from a text analysis.
public class SentimentResult {
    public int id;
    public String sentiment;
    public SentimentResult(int id, String sentiment) {
        this.id = id;
        this.sentiment = sentiment;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
// Analyze Conversation REST API request and response JSON sample and schema:
// https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_SubmitJob
public class ConversationItem {
    public int id;
    public String text;
    public String role;
    public int participantId;
    public ConversationItem(int id, String text, String role, int participantId) {
        this.id = id;
        this.text = text;
        this.role = role;
        this.participantId = participantId;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
public class Conversation {
    public String id;
    public String language;
    public String modality;
    public List<ConversationItem> conversationItems;
    public Conversation(String id, String language, String modality, List<ConversationItem> conversationItems) {
        this.id = id;
        this.language = language;
        this.modality = modality;
        this.conversationItems = conversationItems;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
public class Conversations {
    public List<Conversation> conversations;
    public Conversations(List<Conversation> conversations) {
        this.conversations = conversations;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
public class ConversationalSummarizationTaskParameters {
    final public String modelVersion = CallCenter.conversationSummaryModelVersion;
    public List<String> summaryAspects;
    public ConversationalSummarizationTaskParameters(List<String> summaryAspects) {
        this.summaryAspects = summaryAspects;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
public class ConversationAnalysisRequestTask {
    public String taskName;
    public String kind;
    public ConversationAnalysisRequestTask(String taskName, String kind) {
        this.taskName = taskName;
        this.kind = kind;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
public class ConversationalSummarizationTask extends ConversationAnalysisRequestTask {
    ConversationalSummarizationTaskParameters parameters;
    public ConversationalSummarizationTask(String taskName, ConversationalSummarizationTaskParameters parameters) {
        super(taskName, "ConversationalSummarizationTask");
        this.parameters = parameters;
    }
}

// This describes the body of our request to the Analyze Conversation REST API.
public class ConversationAnalysisRequest {
    public String displayName;
    public Conversations analysisInput;
    public List<ConversationAnalysisRequestTask> tasks;
    public ConversationAnalysisRequest(String displayName, Conversations analysisInput, List<ConversationAnalysisRequestTask> tasks) {
        this.displayName = displayName;
        this.analysisInput = analysisInput;
        this.tasks = tasks;
    }
}

// This contains part of the data we want from a conversation analysis (summary).
public class ConversationAspectAndSummary {
    public String aspect;
    public String summary;
    public ConversationAspectAndSummary(String aspect, String summary) {
        this.aspect = aspect;
        this.summary = summary;
    }
}

// This contains part of the data we want from a conversation analysis (PII analysis).
public class PIICategoryAndText {
    public String category;
    public String text;
    public PIICategoryAndText(String category, String text) {
        this.category = category;
        this.text = text;
    }
}

// This contains the data we want from a conversation analysis.
public class ConversationAnalysisResult
{
    public List<ConversationAspectAndSummary> summary;
    public List<List<PIICategoryAndText>> PIIAnalysis;
    public ConversationAnalysisResult(List<ConversationAspectAndSummary> summary, List<List<PIICategoryAndText>> PIIAnalysis)
    {
        this.summary = summary;
        this.PIIAnalysis = PIIAnalysis;
    }
}
