//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

namespace Microsoft.SpeechServices.CommonLib.CommandParser
{
    using System;
    using System.IO;
    using System.Text;
    using System.Threading.Tasks;

    public static class ConsoleApp<T>
        where T : new()
    {
        public static async Task<int> RunAsync(string[] arguments, Func<T, Task<int>> processAsync)
        {
            ArgumentNullException.ThrowIfNull(processAsync);

            ArgumentNullException.ThrowIfNull(arguments);

            int ret = ExitCode.NoError;

            T arg = new T();
            try
            {
                try
                {
                    CommandLineParser.Parse(arguments, arg);
                }
                catch (CommandLineParseException cpe)
                {
                    if (cpe.ErrorString == CommandLineParseException.ErrorStringHelp)
                    {
                        CommandLineParser.PrintUsage(arg);
                    }
                    else if (!string.IsNullOrEmpty(cpe.Message))
                    {
                        Console.WriteLine(cpe.Message);
                    }

                    return ExitCode.InvalidArgument;
                }

                ret = await processAsync(arg).ConfigureAwait(false);
                return ret;
            }
            catch (Exception)
            {
                if (ret != ExitCode.NoError)
                {
                    return ret;
                }

                throw;
            }
        }
    }
}