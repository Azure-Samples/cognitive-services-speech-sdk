//
// copyright (c) Microsoft. All rights reserved.
// licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

/**
 * Defines constants used in speech recognition, intent, and translation.
 * @class
 */
export class ResultParameterNames {
     /**
      * The resulting language understanding json output.
      * @property
      */
     public static LanguageUnderstandingJson: string = "RESULT-LanguageUnderstandingJson";

     /**
      * The raw json output.
      * @property
      */
     public static Json: string = "RESULT-Json";

     /**
      *  Error details, if the call failed.
      * @property
      */
     public ErrorDetails: string = "RESULT-ErrorDetails";
 }
