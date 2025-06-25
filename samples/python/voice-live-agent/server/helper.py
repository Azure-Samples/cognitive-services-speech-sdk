import json
from azure.core import AzureError

def get_json_object(data: bytes | str):
    """
    Parse raw payload into a Python dict.
    `data` may be bytes (request body) or a JSON string.
    """
    try:
        if isinstance(data, bytes):
            data = data.decode('utf-8')
        return json.loads(data)
    except (TypeError, json.JSONDecodeError) as e:
        raise AzureError(f"Failed to parse JSON: {e}")

def get_caller_id(json_obj: dict) -> str:
    """Retrieve the caller's rawId from incoming call event."""
    return json_obj["from"]["rawId"]

def get_incoming_call_context(json_obj: dict) -> str:
    """Extract the incomingCallContext value."""
    return json_obj["incomingCallContext"]
