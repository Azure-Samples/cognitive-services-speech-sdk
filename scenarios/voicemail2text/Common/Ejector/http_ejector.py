from Common.Ejector.abstract_ejector import Ejector
import v2ticlib.http_client_utils as http_util
import v2ticlib.constants.fields as Fields
from v2ticlib.retry_utils import retry_with_custom_strategy
from v2ticlib.logger_utils import log
import v2ticlib.constants.constants as Constants

class HttpEjector(Ejector):

    async def eject(self, request:dict, response:dict) -> str:
        url = response[Fields.RETURN_URL]
        headers = response[Fields.HEADERS]
        content = response[Fields.BODY]
        verify_ssl = self.get_as_boolean(response.get(Fields.VERIFY_SSL))
        try:
            return await self.do_eject(url, headers, content, verify_ssl)
        except Exception as e:
            log(self.do_eject.retry.statistics)
            raise e

    @retry_with_custom_strategy(Constants.EJECTOR)
    async def do_eject(self, url, headers, content, verify_ssl) -> str:
        return await http_util.post(url, headers, content, verify_ssl)
