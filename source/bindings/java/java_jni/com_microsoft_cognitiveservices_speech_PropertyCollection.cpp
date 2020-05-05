//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
//
#include "com_microsoft_cognitiveservices_speech_PropertyCollection.h"
#include "speechapi_c_property_bag.h"
#include "jni_utils.h"
#include <string>

/*
 * Class:     com_microsoft_cognitiveservices_speech_PropertyCollection
 * Method:    setPropertyString
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;ILjava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL Java_com_microsoft_cognitiveservices_speech_PropertyCollection_setPropertyString
  (JNIEnv *env, jobject obj, jobject objHandle, jint id, jstring name, jstring defaultValue)
{
    const char *propertyName = GetStringUTFChars(env, name);
    const char *value = GetStringUTFChars(env, defaultValue);
    jlong propertyHandle = GetObjectHandle(env, objHandle);
    SPXHR hr = property_bag_set_string((SPXPROPERTYBAGHANDLE)propertyHandle, (int) id, propertyName, value);
    ReleaseStringUTFChars(env, name, propertyName);
    ReleaseStringUTFChars(env, defaultValue, value);
    return (jlong)hr;
}

/*
 * Class:     com_microsoft_cognitiveservices_speech_PropertyCollection
 * Method:    getPropertyString
 * Signature: (Lcom/microsoft/cognitiveservices/speech/util/SafeHandle;ILjava/lang/String;Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_microsoft_cognitiveservices_speech_PropertyCollection_getPropertyString
  (JNIEnv *env, jobject obj, jobject objHandle, jint id, jstring name, jstring defaultValue)
{
    jstring result;
    const char *propertyName = GetStringUTFChars(env, name);
    const char *value = GetStringUTFChars(env, defaultValue);
    jlong propertyHandle = GetObjectHandle(env, objHandle);
    const char* str = property_bag_get_string((SPXPROPERTYBAGHANDLE)propertyHandle, (int)id, propertyName, value);
    result = str ? env->NewStringUTF(str) : env->NewStringUTF("");
    if (str != NULL)
    {
        property_bag_free_string(str);
    }
    ReleaseStringUTFChars(env, name, propertyName);
    ReleaseStringUTFChars(env, defaultValue, value);
    return result;
}
