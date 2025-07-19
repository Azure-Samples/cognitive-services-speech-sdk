import json
import logging
import uuid, json
from urllib.parse import urlencode, urlparse, urlunparse
from quart import Response
from azure.eventgrid import EventGridEvent, SystemEventNames
from azure.communication.callautomation import (
    MediaStreamingOptions,
    AudioFormat,
    MediaStreamingContentType,
    MediaStreamingAudioChannelType,
    StreamingTransportType
    )
from azure.communication.callautomation.aio import (
    CallAutomationClient
    )

logger = logging.getLogger(__name__)

class AcsEventHandler:
    def __init__(self, config):
        self.acs_client = CallAutomationClient.from_connection_string(config["ACS_CONNECTION_STRING"])
        self.acs_base_url = config["ACS_DEV_TUNNEL"].rstrip('/')

    async def process_incoming_call(self, events: list, config):
        logger.info("incoming event data")
        for event_dict in events:
            event = EventGridEvent.from_dict(event_dict)
            logger.info("incoming event data --> %s", event.data)
            if event.event_type == SystemEventNames.EventGridSubscriptionValidationEventName:
                logger.info("Validating subscription")
                validation_code = event.data['validationCode']
                validation_response = {'validationResponse': validation_code}
                return Response(response=json.dumps(validation_response), status=200)
            elif event.event_type =="Microsoft.Communication.IncomingCall":
                logger.info("Incoming call received: data=%s", 
                                event.data)  
                if event.data['from']['kind'] =="phoneNumber":
                    caller_id =  event.data['from']["phoneNumber"]["value"]
                else :
                    caller_id =  event.data['from']['rawId'] 
                logger.info("incoming call handler caller id: %s",
                                caller_id)
                incoming_call_context=event.data['incomingCallContext']
                guid =uuid.uuid4()
                query_parameters = urlencode({"callerId": caller_id})
                callback_events_uri = f"{config["ACS_DEV_TUNNEL"]}/acs/callbacks"
                callback_uri = f"{callback_events_uri}/{guid}?{query_parameters}"
                
                parsed_url = urlparse(callback_events_uri)
                websocket_url = urlunparse(('wss',parsed_url.netloc,'/acs/ws','', '', ''))

                logger.info("callback url: %s",  callback_uri)
                logger.info("websocket url: %s",  websocket_url)

                media_streaming_options = MediaStreamingOptions(
                        transport_url=websocket_url,
                        transport_type=StreamingTransportType.WEBSOCKET,
                        content_type=MediaStreamingContentType.AUDIO,
                        audio_channel_type=MediaStreamingAudioChannelType.MIXED,
                        start_media_streaming=True,
                        enable_bidirectional=True,
                        audio_format=AudioFormat.PCM24_K_MONO)
                
                answer_call_result = await self.acs_client.answer_call(incoming_call_context=incoming_call_context,
                                                            operation_context="incomingCall",
                                                            callback_url=callback_uri, 
                                                            media_streaming=media_streaming_options)
                logger.info("Answered call for connection id: %s",
                                answer_call_result.call_connection_id)
            return Response(status=200)

    async def process_callback_events(self, context_id: str, raw_events: list, config):
        for event in raw_events:
            global call_connection_id
            event_data = event['data']
            call_connection_id = event_data["callConnectionId"]
            logger.info(f"Received Event:-> {event['type']}, Correlation Id:-> {event_data['correlationId']}, CallConnectionId:-> {call_connection_id}")
            if event['type'] == "Microsoft.Communication.CallConnected":
                call_connection_properties = await self.acs_client.get_call_connection(call_connection_id).get_call_properties()
                media_streaming_subscription = call_connection_properties.media_streaming_subscription
                logger.info(f"MediaStreamingSubscription:--> {media_streaming_subscription}")
                logger.info(f"Received CallConnected event for connection id: {call_connection_id}")
                logger.info("CORRELATION ID:--> %s", event_data["correlationId"])
                logger.info("CALL CONNECTION ID:--> %s", event_data["callConnectionId"])
            elif event['type'] == "Microsoft.Communication.MediaStreamingStarted":
                logger.info(f"Media streaming content type:--> {event_data['mediaStreamingUpdate']['contentType']}")
                logger.info(f"Media streaming status:--> {event_data['mediaStreamingUpdate']['mediaStreamingStatus']}")
                logger.info(f"Media streaming status details:--> {event_data['mediaStreamingUpdate']['mediaStreamingStatusDetails']}")
            elif event['type'] == "Microsoft.Communication.MediaStreamingStopped":
                logger.info(f"Media streaming content type:--> {event_data['mediaStreamingUpdate']['contentType']}")
                logger.info(f"Media streaming status:--> {event_data['mediaStreamingUpdate']['mediaStreamingStatus']}")
                logger.info(f"Media streaming status details:--> {event_data['mediaStreamingUpdate']['mediaStreamingStatusDetails']}")
            elif event['type'] == "Microsoft.Communication.MediaStreamingFailed":
                logger.info(f"Code:->{event_data['resultInformation']['code']}, Subcode:-> {event_data['resultInformation']['subCode']}")
                logger.info(f"Message:->{event_data['resultInformation']['message']}")
            elif event['type'] == "Microsoft.Communication.CallDisconnected":
                pass
        return Response(status=200)