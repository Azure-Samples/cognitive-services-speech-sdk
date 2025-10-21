//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
#include "stdafx.h"
#include <iostream>
#include <string>

using namespace std;

extern void PronunciationAssessmentWithMicrophone();
extern void PronunciationAssessmentWithStream();
extern void PronunciationAssessmentConfiguredWithJson();

#ifdef _WIN32
int wmain(int argc, wchar_t** argv)
#else
int main(int argc, char** argv)
#endif
{
    string input;
    do
    {
        cout << "\nSPEECH RECOGNITION SAMPLES:\n";
        cout << "1.) Pronunciation assessment using microphone input.\n";
        cout << "2.) Pronunciation assessment with stream.\n";
        cout << "3.) Pronunciation assessment configured with json.\n";
        cout << "\nChoice (0 to Exit): ";
        cout.flush();

        input.clear();
        getline(cin, input);

        switch (input[0])
        {
        case '1':
            PronunciationAssessmentWithMicrophone();
            break;
        case '2':
            PronunciationAssessmentWithStream();
            break;
        case '3':
            PronunciationAssessmentConfiguredWithJson();
            break;
        case '0':
            break;
        }
    } while (input[0] != '0');
}
