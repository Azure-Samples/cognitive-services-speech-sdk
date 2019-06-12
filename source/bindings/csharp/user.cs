//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

using System;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech.Conversation
{
    /// <summary>
    /// Represents a user in a conversation.
    /// Added in version 1.5.0 
    /// </summary>
    public sealed class User
    {
        /// <summary>
        /// Create a user using user id
        /// </summary>
        /// <param name="userId">A user id.</param>
        /// <returns>A user object</returns>
        public static User FromUserId(string userId)
        {
            IntPtr userPtr = IntPtr.Zero;
            IntPtr userIdPtr = Utf8StringMarshaler.MarshalManagedToNative(userId);
            try
            {
                ThrowIfFail(Internal.User.user_create_from_id(userIdPtr, out userPtr));
            }
            finally
            {
                Marshal.FreeHGlobal(userIdPtr);
            }
            return new User(userPtr);
        }

        /// <summary>
        /// Internal constructor
        /// </summary>
        internal User(IntPtr userPtr)
        {
            ThrowIfNull(userPtr);
            userHandle = new InteropSafeHandle(userPtr, Internal.User.user_release_handle);
        }

        /// <summary>
        /// Get user id.
        /// </summary>
        public string UserId
        {
            get
            {
                return SpxFactory.GetDataFromHandleUsingDelegate(Internal.User.user_get_id, userHandle, maxCharCount);
            }
        }

        // Hold the reference.
        internal InteropSafeHandle userHandle;
        internal const Int32 maxCharCount = 1024;
    }
}
