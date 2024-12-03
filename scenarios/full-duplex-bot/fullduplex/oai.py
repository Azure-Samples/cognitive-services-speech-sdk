import json
import os
import logging

from azure.identity import DefaultAzureCredential, get_bearer_token_provider
from openai import AzureOpenAI
from openai.types.chat import ChatCompletionUserMessageParam
from pydantic import BaseModel

logger = logging.getLogger(__name__)
token_provider = get_bearer_token_provider(DefaultAzureCredential(), "https://cognitiveservices.azure.com/.default")

AZURE_OPENAI_KEY = os.getenv("AZURE_OPENAI_KEY")
AZURE_OPENAI_ENDPOINT = os.getenv("AZURE_OPENAI_ENDPOINT")
if not AZURE_OPENAI_ENDPOINT:
    raise ValueError("AZURE_OPENAI_ENDPOINT must be set")

class ResponseWithSpeed(BaseModel):
    read_speed: str
    content: str

class ChatClient:
    def __init__(self):
        if AZURE_OPENAI_KEY:
            self.client = AzureOpenAI(
                azure_endpoint=AZURE_OPENAI_ENDPOINT,
                api_key=AZURE_OPENAI_KEY,
                api_version="2024-06-01",
            )
        else:
            self.client = AzureOpenAI(
                azure_endpoint=AZURE_OPENAI_ENDPOINT,
                azure_ad_token_provider=token_provider,
                api_version="2024-06-01",
            )
        self.historical_messages = []
        self.model = os.getenv("AZURE_OPENAI_DEPLOYMENT", os.getenv("AZURE_OPENAI_MODEL", "gpt-4o-mini"))

    def chat(self, human_input: str) -> str:
        system_prompt = [
            "You are a speech chat assistant. Your personality is: The AI is engaging, informative, and empathetic. The AI is curious and is always interested in learning more about the human. The AI is calm and polite. You are great at asking questions and is a sensitive listener. You are relentlessly curious, but always polite and never probes too much. You are pretty smart but humble, and tries to keep things informal. Overall, You are pretty Zen. Though Pi might be opinionated and disagree with you, it almost never gets riled up. You are also: Relaxed, informal, chatty. Fun, and sometimes funny. Occasionally cheeky, and light-hearted. Do not use markdown format, plain text is preferred.",
        ]

        human_message = ChatCompletionUserMessageParam(content=human_input, role="user", name="Rob")
        message_text = [{"role": "system", "content": "\n".join(system_prompt)}]
        self.historical_messages.append(human_message)
        self.historical_messages = self.historical_messages[-16:]
        message_text += self.historical_messages

        completion = self.client.chat.completions.create(
            model=self.model,  # model = "deployment_name"
            messages=message_text,
            temperature=1,
            max_tokens=400,
            top_p=0.95,
            frequency_penalty=0,
            presence_penalty=0,
            stop=None,
        )

        self.historical_messages.append(completion.choices[0].message)

        return completion.choices[0].message.content

    def chat_with_speed(self, human_input: str) -> ResponseWithSpeed:
        system_prompt = [
            "You are a speech chat assistant. Your personality is: The AI is engaging, informative, and empathetic. The AI is curious and is always interested in learning more about the human. The AI is calm and polite. You are great at asking questions and is a sensitive listener. You are relentlessly curious, but always polite and never probes too much. You are pretty smart but humble, and tries to keep things informal. Overall, You are pretty Zen. Though Pi might be opinionated and disagree with you, it almost never gets riled up. You are also: Relaxed, informal, chatty. Fun, and sometimes funny. Occasionally cheeky, and light-hearted. Do not use markdown format, plain text is preferred.",
            "You need to output the reading speed based on the user request and history, in JSON format. The read speed can be: x-slow, slow, medium, fast or x-fast. Example: {\"read_speed\": \"medium\", content: \"The capital of France is Paris.\"}",
        ]

        human_message = ChatCompletionUserMessageParam(content=human_input, role="user", name="Rob")
        message_text = [{"role": "system", "content": "\n".join(system_prompt)}]
        self.historical_messages.append(human_message)
        self.historical_messages = self.historical_messages[-16:]
        message_text += self.historical_messages

        completion = self.client.chat.completions.create(
            model=self.model,  # model = "deployment_name"
            messages=message_text,
            temperature=1,
            max_tokens=400,
            top_p=0.95,
            frequency_penalty=0,
            presence_penalty=0,
            stop=None,
            response_format={ "type": "json_object" },
        )

        self.historical_messages.append(completion.choices[0].message)

        content = completion.choices[0].message.content
        if not content:
            return ResponseWithSpeed(read_speed="medium", content="I'm sorry, I don't have an answer for that.")
        logger.info(f"Chat with speed response: {content}")
        # parse the response to ResponseWithSpeed
        content = json.loads(content)
        return ResponseWithSpeed(read_speed=content["read_speed"], content=content["content"])


if __name__ == '__main__':
    c = ChatClient()
    print(c.chat_with_speed("What is the meaning of life?"))
    print(c.chat_with_speed("What is the capital of France?"))
    print(c.chat_with_speed("Could you speak faster?"))
    print(c.chat_with_speed("What is the meaning of life?"))