//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;

namespace Carbon
{
    /// <summary>
    /// Represents a parameter's value, which can either be a string, an integer or a bool.
    /// </summary>


    public class Value
    {
        /// <summary>
        /// Creates a Value that contains a string.
        /// </summary>
        /// <param name="str">The string to be set.</param>
        public Value(string str)
        {
            strValue = str;
            ValueType = typeof(string);
        }

        /// <summary>
        /// Creates a Value that contains an integer.
        /// </summary>
        /// <param name="i">The integer to be set.</param>
        public Value(int i)
        {
            intValue = i;
            ValueType = typeof(int);
        }

        /// <summary>
        /// Creates a Value that contains a boolean.
        /// </summary>
        /// <param name="b">The boolean to be set.</param>
        public Value(bool b)
        {
            boolValue = b;
            ValueType = typeof(bool);
        }

        /// <summary>
        /// Returns the string stored in this Value. Exception is thrown if this Value does not contain a string.
        /// </summary>
        /// <returns>the string stored in this Value.</returns>
        public string AsString()
        {
            if (ValueType != typeof(string))
            {
                throw new InvalidOperationException("Not a string type.");
            }
            else
            {
                return strValue;
            }
        }

        /// <summary>
        /// Returns the integer stored in this Value. Exception is thrown if this Value does not contain an integer.
        /// </summary>
        /// <returns>the integer stored in this Value.</returns>
        public int AsInt()
        {
            if (ValueType != typeof(int))
            {
                throw new InvalidOperationException("Not an int type.");
            }
            else
            {
                return intValue;
            }
        }

        /// <summary>
        /// Returns the boolean stored in this Value. Exception is thrown if this Value does not contain a boolean.
        /// </summary>
        /// <returns>the boolean stored in this Value.</returns>
        public bool AsBoolean()
        {
            if (ValueType != typeof(bool))
            {
                throw new InvalidOperationException("Not a bool type.");
            }
            else
            {
                return boolValue;
            }
        }

        /// <summary>
        /// The property specifies the type of value stored in this Value.
        /// </summary>
        public Type ValueType { get; }

        private string strValue;
        private int intValue;
        private bool boolValue;
    }
}
       