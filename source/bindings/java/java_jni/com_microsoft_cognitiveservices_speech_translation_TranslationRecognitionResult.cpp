//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_translation_TranslationRecognitionResult.h"
#include "speechapi_c_translation_result.h"
#include "jni_utils.h"
#include <memory>

/*
 * Class:     com_microsoft_cognitiveservices_speech_translation_TranslationRecognitionResult
 * Method:    getTranslations
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;Ljava/util/Map;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_translation_TranslationRecognitionResult_getTranslations
  (JNIEnv* env, jobject obj, jobject translationResultHandle, jobject translationsStringMapRef)
{
    jlong resultHandle = GetObjectHandle(env, translationResultHandle);
    size_t bufLen = 0;
    std::shared_ptr<Result_TranslationTextBufferHeader> phraseBuffer;
    SPXHR hr = translation_text_result_get_translation_text_buffer_header((SPXRESULTHANDLE)resultHandle, nullptr, &bufLen);
    if (hr == SPXERR_BUFFER_TOO_SMALL)
    {
        char* ptr = new char[bufLen];
        phraseBuffer = std::shared_ptr<Result_TranslationTextBufferHeader>((Result_TranslationTextBufferHeader*)ptr,
            [](void* to_delete) { delete[]((char*)to_delete); });
        hr = translation_text_result_get_translation_text_buffer_header((SPXRESULTHANDLE)resultHandle, phraseBuffer.get(), &bufLen);
    }
    if (phraseBuffer->bufferSize > bufLen)
    {
        hr = SPXERR_RUNTIME_ERROR;
    }
    if (SPX_SUCCEEDED(hr))
    {
        for (size_t i = 0; i < phraseBuffer->numberEntries; i++)
        {
            SetStringMapObjectHandle(env, translationsStringMapRef, phraseBuffer->targetLanguages[i], phraseBuffer->translationTexts[i]);
        }
    }
    return (jlong)hr;
}
