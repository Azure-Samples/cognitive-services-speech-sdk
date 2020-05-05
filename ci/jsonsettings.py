#!/usr/bin/python

import sys
import json
import io


def get(jsonFile, jsonData):
    jsonNames = jsonData.split(".")
    with io.open(jsonFile, encoding="utf8", errors='ignore') as file_object:
        # store file data in object
        data = json.load(file_object)

    if(len(jsonNames) > 1):
        print(data.get(jsonNames[0])[jsonNames[1]])
    else:
        print(data.get(jsonData))
