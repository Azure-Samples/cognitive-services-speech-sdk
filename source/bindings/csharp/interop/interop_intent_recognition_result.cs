//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXRESULTHANDLE = System.IntPtr;

    internal class IntentRecognitionResult : RecognitionResult
    {
        internal IntentRecognitionResult(IntPtr resultPtr) : base(resultPtr)
        {
            GetIntentId();
        }

        public string IntentId { get; private set; } = String.Empty;

        private void GetIntentId()
        {
            IntentId = SpxFactory.GetDataFromHandleUsingDelegate(intent_result_get_intent_id, resultHandle, maxCharCount);
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR intent_result_get_intent_id(SPXRESULTHANDLE hresult, IntPtr pszIntentId, UInt32 cchIntentId);
    }
}
