# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import locale
from datetime import datetime
from dataclasses import dataclass
from urllib3.util import Url
from typing import Optional

from microsoft_speech_client_common.client_common_enum import (
    OperationStatus, OneApiState
)


@dataclass(kw_only=True)
class OperationDefinition:
    id: str
    status: OperationStatus


@dataclass(kw_only=True)
class StatelessResourceBaseDefinition:
    id: Optional[str] = None
    displayName: Optional[str] = None
    description: Optional[str] = None
    createdDateTime: Optional[datetime] = None


@dataclass(kw_only=True)
class StatefulResourceBaseDefinition(StatelessResourceBaseDefinition):
    status: Optional[OneApiState] = None
    lastActionDateTime: Optional[datetime] = None
