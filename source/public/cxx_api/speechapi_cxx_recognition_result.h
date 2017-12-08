//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// speechapi_cxx_recognition_result.h: Public API declarations for RecognitionResult C++ base class and related enum class
//

#pragma once
#include <string>
#include <speechapi_cxx_common.h>
#include <speechapi_cxx_todo_recognition.h>


namespace CARBON_NAMESPACE_ROOT {
namespace Recognition {


enum class Reason { Recognized, IntermediateResult, NoMatch, Canceled, OtherRecognizer };


class RecognitionResult
{
public:

    virtual ~RecognitionResult() {};

    const std::wstring& ResultId;
    const enum class Reason& Reason;
    
    const std::wstring& Text;

    const PayloadItems& Payload;
   
    
protected:

    RecognitionResult(const std::wstring& resultId, const enum class Reason& reason, const std::wstring& text, const PayloadItems& payload) :
        ResultId(resultId), Reason(reason), Text(text), Payload(payload)
    {
    };
    

private:

    RecognitionResult(const RecognitionResult&) = delete;
    RecognitionResult(const RecognitionResult&&) = delete;

    RecognitionResult& operator=(const RecognitionResult&) = delete;
};


} }; // CARBON_NAMESPACE_ROOT :: Recognition
