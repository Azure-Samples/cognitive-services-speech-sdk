from v2ticlib.logger_utils import log
from v2ticlib.template_utils import template_utils
from Common.Ejector.smtp_ejector import SmtpEjector

class AfterDepositAckHandler():
    def __init__(self):
        self._smtp_ejector = SmtpEjector()

    async def send_acknowledgement(self, request):
        after_deposit_ack:dict = self.get_after_deposit_ack(request)
        log("Sending acknowledgement ...")
        return_code = await self._smtp_ejector.eject(request, after_deposit_ack)
        log(f'Acknowledgement sent with the following return_code: {return_code}')

    def get_after_deposit_ack(self, request):
        return template_utils.render_after_deposit_ack(request)