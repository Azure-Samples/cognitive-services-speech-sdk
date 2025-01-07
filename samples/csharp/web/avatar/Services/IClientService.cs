//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Avatar.Models;

namespace Avatar.Services
{
    public interface IClientService
    {
        Guid InitializeClient();

        ClientContext GetClientContext(Guid clientId);

        void RemoveClient(Guid clientId);
    }
}
