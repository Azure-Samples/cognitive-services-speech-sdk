//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
using System;
using System.Runtime.InteropServices;
using Microsoft.CognitiveServices.Speech.Internal;
using static Microsoft.CognitiveServices.Speech.Internal.SpxExceptionThrower;

namespace Microsoft.CognitiveServices.Speech
{
    /// <summary>
    /// Class to retrieve or set a property value from a property collection.
    /// </summary>
    public class PropertyCollection
    {
        private InteropSafeHandle propbagHandle;
        /// <summary>
        /// Constructor to create PropertyCollection
        /// </summary>
        /// <param name="propertyBagPtr">The property bag handle.</param>
        protected internal PropertyCollection(IntPtr propertyBagPtr)
        {
            if(propertyBagPtr != IntPtr.Zero)
            {
                propbagHandle = new InteropSafeHandle(propertyBagPtr, Internal.PropertyCollection.property_bag_release);
            }
        }

        /// <summary>
        /// Dispose the property bag
        /// </summary>
        public virtual void Close()
        {
            propbagHandle.Dispose();
        }

        /// <summary>
        /// Returns value of a property.
        /// If the property value is not defined, an empty string is returned.
        /// </summary>
        /// <param name="id">The ID of property. See <see cref="PropertyId"/></param>
        /// <returns>value of the property</returns>
        public virtual string GetProperty(PropertyId id)
        {
            ThrowIfNull(propbagHandle);
            return GetPropertyString(propbagHandle, (int)id, null, "");
        }

        /// <summary>
        /// Returns value of a property.
        /// If the property value is not defined, an empty string is returned.
        /// </summary>
        /// <param name="propertyName">The name of property</param>
        /// <returns>value of the property</returns>
        public virtual string GetProperty(string propertyName)
        {
            ThrowIfNull(propbagHandle);
            return GetPropertyString(propbagHandle, -1, propertyName, "");
        }

        /// <summary>
        /// Returns value of a property.
        /// If the property value is not defined, the specified default value is returned.
        /// </summary>
        /// <param name="id">The id of property. See <see cref="PropertyId"/></param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        public virtual string GetProperty(PropertyId id, string defaultValue)
        {
            ThrowIfNull(propbagHandle);
            return GetPropertyString(propbagHandle, (int)id, null, defaultValue);
        }

        /// <summary>
        /// Returns value of a property.
        /// If the property value is not defined, the specified default value is returned.
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <param name="defaultValue">The default value which is returned if no value is defined for the property.</param>
        /// <returns>value of the property.</returns>
        public virtual string GetProperty(string propertyName, string defaultValue)
        {
            ThrowIfNull(propbagHandle);
            return GetPropertyString(propbagHandle, -1, propertyName, defaultValue);
        }

        /// <summary>
        /// Set value of a property.
        /// </summary>
        /// <param name="id">The id of property. See <see cref="PropertyId"/></param>
        /// <param name="value">value to set</param>
        public virtual void SetProperty(PropertyId id, string value)
        {
            ThrowIfNull(propbagHandle);
            IntPtr valuePtr = Utf8StringMarshaler.MarshalManagedToNative(value);
            try
            {
                ThrowIfFail(Internal.PropertyCollection.property_bag_set_string(propbagHandle, (int)id, IntPtr.Zero, valuePtr));
            }
            finally
            {
                Marshal.FreeHGlobal(valuePtr);
            }
        }

        /// <summary>
        /// Set value of a property.
        /// </summary>
        /// <param name="propertyName">The name of property.</param>
        /// <param name="value">value to set</param>
        public virtual void SetProperty(string propertyName, string value)
        {
            ThrowIfNull(propbagHandle);
            IntPtr valuePtr = Utf8StringMarshaler.MarshalManagedToNative(value);
            IntPtr propertyNamePtr = Utf8StringMarshaler.MarshalManagedToNative(propertyName);
            try
            {
                ThrowIfFail(Internal.PropertyCollection.property_bag_set_string(propbagHandle, -1, propertyNamePtr, valuePtr));
            }
            finally
            {
                Marshal.FreeHGlobal(valuePtr);
                Marshal.FreeHGlobal(propertyNamePtr);
            }
        }

        private string GetPropertyString(InteropSafeHandle propHandle, int id, string name, string defaultValue)
        {
            string propertyVal = string.Empty;
            IntPtr namePtr = Utf8StringMarshaler.MarshalManagedToNative(name);
            IntPtr defaultValuePtr = Utf8StringMarshaler.MarshalManagedToNative(defaultValue);
            try
            {
                IntPtr pStr = Internal.PropertyCollection.property_bag_get_string(propHandle, id, namePtr, defaultValuePtr);
                if (pStr != IntPtr.Zero)
                {
                    try
                    {
                        propertyVal = Utf8StringMarshaler.MarshalNativeToManaged(pStr);
                    }
                    finally
                    {
                        ThrowIfFail(Internal.PropertyCollection.property_bag_free_string(pStr));
                    }
                }
            }
            finally
            {
                Marshal.FreeHGlobal(namePtr);
                Marshal.FreeHGlobal(defaultValuePtr);
            }
            return propertyVal;
        }
    }
}
