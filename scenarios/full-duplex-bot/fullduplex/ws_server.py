import asyncio
import logging
import os

import websockets

from chat_server_azure import AzureChatServer

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
)


class AsyncMsgQueue:
    def __init__(self):
        self.queue = asyncio.Queue()

    async def put(self, frame):
        await self.queue.put(frame)

    async def get(self):
        return await self.queue.get()

    def size(self):
        return self.queue.qsize()


class WebSocketServer:
    def __init__(self, host, port, async_msg_fn=None):
        self.host = host
        self.port = port
        self.async_msg_fn = async_msg_fn
        self.in_queue = AsyncMsgQueue()
        self.out_queue = AsyncMsgQueue()
        self.speech_recognizer_provider = os.getenv("SPEECH_RECOGNIZER_PROVIDER", "azure")

    async def _stats(self):
        while True:
            logging.info(
                f"In Queue: {self.in_queue.size()}. Out Queue: {self.out_queue.size()}"
            )
            await asyncio.sleep(5)

    async def _worker(self):
        while True:
            msg = await self.in_queue.get()
            msg = await self.async_msg_fn(msg) if self.async_msg_fn else msg
            await self.out_queue.put(msg)

    async def _reader_handler(self, in_queue, websocket):
        async for message in websocket:
            await in_queue.put(message)

    async def _writer_handler(self, out_queue, websocket):
        while True:
            msg = await out_queue.get()
            await websocket.send(msg)
            await asyncio.sleep(0)

    async def _handler(self, websocket):
        # get query params
        path = websocket.request.path
        query_string = path.split("?")[1] if "?" in path else ""
        query_params = dict(
            (x.split("=")[0], x.split("=")[1]) for x in query_string.split("&") if "=" in x
        )
        logging.info(f"Query Params: {query_params}")
        language = query_params.get("lang", "en-US")
        in_queue = AsyncMsgQueue()
        out_queue = AsyncMsgQueue()
        read_task = asyncio.create_task(self._reader_handler(in_queue, websocket))
        write_task = asyncio.create_task(self._writer_handler(out_queue, websocket))
        server = AzureChatServer(in_queue, out_queue, language, provider=self.speech_recognizer_provider)
        server_task = asyncio.create_task(server.worker())
        done, pending = await asyncio.wait(
            [read_task, write_task, server_task],
            return_when=asyncio.FIRST_COMPLETED,
        )
        for task in pending:
            task.cancel()
        logging.info("WebSocket connection closed")

    async def start(self):
        asyncio.create_task(self._stats())
        # asyncio.create_task(self._worker())
        async with websockets.serve(self._handler, self.host, self.port):
            print(f"WebSocket Server started at ws://{self.host}:{self.port}")
            await asyncio.Future()  # run forever


def main():
    server = WebSocketServer("0.0.0.0", 5000)
    asyncio.run(server.start())


if __name__ == "__main__":
    main()
