// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.
// response

export interface IIntentResponse {
    query: string;
    topScoringIntent: ISingleIntent;
    entities: IIntentEntity[];
}

export interface IIntentEntity {
    entity: string;
    type: string;
    startIndex: number;
    endIndex: number;
    score: number;
}

export interface ISingleIntent {
    intent: string;
    score: number;
}

export class IntentResponse implements IIntentResponse {
    private privIntentResponse: IIntentResponse;

    private constructor(json: string) {
        this.privIntentResponse = JSON.parse(json);
    }

    public static fromJSON(json: string): IntentResponse {
        return new IntentResponse(json);
    }

    public get query(): string {
        return this.privIntentResponse.query;
    }

    public get topScoringIntent(): ISingleIntent {
        return this.privIntentResponse.topScoringIntent;
    }

    public get entities(): IIntentEntity[] {
        return this.privIntentResponse.entities;
    }
}
