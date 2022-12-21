//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import java.util.*;

import com.google.gson.JsonObject;

public class RestResult {
    final private String text;
    final private JsonObject json;
    final private Map<String,List<String>> headers;
    
    public String getText() {
        return text;
    }
    public JsonObject getJson() {
        return json;
    }
    public Map<String,List<String>> getHeaders() {
        return headers;
    }
    
    public RestResult(
        String text,
        JsonObject json,
        Map<String,List<String>> headers
    ) {
        this.text = text;
        this.json = json;
        this.headers = headers;
    }
}