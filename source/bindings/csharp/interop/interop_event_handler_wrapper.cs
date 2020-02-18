//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace Microsoft.CognitiveServices.Speech.Internal
{
    /// <summary>
    /// Helper class to simplify working with native events.
    /// </summary>
    /// <typeparam name="TEventArgs">The type of the event arguments</typeparam>
    internal class InteropEvent<TEventArgs> : DisposableBase
    {
        /// <summary>
        /// Delegate used to set the event callback in the native code
        /// </summary>
        /// <param name="instanceHandle">The handle to the native instance</param>
        /// <param name="callback">The managed callback to invoke when the event is fired</param>
        /// <param name="context">The context to pass to the native code. This will be set to the managed
        /// instance GC handle</param>
        /// <returns>An SPXHR code. Non-zero values indicate failures.</returns>
        internal delegate IntPtr SetNativeCallback(InteropSafeHandle instanceHandle, CallbackFunctionDelegate callback, IntPtr context);

        private static readonly CallbackFunctionDelegate delegateKeepAlive = FromNativeCallback;
        private readonly object locker = new object();
        private GCHandle _gcHandle;
        private object _sender;
        private InteropSafeHandle _senderHandle;
        private SetNativeCallback _nativeSetter;
        private event EventHandler<TEventArgs> _managedHandlers;

        /// <summary>
        /// Creates a new instance
        /// </summary>
        /// <param name="sender">The parent instance so events have the right sender</param>
        /// <param name="senderHandle">The handle to native version of the instance</param>
        /// <param name="setter">The delegate used to set the handler in native</param>
        public InteropEvent(object sender, InteropSafeHandle senderHandle, SetNativeCallback setter)
        {
            _gcHandle = GCHandle.Alloc(this, GCHandleType.Weak);
            _sender = sender;
            _senderHandle = senderHandle ?? throw new ArgumentNullException(nameof(senderHandle));
            _nativeSetter = setter ?? throw new ArgumentNullException(nameof(setter));
        }

        /// <summary>
        /// Adds a new managed event handler
        /// </summary>
        /// <param name="handler">The handler</param>
        public void Add(EventHandler<TEventArgs> handler)
        {
            CheckDisposed();
            lock (locker)
            {
                if (_managedHandlers == null)
                {
                    SpxExceptionThrower.ThrowIfFail(_nativeSetter(_senderHandle, delegateKeepAlive, GCHandle.ToIntPtr(_gcHandle)));
                }

                _managedHandlers += handler;
            }

            // Extend the lifetime of this to after the call to the native code to ensure
            // the finalizer doesn't run unexpectedly
            GC.KeepAlive(this);
        }

        /// <summary>
        /// Removes a managed handler
        /// </summary>
        /// <param name="handler">The handler</param>
        public void Remove(EventHandler<TEventArgs> handler)
        {
            CheckDisposed();
            lock (locker)
            {
                _managedHandlers -= handler;

                if (_managedHandlers == null)
                {
                    SpxExceptionThrower.ThrowIfFail(_nativeSetter(_senderHandle, null, IntPtr.Zero));
                }
            }

            // Extend the lifetime of this to after the call to the native code to ensure
            // the finalizer doesn't run unexpectedly
            GC.KeepAlive(this);
        }

        /// <summary>
        /// Disposes the object
        /// </summary>
        /// <param name="disposeManaged">True to dispose managed resources</param>
        protected override void Dispose(bool disposeManaged)
        {
            if (disposeManaged)
            {
                _sender = null;

                if (!_senderHandle.IsInvalid && !_senderHandle.IsClosed)
                {
                    SpxExceptionThrower.LogErrorIfFail(_nativeSetter(_senderHandle, null, IntPtr.Zero));
                }

                _senderHandle = null;
                _nativeSetter = null;

                if (_gcHandle.IsAllocated)
                {
                    _gcHandle.Free();
                }
            }
            else
            {
                if (!_senderHandle.IsInvalid && !_senderHandle.IsClosed)
                {
                    SpxExceptionThrower.LogErrorIfFail(_nativeSetter(_senderHandle, null, IntPtr.Zero));
                }
            }
        }

        /// <summary>
        /// The method called from native code to handle event.
        /// </summary>
        /// <param name="instanceHandle">The handle to the object in native code.</param>
        /// <param name="eventHandle">The handle to event object in native code.</param>
        /// <param name="context">The native pointer used to retrieve the corresponding managed instance.</param>
        [System.Diagnostics.CodeAnalysis.SuppressMessage(
            "Microsoft.Design",
            "CA1031",
            Justification = "We don't want to 'leak' exceptions back to the native code")]
        [MonoPInvokeCallback(typeof(CallbackFunctionDelegate))]
        private static void FromNativeCallback(IntPtr instanceHandle, IntPtr eventHandle, IntPtr context)
        {
            try
            {
                GCHandle weakManaged = GCHandle.FromIntPtr(context);
                if (weakManaged.IsAllocated)
                {
                    var instance = (InteropEvent<TEventArgs>)weakManaged.Target;
                    instance.CheckDisposed();

                    var eventArgs = (TEventArgs)Activator.CreateInstance(
                        typeof(TEventArgs),
                        BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Instance,
                        null,
                        new object[] { eventHandle },
                        null);
                    instance._managedHandlers?.Invoke(instance._sender, eventArgs);
                }
            }
            catch (Exception ex)
            {
                SpxExceptionThrower.LogError($"Unhandled exception in callback: {ex.ToString()}");
            }
        }
    }
}
