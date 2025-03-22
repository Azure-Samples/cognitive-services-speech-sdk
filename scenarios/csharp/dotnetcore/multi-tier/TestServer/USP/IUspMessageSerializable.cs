//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Azure.AI.Test.Common.Servers.USP
{
    public interface IUspMessageSerializable
    {
        Message ToUspMessage();
    }
}
