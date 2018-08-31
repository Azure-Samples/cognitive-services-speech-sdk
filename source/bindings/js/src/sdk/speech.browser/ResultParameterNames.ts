//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines constants used in speech recognition and translation.
 */
export class ResultParameterNames {
     /**
      * The resulting language understanding json output.
      */
     public static LanguageUnderstandingJson: string = "RESULT-LanguageUnderstandingJson";

     /**
      * The raw json output.
      */
     public static Json: string = "RESULT-Json";

     /**
      *  Error details, if the call failed.
      */
     public ErrorDetails: string = "RESULT-ErrorDetails";
 }
