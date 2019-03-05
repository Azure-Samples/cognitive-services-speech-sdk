#!/usr/bin/env bash

PATH_TO_TGZ="$1"
VERSION="$2"
NPM_REGISTRY="$3"
TGZ="$(basename "$PATH_TO_TGZ")"

SHA1SUM="$(sha1sum "$PATH_TO_TGZ" | cut -d' ' -f1)"
SRI_SHA512="sha512-$(openssl dgst -sha512 -binary "$PATH_TO_TGZ" | openssl base64 -A)"
DATE="$(date -u +"%Y-%m-%dT%H:%M:%S.000Z")"

cat <<HEREDOC
{
  "name": "microsoft-cognitive-services-speech-sdk",
  "description": "Microsoft Cognitive Services Speech SDK",
  "dist-tags": { },
  "versions": {
    "$VERSION": {
      "name": "microsoft-cognitiveservices-speech-sdk",
      "version": "$VERSION",
      "description": "Microsoft Cognitive Services Speech SDK",
      "main": "distrib/microsoft.cognitiveservices.speech.sdk.js",
      "types": "distrib/microsoft.cognitiveservices.speech.sdk.d.ts",
      "keywords": [
        "microsoft",
        "speech",
        "cognitiveservices",
        "sdk",
        "javascript",
        "typescript",
        "ts",
        "js",
        "browser",
        "websocket",
        "speechtotext"
      ],
      "author": {
        "name": "Microsoft"
      },
      "license": "https://aka.ms/csspeech/license201809",
      "devDependencies": {
        "gulp": "^3.9.1",
        "gulp-minify": "0.0.15",
        "gulp-sourcemaps": "^2.6.0",
        "gulp-tslint": "^8.0.0",
        "gulp-typescript": "^3.2.3",
        "source-map-loader": "^0.2.3",
        "tslint": "^5.8.0",
        "typescript": "^2.6.2",
        "webpack-stream": "^4.0.0"
      },
      "scripts": {
        "prepublishOnly": "npm install && gulp build",
        "build": "npm install && gulp build",
        "bundle": "npm install && gulp bundle"
      },
      "dist": {
        "integrity": "$SRI_SHA512",
        "shasum": "$SHA1SUM",
        "tarball": "$NPM_REGISTRY/microsoft-cognitiveservices-speech-sdk-$VERSION.tgz"
      },
      "maintainers": [
        {
          "email": "fmegen@microsoft.com",
          "name": "fmegen"
        },
        {
          "email": "mahilleb@microsoft.com",
          "name": "mahilleb"
        }
      ],
      "directories": {}
    }
  },
  "readme": "https://aka.ms/csspeech",
  "maintainers": [
    {
      "email": "fmegen@microsoft.com",
      "name": "fmegen"
    },
    {
      "email": "mahilleb@microsoft.com",
      "name": "mahilleb"
    }
  ],
  "time": {
    "modified": "$DATE",
    "created": "$DATE",
    "$VERSION": "$DATE"
  },
  "homepage": "https://aka.ms/csspeech",
  "keywords": [
    "microsoft",
    "cognitiveservices",
    "speech",
    "sdk",
    "javascript",
    "typescript",
    "ts",
    "js",
    "browser",
    "websocket",
    "speechtotext"
  ],
  "author": {
    "name": "Microsoft"
  },
  "license": "MIT",
  "readmeFilename": "README.md"
}

HEREDOC
