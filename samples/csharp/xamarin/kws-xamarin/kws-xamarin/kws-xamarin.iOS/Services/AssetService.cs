//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

using Foundation;
using UIKit;

namespace kws_xamarin.iOS
{
    class AssetService : IAssetService
    {
        public string GetAssetPath(string filename)
        {
            var filePath = NSBundle.MainBundle.PathForResource(filename, "");
            return filePath;
        }
    }
}
