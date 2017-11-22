using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Azure.IoT.Internal.Tools
{
    class Program
    {
        static void Main(string[] args)
        {
            SerialPort sp = new SerialPort("COM4");
            if (sp.Open())
            {
                sp.SetBreakState();
                Thread.Sleep(50);
                sp.ClearBreakState();

                sp.SetDefaultTimeouts();

                SerialPortState cfg = sp.GetState();
                cfg.BaudRate = 115200;
                sp.SetState(cfg);

                for (int i = 0; i < 3; i++)
                {
                    string message = sp.ReadLine();

                    bool result = sp.WriteLine("cataca");
                }
                
                sp.Close();
            }
        }
    }
}
