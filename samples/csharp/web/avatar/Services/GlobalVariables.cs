//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using Avatar.Models;

namespace Avatar.Services
{
    public static class GlobalVariables
    {
        public static Dictionary<Guid, ClientContext> ClientContexts { get; } = new();

        public static string? SpeechToken { get; set; }

        public static string? IceToken { get; set; }
    }
}
