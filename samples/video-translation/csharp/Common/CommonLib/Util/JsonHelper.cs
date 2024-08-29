//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CustomVoice;

using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.SpeechServices.CommonLib;
using Microsoft.SpeechServices.CustomVoice.TtsLib.TtsUtil;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

public static class JsonHelper
{
    public static (bool success, string error, IEnumerable<string> texts) ExtractElementsByJSONPath(string jsonContent, string jsonPath)
    {
        var texts = new List<string>();
        if (string.IsNullOrEmpty(jsonContent))
        {
            return (true, null, texts);
        }

        try
        {
            JObject o = JObject.Parse(jsonContent);
            if (o == null)
            {
                return (false, "Empty JObject returned when parse content with JObject.", null);
            }

            var tokens = o.SelectTokens(jsonPath);
            if (tokens?.Any() ?? false)
            {
                tokens.Where(x => x != null).ToList().ForEach(x => texts.Add(x.ToString()));
            }
        }
        catch (JsonReaderException e)
        {
            return (false, e.Message, null);
        }

        return (true, null, texts);
    }

    public static (bool success, string error, T result) TryParse<T>(string jsonString, JsonSerializerSettings readerSettings)
    {
        ArgumentNullException.ThrowIfNull(readerSettings);

        if (string.IsNullOrEmpty(jsonString))
        {
            return (false, "Json input should not be empty.", default(T));
        }

        T result = default(T);
        (var success, var error) = ExceptionHelper.HasRunWithoutException(() =>
        {
            result = JsonConvert.DeserializeObject<T>(jsonString, readerSettings);
        });

        return (success, error, result);
    }

    public static (bool success, string error, T result) TryParse<T>(string jsonString)
    {
        return TryParse<T>(jsonString, CustomContractResolver.ReaderSettings);
    }
}
