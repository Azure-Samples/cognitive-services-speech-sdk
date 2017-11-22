using Microsoft.Win32.SafeHandles;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace Azure.IoT.Internal.Tools
{
    /// <summary>
    /// Provides a wrapper around Win32 API for serial port communication.
    /// </summary>
    /// <remarks>
    /// Motivation for this wrapper:
    /// The .Net 4.5 SerialPort currently has a bug in BreakState, making it unusable if this function is needed.
    /// </remarks>
    public class SerialPort : IDisposable
    {
        #region Data
        
        private IntPtr m_fileHandle;
        private string m_portName;
        
        #endregion Data

        public bool IsOpen
        {
            get { return m_fileHandle != IntPtr.Zero && m_fileHandle.ToInt32() != Win32Api.INVALID_HANDLE_VALUE; }
        }

        public SerialPort(string portName)
        {
            m_portName = portName;
        }

        /// <summary>
        /// Opens the serial port for communication.
        /// </summary>
        /// <returns>True if the port is open successfully.</returns>
        public bool Open()
        {
            m_fileHandle = Win32Api.CreateFile(m_portName);
            
            return IsOpen;
        }

        /// <summary>
        /// Closes the serial port for communication.
        /// </summary>
        /// <returns>True if the port is closed successfully.</returns>
        public bool Close()
        {
            if (Win32Api.CloseHandle(m_fileHandle))
            {
                m_fileHandle = IntPtr.Zero;
            }

            return !IsOpen;
        }

        /// <summary>
        /// Sets the serial port into break state.
        /// </summary>
        /// <remarks>SetBreakState() followed by ClearBrakeState() is equivalent to sending CTRL+BREAK.</remarks>
        /// <returns>True if the state was set correctly, false otherwise.</returns>
        public bool SetBreakState()
        {
            return Win32Api.SetCommBreak(m_fileHandle);
        }

        /// <summary>
        /// Clears the break state set in the serial port.
        /// </summary>
        /// <returns>True if the state was set correctly, false otherwise.</returns>
        public bool ClearBreakState()
        {
            return Win32Api.ClearCommBreak(m_fileHandle);
        }

        /// <summary>
        /// Gets the timeouts currently set for the serial port communication.
        /// </summary>
        /// <returns>The timeouts currently set.</returns>
        public SerialPortTimeouts GetTimeouts()
        {
            if (!IsOpen) throw new InvalidOperationException("Port is not open.");

            SerialPortTimeouts commTimeouts = new SerialPortTimeouts();
            if (!Win32Api.GetCommTimeouts(m_fileHandle, ref commTimeouts))
            {
                throw new Win32Exception(Win32Api.GetLastError(), "'GetCommTimeouts()' failed");
            }

            return commTimeouts;
        }

        /// <summary>
        /// Sets the timeouts for the serial port communication.
        /// </summary>
        /// <param name="timeouts">Timeouts to be set.</param>
        public void SetTimeouts(SerialPortTimeouts timeouts)
        {
            if (!Win32Api.SetCommTimeouts(m_fileHandle, ref timeouts))
            {
                throw new Win32Exception(Win32Api.GetLastError(), "'SetCommTimeouts()' failed");                
            }
        }

        /// <summary>
        /// Sets the read timeouts to return only the contents in the buffer, returning immediatelly.
        /// </summary>
        public void SetDefaultTimeouts()
        {
            if (IsOpen)
            {
                SerialPortTimeouts timeouts = this.GetTimeouts();
                timeouts.ReadIntervalTimeout = UInt32.MaxValue;
                timeouts.ReadTotalTimeoutConstant = 0;
                timeouts.ReadTotalTimeoutMultiplier = 0;
                this.SetTimeouts(timeouts);
            }
        }

        /// <summary>
        /// Gets the current state (config parameters) set on the serial port.
        /// </summary>
        /// <returns>The current serial port state.</returns>
        public SerialPortState GetState()
        {
            if (!IsOpen) throw new InvalidOperationException("Port is not open.");

            SerialPortState dcb = new SerialPortState();
            if (!Win32Api.GetCommState(m_fileHandle, ref dcb))
            {
                throw new Win32Exception(Win32Api.GetLastError(), "'GetCommState()' failed");                
            }

            return dcb;
        }

        /// <summary>
        /// Sets the state (config parameters) on the serial port.
        /// </summary>
        /// <param name="state">The configuation to be applied to the serial port.</param>
        public void SetState(SerialPortState state)
        {
            if (!IsOpen) throw new InvalidOperationException("Port is not open.");

            if (!Win32Api.SetCommState(m_fileHandle, ref state))
            {
                throw new Win32Exception(Win32Api.GetLastError(), "'SetCommState()' failed");
            }
        }

        /// <summary>
        /// Reads data from the serial port.
        /// </summary>
        /// <param name="data">Array where to store the bytes read.</param>
        /// <param name="offset">(Not used)</param>
        /// <param name="count">Number of bytes to read.</param>
        /// <returns>Number of bytes actually read.</returns>
        public int Read(byte[] data, int offset, int count)
        {
            if (!IsOpen) throw new InvalidOperationException("Port is not open.");

            uint bytesRead = 0;
            if (!Win32Api.ReadFile(m_fileHandle, data, (uint)count, out bytesRead, IntPtr.Zero))
            {
                throw new Win32Exception(Win32Api.GetLastError(), "'ReadFile()' failed");
            }
        
            return (int)bytesRead;
        }

        /// <summary>
        /// Reads text from the serial port.
        /// </summary>
        /// <returns>A string representing the data read from the serial port.</returns>
        public string ReadLine()
        {
            StringBuilder message = new StringBuilder();

            int bytesRead = 0;
            byte[] data = new byte[1024];
            while ((bytesRead = this.Read(data, 0, data.Length)) > 0)
            {
                message.Append(Encoding.ASCII.GetString(data, 0, bytesRead));
                Thread.Sleep(50);
            }

            return message.ToString();
        }

        /// <summary>
        /// Writes the text message, followed by CRLF.
        /// </summary>
        /// <param name="message">Text message to be written to the port.</param>
        /// <param name="args">Arguments to be replaced on the message.</param>
        /// <returns>True if the message was written successfully, false otherwise.</returns>
        public bool WriteLine(string message, params object[] args)
        {
            message = args.Length > 0 ? String.Format(message, args) : message;

            byte[] data = Encoding.ASCII.GetBytes(String.Format("{0}\r\n", message));
            
            return (Write(data, 0, data.Length) > 0);
        }

        /// <summary>
        /// Writes data to the serial port.
        /// </summary>
        /// <param name="data">The data to be written.</param>
        /// <param name="offset">(Not used)</param>
        /// <param name="count">Number of bytes from the data array to be written into the port.</param>
        /// <returns>Number of bytes actually written.</returns>
        public int Write(byte[] data, int offset, int count)
        {
            if (!IsOpen) throw new InvalidOperationException("Port is not open.");

            uint bytesWritten = 0;
            if (!Win32Api.WriteFile(m_fileHandle, data, (uint)count, out bytesWritten, IntPtr.Zero))
            {
                throw new Win32Exception(Win32Api.GetLastError(), "'WriteFile()' failed");
            }

            return (int)bytesWritten;
        }

        /// <summary>
        /// Disposes the current serial port connection (closing it).
        /// </summary>
        public void Dispose()
        {
            if (IsOpen)
            {
                this.Close();
            }
        }
    }
}
