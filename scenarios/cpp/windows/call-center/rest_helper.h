//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#pragma once

#include <iostream>
#include <optional>
#include <set>
#include <sstream>
// You can download libcurl from:
// https://curl.se/download.html
#include <curl/curl.h>
// You can download json.hpp from:
// https://github.com/nlohmann/json/releases
#include "json.hpp"
#include "string_helper.h"

enum class RequestType { HTTP_GET, HTTP_POST, HTTP_DELETE };

enum HTTPStatusCode {
    HTTP_OK = 200,
    HTTP_CREATED = 201,
    HTTP_ACCEPTED = 202,
    HTTP_NO_CONTENT = 204
};

struct RestResult
{
    std::string text;
    nlohmann::json json;
    std::map<std::string, std::string> headers;
    
    RestResult(std::string text, nlohmann::json json, std::map<std::string, std::string> headers) : text(text), json(json), headers(headers) {}
};

class RestHelper
{
private:

    static size_t ContentCallback(char *data, size_t size, size_t nmemb, void *userdata)
    {
        std::string *response = (std::string *)userdata;
        response->append(data, nmemb * size);
        return nmemb * size;
    }

    static size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata)
    {
        std::map<std::string, std::string> *headers = (std::map<std::string, std::string> *) userdata;
        std::string header(buffer);
        int index = header.find(':', 0);
        if(std::string::npos != index)
        {
            // Use Trim to remove spaces before and after ':'.
            headers->insert(std::pair<std::string, std::string>(StringHelper::Trim(header.substr(0, index)), StringHelper::Trim(header.substr(index + 2))));
        }
        return nitems * size;
    }
    
    static std::shared_ptr<RestResult> Send(const RequestType requestType, const std::string& certificatePath, const std::string& url, std::optional<std::string> content, const std::string& key, const std::set<int>& expectedStatusCodes)
    {
        CURLcode result;
        CURL *curl_handle = curl_easy_init();
        struct curl_slist *request_headers = NULL;
        std::string response;
        std::map<std::string, std::string> response_headers;
        
        if (NULL != curl_handle) {
            curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYSTATUS, 1);
            curl_easy_setopt(curl_handle, CURLOPT_CAINFO, certificatePath.c_str());
            curl_easy_setopt(curl_handle, CURLOPT_CAPATH, certificatePath.c_str());
            
            // Default is GET
            if (RequestType::HTTP_POST == requestType)
            {
                curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "POST");
            }
            else if (RequestType::HTTP_DELETE == requestType)
            {
                curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");
            }
            
            curl_easy_setopt(curl_handle, CURLOPT_DEFAULT_PROTOCOL, "https");
            curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
            
            curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, ContentCallback);
            curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&response);
            curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, HeaderCallback);
            curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, (void *)&response_headers);
            
            request_headers = curl_slist_append(request_headers, std::string("Ocp-Apim-Subscription-Key: " + key).c_str());
            if (RequestType::HTTP_POST == requestType)
            {
                request_headers = curl_slist_append(request_headers, "Content-Type: application/json");
                curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, content.value().c_str());
            }
            curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, request_headers);
            result = curl_easy_perform(curl_handle);
            if (CURLE_OK != result)
            {
                std::ostringstream error;
                error << "curl_easy_perform() failed: " << curl_easy_strerror(result);
                throw std::exception(error.str().c_str());
            }

            long responseCode = 0;
            curl_easy_getinfo (curl_handle, CURLINFO_RESPONSE_CODE, &responseCode);
            if (!std::any_of(expectedStatusCodes.begin(), expectedStatusCodes.end(), [responseCode](int statusCode){ return statusCode == responseCode; }))
            {
                std::ostringstream error;
                error << "The response from " << url << " has an unexpected status code: " + responseCode << ". Response:\n" << response;
                throw std::exception(error.str().c_str());
            }

            curl_easy_cleanup(curl_handle);
            curl_slist_free_all(request_headers);
            if (!response.empty())
            {
                return std::make_shared<RestResult>(response, nlohmann::json::parse(response), response_headers);
            }
            else
            {
                return std::make_shared<RestResult>(response, nlohmann::json(), response_headers);
            }
        }
        else
        {
            throw std::exception("curl_easy_init() returned NULL.");
        }
    }
    
public:
    static void Initialize()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    static void Dispose()
    {
        curl_global_cleanup();
    }

    static std::shared_ptr<RestResult> SendGet(const std::string& certificatePath, const std::string& url, const std::string& key, const std::set<int>& expectedStatusCodes)
    {
        std::string content;
        return Send(RequestType::HTTP_GET, certificatePath, url, std::nullopt, key, expectedStatusCodes);
    }
    
    static std::shared_ptr<RestResult> SendPost(const std::string& certificatePath, const std::string& url, const std::string& content, const std::string& key, const std::set<int>& expectedStatusCodes)
    {
        return Send(RequestType::HTTP_POST, certificatePath, url, std::optional<std::string> { content }, key, expectedStatusCodes);
    }
    
    static std::shared_ptr<RestResult> SendDelete(const std::string& certificatePath, const std::string& url, const std::string& key, const std::set<int>& expectedStatusCodes)
    {
        return Send(RequestType::HTTP_DELETE, certificatePath, url, std::nullopt, key, expectedStatusCodes);
    }
};
