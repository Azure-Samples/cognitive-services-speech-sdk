//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Threading;

namespace Azure.AI.Test.Common
{
    /// <summary>
    /// Base class that implements IDisposable
    /// </summary>
    public abstract class DisposableBase : IDisposable
    {
        private int isDisposed;

        /// <summary>
        /// The base destructor
        /// </summary>
        ~DisposableBase()
        {
            this.Dispose(false);
        }

        /// <summary>
        /// Gets whether or not this object has been disposed
        /// </summary>
        public virtual bool IsDisposed
        {
            get
            {
                return this.isDisposed == 1;
            }
        }

        /// <summary>
        /// Disposes the current object. It is safe to call this multiple times
        /// </summary>
        public void Dispose()
        {
            if (Interlocked.Exchange(ref this.isDisposed, 1) == 0)
            {
                this.Dispose(true);
                GC.SuppressFinalize(this);
            }
        }

        /// <summary>
        /// The method that actually disposes resources
        /// </summary>
        /// <param name="disposeManaged">True if we should dispose both managed and native
        /// resources, false if we should only dispose native resources</param>
        protected abstract void Dispose(bool disposeManaged);

        /// <summary>
        /// Checks if the object has been disposed and throws an exception if so
        /// </summary>
        /// <exception cref="System.ObjectDisposedException">If the object has been disposed</exception>
        protected virtual void CheckDisposed()
        {
            if (this.IsDisposed)
            {
                throw new ObjectDisposedException(this.GetType().FullName);
            }
        }
    }
}
