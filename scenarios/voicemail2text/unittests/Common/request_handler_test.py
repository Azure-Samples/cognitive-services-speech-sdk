import pytest
from unittest.mock import MagicMock, AsyncMock
from unittest.mock import patch

import sys, os
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
from Common.request_handler import *


@pytest.mark.asyncio
async def test_process_request():
    # create a mock request
    request = {'audio': 'some_audio_data'}

    # create mock functions for transcode, transcribe, and eject
    transcode_mock = AsyncMock()
    transcribe_mock = AsyncMock()
    eject_mock = AsyncMock()

    # patch the functions in the module with the mock functions
    with patch('Common.request_handler.do_transcode', transcode_mock), \
         patch('Common.request_handler.do_transcribe', transcribe_mock), \
         patch('Common.request_handler.ejector.eject', eject_mock):

        # call the function being tested
        await process_request(request)

    # assert that the transcode and transcribe functions were called with the request
    transcode_mock.assert_called_once_with(request)
    transcribe_mock.assert_called_once_with(request)

    # assert that the eject function was called with the request and a response
    eject_mock.assert_called_once_with(request, MagicMock())