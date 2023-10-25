from AcsClient.acs_client import AcsClient
from AcsClient.acs_client_lid_enabled import AcsClientLid
import v2ticlib.request_utils as request_utils

class AcsHandler():
    def __init__(self):
        self._acs_client = AcsClient()
        self._acs_client_lid = AcsClientLid()
        self._acs_client_handler = {}
        lid_enabled = True
        lid_disabled = False
        self._acs_client_handler[lid_disabled] = self._acs_client
        self._acs_client_handler[lid_enabled] = self._acs_client_lid

    async def transcribe(self, request):
        lid_enabled = request_utils.is_lid_enabled(request)
        acs_handler: AcsClient = self._acs_client_handler[lid_enabled]
        await acs_handler.transcribe(request)
