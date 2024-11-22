# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

from typing import Literal
from rtclient import models as rt_models
from pydantic import BaseModel, Field

class SessionUpdateParams(rt_models.SessionUpdateParams):
    voice: str | None = None

class IceServer(BaseModel):
    urls: list[str] = Field(..., alias="Urls")
    username: str | None = Field(None, alias="Username")
    credential: str | None = Field(None, alias="Password")

class Session(rt_models.Session):
    ice_servers: list[IceServer] | None = None

class SessionUpdatedMessage(rt_models.SessionUpdatedMessage):
    session: Session | rt_models.Session

class AvatarConnectingMessage(rt_models.ServerMessageBase):
    type: Literal["extension.avatar.connecting"] = "extension.avatar.connecting"
    server_description: str