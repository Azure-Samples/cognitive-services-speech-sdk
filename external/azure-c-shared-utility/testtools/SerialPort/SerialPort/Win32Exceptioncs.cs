using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Azure.IoT.Internal.Tools
{
    public class Win32Exception : Exception
    {
        public Win32Exception(int errorCode, string message, params object[] args)
            :base(String.Format(message, args) + String.Format("(error {0})", errorCode))
        { }
    }
}
