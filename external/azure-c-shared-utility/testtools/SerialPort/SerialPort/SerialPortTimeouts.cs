using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Azure.IoT.Internal.Tools
{
    /// <summary>
    /// See the Win32 API COMMTIMEOUTS for details.
    /// </summary>
    public struct SerialPortTimeouts // a.k.a. COMMTIMEOUTS
    {
        public uint ReadIntervalTimeout;
        public uint ReadTotalTimeoutMultiplier;
        public uint ReadTotalTimeoutConstant;
        public uint WriteTotalTimeoutMultiplier;
        public uint WriteTotalTimeoutConstant;
    }
}
