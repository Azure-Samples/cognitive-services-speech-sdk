using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Azure.IoT.Internal.Tools
{
    internal class Win32Api
    {
        public const int INVALID_HANDLE_VALUE = -1;

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern IntPtr CreateFile(
             [MarshalAs(UnmanagedType.LPTStr)] string filename,
             [MarshalAs(UnmanagedType.U4)] FileAccess access,
             [MarshalAs(UnmanagedType.U4)] FileShare share,
             IntPtr securityAttributes, // optional SECURITY_ATTRIBUTES struct or IntPtr.Zero
             [MarshalAs(UnmanagedType.U4)] FileMode creationDisposition,
             [MarshalAs(UnmanagedType.U4)] FileAttributes flagsAndAttributes,
             IntPtr templateFile);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool CloseHandle(IntPtr hObject);

        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        public static extern bool GetCommState(IntPtr hFile, ref SerialPortState lpDcb);

        [DllImport("kernel32.dll")]
        public static extern bool SetCommState(IntPtr hFile, [In] ref SerialPortState lpDCB);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool GetCommTimeouts(IntPtr hFile, [In] ref SerialPortTimeouts lpCommTimeouts);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool SetCommTimeouts(IntPtr hFile, [In] ref SerialPortTimeouts lpCommTimeouts);

        [DllImport("kernel32.dll", SetLastError = true)]
        [return: MarshalAsAttribute(UnmanagedType.Bool)]
        public static extern bool SetCommBreak(IntPtr fileHandle);

        [DllImport("kernel32.dll")]
        public static extern bool ClearCommBreak(IntPtr fileHandle);

        [DllImport("kernel32.dll", SetLastError = true)]
        public static extern bool ReadFile(IntPtr hFile, [Out] byte[] lpBuffer,
           uint nNumberOfBytesToRead, out uint lpNumberOfBytesRead, IntPtr lpOverlapped);

        [DllImport("kernel32.dll")]
        public static extern bool WriteFile(IntPtr hFile, byte[] lpBuffer,
           uint nNumberOfBytesToWrite, out uint lpNumberOfBytesWritten,
           IntPtr lpOverlapped);

        public static int GetLastError()
        {
            return Marshal.GetLastWin32Error();
        }

        public static IntPtr CreateFile(string portName)
        {
            return CreateFile(@"\\.\" + portName, FileAccess.GenericWrite | FileAccess.GenericRead, FileShare.None, IntPtr.Zero, FileMode.OpenExisting, FileAttributes.Normal, IntPtr.Zero);
        }
    }
}
