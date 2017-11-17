#include "private-iot-cortana-sdk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

IOBUFFER* IOBUFFER_NEW()
{
	IOBUFFER* p = malloc(sizeof(IOBUFFER));
    if (p)
    {
        memset(p, 0, sizeof(IOBUFFER));
        p->hLocker = Lock_Init();
        p->hDataCondition = Condition_Init();
        p->hBuffer = BUFFER_new();
        p->nRef = 1;
		p->nActiveReaders = 0;
	}

	return p;
}

void IOBUFFER_DELETE(IOBUFFER* pIoBuffer)
{
    int nRef = 0;
    Lock(pIoBuffer->hLocker);
    nRef = --pIoBuffer->nRef;
    Unlock(pIoBuffer->hLocker);
    if (nRef != 0)
    {
        return;
    }

	IOBUFFER_Reset(pIoBuffer); // ensure IO readers exit

    if (pIoBuffer->hLocker)
    {
        Lock_Deinit(pIoBuffer->hLocker);
    }

    if (pIoBuffer->hDataCondition)
    {
        Condition_Deinit(pIoBuffer->hDataCondition);
    }

	if (pIoBuffer->hBuffer)
	{
        BUFFER_delete(pIoBuffer->hBuffer);
	}

	free(pIoBuffer);
}

void IOBUFFER_AddRef(IOBUFFER* pIoBuffer)
{
    Lock(pIoBuffer->hLocker);
    ++pIoBuffer->nRef;
    Unlock(pIoBuffer->hLocker);
}

void IOBUFFER_Reset(IOBUFFER* pIoBuffer)
{
    Lock(pIoBuffer->hLocker);

	// make sure all active readers exit as part of the reset
	if (pIoBuffer->nActiveReaders > 0)
	{
		pIoBuffer->nReadBytes = 0;
		pIoBuffer->hasNewCome = 1;
		pIoBuffer->nCompleted = 1;
		pIoBuffer->nTotalBytes = 0;
		Condition_Post(pIoBuffer->hDataCondition);
		while (pIoBuffer->nActiveReaders > 0)
		{
			// spin until they exit
			Unlock(pIoBuffer->hLocker);
			ThreadAPI_Sleep(5);
			Lock(pIoBuffer->hLocker);
		}
	}

	pIoBuffer->nReadBytes = 0;
	pIoBuffer->nCompleted = 0;
	pIoBuffer->nTotalBytes = 0;
	pIoBuffer->hasNewCome = 0;
	Unlock(pIoBuffer->hLocker);
}

int IOBUFFER_GetTotalBytes(IOBUFFER* pIoBuffer)
{
	int ret = 0;
    Lock(pIoBuffer->hLocker);
    ret = pIoBuffer->nCompleted ? pIoBuffer->nTotalBytes : -1;
    Unlock(pIoBuffer->hLocker);
    return ret;
}

int IOBUFFER_GetUnReadBytes(IOBUFFER* pIoBuffer)
{
    int ret = 0;
    Lock(pIoBuffer->hLocker);
    ret = pIoBuffer->nTotalBytes - pIoBuffer->nReadBytes;
    Unlock(pIoBuffer->hLocker);
    return ret;
}

int IOBUFFER_WaitForNewBytes(IOBUFFER* pIoBuffer, int timeout)
{
    COND_RESULT waitret = COND_OK;
    Lock(pIoBuffer->hLocker);
    ++pIoBuffer->nActiveReaders;
    if (!pIoBuffer->hasNewCome)
    {
        waitret = Condition_Wait(pIoBuffer->hDataCondition, pIoBuffer->hLocker, timeout);
    }

    pIoBuffer->hasNewCome = 0;
    --pIoBuffer->nActiveReaders;
    Unlock(pIoBuffer->hLocker);
    return waitret == COND_OK ? 0 : -1;
}

static int IOBUFFER_ReadInternal(IOBUFFER* pIoBuffer, void* pDstBuff, int nOffset, int nBytes, int timeout, int isPeek)
{
    COND_RESULT waitret = COND_OK;
    Lock(pIoBuffer->hLocker);
	++pIoBuffer->nActiveReaders;
    while (waitret == COND_OK && pIoBuffer->nTotalBytes < pIoBuffer->nReadBytes + nBytes && !pIoBuffer->nCompleted)
	{
        waitret = Condition_Wait(pIoBuffer->hDataCondition, pIoBuffer->hLocker, timeout);
	}

	if (pIoBuffer->nTotalBytes < pIoBuffer->nReadBytes + nBytes)
	{
		nBytes = -1;
	}
    else
    {
        memcpy((uint8_t*)pDstBuff + nOffset, BUFFER_u_char(pIoBuffer->hBuffer) + pIoBuffer->nReadBytes, nBytes);
		if (!isPeek)
		{
			pIoBuffer->nReadBytes += nBytes;
		}
    }

    pIoBuffer->hasNewCome = 0;
	--pIoBuffer->nActiveReaders;
	Unlock(pIoBuffer->hLocker);

	return nBytes;
}

int IOBUFFER_PeekRead(IOBUFFER* pIoBuffer, void* pDstBuff, int nOffset, int nBytes, int timeout)
{
	return IOBUFFER_ReadInternal(pIoBuffer, pDstBuff, nOffset, nBytes, timeout, 1);
}


int IOBUFFER_Read(IOBUFFER* pIoBuffer, void* pDstBuff, int nOffset, int nBytes, int timeout)
{
	return IOBUFFER_ReadInternal(pIoBuffer, pDstBuff, nOffset, nBytes, timeout, 0);
}

int IOBUFFER_Write(IOBUFFER* pIoBuffer, const void* pSrcBuff, int nOffset, int nBytes)
{
    int ret = nBytes;
    Lock(pIoBuffer->hLocker);
    if (pSrcBuff == NULL)
	{
		pIoBuffer->nCompleted = 1;
	}
	else
	{
        if (pIoBuffer->nTotalBytes + nBytes > (int)BUFFER_length(pIoBuffer->hBuffer) &&
            BUFFER_enlarge(pIoBuffer->hBuffer, pIoBuffer->nTotalBytes + nBytes))
        {
            ret = -1;
        }
        else
        {
            memcpy(BUFFER_u_char(pIoBuffer->hBuffer) + pIoBuffer->nTotalBytes, (uint8_t*)pSrcBuff + nOffset, nBytes);
            pIoBuffer->nTotalBytes += nBytes;
        }
    }

    if (!pIoBuffer->hasNewCome)
    {
        Condition_Post(pIoBuffer->hDataCondition);
        pIoBuffer->hasNewCome = 1;
    }

    Unlock(pIoBuffer->hLocker);
    return ret;
}
