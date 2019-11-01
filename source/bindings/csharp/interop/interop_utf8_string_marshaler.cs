//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Text;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    /// <summary>
    /// UTF-8 string marshaler.
    /// </summary>
    internal static class Utf8StringMarshaler
    {
        // Limit the length by 4 MB for safety check if something bad has happened on the native side.
        private const int lengthLimit = 1 << 22;

        /// <summary>Converts the unmanaged data to managed data.</summary>
        /// <param name="native">A pointer to the unmanaged data to be wrapped.</param>
        /// <returns>An object that represents the managed view of the COM data.</returns>
        public static string MarshalNativeToManaged(IntPtr native)
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

            // Performance wise this can be improved by switching to unsafe code and avoiding copying.
            Marshal.Copy(native, buffer, 0, lengthInBytes);
            return Encoding.UTF8.GetString(buffer);
        }

        /// <summary>Converts the managed data to unmanaged data.</summary>
        /// <param name="str">The managed string to be converted.</param>
        /// <returns>A pointer to the COM view of the managed object.</returns>
        public static IntPtr MarshalManagedToNative(string str)
        {
            return MarshalManagedToNative(str, out var _);
        }

        internal static IntPtr MarshalManagedToNative(string str, out int length)
        {
            if (str == null)
            {
                length = 0;
                return IntPtr.Zero;
            }

            byte[] buffer = Encoding.UTF8.GetBytes(str);
            length = buffer.Length + 1;

            // allocating memory for byte array and null termination character.
            IntPtr strNativePointer = Marshal.AllocHGlobal(length);
            Marshal.Copy(buffer, 0, strNativePointer, buffer.Length);

            // adding null termination to the end of the allocated memory.
            Marshal.WriteByte(strNativePointer + buffer.Length, 0);
            return strNativePointer;
        }
    }

    /// <summary>
    /// Helper class to simplify marshaling a UTF8 string to native and back. You should use
    /// this in a using() block.
    /// </summary>
    internal class Utf8StringHandle : SafeHandle
    {
        /// <summary>
        /// Returns a null UTF8 native string handle
        /// </summary>
        public static Utf8StringHandle Null = new Utf8StringHandle();

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="str">The string to marshal</param>
        public Utf8StringHandle(string str) : base(IntPtr.Zero, true)
        {
            int length;
            SetHandle(Utf8StringMarshaler.MarshalManagedToNative(str, out length));
            Length = (uint)length;
        }

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="maxLength">The maximum string length including the terminating \0</param>
        public Utf8StringHandle(uint maxLength) : base(IntPtr.Zero, true)
        {
            if (maxLength < 0)
            {
                throw new ArgumentOutOfRangeException(nameof(maxLength), "Cannot have a negative length");
            }

            Length = maxLength;
            SetHandle(Marshal.AllocHGlobal(new IntPtr(Length)));
        }

        /// <summary>
        /// This is deliberately private to prevent using as an out parameter. You cannot free
        /// native code in managed unless it was explicitly allocated using LocalAlloc or
        /// CoTaskMemAlloc(). Using this as an out parameter could result in trying to free
        /// memory allocated using malloc or new().
        /// </summary>
        private Utf8StringHandle() : base(IntPtr.Zero, false)
        {
        }

        /// <summary>
        /// Gets the length of the native string handle (including the trailing \0).
        /// </summary>
        public uint Length { get; }

        /// <summary>
        /// Gets the managed string representation of the native string
        /// </summary>
        /// <returns>The managed string representation</returns>
        public override string ToString()
        {
            if (IsClosed || IsInvalid) return null;

            return Utf8StringMarshaler.MarshalNativeToManaged(handle);
        }

        /// <summary>Gets a value that indicates whether the handle is invalid.</summary>
        /// <returns>
        ///     <see langword="true" /> if the handle is not valid; otherwise, <see langword="false" />.
        /// </returns>
        public override bool IsInvalid => handle == IntPtr.Zero;

        /// <summary>
        /// Executes the code required to free the handle.
        /// </summary>
        /// <returns>
        ///     <see langword="true" /> if the handle is released successfully; otherwise, in the event of a catastrophic failure, <see langword=" false" />.
        /// </returns>
        protected override bool ReleaseHandle()
        {
            if (!IsInvalid)
            {
                Marshal.FreeHGlobal(handle);
            }

            return true;
        }
    }
}
