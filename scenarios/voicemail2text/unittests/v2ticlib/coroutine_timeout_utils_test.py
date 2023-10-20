import asyncio
import pytest
from datetime import timedelta
import sys,os
sys.path.append(os.path.join(os.path.abspath(os.curdir)))
from v2ticlib.coroutine_timeout_utils import timeout_after

async def long_running_coroutine():
    await asyncio.sleep(5)

@pytest.mark.asyncio
async def test_timeout_after():
    with pytest.raises(Exception) as exc_info:
        await timeout_after(long_running_coroutine(), timeout=timedelta(seconds=2))
    assert 'couroutine time excedeed limit' in str(exc_info.value)