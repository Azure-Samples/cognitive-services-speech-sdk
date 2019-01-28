//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    using SPXHR = System.IntPtr;
    using SPXPROPERTYBAGHANDLE = System.IntPtr;

    internal class PropertyCollection : SpxExceptionThrower, IDisposable
    {
        private SPXPROPERTYBAGHANDLE propbagHandle = IntPtr.Zero;
        private bool disposed = false;

        internal PropertyCollection(IntPtr propertyBagPtr)
        {
            propbagHandle = propertyBagPtr;
        }

        ~PropertyCollection()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (disposed) return;

            if (disposing)
            {
                // dispose managed resources
            }
            // dispose unmanaged resources
            if (propbagHandle != IntPtr.Zero)
            {
                LogErrorIfFail(property_bag_release(propbagHandle));
                propbagHandle = IntPtr.Zero;
            }
            disposed = true;
        }

        public string GetProperty(string propertyName, string defaultValue)
        {
            ThrowIfNull(propbagHandle);
            return GetPropertyString(propbagHandle, -1, propertyName, defaultValue);
        }

        public string GetProperty(PropertyId id, string defaultValue)
        {
            ThrowIfNull(propbagHandle);
            return GetPropertyString(propbagHandle, (int)id, null, defaultValue);
        }

        public string GetProperty(string name)
        {
            ThrowIfNull(propbagHandle);
            return GetPropertyString(propbagHandle, -1, name, "");
        }

        public string GetProperty(PropertyId id)
        {
            ThrowIfNull(propbagHandle);
            return GetPropertyString(propbagHandle, (int)id, null, "");
        }

        public string GetPropertyString(SPXPROPERTYBAGHANDLE propHandle, int id, string name, string defaultValue)
        {
            string propertyVal = string.Empty;
            IntPtr pStr = property_bag_get_string(propHandle, id, name, defaultValue);
            if (pStr != IntPtr.Zero)
            {
                try
                {
                    propertyVal = Utf8StringMarshaler.MarshalNativeToManaged(pStr);
                }
                finally
                {
                    ThrowIfFail(property_bag_free_string(pStr));
                }
            }
            return propertyVal;
        }

        public void SetProperty(string name, string value)
        {
            ThrowIfNull(propbagHandle);
            ThrowIfFail(property_bag_set_string(propbagHandle, -1, name, value));
        }

        public void SetProperty(PropertyId id, string value)
        {
            ThrowIfNull(propbagHandle);
            ThrowIfFail(property_bag_set_string(propbagHandle, (int)id, null, value));
        }

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern bool property_bag_is_valid(SPXPROPERTYBAGHANDLE hpropbag);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR property_bag_set_string(SPXPROPERTYBAGHANDLE hpropbag, Int32 id,
            [MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.LPStr)] string value);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern IntPtr property_bag_get_string(SPXPROPERTYBAGHANDLE hpropbag, Int32 id,
            [MarshalAs(UnmanagedType.LPStr)] string name,
            [MarshalAs(UnmanagedType.LPStr)] string defaultValue);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall, CharSet = CharSet.Ansi)]
        public static extern SPXHR property_bag_free_string(IntPtr ptr);

        [DllImport(Import.NativeDllName, CallingConvention = CallingConvention.StdCall)]
        public static extern SPXHR property_bag_release(SPXPROPERTYBAGHANDLE hpropbag);
    }
}
