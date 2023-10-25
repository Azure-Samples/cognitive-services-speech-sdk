import asyncio
from v2ticlib.transcode_utils import TranscodeUtils
from AcsClient.acs_handler import AcsHandler
from Common.response_creator import ResponseCreator
import Common.ejector as ejector
import v2ticlib.request_utils as request_utils
from v2ticlib.logger_utils import log
import v2ticlib.constants.fields as Fields

transcoder = TranscodeUtils()

transcriber = AcsHandler()

response_creator = ResponseCreator()

async def process_request(request:dict):
    await do_transcode(request)
    await do_transcribe(request)
    response:dict = response_creator.create(request)
    await ejector.eject(request, response)

async def do_transcode(request:dict):
    await do_execute(transcoder.transcode, request, Fields.TRANSCODER)

async def do_transcribe(request:dict):
    await do_execute(transcriber.transcribe, request, Fields.TRANSCRIBER)

async def do_execute(handler, request, component):
    if request_utils.has_notes(request):
        log(f'found existing notes for this request')
        log(f'skipping: {component} ...')
        request_utils.add_skip_note(request, component)
        await asyncio.sleep(0)
    else:
        try:
            log(f'executing: {component} ...')
            await handler(request)
        except Exception as e:
            exception_str = str(e)
            log(f' {component} failed: {str(e)}')
            request_utils.add_notes(request, component, exception_str)