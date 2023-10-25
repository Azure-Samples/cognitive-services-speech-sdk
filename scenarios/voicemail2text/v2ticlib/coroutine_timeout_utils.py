from datetime import datetime
import asyncio
import v2ticlib.config_utils as config_utils
async def timeout_after(coroutine, timeout=config_utils.get_coroutine_execution_default_timeout()):
    start = datetime.now()
    try:
        return await asyncio.wait_for(coroutine, timeout=timeout)
    except asyncio.TimeoutError:
        process_time = datetime.now() - start
        raise Exception({'detail': f'couroutine time excedeed limit {process_time}'})