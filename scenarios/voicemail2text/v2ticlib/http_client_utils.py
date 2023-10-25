from typing import Final
from aiohttp import ClientTimeout as ClientTimeout
from aiohttp import ClientSession as ClientSession
from aiohttp import TCPConnector as TCPConnector
from v2ticlib.logger_utils import log
from http import HTTPStatus

DEFAULT_TIMEOUT: Final[ClientTimeout] = ClientTimeout(total=30)

async def post(url, headers, data, verify_ssl=False) -> str:
    async with get_client_session(verify_ssl) as session:
        async with session.post(url, data=data, headers=headers) as response:
            log(f'Got [{response.status}] from {url}')
            if response.status not in [HTTPStatus.OK, HTTPStatus.ACCEPTED]:
                # TODO make exception pristine!
                raise Exception
            return_code = f'{response.status}'
            return return_code

def get_client_session(verify_ssl=True):
    return ClientSession(timeout=DEFAULT_TIMEOUT, connector=TCPConnector(limit=500, verify_ssl=verify_ssl, force_close=True))
