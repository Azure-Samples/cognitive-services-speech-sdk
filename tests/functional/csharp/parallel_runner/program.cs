//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Microsoft.CognitiveServices.Speech.Test.EndToEnd
{
    class Program
    {
        private static readonly string SingleShot = "singleshot";
        private static readonly string Continuous = "continuous";
        private static readonly string Verbose = "verbose";

        private static void PrintUsage()
        {
            Console.WriteLine(@"USAGE: ParallelRunner numberOfParallelTasks inputAudioFolder outputFolder endpoint subscriptionKey mode verbose");
            Console.WriteLine(@"The runner will read all *.wav files from the input folder and will produce two files in the output directory:");
            Console.WriteLine(@"    output.txt - contains <utterance> <transcription>");
            Console.WriteLine(@"    err.txt - contains <utterance> <error description> if an error happened");
            Console.WriteLine(@"Currently two modes are supported:");
            Console.WriteLine(@"    singleShot - recognition is done using sequential calls to RecognizeOnceAsync");
            Console.WriteLine(@"    continuous - recognition is done using Start/StopContinuousRecognition");
        }

        static void Main(string[] args)
        {
            if (args.Length != 6 && args.Length != 7)
            {
                PrintUsage();
                Environment.Exit(-1);
            }

            uint numberOfParallelTasks = 0;
            if (!uint.TryParse(args[0], out numberOfParallelTasks))
            {
                Console.WriteLine(@"Please use a numeric value for a number of parallel tasks. Please see usage.");
                PrintUsage();
                Environment.Exit(-1);
            }

            string inputAudioFolder = args[1];
            string outputFolder = args[2];
            string endpoint = args[3];
            string subscriptionKey = args[4];
            string mode = args[5];

            bool verbose = false;
            if (args.Length == 7)
            {
                verbose = args[6].ToLower() == Verbose;
            }

            if (verbose)
            {
                Console.WriteLine("Running in verbose mode.");
            }

            foreach(var s in args)
            {
                if(string.IsNullOrEmpty(s))
                {
                    Console.WriteLine(@"None of the parameters is allowed to be null or empty. Please see usage.");
                    PrintUsage();
                    Environment.Exit(-1);
                }
            }

            Directory.CreateDirectory(outputFolder);
            var inputFiles = new ConcurrentQueue<string>(ReadAudioFilePaths(inputAudioFolder));

            Console.WriteLine($"Processing {inputFiles.Count} utterances using {numberOfParallelTasks} parallel tasks");

            var startTime = DateTime.Now;
            mode = mode.ToLower();

            if (mode != SingleShot && mode != Continuous)
            {
                throw new NotImplementedException(@"Not supported mode '{mode}'.");
            }

            Uri uri = new Uri(endpoint);
            var recognizer = new ParallelRecognizer(uri, subscriptionKey, numberOfParallelTasks, verbose);
            recognizer.Recognize(inputFiles, outputFolder, mode == SingleShot ? false : true);
            Console.WriteLine("Completed in {0:0.00} minutes", DateTime.Now.Subtract(startTime).TotalMinutes);
        }

        private static List<string> ReadAudioFilePaths(string rootDirectory)
        {
            return Directory.EnumerateFiles(rootDirectory, "*.wav", SearchOption.AllDirectories).ToList();
        }
    }
}
