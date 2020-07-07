//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Microsoft.CognitiveServices.Speech;
using System;
using System.Collections.Generic;

namespace Microsoft.CognitiveServices.Speech.RemoteConversation
{
    internal class RemoteConversationTranscriptionPropertyCollectionImpl : PropertyCollection
    {
        public RemoteConversationTranscriptionPropertyCollectionImpl() : base(IntPtr.Zero)
        {
        }

        public override string GetProperty(string name)
        {
            return this.GetProperty(name, "");
        }

        public override string GetProperty(string name, string defaultValue)
        {
            if (m_localPropertyCollectionStringKey.ContainsKey(name))
            {
                return m_localPropertyCollectionStringKey[name];
            }
            else
            {
                return defaultValue;
            }
        }

        public override string GetProperty(PropertyId id)
        {
            if (PropertyId.SpeechServiceResponse_JsonResult == id)
            {
                return GetProperty("RESULT-Json");
            }
            else
            {
                return "";
            }
        }

        public override void SetProperty(string name, string value)
        {
            m_localPropertyCollectionStringKey.Add(name, value);
        }

        public override void Close()
        {
            base.Close();
        }

        private Dictionary<string, string> m_localPropertyCollectionStringKey = new Dictionary<string, string>();
    }
}
