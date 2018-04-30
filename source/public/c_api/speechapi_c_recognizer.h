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

enum Recognizer_Parameter { RecognizerParameter_DeploymentId = 1 };
typedef enum Recognizer_Parameter Recognizer_Parameter;
SPXAPI Recognizer_GetParameter_Name(Recognizer_Parameter parameter, wchar_t* name, uint32_t cchName);

SPXAPI Recognizer_SetParameter_String(SPXRECOHANDLE hreco, const wchar_t* name, const wchar_t* value);
SPXAPI Recognizer_GetParameter_String(SPXRECOHANDLE hreco, const wchar_t* name, wchar_t* value, uint32_t cchValue, const wchar_t* defaultValue);
SPXAPI_(bool) Recognizer_ContainsParameter_String(SPXRECOHANDLE hreco, const wchar_t* name);

SPXAPI Recognizer_SetParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name, int32_t value);
SPXAPI Recognizer_GetParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name, int32_t* pvalue, int32_t defaultValue);
SPXAPI_(bool) Recognizer_ContainsParameter_Int32(SPXRECOHANDLE hreco, const wchar_t* name);

SPXAPI Recognizer_SetParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name, bool value);
SPXAPI Recognizer_GetParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name, bool* pvalue, bool defaultValue);
SPXAPI_(bool) Recognizer_ContainsParameter_Bool(SPXRECOHANDLE hreco, const wchar_t* name);

SPXAPI Recognizer_Recognize(SPXRECOHANDLE hreco, SPXRESULTHANDLE* phresult);
SPXAPI Recognizer_RecognizeAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync);
SPXAPI Recognizer_RecognizeAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds, SPXRESULTHANDLE* phresult);

SPXAPI Recognizer_StartContinuousRecognition(SPXRECOHANDLE hreco);
SPXAPI Recognizer_StartContinuousRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync);
SPXAPI Recognizer_StartContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds);

SPXAPI Recognizer_StopContinuousRecognition(SPXRECOHANDLE hreco);
SPXAPI Recognizer_StopContinuousRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync);
SPXAPI Recognizer_StopContinuousRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds);

SPXAPI Recognizer_StartKeywordRecognition(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword);
SPXAPI Recognizer_StartKeywordRecognitionAsync(SPXRECOHANDLE hreco, SPXKEYWORDHANDLE hkeyword, SPXASYNCHANDLE* phasync);
SPXAPI Recognizer_StartKeywordRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds);

SPXAPI Recognizer_StopKeywordRecognition(SPXRECOHANDLE hreco);
SPXAPI Recognizer_StopKeywordRecognitionAsync(SPXRECOHANDLE hreco, SPXASYNCHANDLE* phasync);
SPXAPI Recognizer_StopKeywordRecognitionAsync_WaitFor(SPXASYNCHANDLE hasync, uint32_t milliseconds);

typedef void (*PSESSION_CALLBACK_FUNC)(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext);
SPXAPI Recognizer_SessionStarted_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_SessionStopped_SetEventCallback(SPXRECOHANDLE hreco, PSESSION_CALLBACK_FUNC pCallback, void* pvContext);


typedef void (*PRECOGNITION_CALLBACK_FUNC)(SPXRECOHANDLE hreco, SPXEVENTHANDLE hevent, void* pvContext);
SPXAPI Recognizer_IntermediateResult_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_FinalResult_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_NoMatch_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_Canceled_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_SpeechStartDetected_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);
SPXAPI Recognizer_SpeechEndDetected_SetEventCallback(SPXRECOHANDLE hreco, PRECOGNITION_CALLBACK_FUNC pCallback, void* pvContext);

SPXAPI Recognizer_SessionEvent_GetSessionId(SPXEVENTHANDLE hevent, wchar_t* pszSessionId, uint32_t cchSessionId);
SPXAPI Recognizer_RecognitionEvent_GetOffset(SPXEVENTHANDLE hevent, uint64_t *pszOffset);
SPXAPI Recognizer_RecognitionEvent_GetResult(SPXEVENTHANDLE hevent, SPXRESULTHANDLE* phresult);
