# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import logging
from websocket_server import WebSocketServer
from aiohttp import web
import argparse

parser = argparse.ArgumentParser(description="aiohttp server example")
parser.add_argument('--path')
parser.add_argument('--port', type=int, default=8080)

app = web.Application()
server = WebSocketServer()
app.router.add_get('/openai/realtime', server.websocket_handler)

if __name__ == '__main__':
    logging.basicConfig(level=logging.INFO)
    args = parser.parse_args()
    web.run_app(app, path=args.path, port=args.port, host='0.0.0.0')