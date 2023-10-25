import sys, os, typing
import uvicorn
import asyncio
from starlette.middleware.httpsredirect import HTTPSRedirectMiddleware
from fastapi import FastAPI, Request, Response, HTTPException
from http import HTTPStatus
from starlette.status import HTTP_504_GATEWAY_TIMEOUT
from fastapi.responses import JSONResponse

sys.path.append(os.path.join(os.path.abspath(os.curdir)))
from v2ticlib.logger_utils import log, CONTEXT
from Common.request_injestor import RequestInjestor
import Common.request_handler as request_handler

from v2ticlib import config_utils
from v2ticlib.constants.protocols import HTTPS
from v2ticlib.constants.fields import SCRID, DELIVERY_TYPE
from v2ticlib.constants.headers import LOCATION
import v2ticlib.coroutine_timeout_utils as coroutine_timeout

app = FastAPI()
app.add_middleware(HTTPSRedirectMiddleware)

injestor = RequestInjestor()

@app.get("/")
async def index():
    return {"message":"Welcome to V2TIC interface"}

@app.middleware("http")
async def timeout_middleware(request: Request, call_next):
    try:
        return await coroutine_timeout.timeout_after(call_next(request), timeout=30)
    except Exception as e:
        return JSONResponse(str(e), status_code=HTTP_504_GATEWAY_TIMEOUT)

@app.post("/transcribe")
@app.put("/transcribe")
async def main(external_request: Request, response: Response):
    headers: typing.Mapping[str, str] = external_request.headers
    body: bytes = await external_request.body()

    try:
        scrid = handle_request(headers, body)
    except Exception as e:
        raise HTTPException(status_code=HTTPStatus.BAD_REQUEST, detail=str(e))
    else:
        response.headers[LOCATION] = scrid
        response.status_code = HTTPStatus.ACCEPTED

def handle_request(headers: typing.Mapping[str, str], body: bytes):
    request = injestor.injest(headers, body)
    scrid = request[SCRID]
    request[DELIVERY_TYPE] = HTTPS
    asyncio.create_task(process_request(request))
    return scrid

async def process_request(request):
    reference = request[SCRID]
    CONTEXT.set(reference)
    await request_handler.process_request(request)

# Stub Response
@app.post("/response")
async def print_response(external_response: Request):
    response = await external_response.body()

    print(f'Response from Ejector: {response.decode()}')

def run_https_server():
    log_level = config_utils.get_logging_level()
    server_host = config_utils.get_host()
    server_port = config_utils.get_port()
    cert_file = config_utils.get_cert_file()
    key_file = config_utils.get_key_file()
    uvicorn.run("https_server:app", host=server_host, port=server_port, ssl_keyfile=key_file, ssl_certfile=cert_file, log_level=log_level)

if __name__ == "__main__":
    run_https_server()