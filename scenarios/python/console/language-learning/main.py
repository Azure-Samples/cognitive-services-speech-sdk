#!/usr/bin/env python
# coding: utf-8

# Copyright (c) Microsoft. All rights reserved.
# Licensed under the MIT license. See LICENSE.md file in the project root for full license information.

import pronunciation_assessment


eof_key = "q"
samples = [
    pronunciation_assessment.pronunciation_assessment_from_microphone,
    pronunciation_assessment.pronunciation_assessment_from_stream,
    pronunciation_assessment.pronunciation_assessment_configured_with_json,
    pronunciation_assessment.pronunciation_assessment_continuous_from_file,
    pronunciation_assessment.pronunciation_assessment_with_rest_api,
]


def select():
    print(f">>> Select sample function, enter '{eof_key}' to exit\n")
    for i, func in enumerate(samples):
        print("{}: {}\n\t{}".format(i, func.__name__, func.__doc__))

    try:
        num = input(f"Enter the number of the sample function to run or '{eof_key}' to exit: ")
        if num == eof_key:
            raise EOFError
        num = int(num)
        selected_function = samples[num]
    except EOFError:
        raise
    except Exception as e:
        print(e)
        return

    print(f"You selected {selected_function.__name__}.\n")
    try:
        selected_function()
    except Exception as e:
        print(f"Error running samples: {e}")

    print()


if __name__ == "__main__":
    while True:
        try:
            select()
        except EOFError:
            break
