//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System.Threading.Tasks;

namespace kws_xamarin
{
    public interface IAssetService
    {
        string GetAssetPath(string filename);
    }
}
