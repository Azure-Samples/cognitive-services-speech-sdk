from typing import Final
import asyncio
from concurrent.futures import ThreadPoolExecutor
import contextvars
from v2ticlib import config_utils
import v2ticlib.coroutine_timeout_utils as coroutine_timeout

def get_context_executor(base):
    global context_executors
    context_executor = context_executors.get(base)
    if context_executor is None:
        if config_utils.has_property(base, 'max_workers'):
            max_workers = config_utils.get_property(base, 'max_workers', literal_eval=True)
            context_executor = ContextExecutor(thread_name_prefix=base, max_workers=max_workers)
        else:
            context_executor = ContextExecutor(thread_name_prefix=base)
        context_executors[base] = context_executor
    return context_executor

def get_executor_async_future(base, func, *args, **kwargs):
    eventloop = asyncio.get_event_loop()
    return eventloop.run_in_executor(get_context_executor(base), func, *args, **kwargs)

async def run_executor_async_with_timeout(timeout, base, func, *args, **kwargs):
    return await coroutine_timeout.timeout_after(get_executor_async_future(base, func, *args, **kwargs), timeout)

async def run_in_context_executor_async(base, func, *args, **kwargs):
    await get_executor_async_future(base, func, *args, **kwargs)

class ContextExecutor(ThreadPoolExecutor):
    def __init__(self, thread_name_prefix, max_workers=None):
        self.context = contextvars.copy_context()
        super().__init__(initializer=self._set_child_context, max_workers=max_workers, thread_name_prefix=thread_name_prefix)
    def _set_child_context(self):
        for var, value in self.context.items():
            var.set(value)

global context_executors
context_executors: Final[dict[str, ContextExecutor]] = {}