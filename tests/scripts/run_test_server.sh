#!/bin/bash

set -e

npm install ws http-headers && node test-server.js
