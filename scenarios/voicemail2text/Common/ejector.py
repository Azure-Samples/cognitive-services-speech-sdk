from Common.Ejector.abstract_ejector import Ejector
from Common.Ejector.http_ejector import HttpEjector
from Common.Ejector.smtp_ejector import SmtpEjector
from v2ticlib.constants.protocols import HTTPS, SMTP
import v2ticlib.request_utils as request_utils

ejectors = {}
ejectors[HTTPS] = HttpEjector()
ejectors[SMTP] = SmtpEjector()

async def eject(request, response):
    delivery_type = request_utils.get_delivery_type(request)
    ejector: Ejector = ejectors[delivery_type]
    await ejector.eject(request, response)