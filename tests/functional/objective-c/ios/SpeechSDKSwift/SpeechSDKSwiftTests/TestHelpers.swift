//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

import Foundation

func normalizeText(input: String) -> String {
    return (input.components(separatedBy: NSCharacterSet.letters.inverted)).joined(separator:"")
}
