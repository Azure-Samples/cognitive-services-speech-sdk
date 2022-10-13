//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using AVFoundation;
using System;
using System.Threading.Tasks;

namespace kws_xamarin.iOS
{
    class MicrophoneService : IMicrophoneService
    {
        private TaskCompletionSource<bool> tcsPermissions;

        public Task<bool> GetPermissionsAsync()
        {
            tcsPermissions = new TaskCompletionSource<bool>(TaskCreationOptions.RunContinuationsAsynchronously);
            RequestMicPermission();
            return tcsPermissions.Task;
        }

        private void RequestMicPermission()
        {
            var session = AVAudioSession.SharedInstance();
            session.RequestRecordPermission((granted) =>
            {
                Console.WriteLine($"Audio Permission: {granted}");
                if (granted)
                {
                    tcsPermissions.TrySetResult(granted);
                }
                else
                {
                    tcsPermissions.TrySetResult(false);
                    Console.WriteLine("YOU MUST ENABLE MICROPHONE PERMISSION");
                }
            });
        }

        public void OnRequestPermissionsResult(bool isGranted)
        {
            tcsPermissions.TrySetResult(isGranted);
        }
    }
}
