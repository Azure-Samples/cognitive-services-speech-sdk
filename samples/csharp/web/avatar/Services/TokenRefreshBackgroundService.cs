//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Avatar.Services
{
    public class TokenRefreshBackgroundService : BackgroundService
    {
        private readonly IceTokenService _iceTokenService;

        private readonly SpeechTokenService _speechTokenService;

        public TokenRefreshBackgroundService(IceTokenService iceTokenService, SpeechTokenService speechTokenService)
        {
            _iceTokenService = iceTokenService;
            _speechTokenService = speechTokenService;
        }

        protected override async Task ExecuteAsync(CancellationToken stoppingToken)
        {
            // Refresh ICE token once at startup
            await _iceTokenService.RefreshIceTokenAsync();

            // Start the background task for refreshing the speech token
            var refreshSpeechTokenTask = _speechTokenService.RefreshSpeechTokenAsync(stoppingToken);
            await Task.WhenAny(refreshSpeechTokenTask, Task.Delay(-1, stoppingToken));
        }
    }
}
