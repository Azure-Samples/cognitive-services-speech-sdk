// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

import { PropertyCollection } from "../sdk/Exports";

export enum RecognitionMode {
    Interactive,
    Conversation,
    Dictation,
}

export enum SpeechResultFormat {
    Simple,
    Detailed,
}

export class RecognizerConfig {
    private privRecognitionMode: RecognitionMode = RecognitionMode.Interactive;
    private privPlatformConfig: PlatformConfig;
    private privRecognitionActivityTimeout: number;
    private privSpeechConfig: PropertyCollection;

    constructor(
        platformConfig: PlatformConfig,
        recognitionMode: RecognitionMode = RecognitionMode.Interactive,
        speechConfig: PropertyCollection) {
        this.privPlatformConfig = platformConfig ? platformConfig : new PlatformConfig(new Context(null));
        this.privRecognitionMode = recognitionMode;
        this.privRecognitionActivityTimeout = recognitionMode === RecognitionMode.Interactive ? 8000 : 25000;
        this.privSpeechConfig = speechConfig;
    }

    public get parameters(): PropertyCollection {
        return this.privSpeechConfig;
    }

    public get recognitionMode(): RecognitionMode {
        return this.privRecognitionMode;
    }

    public get platformConfig(): PlatformConfig {
        return this.privPlatformConfig;
    }

    public get recognitionActivityTimeout(): number {
        return this.privRecognitionActivityTimeout;
    }

    public get isContinuousRecognition(): boolean {
        return this.privRecognitionMode !== RecognitionMode.Interactive;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class PlatformConfig {
    private context: Context;

    constructor(context: Context) {
        this.context = context;
    }

    public serialize = (): string => {
        return JSON.stringify(this, (key: any, value: any): any => {
            if (value && typeof value === "object") {
                const replacement: any = {};
                for (const k in value) {
                    if (Object.hasOwnProperty.call(value, k)) {
                        replacement[k && k.charAt(0).toLowerCase() + k.substring(1)] = value[k];
                    }
                }
                return replacement;
            }
            return value;
        });
    }

    public get Context(): Context {
        return this.context;
    }

}

// tslint:disable-next-line:max-classes-per-file
export class Context {
    public system: System;
    public os: OS;

    constructor(os: OS) {
        this.system = new System();
        this.os = os;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class System {
    public name: string;
    public version: string;
    public build: string;
    public lang: string;

    constructor() {
        // Note: below will be patched for official builds.
        const SPEECHSDK_CLIENTSDK_VERSION = "1.1.0-alpha.0.1";

        this.name = "SpeechSDK";
        this.version = SPEECHSDK_CLIENTSDK_VERSION;
        this.build = "JavaScript";
        this.lang = "JavaScript";
    }
}

// tslint:disable-next-line:max-classes-per-file
export class OS {
    public platform: string;
    public name: string;
    public version: string;

    constructor(platform: string, name: string, version: string) {
        this.platform = platform;
        this.name = name;
        this.version = version;
    }
}

// tslint:disable-next-line:max-classes-per-file
export class Device {
    public manufacturer: string;
    public model: string;
    public version: string;

    constructor(manufacturer: string, model: string, version: string) {
        this.manufacturer = manufacturer;
        this.model = model;
        this.version = version;
    }
}
