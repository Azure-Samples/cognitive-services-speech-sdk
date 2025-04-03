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
            // Start the background task for refreshing the token
            var refreshIceTokenTask = _iceTokenService.RefreshIceTokenAsync(stoppingToken);
            var refreshSpeechTokenTask = _speechTokenService.RefreshSpeechTokenAsync(stoppingToken);

            await Task.WhenAll(refreshIceTokenTask, refreshSpeechTokenTask);
        }
    }
}
