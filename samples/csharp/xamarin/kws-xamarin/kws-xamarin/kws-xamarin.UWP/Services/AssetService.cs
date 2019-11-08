//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Threading.Tasks;
using Windows.Storage;

namespace kws_xamarin.UWP
{
    class AssetService : IAssetService
    {
        public string GetAssetPath(string filename)
        {
            Task<string> task = Task.Run(async () => await GetAssetPathAsync(filename));
            return task.Result;
        }

        private async Task<string> GetAssetPathAsync(string filename)
        {
            string kwsModel = "ms-appx:///Assets/" + filename;
            StorageFile kwsModelfile = await StorageFile.GetFileFromApplicationUriAsync(new Uri(kwsModel));
            return kwsModelfile.Path;
        }
    }
}
