#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

#this is CMakeLists for ctest unittest folder. It only redirects to the real unittests
if(NOT ${skip_unittests})
    add_subdirectory(ctestunittests)
endif()