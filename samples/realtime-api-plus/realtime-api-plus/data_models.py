# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

from rtclient import models as rt_models

class SessionUpdateParams(rt_models.SessionUpdateParams):
    voice: str | None = None