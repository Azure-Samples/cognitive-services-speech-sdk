import base64
import json
from typing import Optional, Union


class AudioData:
    def __init__(self, data: bytes, is_silent: bool = False):
        self.data = data
        self.is_silent = is_silent


def parse_streaming_message(message: str) -> Optional[Union[AudioData, dict]]:
    try:
        obj = json.loads(message)
        msg_type = obj.get("type")

        if msg_type == "audio":
            # Decode base64-encoded audio
            audio_base64 = obj.get("data")
            if not audio_base64:
                return None

            audio_bytes = base64.b64decode(audio_base64)
            is_silent = obj.get("is_silent", False)
            return AudioData(data=audio_bytes, is_silent=is_silent)

        return obj  # Other message types (e.g., control)
    except Exception as e:
        print(f"[streaming_data] Failed to parse message: {e}")
        return None
