//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_c_recognizer.h: Public API declarations for Recognizer related C methods and typedefs
//

#pragma once
#include <speechapi_c_common.h>


SPXAPI_(bool) Recognizer_Handle_IsValid(SPXRECOHANDLE hreco);
SPXAPI Recognizer_Handle_Close(SPXRECOHANDLE hreco);

SPXAPI_(bool) Recognizer_AsyncHandle_IsValid(SPXASYNCHANDLE hasync);
SPXAPI Recognizer_AsyncHandle_Close(SPXASYNCHANDLE hasync);

SPXAPI_(bool) Recognizer_ResultHandle_IsValid(SPXRESULTHANDLE hresult);
SPXAPI Recognizer_ResultHandle_Close(SPXRESULTHANDLE hresult);

SPXAPI_(bool) Recognizer_EventHandle_IsValid(SPXEVENTHANDLE hevent);
SPXAPI Recognizer_EventHandle_Close(SPXEVENTHANDLE hevent);

SPXAPI Recognizer_Enable(SPXRECOHANDLE hreco);
SPXAPI Recognizer_Disable(SPXRECOHANDLE hreco);
SPXAPI Recognizer_IsEnabled(SPXRECOHANDLE hreco, bool* pfEnabled);

SPXAPI Recognizer_SetParameter_String(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* value);
SPXAPI Recognizer_GetParameter_String(SPXRECOHANDLE hreco, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue);
SPXAPI_(bool) Recognizer_HasParameter_String(SPXRECOHANDLE hreco, const wchar_t* name);

SPXAPI Recognizer_SetParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name, int32_t value);
SPXAPI Recognizer_GetParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name, int32_t* pvalue, int32_t defaultValue);
SPXAPI_(bool) Recognizer_HasParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name);

SPXAPI Recognizer_SetParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name, bool value);
SPXAPI Recognizer_GetParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name, bool* pvalue, bool defaultValue);
SPXAPI_(bool) Recognizer_HasParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name);

SPXAPI Recognizer_Recognize(SPXRECOHANDLE hreco, SPXRESULTHANDLE* phresult);
SPXAPI Recognizer_RecognizeAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync);
SPXAPI Recognizer_RecognizeAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult);

SPXAPI Recognizer_StartContinuousRecognition(SPXRECOHANDLE hreco);
SPXAPI Recognizer_StartContinuousRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync);
SPXAPI Recognizer_StartContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds);

SPXAPI Recognizer_StopContinuousRecognition(SPXRECOHANDLE hreco);
SPXAPI Recognizer_StopContinuousRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync);
SPXAPI Recognizer_StopContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds);

typedef void (*PSESSION_CALLBACK_FUNC)(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext);
SPXAPI Recognizer_SessionStarted_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_SessionStopped_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_SoundStarted_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_SoundStopped_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext);

typedef void (*PRECOGNITION_CALLBACK_FUNC)(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext);
SPXAPI Recognizer_IntermediateResult_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_FinalResult_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_NoMatch_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_Canceled_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);

SPXAPI Recognizer_SessionEvent_GetSessionId(SPXEVENTHANDLE hevent, wchar_t* pszSessionId, uint32_t cchSessionId);
SPXAPI Recognizer_RecognitionEvent_GetResult(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult);

