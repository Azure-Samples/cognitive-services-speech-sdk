import os
import azure.cognitiveservices.speech as speechsdk
from openai import AzureOpenAI, OpenAI

# Inialize speech recognition engine
service_region = os.environ.get('SPEECH_REGION')
speech_key=os.environ.get('SPEECH_KEY')
speech_config = speechsdk.SpeechConfig(subscription=speech_key, region=service_region)
speech_recognizer = speechsdk.SpeechRecognizer(speech_config=speech_config, language="en-us")

# Inialize AOAI engine
client = AzureOpenAI(
  azure_endpoint = os.environ.get('AZURE_OPENAI_ENDPOINT'), 
  api_key=os.environ.get('AZURE_OPENAI_API_KEY'), 
  api_version="2024-10-21"
)


def rewrite_content(input_reco):

    my_messages=[
            {"role": "system", "content": "You are a helpful assistant to help user to rewrite sentences."
                "Please fix the grammar errors in the user provided sentence and make it more reading friendly."
                "You can do some minor rewriting, but DON'T do big change, especially don't change the meaning of the sentence."
                "DON'T make up any new content. DON'T do question answering. User is not asking you to answer questions."
                "You just need to copy and refine the sentences from user."
                "Meanwhile, this is a list of phrases relevant to the sentences \"{}\"."
                "If they appear in the sentence and wrongly spelled, please fix them."
                "Here is some examples:\n"
                "User: how ar you\n"
                "Your response: How are you?\n\n"
                "User: what yur name?\n"
                "Your response: What's your name?\n\n"
                ""
            },
            {"role": "user", "content": ""}
          ]

    my_messages[0]["content"] = my_messages[0]["content"].format(
            "PAFE music festival, non-profit 501(c)(3), Changliang Liu"
    )
    my_messages[1]["content"] = input_reco
    
    response = client.chat.completions.create(
      model="gpt-4o-mini",
      messages=my_messages
    )

    return response.choices[0].message.content 


def recognized_cb(evt: speechsdk.SpeechRecognitionEventArgs):
    current_sentence = evt.result.text
    if current_sentence == "":
        return
    

    print("RAW RECO: ", current_sentence)
    print("REWRITE : ", rewrite_content(current_sentence))


speech_recognizer.recognized.connect(recognized_cb)
result_future = speech_recognizer.start_continuous_recognition_async()
result_future.get()  # wait for voidfuture, so we know engine initialization is done.

print('Continuous Recognition is now running, say something.')
while True:
    # No real sample parallel work to do on this thread, so just wait for user to type stop.
    # Can't exit function or speech_recognizer will go out of scope and be destroyed while running.
    print('type "stop" then enter when done')
    stop = input()
    if (stop.lower() == "stop"):
        print('Stopping async recognition.')
        speech_recognizer.stop_continuous_recognition_async()
        break