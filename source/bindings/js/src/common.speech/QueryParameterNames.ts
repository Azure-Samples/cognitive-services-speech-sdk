//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//

export class QueryParameterNames {
    public static get TestHooksParamName(): string {
        return "testhooks";
    }
    public static get ConnectionIdHeader(): string {
        return "X-ConnectionId";
    }
    public static get DeploymentIdParamName(): string {
        return "cid";
    }
    public static get FormatParamName(): string {
        return "format";
    }
    public static get LanguageParamName(): string {
        return "language";
    }
    public static get TranslationFromParamName(): string {
        return "from";
    }
    public static get TranslationToParamName(): string {
        return "to";
    }
}
