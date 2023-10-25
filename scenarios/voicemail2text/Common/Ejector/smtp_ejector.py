from Common.Ejector.abstract_ejector import Ejector
import v2ticlib.smtp_client_utils as smtp_util
from v2ticlib import config_utils, string_utils
from v2ticlib.constants.protocols import SMTP
import v2ticlib.constants.fields as Fields
import v2ticlib.constants.constants as Constants
from v2ticlib.retry_utils import retry_with_custom_strategy
from v2ticlib.logger_utils import log

class SmtpEjector(Ejector):
    def __init__(self):
        self._config_base = SMTP

    def get_property(self, property, literal_eval=False):
        return config_utils.get_property(self._config_base, property, literal_eval=literal_eval)

    def get_response_port(self):
        return self.get_property('response_port', literal_eval=True)

    def get_response_host(self):
        return self.get_property('response_host')

    def get_default_mail_from(self):
        return self.get_property('default_mail_from')

    def get_default_rcpt_to(self):
        return self.get_property('default_rcpt_to')

    def get_default_subject(self):
        return self.get_property('default_subject')

    def get_host_port(self, response:dict):
        host = self.get_response_host()
        port = self.get_response_port()
        response_address:str = response.get(Fields.RESPONSE_ADDRESS)
        if string_utils.is_not_blank(response_address):
            host_port = response_address.split(":")
            host = host_port[0]
            port = int(host_port[1])

        return host, port

    async def eject(self, request:dict, response:dict) -> str:
        host, port = self.get_host_port(response)
        mail_from = response[Fields.MAIL_FROM]
        rcpt_to = response[Fields.RCPT_TO]
        headers = response[Fields.HEADERS]
        content = response[Fields.BODY]
        start_tls = self.get_as_boolean(response.get(Fields.START_TLS))
        try:
            return await self.do_eject(host, port, mail_from, rcpt_to, headers, content, start_tls)
        except Exception as e:
            log(self.do_eject.retry.statistics)
            raise e

    @retry_with_custom_strategy(Constants.EJECTOR)
    async def do_eject(self, host, port, mail_from, rcpt_to, headers, content, start_tls):
        return await smtp_util.send(host, port, mail_from, rcpt_to, headers, content, start_tls)
