//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech
{
    internal sealed class Utf32StringMarshaler : ICustomMarshaler
    {
        private static Utf32StringMarshaler marshaler;

        // UTF32 is fixed length encoding, 4 bytes per codepoint.
        private static readonly int sizeOfUnicodeCodepointInBytes = 4;

        // Limit the length by 4 MB for safety check if something bad has happened on the native side.
        private static readonly int lengthLimit = 1 << 22;

        public object MarshalNativeToManaged(IntPtr native)
        {
            // Identifying the length of the UTF32 string by searching for four 0 bytes.
            int lengthInBytes = 0;
            while (Marshal.ReadInt32(native + lengthInBytes) != 0 && lengthInBytes < lengthLimit)
            {
                lengthInBytes += sizeOfUnicodeCodepointInBytes;
            }

            if (lengthInBytes >= lengthLimit)
            {
                throw new ArgumentException("Provided data is not a string or it has the size exceeding ${lengthLimit} bytes.");
            }

            var buffer = new byte[lengthInBytes];

            // Performancewise this can be improved by switching to unsafe code and avoiding copying.
            Marshal.Copy(native, buffer, 0, lengthInBytes);
            return Encoding.UTF32.GetString(buffer);
        }

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

            byte[] buffer = Encoding.UTF32.GetBytes(managedAsString);

            // allocating memory for byte array and null termination character.
            IntPtr strNativePointer = Marshal.AllocHGlobal(buffer.Length + sizeOfUnicodeCodepointInBytes);
            Marshal.Copy(buffer, 0, strNativePointer, buffer.Length);

            // adding null termination to the end of the allocated memory. 
            Marshal.WriteInt32(strNativePointer + buffer.Length, 0);
            return strNativePointer; 
        }

        public void CleanUpNativeData(IntPtr native)
        {
            Marshal.FreeHGlobal(native);
            native = IntPtr.Zero;
        }

        public void CleanUpManagedData(object managed) {}

        public int GetNativeDataSize()
        {
            // The size is always unknown for this marshaler. 
            return -1;
        }

        public static ICustomMarshaler GetInstance(string cookie)
        {
            if (marshaler == null)
            {
                marshaler = new Utf32StringMarshaler();
            }

            return marshaler;
        }
    }
}
