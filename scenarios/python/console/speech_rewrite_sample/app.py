import os
import argparse
import azure.cognitiveservices.speech as speechsdk
from openai import AzureOpenAI

# Initialize speech recognition engine
service_region = os.environ.get('SPEECH_REGION')
speech_key = os.environ.get('SPEECH_KEY')
speech_config = speechsdk.SpeechConfig(subscription=speech_key, region=service_region)
speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, language="en-us")

# Initialize Azure OpenAI client
client = AzureOpenAI(
    azure_endpoint=os.environ.get('AZURE_OPENAI_ENDPOINT'), 
    api_key=os.environ.get('AZURE_OPENAI_API_KEY'), 
    api_version="2024-10-21"
)

# Parse command-line arguments
parser = argparse.ArgumentParser(description="Run app.py with custom parameters.")
parser.add_argument(
    "--relevant_phrases", 
    type=str, 
    default="Azure Cognitive Services, non-profit organization, speech recognition, OpenAI API", 
    help="Comma-separated relevant phrases for text rewriting."
)
args = parser.parse_args()

# Use user-provided or default relevant_phrases
relevant_phrases = args.relevant_phrases

def rewrite_content(input_reco):
    """
    Refines the user's input sentence by fixing grammar issues, making it more readable,
    and ensuring spelling correctness for specific phrases.

    Args:
        input_reco (str): The raw input sentence to rewrite.

    Returns:
        str: The refined sentence.
    """

    # A list of phrases relevant to the context, used to ensure their correct spelling and formatting.
    # Users can customize these phrases based on their specific use case or domain.
    relevant_phrases = args.relevant_phrases
    
    my_messages = [
        {
            "role": "system", 
            "content": (
                "You are a helpful assistant to help the user rewrite sentences. "
                "Please fix the grammar errors in the user-provided sentence and make it more readable. "
                "You can do minor rewriting but MUST NOT change the sentence's meaning. "
                "DO NOT make up new content. DO NOT answer questions. "
                "Here are phrases relevant to the sentences: '{}'. "
                "If they appear in the sentence and are misspelled, please fix them. "
                "Example corrections:\n"
                "User: how ar you\nYour response: How are you?\n\n"
                "User: what yur name?\nYour response: What's your name?\n\n"
            ).format(relevant_phrases)
        },
        {"role": "user", "content": input_reco}
    ]

    try:
        response = client.chat.completions.create(
            model="gpt-4o-mini",
            messages=my_messages
        )
    except Exception as e:
        print("Error occurred:", e)

    return response.choices[0].message.content

def recognized_cb(evt: speechsdk.SpeechRecognitionEventArgs):
    """
    Callback function triggered when speech is recognized.

    Args:
        evt (SpeechRecognitionEventArgs): The event argument containing recognized text.
    """
    current_sentence = evt.result.text
    if not current_sentence:
        return

    print("RAW RECO:", current_sentence)
    print("REWRITE:", rewrite_content(current_sentence))

# Connect the speech recognizer to the callback
speech_recognizer.recognized.connect(recognized_cb)
result_future = speech_recognizer.start_continuous_recognition_async()
result_future.get()  # Ensure engine initialization is complete

print('Continuous Recognition is now running. Say something.')
while True:
    print('Type "stop" then press Enter to stop recognition.')
    stop = input()
    if stop.lower() == "stop":
        print('Stopping async recognition...')
        speech_recognizer.stop_continuous_recognition_async()
        break
