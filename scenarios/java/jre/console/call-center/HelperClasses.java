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
class TranscriptionRequest {
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
class PhraseAndSpeaker {
    public String phrase;
    public int speakerNumber;
    public double offset;
    public PhraseAndSpeaker(String phrase, int speakerNumber, double offset) {
        this.phrase = phrase;
        this.speakerNumber = speakerNumber;
        this.offset = offset;
    }
}

// Part of the body of our request to the Analyze Text REST API.
// Analyze Text REST API request and response JSON sample and schema:
// https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeText
class Document {
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
class SentimentRequestDocuments {
    public List<Document> documents;
    public SentimentRequestDocuments(List<Document> documents) {
        this.documents = documents;
    }
}

// This describes the body of our request to the Analyze Text API.
class SentimentRequest {
    final public String kind = "SentimentAnalysis";
    public SentimentRequestDocuments analysisInput;
    public SentimentRequest(List<Document> documents) {
        this.analysisInput = new SentimentRequestDocuments(documents);
    }
}

// Part of the response from the Analyze Text API.
class SentimentResults {
    public JsonArray documents;
    public SentimentResults(JsonArray documents) {
        this.documents = documents;
    }
}

// This describes the response from the Analyze Text API.
class SentimentResponse {
    final public String kind = "SentimentAnalysisResults";
    public SentimentResults results;
    public SentimentResponse(SentimentResults results) {
        this.results = results;
    }
}

// This contains the data we want from a text analysis.
class Sentiment {
    public int id;
    public String sentiment;
    public Sentiment(int id, String sentiment) {
        this.id = id;
        this.sentiment = sentiment;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
// Analyze Conversation REST API request and response JSON sample and schema:
// https://westus2.dev.cognitive.microsoft.com/docs/services/Language-2022-05-15-preview/operations/AnalyzeConversation_SubmitJob
class ConversationItem {
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
class Conversation {
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
class Conversations {
    public List<Conversation> conversations;
    public Conversations(List<Conversation> conversations) {
        this.conversations = conversations;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class ConversationalSummarizationTaskParameters {
    final public String modelVersion = CallCenter.conversationSummaryModelVersion;
    public List<String> summaryAspects;
    public ConversationalSummarizationTaskParameters(List<String> summaryAspects) {
        this.summaryAspects = summaryAspects;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class ConversationAnalysisRequestTask {
    public String taskName;
    public String kind;
    public ConversationAnalysisRequestTask(String taskName, String kind) {
        this.taskName = taskName;
        this.kind = kind;
    }
}

// Part of the body of our request to the Analyze Conversation REST API.
class ConversationalSummarizationTask extends ConversationAnalysisRequestTask {
    ConversationalSummarizationTaskParameters parameters;
    public ConversationalSummarizationTask(String taskName, ConversationalSummarizationTaskParameters parameters) {
        super(taskName, "ConversationalSummarizationTask");
        this.parameters = parameters;
    }
}

// This describes the body of our request to the Analyze Conversation REST API.
class ConversationAnalysisRequest {
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
class ConversationAspectAndSummary {
    public String aspect;
    public String summary;
    public ConversationAspectAndSummary(String aspect, String summary) {
        this.aspect = aspect;
        this.summary = summary;
    }
}

// This contains part of the data we want from a conversation analysis (PII analysis).
class PIICategoryAndText {
    public String category;
    public String text;
    public PIICategoryAndText(String category, String text) {
        this.category = category;
        this.text = text;
    }
}

// This contains the data we want from a conversation analysis.
class ConversationAnalysisResult {
    public List<ConversationAspectAndSummary> summary;
    public List<List<PIICategoryAndText>> PIIAnalysis;
    public ConversationAnalysisResult(List<ConversationAspectAndSummary> summary, List<List<PIICategoryAndText>> PIIAnalysis) {
        this.summary = summary;
        this.PIIAnalysis = PIIAnalysis;
    }
}

class CombinedResults {
    public JsonObject transcription;
    public JsonObject sentimentAnalysis;
    public JsonObject conversationAnalysis;
    public CombinedResults(JsonObject transcription, JsonObject sentimentAnalysis, JsonObject conversationAnalysis) {
        this.transcription = transcription;
        this.sentimentAnalysis = sentimentAnalysis;
        this.conversationAnalysis = conversationAnalysis;
    }
}