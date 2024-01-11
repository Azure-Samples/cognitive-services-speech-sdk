import os
import json
import requests
import logging

def raise_exception_when_reqeust_failed(method: str, api_url: str, response: requests.Response, logger: logging.Logger):
    if response is None:
        return
    if response.status_code >= 400:
        message = ('Service return error' + os.linesep + 
                   'Request URL: %s ' % method + api_url + os.linesep + 
                   'status code: %s' % response.status_code + os.linesep +
                   'response:' + os.linesep + 
                   json.dumps(response.json(), indent = 4))
        logger.error(message)
        raise Exception(message)
