//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.IO;
using System.Runtime.Remoting.Contexts;
using System.Threading.Tasks;
using Android;
using Android.App;
using Android.OS;
using Java.IO;
using File = Java.IO.File;

namespace kws_xamarin.Droid
{
    class AssetService : IAssetService 
    {
        private File cacheFile;

        public string GetAssetPath(string filename)
        {
            var context = MainActivity.Instance;
            cacheFile = new File(context.CacheDir, filename);

            if (!cacheFile.Exists())
            {
                using (var br = new BinaryReader(Application.Context.Assets.Open(filename)))
                {
                    using (var bw = new BinaryWriter(new FileStream(cacheFile.AbsolutePath, FileMode.Create)))
                    {
                        byte[] buffer = new byte[2048];
                        int length = 0;
                        while ((length = br.Read(buffer, 0, buffer.Length)) > 0)
                        {
                            bw.Write(buffer, 0, length);
                        }
                    }
                }
            }
            return cacheFile.AbsolutePath;
        }
    }
}
