//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// UTF-8 string marshaler.
    /// </summary>
    internal sealed class Utf8StringMarshaler : ICustomMarshaler
    {
        private static Utf8StringMarshaler marshaler;

        // Limit the length by 4 MB for safety check if something bad has happened on the native side.
        private const int lengthLimit = 1 << 22;

        /// <summary>Converts the unmanaged data to managed data.</summary>
        /// <param name="native">A pointer to the unmanaged data to be wrapped.</param>
        /// <returns>An object that represents the managed view of the COM data.</returns>
        public object MarshalNativeToManaged(IntPtr native)
        {
            // Identifying the length of the UTF8 string by searching for 0 byte.
            int lengthInBytes = 0;
            while (Marshal.ReadByte(native + lengthInBytes) != 0 && lengthInBytes < lengthLimit)
            {
                lengthInBytes++;
            }

            if (lengthInBytes >= lengthLimit)
            {
                throw new ArgumentException("Provided data is not a string or it has the size exceeding ${lengthLimit} bytes.");
            }

            var buffer = new byte[lengthInBytes];

            // Performancewise this can be improved by switching to unsafe code and avoiding copying.
            Marshal.Copy(native, buffer, 0, lengthInBytes);
            return Encoding.UTF8.GetString(buffer);
        }

        /// <summary>Converts the managed data to unmanaged data.</summary>
        /// <param name="managed">The managed object to be converted.</param>
        /// <returns>A pointer to the COM view of the managed object.</returns>
        public IntPtr MarshalManagedToNative(object managed)
        {
            if (managed == null)
            {
                return IntPtr.Zero;
            }

            var managedAsString = managed as string;

            if  (managedAsString == null)
            {
                throw new ArgumentException("The input argument is not a string type.");
            }

            byte[] buffer = Encoding.UTF8.GetBytes(managedAsString);

            // allocating memory for byte array and null termination character.
            IntPtr strNativePointer = Marshal.AllocHGlobal(buffer.Length + 1);
            Marshal.Copy(buffer, 0, strNativePointer, buffer.Length);

            // adding null termination to the end of the allocated memory.
            Marshal.WriteByte(strNativePointer + buffer.Length, 0);
            return strNativePointer;
        }

        /// <summary>Performs necessary cleanup of the unmanaged data when it is no longer needed.</summary>
        /// <param name="native">A pointer to the unmanaged data to be destroyed.</param>
        public void CleanUpNativeData(IntPtr native)
        {
            Marshal.FreeHGlobal(native);
            native = IntPtr.Zero;
        }

        /// <summary>Performs necessary cleanup of the managed data when it is no longer needed.</summary>
        /// <param name="managed">The managed object to be destroyed.</param>
        public void CleanUpManagedData(object managed) {}

        /// <summary>Returns the size of the native data to be marshaled.</summary>
        /// <returns>The size, in bytes, of the native data.</returns>
        public int GetNativeDataSize()
        {
            // The size is always unknown for this marshaler.
            return -1;
        }

        /// <summary>Returns an instance of the custom marshaler.</summary>
        /// <param name="cookie">String "cookie" parameter that can be used by the custom marshaler.</param>
        /// <returns>An instance of the custom marshaler.</returns>
        public static ICustomMarshaler GetInstance(string cookie)
        {
            if (marshaler == null)
            {
                marshaler = new Utf8StringMarshaler();
            }

            return marshaler;
        }
    }
}
