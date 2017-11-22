#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

set(sdk_base_path ../../../..)
set(mbed_project_base "azure_c_shared_utility" CACHE STRING "The item being built")
include (${sdk_base_path}/c/tools/mbed_build/mbedbldtemplate.txt)
