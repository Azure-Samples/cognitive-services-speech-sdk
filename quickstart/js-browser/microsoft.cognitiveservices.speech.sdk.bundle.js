/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, {
/******/ 				configurable: false,
/******/ 				enumerable: true,
/******/ 				get: getter
/******/ 			});
/******/ 		}
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = 20);
/******/ })
/************************************************************************/
/******/ ([
/* 0 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var AudioConfig_1 = __webpack_require__(35);
exports.AudioConfig = AudioConfig_1.AudioConfig;
var AudioStreamFormat_1 = __webpack_require__(17);
exports.AudioStreamFormat = AudioStreamFormat_1.AudioStreamFormat;
var AudioInputStream_1 = __webpack_require__(16);
exports.AudioInputStream = AudioInputStream_1.AudioInputStream;
exports.PullAudioInputStream = AudioInputStream_1.PullAudioInputStream;
exports.PushAudioInputStream = AudioInputStream_1.PushAudioInputStream;
var CancellationReason_1 = __webpack_require__(36);
exports.CancellationReason = CancellationReason_1.CancellationReason;
var PullAudioInputStreamCallback_1 = __webpack_require__(37);
exports.PullAudioInputStreamCallback = PullAudioInputStreamCallback_1.PullAudioInputStreamCallback;
var KeywordRecognitionModel_1 = __webpack_require__(38);
exports.KeywordRecognitionModel = KeywordRecognitionModel_1.KeywordRecognitionModel;
var SessionEventArgs_1 = __webpack_require__(39);
exports.SessionEventArgs = SessionEventArgs_1.SessionEventArgs;
var RecognitionEventArgs_1 = __webpack_require__(40);
exports.RecognitionEventArgs = RecognitionEventArgs_1.RecognitionEventArgs;
var OutputFormat_1 = __webpack_require__(41);
exports.OutputFormat = OutputFormat_1.OutputFormat;
var IntentRecognitionEventArgs_1 = __webpack_require__(42);
exports.IntentRecognitionEventArgs = IntentRecognitionEventArgs_1.IntentRecognitionEventArgs;
var RecognitionResult_1 = __webpack_require__(43);
exports.RecognitionResult = RecognitionResult_1.RecognitionResult;
var SpeechRecognitionResult_1 = __webpack_require__(44);
exports.SpeechRecognitionResult = SpeechRecognitionResult_1.SpeechRecognitionResult;
var IntentRecognitionResult_1 = __webpack_require__(45);
exports.IntentRecognitionResult = IntentRecognitionResult_1.IntentRecognitionResult;
var LanguageUnderstandingModel_1 = __webpack_require__(46);
exports.LanguageUnderstandingModel = LanguageUnderstandingModel_1.LanguageUnderstandingModel;
var SpeechRecognitionEventArgs_1 = __webpack_require__(47);
exports.SpeechRecognitionEventArgs = SpeechRecognitionEventArgs_1.SpeechRecognitionEventArgs;
var SpeechRecognitionCanceledEventArgs_1 = __webpack_require__(48);
exports.SpeechRecognitionCanceledEventArgs = SpeechRecognitionCanceledEventArgs_1.SpeechRecognitionCanceledEventArgs;
var TranslationRecognitionEventArgs_1 = __webpack_require__(49);
exports.TranslationRecognitionEventArgs = TranslationRecognitionEventArgs_1.TranslationRecognitionEventArgs;
var TranslationSynthesisEventArgs_1 = __webpack_require__(50);
exports.TranslationSynthesisEventArgs = TranslationSynthesisEventArgs_1.TranslationSynthesisEventArgs;
var TranslationRecognitionResult_1 = __webpack_require__(51);
exports.TranslationRecognitionResult = TranslationRecognitionResult_1.TranslationRecognitionResult;
var TranslationSynthesisResult_1 = __webpack_require__(52);
exports.TranslationSynthesisResult = TranslationSynthesisResult_1.TranslationSynthesisResult;
var ResultReason_1 = __webpack_require__(53);
exports.ResultReason = ResultReason_1.ResultReason;
var SpeechConfig_1 = __webpack_require__(54);
exports.SpeechConfig = SpeechConfig_1.SpeechConfig;
var SpeechTranslationConfig_1 = __webpack_require__(66);
exports.SpeechTranslationConfig = SpeechTranslationConfig_1.SpeechTranslationConfig;
var PropertyCollection_1 = __webpack_require__(67);
exports.PropertyCollection = PropertyCollection_1.PropertyCollection;
var PropertyId_1 = __webpack_require__(68);
exports.PropertyId = PropertyId_1.PropertyId;
var Recognizer_1 = __webpack_require__(69);
exports.Recognizer = Recognizer_1.Recognizer;
var SpeechRecognizer_1 = __webpack_require__(70);
exports.SpeechRecognizer = SpeechRecognizer_1.SpeechRecognizer;
var IntentRecognizer_1 = __webpack_require__(71);
exports.IntentRecognizer = IntentRecognizer_1.IntentRecognizer;
var TranslationRecognizer_1 = __webpack_require__(72);
exports.TranslationRecognizer = TranslationRecognizer_1.TranslationRecognizer;
var Translations_1 = __webpack_require__(73);
exports.Translations = Translations_1.Translations;
var NoMatchReason_1 = __webpack_require__(74);
exports.NoMatchReason = NoMatchReason_1.NoMatchReason;
var NoMatchDetails_1 = __webpack_require__(75);
exports.NoMatchDetails = NoMatchDetails_1.NoMatchDetails;
var TranslationRecognitionCanceledEventArgs_1 = __webpack_require__(76);
exports.TranslationRecognitionCanceledEventArgs = TranslationRecognitionCanceledEventArgs_1.TranslationRecognitionCanceledEventArgs;
var IntentRecognitionCanceledEventArgs_1 = __webpack_require__(77);
exports.IntentRecognitionCanceledEventArgs = IntentRecognitionCanceledEventArgs_1.IntentRecognitionCanceledEventArgs;
var CancellationDetails_1 = __webpack_require__(78);
exports.CancellationDetails = CancellationDetails_1.CancellationDetails;



/***/ }),
/* 1 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
__export(__webpack_require__(23));
__export(__webpack_require__(24));
__export(__webpack_require__(10));
__export(__webpack_require__(25));
__export(__webpack_require__(3));
__export(__webpack_require__(26));
__export(__webpack_require__(11));
__export(__webpack_require__(5));
__export(__webpack_require__(27));
__export(__webpack_require__(12));
__export(__webpack_require__(13));
__export(__webpack_require__(8));
__export(__webpack_require__(14));
__export(__webpack_require__(15));
__export(__webpack_require__(28));
__export(__webpack_require__(29));
__export(__webpack_require__(30));
__export(__webpack_require__(31));
var TranslationStatus_1 = __webpack_require__(32);
exports.TranslationStatus = TranslationStatus_1.TranslationStatus;



/***/ }),
/* 2 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
// IMPORTANT - Dont publish internal modules.
__export(__webpack_require__(55));
__export(__webpack_require__(56));
__export(__webpack_require__(9));
__export(__webpack_require__(57));
__export(__webpack_require__(7));
__export(__webpack_require__(58));
__export(__webpack_require__(61));
__export(__webpack_require__(62));
__export(__webpack_require__(63));
__export(__webpack_require__(18));
__export(__webpack_require__(64));
__export(__webpack_require__(65));
exports.OutputFormatPropertyName = "OutputFormat";



/***/ }),
/* 3 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * The error that is thrown when an argument passed in is null.
 *
 * @export
 * @class ArgumentNullError
 * @extends {Error}
 */
var ArgumentNullError = /** @class */ (function (_super) {
    __extends(ArgumentNullError, _super);
    /**
     * Creates an instance of ArgumentNullError.
     *
     * @param {string} argumentName - Name of the argument that is null
     *
     * @memberOf ArgumentNullError
     */
    function ArgumentNullError(argumentName) {
        var _this = _super.call(this, argumentName) || this;
        _this.name = "ArgumentNull";
        _this.message = argumentName;
        return _this;
    }
    return ArgumentNullError;
}(Error));
exports.ArgumentNullError = ArgumentNullError;
/**
 * The error that is thrown when an invalid operation is performed in the code.
 *
 * @export
 * @class InvalidOperationError
 * @extends {Error}
 */
// tslint:disable-next-line:max-classes-per-file
var InvalidOperationError = /** @class */ (function (_super) {
    __extends(InvalidOperationError, _super);
    /**
     * Creates an instance of InvalidOperationError.
     *
     * @param {string} error - The error
     *
     * @memberOf InvalidOperationError
     */
    function InvalidOperationError(error) {
        var _this = _super.call(this, error) || this;
        _this.name = "InvalidOperation";
        _this.message = error;
        return _this;
    }
    return InvalidOperationError;
}(Error));
exports.InvalidOperationError = InvalidOperationError;
/**
 * The error that is thrown when an object is disposed.
 *
 * @export
 * @class ObjectDisposedError
 * @extends {Error}
 */
// tslint:disable-next-line:max-classes-per-file
var ObjectDisposedError = /** @class */ (function (_super) {
    __extends(ObjectDisposedError, _super);
    /**
     * Creates an instance of ObjectDisposedError.
     *
     * @param {string} objectName - The object that is disposed
     * @param {string} error - The error
     *
     * @memberOf ObjectDisposedError
     */
    function ObjectDisposedError(objectName, error) {
        var _this = _super.call(this, error) || this;
        _this.name = objectName + "ObjectDisposed";
        _this.message = error;
        return _this;
    }
    return ObjectDisposedError;
}(Error));
exports.ObjectDisposedError = ObjectDisposedError;



/***/ }),
/* 4 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * @class Contracts
 * @private
 */
var Contracts = /** @class */ (function () {
    function Contracts() {
    }
    Contracts.throwIfNullOrUndefined = function (param, name) {
        if (param === undefined || param === null) {
            throw new Error("throwIfNullOrUndefined:" + name);
        }
    };
    Contracts.throwIfNull = function (param, name) {
        if (param === null) {
            throw new Error("throwIfNull:" + name);
        }
    };
    Contracts.throwIfNullOrWhitespace = function (param, name) {
        Contracts.throwIfNullOrUndefined(param, name);
        if (("" + param).trim().length < 1) {
            throw new Error("throwIfNullOrWhitespace:" + name);
        }
    };
    Contracts.throwIfDisposed = function (isDisposed) {
        if (isDisposed) {
            throw new Error("the object is already disposed");
        }
    };
    Contracts.throwIfArrayEmptyOrWhitespace = function (array, name) {
        Contracts.throwIfNullOrUndefined(array, name);
        if (array.length === 0) {
            throw new Error("throwIfArrayEmptyOrWhitespace:" + name);
        }
        for (var _i = 0, array_1 = array; _i < array_1.length; _i++) {
            var item = array_1[_i];
            Contracts.throwIfNullOrWhitespace(item, name);
        }
    };
    Contracts.throwIfFileDoesNotExist = function (param, name) {
        Contracts.throwIfNullOrWhitespace(param, name);
        // TODO check for file existence.
    };
    return Contracts;
}());
exports.Contracts = Contracts;



/***/ }),
/* 5 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var CreateGuid = function () {
    var d = new Date().getTime();
    var guid = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx".replace(/[xy]/g, function (c) {
        var r = (d + Math.random() * 16) % 16 | 0;
        d = Math.floor(d / 16);
        return (c === "x" ? r : (r & 0x3 | 0x8)).toString(16);
    });
    return guid;
};
exports.CreateGuid = CreateGuid;
var CreateNoDashGuid = function () {
    return CreateGuid().replace(new RegExp("-", "g"), "").toUpperCase();
};
exports.CreateNoDashGuid = CreateNoDashGuid;



/***/ }),
/* 6 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
__export(__webpack_require__(22));
__export(__webpack_require__(33));
__export(__webpack_require__(34));
__export(__webpack_require__(79));
__export(__webpack_require__(80));
__export(__webpack_require__(81));
__export(__webpack_require__(82));
__export(__webpack_require__(83));
__export(__webpack_require__(84));
__export(__webpack_require__(19));



/***/ }),
/* 7 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var SpeechRecognitionEvent = /** @class */ (function (_super) {
    __extends(SpeechRecognitionEvent, _super);
    function SpeechRecognitionEvent(eventName, requestId, sessionId, eventType) {
        if (eventType === void 0) { eventType = Exports_1.EventType.Info; }
        var _this = _super.call(this, eventName, eventType) || this;
        _this.requestId = requestId;
        _this.sessionId = sessionId;
        return _this;
    }
    Object.defineProperty(SpeechRecognitionEvent.prototype, "RequestId", {
        get: function () {
            return this.requestId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechRecognitionEvent.prototype, "SessionId", {
        get: function () {
            return this.sessionId;
        },
        enumerable: true,
        configurable: true
    });
    return SpeechRecognitionEvent;
}(Exports_1.PlatformEvent));
exports.SpeechRecognitionEvent = SpeechRecognitionEvent;
// tslint:disable-next-line:max-classes-per-file
var SpeechRecognitionResultEvent = /** @class */ (function (_super) {
    __extends(SpeechRecognitionResultEvent, _super);
    function SpeechRecognitionResultEvent(eventName, requestId, sessionId, result) {
        var _this = _super.call(this, eventName, requestId, sessionId) || this;
        _this.result = result;
        return _this;
    }
    Object.defineProperty(SpeechRecognitionResultEvent.prototype, "Result", {
        get: function () {
            return this.result;
        },
        enumerable: true,
        configurable: true
    });
    return SpeechRecognitionResultEvent;
}(SpeechRecognitionEvent));
exports.SpeechRecognitionResultEvent = SpeechRecognitionResultEvent;
// tslint:disable-next-line:max-classes-per-file
var RecognitionTriggeredEvent = /** @class */ (function (_super) {
    __extends(RecognitionTriggeredEvent, _super);
    function RecognitionTriggeredEvent(requestId, sessionId, audioSourceId, audioNodeId) {
        var _this = _super.call(this, "RecognitionTriggeredEvent", requestId, sessionId) || this;
        _this.audioSourceId = audioSourceId;
        _this.audioNodeId = audioNodeId;
        return _this;
    }
    Object.defineProperty(RecognitionTriggeredEvent.prototype, "AudioSourceId", {
        get: function () {
            return this.audioSourceId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionTriggeredEvent.prototype, "AudioNodeId", {
        get: function () {
            return this.audioNodeId;
        },
        enumerable: true,
        configurable: true
    });
    return RecognitionTriggeredEvent;
}(SpeechRecognitionEvent));
exports.RecognitionTriggeredEvent = RecognitionTriggeredEvent;
// tslint:disable-next-line:max-classes-per-file
var ListeningStartedEvent = /** @class */ (function (_super) {
    __extends(ListeningStartedEvent, _super);
    function ListeningStartedEvent(requestId, sessionId, audioSourceId, audioNodeId) {
        var _this = _super.call(this, "ListeningStartedEvent", requestId, sessionId) || this;
        _this.audioSourceId = audioSourceId;
        _this.audioNodeId = audioNodeId;
        return _this;
    }
    Object.defineProperty(ListeningStartedEvent.prototype, "AudioSourceId", {
        get: function () {
            return this.audioSourceId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ListeningStartedEvent.prototype, "AudioNodeId", {
        get: function () {
            return this.audioNodeId;
        },
        enumerable: true,
        configurable: true
    });
    return ListeningStartedEvent;
}(SpeechRecognitionEvent));
exports.ListeningStartedEvent = ListeningStartedEvent;
// tslint:disable-next-line:max-classes-per-file
var ConnectingToServiceEvent = /** @class */ (function (_super) {
    __extends(ConnectingToServiceEvent, _super);
    function ConnectingToServiceEvent(requestId, authFetchEventid, sessionId) {
        var _this = _super.call(this, "ConnectingToServiceEvent", requestId, sessionId) || this;
        _this.authFetchEventid = authFetchEventid;
        return _this;
    }
    Object.defineProperty(ConnectingToServiceEvent.prototype, "AuthFetchEventid", {
        get: function () {
            return this.authFetchEventid;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectingToServiceEvent;
}(SpeechRecognitionEvent));
exports.ConnectingToServiceEvent = ConnectingToServiceEvent;
// tslint:disable-next-line:max-classes-per-file
var RecognitionStartedEvent = /** @class */ (function (_super) {
    __extends(RecognitionStartedEvent, _super);
    function RecognitionStartedEvent(requestId, audioSourceId, audioNodeId, authFetchEventId, sessionId) {
        var _this = _super.call(this, "RecognitionStartedEvent", requestId, sessionId) || this;
        _this.audioSourceId = audioSourceId;
        _this.audioNodeId = audioNodeId;
        _this.authFetchEventId = authFetchEventId;
        return _this;
    }
    Object.defineProperty(RecognitionStartedEvent.prototype, "AudioSourceId", {
        get: function () {
            return this.audioSourceId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionStartedEvent.prototype, "AudioNodeId", {
        get: function () {
            return this.audioNodeId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionStartedEvent.prototype, "AuthFetchEventId", {
        get: function () {
            return this.authFetchEventId;
        },
        enumerable: true,
        configurable: true
    });
    return RecognitionStartedEvent;
}(SpeechRecognitionEvent));
exports.RecognitionStartedEvent = RecognitionStartedEvent;
// tslint:disable-next-line:max-classes-per-file
var SpeechStartDetectedEvent = /** @class */ (function (_super) {
    __extends(SpeechStartDetectedEvent, _super);
    function SpeechStartDetectedEvent(requestId, sessionId, result) {
        return _super.call(this, "SpeechStartDetectedEvent", requestId, sessionId, result) || this;
    }
    return SpeechStartDetectedEvent;
}(SpeechRecognitionResultEvent));
exports.SpeechStartDetectedEvent = SpeechStartDetectedEvent;
// tslint:disable-next-line:max-classes-per-file
var SpeechHypothesisEvent = /** @class */ (function (_super) {
    __extends(SpeechHypothesisEvent, _super);
    function SpeechHypothesisEvent(requestId, sessionId, result) {
        return _super.call(this, "SpeechHypothesisEvent", requestId, sessionId, result) || this;
    }
    return SpeechHypothesisEvent;
}(SpeechRecognitionResultEvent));
exports.SpeechHypothesisEvent = SpeechHypothesisEvent;
// tslint:disable-next-line:max-classes-per-file
var SpeechFragmentEvent = /** @class */ (function (_super) {
    __extends(SpeechFragmentEvent, _super);
    function SpeechFragmentEvent(requestId, sessionId, result) {
        return _super.call(this, "SpeechFragmentEvent", requestId, sessionId, result) || this;
    }
    return SpeechFragmentEvent;
}(SpeechRecognitionResultEvent));
exports.SpeechFragmentEvent = SpeechFragmentEvent;
// tslint:disable-next-line:max-classes-per-file
var SpeechEndDetectedEvent = /** @class */ (function (_super) {
    __extends(SpeechEndDetectedEvent, _super);
    function SpeechEndDetectedEvent(requestId, sessionId, result) {
        return _super.call(this, "SpeechEndDetectedEvent", requestId, sessionId, result) || this;
    }
    return SpeechEndDetectedEvent;
}(SpeechRecognitionResultEvent));
exports.SpeechEndDetectedEvent = SpeechEndDetectedEvent;
// tslint:disable-next-line:max-classes-per-file
var SpeechSimplePhraseEvent = /** @class */ (function (_super) {
    __extends(SpeechSimplePhraseEvent, _super);
    function SpeechSimplePhraseEvent(requestId, sessionId, result) {
        return _super.call(this, "SpeechSimplePhraseEvent", requestId, sessionId, result) || this;
    }
    return SpeechSimplePhraseEvent;
}(SpeechRecognitionResultEvent));
exports.SpeechSimplePhraseEvent = SpeechSimplePhraseEvent;
// tslint:disable-next-line:max-classes-per-file
var SpeechDetailedPhraseEvent = /** @class */ (function (_super) {
    __extends(SpeechDetailedPhraseEvent, _super);
    function SpeechDetailedPhraseEvent(requestId, sessionId, result) {
        return _super.call(this, "SpeechDetailedPhraseEvent", requestId, sessionId, result) || this;
    }
    return SpeechDetailedPhraseEvent;
}(SpeechRecognitionResultEvent));
exports.SpeechDetailedPhraseEvent = SpeechDetailedPhraseEvent;
// tslint:disable-next-line:max-classes-per-file
var TranslationHypothesisEvent = /** @class */ (function (_super) {
    __extends(TranslationHypothesisEvent, _super);
    function TranslationHypothesisEvent(requestId, sessionId, result) {
        return _super.call(this, "TranslationHypothesisEvent", requestId, sessionId, result) || this;
    }
    return TranslationHypothesisEvent;
}(SpeechRecognitionResultEvent));
exports.TranslationHypothesisEvent = TranslationHypothesisEvent;
// tslint:disable-next-line:max-classes-per-file
var TranslationPhraseEvent = /** @class */ (function (_super) {
    __extends(TranslationPhraseEvent, _super);
    function TranslationPhraseEvent(requestId, sessionId, result) {
        return _super.call(this, "TranslationPhraseEvent", requestId, sessionId, result) || this;
    }
    return TranslationPhraseEvent;
}(SpeechRecognitionResultEvent));
exports.TranslationPhraseEvent = TranslationPhraseEvent;
// tslint:disable-next-line:max-classes-per-file
var TranslationFailedEvent = /** @class */ (function (_super) {
    __extends(TranslationFailedEvent, _super);
    function TranslationFailedEvent(requestId, sessionId, result) {
        return _super.call(this, "TranslationFailedEvent", requestId, sessionId, result) || this;
    }
    return TranslationFailedEvent;
}(SpeechRecognitionResultEvent));
exports.TranslationFailedEvent = TranslationFailedEvent;
// tslint:disable-next-line:max-classes-per-file
var RecognitionFailedEvent = /** @class */ (function (_super) {
    __extends(RecognitionFailedEvent, _super);
    function RecognitionFailedEvent(requestId, sessionId, result) {
        return _super.call(this, "RecognitionFailedEvent", requestId, sessionId, result) || this;
    }
    return RecognitionFailedEvent;
}(SpeechRecognitionResultEvent));
exports.RecognitionFailedEvent = RecognitionFailedEvent;
// tslint:disable-next-line:max-classes-per-file
var TranslationSynthesisEvent = /** @class */ (function (_super) {
    __extends(TranslationSynthesisEvent, _super);
    function TranslationSynthesisEvent(requestId, sessionId, result) {
        return _super.call(this, "TranslationSynthesisEvent", requestId, sessionId, result) || this;
    }
    return TranslationSynthesisEvent;
}(SpeechRecognitionResultEvent));
exports.TranslationSynthesisEvent = TranslationSynthesisEvent;
// tslint:disable-next-line:max-classes-per-file
var TranslationSynthesisErrorEvent = /** @class */ (function (_super) {
    __extends(TranslationSynthesisErrorEvent, _super);
    function TranslationSynthesisErrorEvent(requestId, sessionId, result) {
        return _super.call(this, "TranslationSynthesisErrorEvent", requestId, sessionId, result) || this;
    }
    return TranslationSynthesisErrorEvent;
}(SpeechRecognitionResultEvent));
exports.TranslationSynthesisErrorEvent = TranslationSynthesisErrorEvent;
// tslint:disable-next-line:max-classes-per-file
var IntentResponseEvent = /** @class */ (function (_super) {
    __extends(IntentResponseEvent, _super);
    function IntentResponseEvent(requestId, sessionId, result) {
        return _super.call(this, "IntentResponseEvent", requestId, sessionId, result) || this;
    }
    return IntentResponseEvent;
}(SpeechRecognitionResultEvent));
exports.IntentResponseEvent = IntentResponseEvent;
// tslint:disable-next-line:max-classes-per-file
var InternalErrorEvent = /** @class */ (function (_super) {
    __extends(InternalErrorEvent, _super);
    function InternalErrorEvent(requestId, sessionId, result) {
        return _super.call(this, "InternalErrorEvent", requestId, sessionId, result) || this;
    }
    return InternalErrorEvent;
}(SpeechRecognitionResultEvent));
exports.InternalErrorEvent = InternalErrorEvent;
var RecognitionCompletionStatus;
(function (RecognitionCompletionStatus) {
    RecognitionCompletionStatus[RecognitionCompletionStatus["Success"] = 0] = "Success";
    RecognitionCompletionStatus[RecognitionCompletionStatus["AudioSourceError"] = 1] = "AudioSourceError";
    RecognitionCompletionStatus[RecognitionCompletionStatus["AudioSourceTimeout"] = 2] = "AudioSourceTimeout";
    RecognitionCompletionStatus[RecognitionCompletionStatus["AuthTokenFetchError"] = 3] = "AuthTokenFetchError";
    RecognitionCompletionStatus[RecognitionCompletionStatus["AuthTokenFetchTimeout"] = 4] = "AuthTokenFetchTimeout";
    RecognitionCompletionStatus[RecognitionCompletionStatus["UnAuthorized"] = 5] = "UnAuthorized";
    RecognitionCompletionStatus[RecognitionCompletionStatus["ConnectTimeout"] = 6] = "ConnectTimeout";
    RecognitionCompletionStatus[RecognitionCompletionStatus["ConnectError"] = 7] = "ConnectError";
    RecognitionCompletionStatus[RecognitionCompletionStatus["ClientRecognitionActivityTimeout"] = 8] = "ClientRecognitionActivityTimeout";
    RecognitionCompletionStatus[RecognitionCompletionStatus["UnknownError"] = 9] = "UnknownError";
})(RecognitionCompletionStatus = exports.RecognitionCompletionStatus || (exports.RecognitionCompletionStatus = {}));
// tslint:disable-next-line:max-classes-per-file
var RecognitionEndedEvent = /** @class */ (function (_super) {
    __extends(RecognitionEndedEvent, _super);
    function RecognitionEndedEvent(requestId, audioSourceId, audioNodeId, authFetchEventId, sessionId, serviceTag, status, error) {
        var _this = _super.call(this, "RecognitionEndedEvent", requestId, sessionId, status === RecognitionCompletionStatus.Success ? Exports_1.EventType.Info : Exports_1.EventType.Error) || this;
        _this.audioSourceId = audioSourceId;
        _this.audioNodeId = audioNodeId;
        _this.authFetchEventId = authFetchEventId;
        _this.status = status;
        _this.error = error;
        _this.serviceTag = serviceTag;
        return _this;
    }
    Object.defineProperty(RecognitionEndedEvent.prototype, "AudioSourceId", {
        get: function () {
            return this.audioSourceId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionEndedEvent.prototype, "AudioNodeId", {
        get: function () {
            return this.audioNodeId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionEndedEvent.prototype, "AuthFetchEventId", {
        get: function () {
            return this.authFetchEventId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionEndedEvent.prototype, "ServiceTag", {
        get: function () {
            return this.serviceTag;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionEndedEvent.prototype, "Status", {
        get: function () {
            return this.status;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionEndedEvent.prototype, "Error", {
        get: function () {
            return this.error;
        },
        enumerable: true,
        configurable: true
    });
    return RecognitionEndedEvent;
}(SpeechRecognitionEvent));
exports.RecognitionEndedEvent = RecognitionEndedEvent;



/***/ }),
/* 8 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Guid_1 = __webpack_require__(5);
var EventType;
(function (EventType) {
    EventType[EventType["Debug"] = 0] = "Debug";
    EventType[EventType["Info"] = 1] = "Info";
    EventType[EventType["Warning"] = 2] = "Warning";
    EventType[EventType["Error"] = 3] = "Error";
})(EventType = exports.EventType || (exports.EventType = {}));
var PlatformEvent = /** @class */ (function () {
    function PlatformEvent(eventName, eventType) {
        this.name = eventName;
        this.eventId = Guid_1.CreateNoDashGuid();
        this.eventTime = new Date().toISOString();
        this.eventType = eventType;
        this.metadata = {};
    }
    Object.defineProperty(PlatformEvent.prototype, "Name", {
        get: function () {
            return this.name;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(PlatformEvent.prototype, "EventId", {
        get: function () {
            return this.eventId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(PlatformEvent.prototype, "EventTime", {
        get: function () {
            return this.eventTime;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(PlatformEvent.prototype, "EventType", {
        get: function () {
            return this.eventType;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(PlatformEvent.prototype, "Metadata", {
        get: function () {
            return this.metadata;
        },
        enumerable: true,
        configurable: true
    });
    return PlatformEvent;
}());
exports.PlatformEvent = PlatformEvent;



/***/ }),
/* 9 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var AuthInfo = /** @class */ (function () {
    function AuthInfo(headerName, token) {
        this.headerName = headerName;
        this.token = token;
    }
    Object.defineProperty(AuthInfo.prototype, "HeaderName", {
        get: function () {
            return this.headerName;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(AuthInfo.prototype, "Token", {
        get: function () {
            return this.token;
        },
        enumerable: true,
        configurable: true
    });
    return AuthInfo;
}());
exports.AuthInfo = AuthInfo;



/***/ }),
/* 10 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var Guid_1 = __webpack_require__(5);
var MessageType;
(function (MessageType) {
    MessageType[MessageType["Text"] = 0] = "Text";
    MessageType[MessageType["Binary"] = 1] = "Binary";
})(MessageType = exports.MessageType || (exports.MessageType = {}));
var ConnectionMessage = /** @class */ (function () {
    function ConnectionMessage(messageType, body, headers, id) {
        this.body = null;
        if (messageType === MessageType.Text && body && !(typeof (body) === "string")) {
            throw new Error_1.InvalidOperationError("Payload must be a string");
        }
        if (messageType === MessageType.Binary && body && !(body instanceof ArrayBuffer)) {
            throw new Error_1.InvalidOperationError("Payload must be ArrayBuffer");
        }
        this.messageType = messageType;
        this.body = body;
        this.headers = headers ? headers : {};
        this.id = id ? id : Guid_1.CreateNoDashGuid();
    }
    Object.defineProperty(ConnectionMessage.prototype, "MessageType", {
        get: function () {
            return this.messageType;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionMessage.prototype, "Headers", {
        get: function () {
            return this.headers;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionMessage.prototype, "Body", {
        get: function () {
            return this.body;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionMessage.prototype, "TextBody", {
        get: function () {
            if (this.messageType === MessageType.Binary) {
                throw new Error_1.InvalidOperationError("Not supported for binary message");
            }
            return this.body;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionMessage.prototype, "BinaryBody", {
        get: function () {
            if (this.messageType === MessageType.Text) {
                throw new Error_1.InvalidOperationError("Not supported for text message");
            }
            return this.body;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionMessage.prototype, "Id", {
        get: function () {
            return this.id;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectionMessage;
}());
exports.ConnectionMessage = ConnectionMessage;



/***/ }),
/* 11 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var Guid_1 = __webpack_require__(5);
var EventSource = /** @class */ (function () {
    function EventSource(metadata) {
        var _this = this;
        this.eventListeners = {};
        this.isDisposed = false;
        this.OnEvent = function (event) {
            if (_this.IsDisposed()) {
                throw (new Error_1.ObjectDisposedError("EventSource"));
            }
            if (_this.Metadata) {
                for (var paramName in _this.Metadata) {
                    if (paramName) {
                        if (event.Metadata) {
                            if (!event.Metadata[paramName]) {
                                event.Metadata[paramName] = _this.Metadata[paramName];
                            }
                        }
                    }
                }
            }
            for (var eventId in _this.eventListeners) {
                if (eventId && _this.eventListeners[eventId]) {
                    _this.eventListeners[eventId](event);
                }
            }
        };
        this.Attach = function (onEventCallback) {
            var id = Guid_1.CreateNoDashGuid();
            _this.eventListeners[id] = onEventCallback;
            return {
                Detach: function () {
                    delete _this.eventListeners[id];
                },
            };
        };
        this.AttachListener = function (listener) {
            return _this.Attach(listener.OnEvent);
        };
        this.IsDisposed = function () {
            return _this.isDisposed;
        };
        this.Dispose = function () {
            _this.eventListeners = null;
            _this.isDisposed = true;
        };
        this.metadata = metadata;
    }
    Object.defineProperty(EventSource.prototype, "Metadata", {
        get: function () {
            return this.metadata;
        },
        enumerable: true,
        configurable: true
    });
    return EventSource;
}());
exports.EventSource = EventSource;



/***/ }),
/* 12 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var InMemoryStorage = /** @class */ (function () {
    function InMemoryStorage() {
        var _this = this;
        this.store = {};
        this.Get = function (key) {
            if (!key) {
                throw new Error_1.ArgumentNullError("key");
            }
            return _this.store[key];
        };
        this.GetOrAdd = function (key, valueToAdd) {
            if (!key) {
                throw new Error_1.ArgumentNullError("key");
            }
            if (_this.store[key] === undefined) {
                _this.store[key] = valueToAdd;
            }
            return _this.store[key];
        };
        this.Set = function (key, value) {
            if (!key) {
                throw new Error_1.ArgumentNullError("key");
            }
            _this.store[key] = value;
        };
        this.Remove = function (key) {
            if (!key) {
                throw new Error_1.ArgumentNullError("key");
            }
            if (_this.store[key] !== undefined) {
                delete _this.store[key];
            }
        };
    }
    return InMemoryStorage;
}());
exports.InMemoryStorage = InMemoryStorage;



/***/ }),
/* 13 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var List = /** @class */ (function () {
    function List(list) {
        var _this = this;
        this.subscriptionIdCounter = 0;
        this.addSubscriptions = {};
        this.removeSubscriptions = {};
        this.disposedSubscriptions = {};
        this.disposeReason = null;
        this.Get = function (itemIndex) {
            _this.ThrowIfDisposed();
            return _this.list[itemIndex];
        };
        this.First = function () {
            return _this.Get(0);
        };
        this.Last = function () {
            return _this.Get(_this.Length() - 1);
        };
        this.Add = function (item) {
            _this.ThrowIfDisposed();
            _this.InsertAt(_this.list.length, item);
        };
        this.InsertAt = function (index, item) {
            _this.ThrowIfDisposed();
            if (index === 0) {
                _this.list.unshift(item);
            }
            else if (index === _this.list.length) {
                _this.list.push(item);
            }
            else {
                _this.list.splice(index, 0, item);
            }
            _this.TriggerSubscriptions(_this.addSubscriptions);
        };
        this.RemoveFirst = function () {
            _this.ThrowIfDisposed();
            return _this.RemoveAt(0);
        };
        this.RemoveLast = function () {
            _this.ThrowIfDisposed();
            return _this.RemoveAt(_this.Length() - 1);
        };
        this.RemoveAt = function (index) {
            _this.ThrowIfDisposed();
            return _this.Remove(index, 1)[0];
        };
        this.Remove = function (index, count) {
            _this.ThrowIfDisposed();
            var removedElements = _this.list.splice(index, count);
            _this.TriggerSubscriptions(_this.removeSubscriptions);
            return removedElements;
        };
        this.Clear = function () {
            _this.ThrowIfDisposed();
            _this.Remove(0, _this.Length());
        };
        this.Length = function () {
            _this.ThrowIfDisposed();
            return _this.list.length;
        };
        this.OnAdded = function (addedCallback) {
            _this.ThrowIfDisposed();
            var subscriptionId = _this.subscriptionIdCounter++;
            _this.addSubscriptions[subscriptionId] = addedCallback;
            return {
                Detach: function () {
                    delete _this.addSubscriptions[subscriptionId];
                },
            };
        };
        this.OnRemoved = function (removedCallback) {
            _this.ThrowIfDisposed();
            var subscriptionId = _this.subscriptionIdCounter++;
            _this.removeSubscriptions[subscriptionId] = removedCallback;
            return {
                Detach: function () {
                    delete _this.removeSubscriptions[subscriptionId];
                },
            };
        };
        this.OnDisposed = function (disposedCallback) {
            _this.ThrowIfDisposed();
            var subscriptionId = _this.subscriptionIdCounter++;
            _this.disposedSubscriptions[subscriptionId] = disposedCallback;
            return {
                Detach: function () {
                    delete _this.disposedSubscriptions[subscriptionId];
                },
            };
        };
        this.Join = function (seperator) {
            _this.ThrowIfDisposed();
            return _this.list.join(seperator);
        };
        this.ToArray = function () {
            var cloneCopy = Array();
            _this.list.forEach(function (val) {
                cloneCopy.push(val);
            });
            return cloneCopy;
        };
        this.Any = function (callback) {
            _this.ThrowIfDisposed();
            if (callback) {
                return _this.Where(callback).Length() > 0;
            }
            else {
                return _this.Length() > 0;
            }
        };
        this.All = function (callback) {
            _this.ThrowIfDisposed();
            return _this.Where(callback).Length() === _this.Length();
        };
        this.ForEach = function (callback) {
            _this.ThrowIfDisposed();
            for (var i = 0; i < _this.Length(); i++) {
                callback(_this.list[i], i);
            }
        };
        this.Select = function (callback) {
            _this.ThrowIfDisposed();
            var selectList = [];
            for (var i = 0; i < _this.list.length; i++) {
                selectList.push(callback(_this.list[i], i));
            }
            return new List(selectList);
        };
        this.Where = function (callback) {
            _this.ThrowIfDisposed();
            var filteredList = new List();
            for (var i = 0; i < _this.list.length; i++) {
                if (callback(_this.list[i], i)) {
                    filteredList.Add(_this.list[i]);
                }
            }
            return filteredList;
        };
        this.OrderBy = function (compareFn) {
            _this.ThrowIfDisposed();
            var clonedArray = _this.ToArray();
            var orderedArray = clonedArray.sort(compareFn);
            return new List(orderedArray);
        };
        this.OrderByDesc = function (compareFn) {
            _this.ThrowIfDisposed();
            return _this.OrderBy(function (a, b) { return compareFn(b, a); });
        };
        this.Clone = function () {
            _this.ThrowIfDisposed();
            return new List(_this.ToArray());
        };
        this.Concat = function (list) {
            _this.ThrowIfDisposed();
            return new List(_this.list.concat(list.ToArray()));
        };
        this.ConcatArray = function (array) {
            _this.ThrowIfDisposed();
            return new List(_this.list.concat(array));
        };
        this.IsDisposed = function () {
            return _this.list == null;
        };
        this.Dispose = function (reason) {
            if (!_this.IsDisposed()) {
                _this.disposeReason = reason;
                _this.list = null;
                _this.addSubscriptions = null;
                _this.removeSubscriptions = null;
                _this.TriggerSubscriptions(_this.disposedSubscriptions);
            }
        };
        this.ThrowIfDisposed = function () {
            if (_this.IsDisposed()) {
                throw new Error_1.ObjectDisposedError("List", _this.disposeReason);
            }
        };
        this.TriggerSubscriptions = function (subscriptions) {
            if (subscriptions) {
                for (var subscriptionId in subscriptions) {
                    if (subscriptionId) {
                        subscriptions[subscriptionId]();
                    }
                }
            }
        };
        this.list = [];
        // copy the list rather than taking as is.
        if (list) {
            for (var _i = 0, list_1 = list; _i < list_1.length; _i++) {
                var item = list_1[_i];
                this.list.push(item);
            }
        }
    }
    return List;
}());
exports.List = List;



/***/ }),
/* 14 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var PromiseState;
(function (PromiseState) {
    PromiseState[PromiseState["None"] = 0] = "None";
    PromiseState[PromiseState["Resolved"] = 1] = "Resolved";
    PromiseState[PromiseState["Rejected"] = 2] = "Rejected";
})(PromiseState = exports.PromiseState || (exports.PromiseState = {}));
var PromiseResult = /** @class */ (function () {
    function PromiseResult(promiseResultEventSource) {
        var _this = this;
        this.ThrowIfError = function () {
            if (_this.IsError) {
                throw _this.Error;
            }
        };
        promiseResultEventSource.On(function (result) {
            if (!_this.isCompleted) {
                _this.isCompleted = true;
                _this.isError = false;
                _this.result = result;
            }
        }, function (error) {
            if (!_this.isCompleted) {
                _this.isCompleted = true;
                _this.isError = true;
                _this.error = error;
            }
        });
    }
    Object.defineProperty(PromiseResult.prototype, "IsCompleted", {
        get: function () {
            return this.isCompleted;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(PromiseResult.prototype, "IsError", {
        get: function () {
            return this.isError;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(PromiseResult.prototype, "Error", {
        get: function () {
            return this.error;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(PromiseResult.prototype, "Result", {
        get: function () {
            return this.result;
        },
        enumerable: true,
        configurable: true
    });
    return PromiseResult;
}());
exports.PromiseResult = PromiseResult;
// tslint:disable-next-line:max-classes-per-file
var PromiseResultEventSource = /** @class */ (function () {
    function PromiseResultEventSource() {
        var _this = this;
        this.SetResult = function (result) {
            _this.onSetResult(result);
        };
        this.SetError = function (error) {
            _this.onSetError(error);
        };
        this.On = function (onSetResult, onSetError) {
            _this.onSetResult = onSetResult;
            _this.onSetError = onSetError;
        };
    }
    return PromiseResultEventSource;
}());
exports.PromiseResultEventSource = PromiseResultEventSource;
// tslint:disable-next-line:max-classes-per-file
var PromiseHelper = /** @class */ (function () {
    function PromiseHelper() {
    }
    PromiseHelper.WhenAll = function (promises) {
        if (!promises || promises.length === 0) {
            throw new Error_1.ArgumentNullError("promises");
        }
        var deferred = new Deferred();
        var errors = [];
        var completedPromises = 0;
        var checkForCompletion = function () {
            completedPromises++;
            if (completedPromises === promises.length) {
                if (errors.length === 0) {
                    deferred.Resolve(true);
                }
                else {
                    deferred.Reject(errors.join(", "));
                }
            }
        };
        for (var _i = 0, promises_1 = promises; _i < promises_1.length; _i++) {
            var promise = promises_1[_i];
            promise.On(function (r) {
                checkForCompletion();
            }, function (e) {
                errors.push(e);
                checkForCompletion();
            });
        }
        return deferred.Promise();
    };
    PromiseHelper.FromResult = function (result) {
        var deferred = new Deferred();
        deferred.Resolve(result);
        return deferred.Promise();
    };
    PromiseHelper.FromError = function (error) {
        var deferred = new Deferred();
        deferred.Reject(error);
        return deferred.Promise();
    };
    return PromiseHelper;
}());
exports.PromiseHelper = PromiseHelper;
// TODO: replace with ES6 promises
// tslint:disable-next-line:max-classes-per-file
var Promise = /** @class */ (function () {
    function Promise(sink) {
        var _this = this;
        this.Result = function () {
            return _this.sink.Result;
        };
        this.ContinueWith = function (continuationCallback) {
            if (!continuationCallback) {
                throw new Error_1.ArgumentNullError("continuationCallback");
            }
            var continuationDeferral = new Deferred();
            _this.sink.on(function (r) {
                try {
                    var continuationResult = continuationCallback(_this.sink.Result);
                    continuationDeferral.Resolve(continuationResult);
                }
                catch (e) {
                    continuationDeferral.Reject(e);
                }
            }, function (error) {
                try {
                    var continuationResult = continuationCallback(_this.sink.Result);
                    continuationDeferral.Resolve(continuationResult);
                }
                catch (e) {
                    continuationDeferral.Reject("'Error handler for error " + error + " threw error " + e + "'");
                }
            });
            return continuationDeferral.Promise();
        };
        this.OnSuccessContinueWith = function (continuationCallback) {
            if (!continuationCallback) {
                throw new Error_1.ArgumentNullError("continuationCallback");
            }
            var continuationDeferral = new Deferred();
            _this.sink.on(function (r) {
                try {
                    var continuationResult = continuationCallback(r);
                    continuationDeferral.Resolve(continuationResult);
                }
                catch (e) {
                    continuationDeferral.Reject(e);
                }
            }, function (error) {
                continuationDeferral.Reject(error);
            });
            return continuationDeferral.Promise();
        };
        this.ContinueWithPromise = function (continuationCallback) {
            if (!continuationCallback) {
                throw new Error_1.ArgumentNullError("continuationCallback");
            }
            var continuationDeferral = new Deferred();
            _this.sink.on(function (r) {
                try {
                    var continuationPromise = continuationCallback(_this.sink.Result);
                    if (!continuationPromise) {
                        throw new Error("'Continuation callback did not return promise'");
                    }
                    continuationPromise.On(function (continuationResult) {
                        continuationDeferral.Resolve(continuationResult);
                    }, function (e) {
                        continuationDeferral.Reject(e);
                    });
                }
                catch (e) {
                    continuationDeferral.Reject(e);
                }
            }, function (error) {
                try {
                    var continuationPromise = continuationCallback(_this.sink.Result);
                    if (!continuationPromise) {
                        throw new Error("Continuation callback did not return promise");
                    }
                    continuationPromise.On(function (continuationResult) {
                        continuationDeferral.Resolve(continuationResult);
                    }, function (e) {
                        continuationDeferral.Reject(e);
                    });
                }
                catch (e) {
                    continuationDeferral.Reject("'Error handler for error " + error + " threw error " + e + "'");
                }
            });
            return continuationDeferral.Promise();
        };
        this.OnSuccessContinueWithPromise = function (continuationCallback) {
            if (!continuationCallback) {
                throw new Error_1.ArgumentNullError("continuationCallback");
            }
            var continuationDeferral = new Deferred();
            _this.sink.on(function (r) {
                try {
                    var continuationPromise = continuationCallback(r);
                    if (!continuationPromise) {
                        throw new Error("Continuation callback did not return promise");
                    }
                    continuationPromise.On(function (continuationResult) {
                        continuationDeferral.Resolve(continuationResult);
                    }, function (e) {
                        continuationDeferral.Reject(e);
                    });
                }
                catch (e) {
                    continuationDeferral.Reject(e);
                }
            }, function (error) {
                continuationDeferral.Reject(error);
            });
            return continuationDeferral.Promise();
        };
        this.On = function (successCallback, errorCallback) {
            if (!successCallback) {
                throw new Error_1.ArgumentNullError("successCallback");
            }
            if (!errorCallback) {
                throw new Error_1.ArgumentNullError("errorCallback");
            }
            _this.sink.on(successCallback, errorCallback);
            return _this;
        };
        this.Finally = function (callback) {
            if (!callback) {
                throw new Error_1.ArgumentNullError("callback");
            }
            var callbackWrapper = function (_) {
                callback();
            };
            return _this.On(callbackWrapper, callbackWrapper);
        };
        this.sink = sink;
    }
    return Promise;
}());
exports.Promise = Promise;
// tslint:disable-next-line:max-classes-per-file
var Deferred = /** @class */ (function () {
    function Deferred() {
        var _this = this;
        this.State = function () {
            return _this.sink.State;
        };
        this.Promise = function () {
            return _this.promise;
        };
        this.Resolve = function (result) {
            _this.sink.Resolve(result);
            return _this;
        };
        this.Reject = function (error) {
            _this.sink.Reject(error);
            return _this;
        };
        this.sink = new Sink();
        this.promise = new Promise(this.sink);
    }
    return Deferred;
}());
exports.Deferred = Deferred;
// tslint:disable-next-line:max-classes-per-file
var Sink = /** @class */ (function () {
    function Sink() {
        var _this = this;
        this.state = PromiseState.None;
        this.promiseResult = null;
        this.promiseResultEvents = null;
        this.successHandlers = [];
        this.errorHandlers = [];
        this.Resolve = function (result) {
            if (_this.state !== PromiseState.None) {
                throw new Error("'Cannot resolve a completed promise'");
            }
            _this.state = PromiseState.Resolved;
            _this.promiseResultEvents.SetResult(result);
            for (var i = 0; i < _this.successHandlers.length; i++) {
                _this.ExecuteSuccessCallback(result, _this.successHandlers[i], _this.errorHandlers[i]);
            }
            _this.DetachHandlers();
        };
        this.Reject = function (error) {
            if (_this.state !== PromiseState.None) {
                throw new Error("'Cannot reject a completed promise'");
            }
            _this.state = PromiseState.Rejected;
            _this.promiseResultEvents.SetError(error);
            for (var _i = 0, _a = _this.errorHandlers; _i < _a.length; _i++) {
                var errorHandler = _a[_i];
                _this.ExecuteErrorCallback(error, errorHandler);
            }
            _this.DetachHandlers();
        };
        this.on = function (successCallback, errorCallback) {
            if (successCallback == null) {
                successCallback = function (r) { return; };
            }
            if (_this.state === PromiseState.None) {
                _this.successHandlers.push(successCallback);
                _this.errorHandlers.push(errorCallback);
            }
            else {
                if (_this.state === PromiseState.Resolved) {
                    _this.ExecuteSuccessCallback(_this.promiseResult.Result, successCallback, errorCallback);
                }
                else if (_this.state === PromiseState.Rejected) {
                    _this.ExecuteErrorCallback(_this.promiseResult.Error, errorCallback);
                }
                _this.DetachHandlers();
            }
        };
        this.ExecuteSuccessCallback = function (result, successCallback, errorCallback) {
            try {
                successCallback(result);
            }
            catch (e) {
                _this.ExecuteErrorCallback("'Unhandled callback error: " + e + "'", errorCallback);
            }
        };
        this.ExecuteErrorCallback = function (error, errorCallback) {
            if (errorCallback) {
                try {
                    errorCallback(error);
                }
                catch (e) {
                    throw new Error("'Unhandled callback error: " + e + ". InnerError: " + error + "'");
                }
            }
            else {
                throw new Error("'Unhandled error: " + error + "'");
            }
        };
        this.DetachHandlers = function () {
            _this.errorHandlers = [];
            _this.successHandlers = [];
        };
        this.promiseResultEvents = new PromiseResultEventSource();
        this.promiseResult = new PromiseResult(this.promiseResultEvents);
    }
    Object.defineProperty(Sink.prototype, "State", {
        get: function () {
            return this.state;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Sink.prototype, "Result", {
        get: function () {
            return this.promiseResult;
        },
        enumerable: true,
        configurable: true
    });
    return Sink;
}());
exports.Sink = Sink;



/***/ }),
/* 15 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var List_1 = __webpack_require__(13);
var Promise_1 = __webpack_require__(14);
var SubscriberType;
(function (SubscriberType) {
    SubscriberType[SubscriberType["Dequeue"] = 0] = "Dequeue";
    SubscriberType[SubscriberType["Peek"] = 1] = "Peek";
})(SubscriberType || (SubscriberType = {}));
var Queue = /** @class */ (function () {
    function Queue(list) {
        var _this = this;
        this.promiseStore = new List_1.List();
        this.isDrainInProgress = false;
        this.isDisposing = false;
        this.disposeReason = null;
        this.Enqueue = function (item) {
            _this.ThrowIfDispose();
            _this.EnqueueFromPromise(Promise_1.PromiseHelper.FromResult(item));
        };
        this.EnqueueFromPromise = function (promise) {
            _this.ThrowIfDispose();
            _this.promiseStore.Add(promise);
            promise.Finally(function () {
                while (_this.promiseStore.Length() > 0) {
                    if (!_this.promiseStore.First().Result().IsCompleted) {
                        break;
                    }
                    else {
                        var p = _this.promiseStore.RemoveFirst();
                        if (!p.Result().IsError) {
                            _this.list.Add(p.Result().Result);
                        }
                        else {
                            // TODO: Log as warning.
                        }
                    }
                }
            });
        };
        this.Dequeue = function () {
            _this.ThrowIfDispose();
            var deferredSubscriber = new Promise_1.Deferred();
            if (_this.subscribers) {
                _this.subscribers.Add({ deferral: deferredSubscriber, type: SubscriberType.Dequeue });
                _this.Drain();
            }
            return deferredSubscriber.Promise();
        };
        this.Peek = function () {
            _this.ThrowIfDispose();
            var deferredSubscriber = new Promise_1.Deferred();
            var subs = _this.subscribers;
            if (subs) {
                _this.subscribers.Add({ deferral: deferredSubscriber, type: SubscriberType.Peek });
                _this.Drain();
            }
            return deferredSubscriber.Promise();
        };
        this.Length = function () {
            _this.ThrowIfDispose();
            return _this.list.Length();
        };
        this.IsDisposed = function () {
            return _this.subscribers == null;
        };
        this.DrainAndDispose = function (pendingItemProcessor, reason) {
            if (!_this.IsDisposed() && !_this.isDisposing) {
                _this.disposeReason = reason;
                _this.isDisposing = true;
                var subs = _this.subscribers;
                if (subs) {
                    while (subs.Length() > 0) {
                        var subscriber = subs.RemoveFirst();
                        // TODO: this needs work (Resolve(null) instead?).
                        subscriber.deferral.Resolve(undefined);
                        // subscriber.deferral.Reject("Disposed");
                    }
                    // note: this block assumes cooperative multitasking, i.e.,
                    // between the if-statement and the assignment there are no
                    // thread switches.
                    // Reason is that between the initial const = this.; and this
                    // point there is the derral.Resolve() operation that might have
                    // caused recursive calls to the Queue, especially, calling
                    // Dispose() on the queue alredy (which would reset the var
                    // here to null!).
                    // That should generally hold true for javascript...
                    if (_this.subscribers === subs) {
                        _this.subscribers = subs;
                    }
                }
                for (var _i = 0, _a = _this.detachables; _i < _a.length; _i++) {
                    var detachable = _a[_i];
                    detachable.Detach();
                }
                if (_this.promiseStore.Length() > 0 && pendingItemProcessor) {
                    return Promise_1.PromiseHelper
                        .WhenAll(_this.promiseStore.ToArray())
                        .ContinueWith(function () {
                        _this.subscribers = null;
                        _this.list.ForEach(function (item, index) {
                            pendingItemProcessor(item);
                        });
                        _this.list = null;
                        return true;
                    });
                }
                else {
                    _this.subscribers = null;
                    _this.list = null;
                }
            }
            return Promise_1.PromiseHelper.FromResult(true);
        };
        this.Dispose = function (reason) {
            _this.DrainAndDispose(null, reason);
        };
        this.Drain = function () {
            if (!_this.isDrainInProgress && !_this.isDisposing) {
                _this.isDrainInProgress = true;
                var subs = _this.subscribers;
                var lists = _this.list;
                if (subs && lists) {
                    while (lists.Length() > 0 && subs.Length() > 0 && !_this.isDisposing) {
                        var subscriber = subs.RemoveFirst();
                        if (subscriber.type === SubscriberType.Peek) {
                            subscriber.deferral.Resolve(lists.First());
                        }
                        else {
                            var dequeuedItem = lists.RemoveFirst();
                            subscriber.deferral.Resolve(dequeuedItem);
                        }
                    }
                    // note: this block assumes cooperative multitasking, i.e.,
                    // between the if-statement and the assignment there are no
                    // thread switches.
                    // Reason is that between the initial const = this.; and this
                    // point there is the derral.Resolve() operation that might have
                    // caused recursive calls to the Queue, especially, calling
                    // Dispose() on the queue alredy (which would reset the var
                    // here to null!).
                    // That should generally hold true for javascript...
                    if (_this.subscribers === subs) {
                        _this.subscribers = subs;
                    }
                    // note: this block assumes cooperative multitasking, i.e.,
                    // between the if-statement and the assignment there are no
                    // thread switches.
                    // Reason is that between the initial const = this.; and this
                    // point there is the derral.Resolve() operation that might have
                    // caused recursive calls to the Queue, especially, calling
                    // Dispose() on the queue alredy (which would reset the var
                    // here to null!).
                    // That should generally hold true for javascript...
                    if (_this.list === lists) {
                        _this.list = lists;
                    }
                }
                _this.isDrainInProgress = false;
            }
        };
        this.ThrowIfDispose = function () {
            if (_this.IsDisposed()) {
                if (_this.disposeReason) {
                    throw new Error_1.InvalidOperationError(_this.disposeReason);
                }
                throw new Error_1.ObjectDisposedError("Queue");
            }
            else if (_this.isDisposing) {
                throw new Error_1.InvalidOperationError("Queue disposing");
            }
        };
        this.list = list ? list : new List_1.List();
        this.detachables = [];
        this.subscribers = new List_1.List();
        this.detachables.push(this.list.OnAdded(this.Drain));
    }
    return Queue;
}());
exports.Queue = Queue;



/***/ }),
/* 16 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Guid_1 = __webpack_require__(5);
var Exports_1 = __webpack_require__(1);
var Exports_2 = __webpack_require__(0);
var AudioStreamFormat_1 = __webpack_require__(17);
var bufferSize = 4096;
/**
 * Represents audio input stream used for custom audio input configurations.
 * @class AudioInputStream
 */
var AudioInputStream = /** @class */ (function () {
    /**
     * Creates and initializes an instance.
     * @constructor
     */
    function AudioInputStream() {
    }
    /**
     * Creates a memory backed PushAudioInputStream with the specified audio format.
     * @member AudioInputStream.createPushStream
     * @function
     * @public
     * @param {AudioStreamFormat} format - The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16 kHz 16bit mono PCM).
     * @returns {PushAudioInputStream} The audio input stream being created.
     */
    AudioInputStream.createPushStream = function (format) {
        return PushAudioInputStream.create(format);
    };
    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods.
     * @member AudioInputStream.createPullStream
     * @function
     * @public
     * @param {PullAudioInputStreamCallback} callback - The custom audio input object, derived from PullAudioInputStreamCallback
     * @param {AudioStreamFormat} format - The audio data format in which audio will be returned from the callback's read() method (currently only support 16 kHz 16bit mono PCM).
     * @returns {PullAudioInputStream} The audio input stream being created.
     */
    AudioInputStream.createPullStream = function (callback, format) {
        return PullAudioInputStream.create(callback, format);
        // throw new Error("Oops");
    };
    return AudioInputStream;
}());
exports.AudioInputStream = AudioInputStream;
/**
 * Represents memory backed push audio input stream used for custom audio input configurations.
 * @class PushAudioInputStream
 */
// tslint:disable-next-line:max-classes-per-file
var PushAudioInputStream = /** @class */ (function (_super) {
    __extends(PushAudioInputStream, _super);
    function PushAudioInputStream() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    /**
     * Creates a memory backed PushAudioInputStream with the specified audio format.
     * @member PushAudioInputStream.create
     * @function
     * @public
     * @param {AudioStreamFormat} format - The audio data format in which audio will be written to the push audio stream's write() method (currently only support 16 kHz 16bit mono PCM).
     * @returns {PushAudioInputStream} The push audio input stream being created.
     */
    PushAudioInputStream.create = function (format) {
        return new PushAudioInputStreamImpl(format);
    };
    return PushAudioInputStream;
}(AudioInputStream));
exports.PushAudioInputStream = PushAudioInputStream;
/**
 * Represents memory backed push audio input stream used for custom audio input configurations.
 * @private
 * @class PushAudioInputStreamImpl
 */
// tslint:disable-next-line:max-classes-per-file
var PushAudioInputStreamImpl = /** @class */ (function (_super) {
    __extends(PushAudioInputStreamImpl, _super);
    /**
     * Creates and initalizes an instance with the given values.
     * @constructor
     * @param {AudioStreamFormat} format - The audio stream format.
     */
    function PushAudioInputStreamImpl(format) {
        var _this = _super.call(this) || this;
        _this.stream = new Exports_1.Stream();
        _this.OnEvent = function (event) {
            _this.events.OnEvent(event);
            Exports_1.Events.Instance.OnEvent(event);
        };
        if (format === undefined) {
            _this.format = AudioStreamFormat_1.AudioStreamFormatImpl.getDefaultInputFormat();
        }
        else {
            _this.format = format;
        }
        _this.events = new Exports_1.EventSource();
        _this.id = Guid_1.CreateNoDashGuid();
        return _this;
    }
    Object.defineProperty(PushAudioInputStreamImpl.prototype, "Format", {
        /**
         * Format information for the audio
         */
        get: function () {
            return this.format;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Writes the audio data specified by making an internal copy of the data.
     * @member PushAudioInputStreamImpl.prototype.write
     * @function
     * @public
     * @param {ArrayBuffer} dataBuffer - The audio buffer of which this function will make a copy.
     */
    PushAudioInputStreamImpl.prototype.write = function (dataBuffer) {
        // Break the data up into smaller chunks if needed.
        var i;
        for (i = bufferSize - 1; i < dataBuffer.byteLength; i += bufferSize) {
            this.stream.Write(dataBuffer.slice(i - (bufferSize - 1), i));
        }
        this.stream.Write(dataBuffer.slice(i - (bufferSize - 1), dataBuffer.byteLength - 1));
    };
    /**
     * Closes the stream.
     * @member PushAudioInputStreamImpl.prototype.close
     * @function
     * @public
     */
    PushAudioInputStreamImpl.prototype.close = function () {
        this.stream.Close();
    };
    PushAudioInputStreamImpl.prototype.Id = function () {
        return this.id;
    };
    PushAudioInputStreamImpl.prototype.TurnOn = function () {
        this.OnEvent(new Exports_1.AudioSourceInitializingEvent(this.id)); // no stream id
        this.OnEvent(new Exports_1.AudioSourceReadyEvent(this.id));
        return Exports_1.PromiseHelper.FromResult(true);
    };
    PushAudioInputStreamImpl.prototype.Attach = function (audioNodeId) {
        var _this = this;
        this.OnEvent(new Exports_1.AudioStreamNodeAttachingEvent(this.id, audioNodeId));
        return this.TurnOn()
            .OnSuccessContinueWith(function (_) {
            // For now we support a single parallel reader of the pushed stream.
            // So we can simiply hand the stream to the recognizer and let it recognize.
            return _this.stream.GetReader();
        })
            .OnSuccessContinueWith(function (streamReader) {
            _this.OnEvent(new Exports_1.AudioStreamNodeAttachedEvent(_this.id, audioNodeId));
            return {
                Detach: function () {
                    streamReader.Close();
                    _this.OnEvent(new Exports_1.AudioStreamNodeDetachedEvent(_this.id, audioNodeId));
                    _this.TurnOff();
                },
                Id: function () {
                    return audioNodeId;
                },
                Read: function () {
                    return streamReader.Read();
                },
            };
        });
    };
    PushAudioInputStreamImpl.prototype.Detach = function (audioNodeId) {
        this.OnEvent(new Exports_1.AudioStreamNodeDetachedEvent(this.id, audioNodeId));
    };
    PushAudioInputStreamImpl.prototype.TurnOff = function () {
        return Exports_1.PromiseHelper.FromResult(false);
    };
    Object.defineProperty(PushAudioInputStreamImpl.prototype, "Events", {
        get: function () {
            return this.events;
        },
        enumerable: true,
        configurable: true
    });
    return PushAudioInputStreamImpl;
}(PushAudioInputStream));
exports.PushAudioInputStreamImpl = PushAudioInputStreamImpl;
/*
 * Represents audio input stream used for custom audio input configurations.
 * @class PullAudioInputStream
 */
// tslint:disable-next-line:max-classes-per-file
var PullAudioInputStream = /** @class */ (function (_super) {
    __extends(PullAudioInputStream, _super);
    /**
     * Creates and initializes and instance.
     * @constructor
     */
    function PullAudioInputStream() {
        return _super.call(this) || this;
    }
    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16 kHz 16bit mono PCM).
     * @member PullAudioInputStream.create
     * @function
     * @public
     * @param {PullAudioInputStreamCallback} callback - The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @param {AudioStreamFormat} format - The audio data format in which audio will be returned from the callback's read() method (currently only support 16 kHz 16bit mono PCM).
     * @returns {PullAudioInputStream} The push audio input stream being created.
     */
    PullAudioInputStream.create = function (callback, format) {
        return new PullAudioInputStreamImpl(callback, format);
    };
    return PullAudioInputStream;
}(AudioInputStream));
exports.PullAudioInputStream = PullAudioInputStream;
/**
 * Represents audio input stream used for custom audio input configurations.
 * @private
 * @class PullAudioInputStreamImpl
 */
// tslint:disable-next-line:max-classes-per-file
var PullAudioInputStreamImpl = /** @class */ (function (_super) {
    __extends(PullAudioInputStreamImpl, _super);
    /**
     * Creates a PullAudioInputStream that delegates to the specified callback interface for read() and close() methods, using the default format (16 kHz 16bit mono PCM).
     * @constructor
     * @param {PullAudioInputStreamCallback} callback - The custom audio input object, derived from PullAudioInputStreamCustomCallback
     * @param {AudioStreamFormat} format - The audio data format in which audio will be returned from the callback's read() method (currently only support 16 kHz 16bit mono PCM).
     */
    function PullAudioInputStreamImpl(callback, format) {
        var _this = _super.call(this) || this;
        _this.OnEvent = function (event) {
            _this.events.OnEvent(event);
            Exports_1.Events.Instance.OnEvent(event);
        };
        if (undefined === format) {
            _this.format = Exports_2.AudioStreamFormat.getDefaultInputFormat();
        }
        else {
            _this.format = format;
        }
        _this.events = new Exports_1.EventSource();
        _this.id = Guid_1.CreateNoDashGuid();
        _this.callback = callback;
        _this.isClosed = false;
        return _this;
    }
    Object.defineProperty(PullAudioInputStreamImpl.prototype, "Format", {
        /**
         * Format information for the audio
         */
        get: function () {
            return this.format;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Closes the stream.
     * @member PullAudioInputStreamImpl.prototype.close
     * @function
     * @public
     */
    PullAudioInputStreamImpl.prototype.close = function () {
        this.isClosed = true;
        this.callback.close();
    };
    PullAudioInputStreamImpl.prototype.Id = function () {
        return this.id;
    };
    PullAudioInputStreamImpl.prototype.TurnOn = function () {
        this.OnEvent(new Exports_1.AudioSourceInitializingEvent(this.id)); // no stream id
        this.OnEvent(new Exports_1.AudioSourceReadyEvent(this.id));
        return Exports_1.PromiseHelper.FromResult(true);
    };
    PullAudioInputStreamImpl.prototype.Attach = function (audioNodeId) {
        var _this = this;
        this.OnEvent(new Exports_1.AudioStreamNodeAttachingEvent(this.id, audioNodeId));
        return this.TurnOn()
            .OnSuccessContinueWith(function (result) {
            _this.OnEvent(new Exports_1.AudioStreamNodeAttachedEvent(_this.id, audioNodeId));
            return {
                Detach: function () {
                    _this.callback.close();
                    _this.OnEvent(new Exports_1.AudioStreamNodeDetachedEvent(_this.id, audioNodeId));
                    _this.TurnOff();
                },
                Id: function () {
                    return audioNodeId;
                },
                Read: function () {
                    var readBuff = new ArrayBuffer(bufferSize);
                    var pulledBytes = _this.callback.read(readBuff);
                    return Exports_1.PromiseHelper.FromResult({
                        Buffer: readBuff.slice(0, pulledBytes),
                        IsEnd: _this.isClosed,
                    });
                },
            };
        });
    };
    PullAudioInputStreamImpl.prototype.Detach = function (audioNodeId) {
        this.OnEvent(new Exports_1.AudioStreamNodeDetachedEvent(this.id, audioNodeId));
    };
    PullAudioInputStreamImpl.prototype.TurnOff = function () {
        return Exports_1.PromiseHelper.FromResult(false);
    };
    Object.defineProperty(PullAudioInputStreamImpl.prototype, "Events", {
        get: function () {
            return this.events;
        },
        enumerable: true,
        configurable: true
    });
    return PullAudioInputStreamImpl;
}(PullAudioInputStream));
exports.PullAudioInputStreamImpl = PullAudioInputStreamImpl;



/***/ }),
/* 17 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Represents audio stream format used for custom audio input configurations.
 * @class AudioStreamFormat
 */
var AudioStreamFormat = /** @class */ (function () {
    function AudioStreamFormat() {
    }
    /**
     * Creates an audio stream format object representing the default audio stream format (16KHz 16bit mono PCM).
     * @member AudioStreamFormat.getDefaultInputFormat
     * @function
     * @public
     * @returns {AudioStreamFormat} The audio stream format being created.
     */
    AudioStreamFormat.getDefaultInputFormat = function () {
        return AudioStreamFormatImpl.getDefaultInputFormat();
    };
    /**
     * Creates an audio stream format object with the specified pcm waveformat characteristics.
     * @member AudioStreamFormat.getWaveFormatPCM
     * @function
     * @public
     * @param {number} samplesPerSecond - Sample rate, in samples per second (Hertz).
     * @param {number} bitsPerSample - Bits per sample, typically 16.
     * @param {number} channels - Number of channels in the waveform-audio data. Monaural data uses one channel and stereo data uses two channels.
     * @returns {AudioStreamFormat} The audio stream format being created.
     */
    AudioStreamFormat.getWaveFormatPCM = function (samplesPerSecond, bitsPerSample, channels) {
        return new AudioStreamFormatImpl(samplesPerSecond, bitsPerSample, channels);
    };
    return AudioStreamFormat;
}());
exports.AudioStreamFormat = AudioStreamFormat;
/**
 * @private
 * @class AudioStreamFormatImpl
 */
// tslint:disable-next-line:max-classes-per-file
var AudioStreamFormatImpl = /** @class */ (function (_super) {
    __extends(AudioStreamFormatImpl, _super);
    /**
     * Creates an instance with the given values.
     * @constructor
     * @param {number} samplesPerSec - Samples per second.
     * @param {number} bitsPerSample - Bits per sample.
     * @param {number} channels - Number of channels.
     */
    function AudioStreamFormatImpl(samplesPerSec, bitsPerSample, channels) {
        if (samplesPerSec === void 0) { samplesPerSec = 16000; }
        if (bitsPerSample === void 0) { bitsPerSample = 16; }
        if (channels === void 0) { channels = 1; }
        var _this = _super.call(this) || this;
        _this.formatTag = 1;
        _this.bitsPerSample = bitsPerSample;
        _this.samplesPerSec = samplesPerSec;
        _this.channels = channels;
        _this.avgBytesPerSec = _this.samplesPerSec * _this.channels * (_this.bitsPerSample / 8);
        _this.blockAlign = _this.channels * Math.max(_this.bitsPerSample, 8);
        return _this;
    }
    /**
     * Retrieves the default input format.
     * @member AudioStreamFormatImpl.getDefaultInputFormat
     * @function
     * @public
     * @returns {AudioStreamFormatImpl} The default input format.
     */
    AudioStreamFormatImpl.getDefaultInputFormat = function () {
        return new AudioStreamFormatImpl();
    };
    /**
     * Closes the configuration object.
     * @member AudioStreamFormatImpl.prototype.close
     * @function
     * @public
     */
    AudioStreamFormatImpl.prototype.close = function () { return; };
    return AudioStreamFormatImpl;
}(AudioStreamFormat));
exports.AudioStreamFormatImpl = AudioStreamFormatImpl;



/***/ }),
/* 18 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(6);
var Exports_2 = __webpack_require__(2);
var Exports_3 = __webpack_require__(1);
var Exports_4 = __webpack_require__(0);
var Exports_5 = __webpack_require__(2);
var TestHooksParamName = "testhooks";
var ConnectionIdHeader = "X-ConnectionId";
var SpeechConnectionFactory = /** @class */ (function () {
    function SpeechConnectionFactory() {
        var _this = this;
        this.Create = function (config, authInfo, connectionId) {
            var endpoint = config.parameters.getProperty(Exports_4.PropertyId.SpeechServiceConnection_Endpoint, undefined);
            if (!endpoint) {
                var region = config.parameters.getProperty(Exports_4.PropertyId.SpeechServiceConnection_Region, undefined);
                switch (config.RecognitionMode) {
                    case Exports_5.RecognitionMode.Conversation:
                        endpoint = _this.Host(region) + _this.ConversationRelativeUri;
                        break;
                    case Exports_5.RecognitionMode.Dictation:
                        endpoint = _this.Host(region) + _this.DictationRelativeUri;
                        break;
                    default:
                        endpoint = _this.Host(region) + _this.InteractiveRelativeUri; // default is interactive
                        break;
                }
            }
            var queryParams = {
                format: config.parameters.getProperty(Exports_2.OutputFormatPropertyName, Exports_4.OutputFormat[Exports_4.OutputFormat.Simple]).toLowerCase(),
                language: config.parameters.getProperty(Exports_4.PropertyId.SpeechServiceConnection_RecoLanguage),
            };
            if (_this.IsDebugModeEnabled) {
                queryParams[TestHooksParamName] = "1";
            }
            var headers = {};
            headers[authInfo.HeaderName] = authInfo.Token;
            headers[ConnectionIdHeader] = connectionId;
            return new Exports_1.WebsocketConnection(endpoint, queryParams, headers, new Exports_5.WebsocketMessageFormatter(), connectionId);
        };
    }
    SpeechConnectionFactory.prototype.Host = function (region) {
        return Exports_3.Storage.Local.GetOrAdd("Host", "wss://" + region + ".stt.speech.microsoft.com");
    };
    Object.defineProperty(SpeechConnectionFactory.prototype, "InteractiveRelativeUri", {
        get: function () {
            return Exports_3.Storage.Local.GetOrAdd("InteractiveRelativeUri", "/speech/recognition/interactive/cognitiveservices/v1");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConnectionFactory.prototype, "ConversationRelativeUri", {
        get: function () {
            return Exports_3.Storage.Local.GetOrAdd("ConversationRelativeUri", "/speech/recognition/conversation/cognitiveservices/v1");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConnectionFactory.prototype, "DictationRelativeUri", {
        get: function () {
            return Exports_3.Storage.Local.GetOrAdd("DictationRelativeUri", "/speech/recognition/dictation/cognitiveservices/v1");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConnectionFactory.prototype, "IsDebugModeEnabled", {
        get: function () {
            var value = Exports_3.Storage.Local.GetOrAdd("IsDebugModeEnabled", "false");
            return value.toLowerCase() === "true";
        },
        enumerable: true,
        configurable: true
    });
    return SpeechConnectionFactory;
}());
exports.SpeechConnectionFactory = SpeechConnectionFactory;



/***/ }),
/* 19 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var WebsocketMessageAdapter = /** @class */ (function () {
    function WebsocketMessageAdapter(uri, connectionId, messageFormatter) {
        var _this = this;
        this.Open = function () {
            if (_this.connectionState === Exports_1.ConnectionState.Disconnected) {
                return Exports_1.PromiseHelper.FromError("Cannot open a connection that is in " + _this.connectionState + " state");
            }
            if (_this.connectionEstablishDeferral) {
                return _this.connectionEstablishDeferral.Promise();
            }
            _this.connectionEstablishDeferral = new Exports_1.Deferred();
            _this.connectionState = Exports_1.ConnectionState.Connecting;
            try {
                _this.websocketClient = new WebSocket(_this.uri);
                _this.websocketClient.binaryType = "arraybuffer";
                _this.receivingMessageQueue = new Exports_1.Queue();
                _this.disconnectDeferral = new Exports_1.Deferred();
                _this.sendMessageQueue = new Exports_1.Queue();
                _this.ProcessSendQueue();
            }
            catch (error) {
                _this.connectionEstablishDeferral.Resolve(new Exports_1.ConnectionOpenResponse(500, error));
                return _this.connectionEstablishDeferral.Promise();
            }
            _this.OnEvent(new Exports_1.ConnectionStartEvent(_this.connectionId, _this.uri));
            _this.websocketClient.onopen = function (e) {
                _this.connectionState = Exports_1.ConnectionState.Connected;
                _this.OnEvent(new Exports_1.ConnectionEstablishedEvent(_this.connectionId));
                _this.connectionEstablishDeferral.Resolve(new Exports_1.ConnectionOpenResponse(200, ""));
            };
            _this.websocketClient.onerror = function (e) {
                // TODO: Understand what this is error is. Will we still get onClose ?
                if (_this.connectionState !== Exports_1.ConnectionState.Connecting) {
                    // TODO: Is this required ?
                    // this.OnEvent(new ConnectionErrorEvent(errorMsg, connectionId));
                }
            };
            _this.websocketClient.onclose = function (e) {
                if (_this.connectionState === Exports_1.ConnectionState.Connecting) {
                    _this.connectionState = Exports_1.ConnectionState.Disconnected;
                    // this.OnEvent(new ConnectionEstablishErrorEvent(this.connectionId, e.code, e.reason));
                    _this.connectionEstablishDeferral.Resolve(new Exports_1.ConnectionOpenResponse(e.code, e.reason));
                }
                else {
                    _this.OnEvent(new Exports_1.ConnectionClosedEvent(_this.connectionId, e.code, e.reason));
                }
                _this.OnClose(e.code, e.reason);
            };
            _this.websocketClient.onmessage = function (e) {
                var networkReceivedTime = new Date().toISOString();
                if (_this.connectionState === Exports_1.ConnectionState.Connected) {
                    var deferred_1 = new Exports_1.Deferred();
                    // let id = ++this.idCounter;
                    _this.receivingMessageQueue.EnqueueFromPromise(deferred_1.Promise());
                    if (e.data instanceof ArrayBuffer) {
                        var rawMessage = new Exports_1.RawWebsocketMessage(Exports_1.MessageType.Binary, e.data);
                        _this.messageFormatter
                            .ToConnectionMessage(rawMessage)
                            .On(function (connectionMessage) {
                            _this.OnEvent(new Exports_1.ConnectionMessageReceivedEvent(_this.connectionId, networkReceivedTime, connectionMessage));
                            deferred_1.Resolve(connectionMessage);
                        }, function (error) {
                            // TODO: Events for these ?
                            deferred_1.Reject("Invalid binary message format. Error: " + error);
                        });
                    }
                    else {
                        var rawMessage = new Exports_1.RawWebsocketMessage(Exports_1.MessageType.Text, e.data);
                        _this.messageFormatter
                            .ToConnectionMessage(rawMessage)
                            .On(function (connectionMessage) {
                            _this.OnEvent(new Exports_1.ConnectionMessageReceivedEvent(_this.connectionId, networkReceivedTime, connectionMessage));
                            deferred_1.Resolve(connectionMessage);
                        }, function (error) {
                            // TODO: Events for these ?
                            deferred_1.Reject("Invalid text message format. Error: " + error);
                        });
                    }
                }
            };
            return _this.connectionEstablishDeferral.Promise();
        };
        this.Send = function (message) {
            if (_this.connectionState !== Exports_1.ConnectionState.Connected) {
                return Exports_1.PromiseHelper.FromError("Cannot send on connection that is in " + _this.connectionState + " state");
            }
            var messageSendStatusDeferral = new Exports_1.Deferred();
            var messageSendDeferral = new Exports_1.Deferred();
            _this.sendMessageQueue.EnqueueFromPromise(messageSendDeferral.Promise());
            _this.messageFormatter
                .FromConnectionMessage(message)
                .On(function (rawMessage) {
                messageSendDeferral.Resolve({
                    Message: message,
                    RawWebsocketMessage: rawMessage,
                    SendStatusDeferral: messageSendStatusDeferral,
                });
            }, function (error) {
                messageSendDeferral.Reject("Error formatting the message. " + error);
            });
            return messageSendStatusDeferral.Promise();
        };
        this.Read = function () {
            if (_this.connectionState !== Exports_1.ConnectionState.Connected) {
                return Exports_1.PromiseHelper.FromError("Cannot read on connection that is in " + _this.connectionState + " state");
            }
            return _this.receivingMessageQueue.Dequeue();
        };
        this.Close = function (reason) {
            if (_this.websocketClient) {
                if (_this.connectionState !== Exports_1.ConnectionState.Disconnected) {
                    _this.websocketClient.close(1000, reason ? reason : "Normal closure by client");
                }
            }
            else {
                var deferral = new Exports_1.Deferred();
                deferral.Resolve(true);
                return deferral.Promise();
            }
            return _this.disconnectDeferral.Promise();
        };
        this.SendRawMessage = function (sendItem) {
            try {
                // indicates we are draining the queue and it came with no message;
                if (!sendItem) {
                    return Exports_1.PromiseHelper.FromResult(true);
                }
                _this.OnEvent(new Exports_1.ConnectionMessageSentEvent(_this.connectionId, new Date().toISOString(), sendItem.Message));
                _this.websocketClient.send(sendItem.RawWebsocketMessage.Payload);
                return Exports_1.PromiseHelper.FromResult(true);
            }
            catch (e) {
                return Exports_1.PromiseHelper.FromError("websocket send error: " + e);
            }
        };
        this.OnClose = function (code, reason) {
            var closeReason = "Connection closed. " + code + ": " + reason;
            _this.connectionState = Exports_1.ConnectionState.Disconnected;
            _this.disconnectDeferral.Resolve(true);
            _this.receivingMessageQueue.Dispose(reason);
            _this.receivingMessageQueue.DrainAndDispose(function (pendingReceiveItem) {
                // TODO: Events for these ?
                // Logger.Instance.OnEvent(new LoggingEvent(LogType.Warning, null, `Failed to process received message. Reason: ${closeReason}, Message: ${JSON.stringify(pendingReceiveItem)}`));
            }, closeReason);
            _this.sendMessageQueue.DrainAndDispose(function (pendingSendItem) {
                pendingSendItem.SendStatusDeferral.Reject(closeReason);
            }, closeReason);
        };
        this.ProcessSendQueue = function () {
            _this.sendMessageQueue
                .Dequeue()
                .On(function (sendItem) {
                // indicates we are draining the queue and it came with no message;
                if (!sendItem) {
                    return;
                }
                _this.SendRawMessage(sendItem)
                    .On(function (result) {
                    sendItem.SendStatusDeferral.Resolve(result);
                    _this.ProcessSendQueue();
                }, function (sendError) {
                    sendItem.SendStatusDeferral.Reject(sendError);
                    _this.ProcessSendQueue();
                });
            }, function (error) {
                // do nothing
            });
        };
        this.OnEvent = function (event) {
            _this.connectionEvents.OnEvent(event);
            Exports_1.Events.Instance.OnEvent(event);
        };
        if (!uri) {
            throw new Exports_1.ArgumentNullError("uri");
        }
        if (!messageFormatter) {
            throw new Exports_1.ArgumentNullError("messageFormatter");
        }
        this.connectionEvents = new Exports_1.EventSource();
        this.connectionId = connectionId;
        this.messageFormatter = messageFormatter;
        this.connectionState = Exports_1.ConnectionState.None;
        this.uri = uri;
    }
    Object.defineProperty(WebsocketMessageAdapter.prototype, "State", {
        get: function () {
            return this.connectionState;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(WebsocketMessageAdapter.prototype, "Events", {
        get: function () {
            return this.connectionEvents;
        },
        enumerable: true,
        configurable: true
    });
    return WebsocketMessageAdapter;
}());
exports.WebsocketMessageAdapter = WebsocketMessageAdapter;



/***/ }),
/* 20 */
/***/ (function(module, exports, __webpack_require__) {

window.SpeechSDK = __webpack_require__(21);


/***/ }),
/* 21 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

function __export(m) {
    for (var p in m) if (!exports.hasOwnProperty(p)) exports[p] = m[p];
}
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(6);
var Exports_2 = __webpack_require__(1);
// Common.Storage.SetLocalStorage(new Common.Browser.LocalStorage());
// Common.Storage.SetSessionStorage(new Common.Browser.SessionStorage());
Exports_2.Events.Instance.AttachListener(new Exports_1.ConsoleLoggingListener());
// Speech SDK API
__export(__webpack_require__(0));



/***/ }),
/* 22 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var ConsoleLoggingListener = /** @class */ (function () {
    function ConsoleLoggingListener(logLevelFilter) {
        if (logLevelFilter === void 0) { logLevelFilter = Exports_1.EventType.Warning; }
        var _this = this;
        this.OnEvent = function (event) {
            if (event.EventType >= _this.logLevelFilter) {
                var log = _this.ToString(event);
                switch (event.EventType) {
                    case Exports_1.EventType.Debug:
                        // tslint:disable-next-line:no-console
                        console.debug(log);
                        break;
                    case Exports_1.EventType.Info:
                        // tslint:disable-next-line:no-console
                        console.info(log);
                        break;
                    case Exports_1.EventType.Warning:
                        // tslint:disable-next-line:no-console
                        console.warn(log);
                        break;
                    case Exports_1.EventType.Error:
                        // tslint:disable-next-line:no-console
                        console.error(log);
                        break;
                    default:
                        // tslint:disable-next-line:no-console
                        console.log(log);
                        break;
                }
            }
        };
        this.ToString = function (event) {
            var logFragments = [
                "" + event.EventTime,
                "" + event.Name,
            ];
            for (var prop in event) {
                if (prop && event.hasOwnProperty(prop) && prop !== "eventTime" && prop !== "eventType" && prop !== "eventId" && prop !== "name" && prop !== "constructor") {
                    var value = event[prop];
                    var valueToLog = "<NULL>";
                    if (value !== undefined && value !== null) {
                        if (typeof (value) === "number" || typeof (value) === "string") {
                            valueToLog = value.toString();
                        }
                        else {
                            valueToLog = JSON.stringify(value);
                        }
                    }
                    logFragments.push(prop + ": " + valueToLog);
                }
            }
            return logFragments.join(" | ");
        };
        this.logLevelFilter = logLevelFilter;
    }
    return ConsoleLoggingListener;
}());
exports.ConsoleLoggingListener = ConsoleLoggingListener;



/***/ }),
/* 23 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var PlatformEvent_1 = __webpack_require__(8);
var AudioSourceEvent = /** @class */ (function (_super) {
    __extends(AudioSourceEvent, _super);
    function AudioSourceEvent(eventName, audioSourceId, eventType) {
        if (eventType === void 0) { eventType = PlatformEvent_1.EventType.Info; }
        var _this = _super.call(this, eventName, eventType) || this;
        _this.audioSourceId = audioSourceId;
        return _this;
    }
    Object.defineProperty(AudioSourceEvent.prototype, "AudioSourceId", {
        get: function () {
            return this.audioSourceId;
        },
        enumerable: true,
        configurable: true
    });
    return AudioSourceEvent;
}(PlatformEvent_1.PlatformEvent));
exports.AudioSourceEvent = AudioSourceEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioSourceInitializingEvent = /** @class */ (function (_super) {
    __extends(AudioSourceInitializingEvent, _super);
    function AudioSourceInitializingEvent(audioSourceId) {
        return _super.call(this, "AudioSourceInitializingEvent", audioSourceId) || this;
    }
    return AudioSourceInitializingEvent;
}(AudioSourceEvent));
exports.AudioSourceInitializingEvent = AudioSourceInitializingEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioSourceReadyEvent = /** @class */ (function (_super) {
    __extends(AudioSourceReadyEvent, _super);
    function AudioSourceReadyEvent(audioSourceId) {
        return _super.call(this, "AudioSourceReadyEvent", audioSourceId) || this;
    }
    return AudioSourceReadyEvent;
}(AudioSourceEvent));
exports.AudioSourceReadyEvent = AudioSourceReadyEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioSourceOffEvent = /** @class */ (function (_super) {
    __extends(AudioSourceOffEvent, _super);
    function AudioSourceOffEvent(audioSourceId) {
        return _super.call(this, "AudioSourceOffEvent", audioSourceId) || this;
    }
    return AudioSourceOffEvent;
}(AudioSourceEvent));
exports.AudioSourceOffEvent = AudioSourceOffEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioSourceErrorEvent = /** @class */ (function (_super) {
    __extends(AudioSourceErrorEvent, _super);
    function AudioSourceErrorEvent(audioSourceId, error) {
        var _this = _super.call(this, "AudioSourceErrorEvent", audioSourceId, PlatformEvent_1.EventType.Error) || this;
        _this.error = error;
        return _this;
    }
    Object.defineProperty(AudioSourceErrorEvent.prototype, "Error", {
        get: function () {
            return this.error;
        },
        enumerable: true,
        configurable: true
    });
    return AudioSourceErrorEvent;
}(AudioSourceEvent));
exports.AudioSourceErrorEvent = AudioSourceErrorEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioStreamNodeEvent = /** @class */ (function (_super) {
    __extends(AudioStreamNodeEvent, _super);
    function AudioStreamNodeEvent(eventName, audioSourceId, audioNodeId) {
        var _this = _super.call(this, eventName, audioSourceId) || this;
        _this.audioNodeId = audioNodeId;
        return _this;
    }
    Object.defineProperty(AudioStreamNodeEvent.prototype, "AudioNodeId", {
        get: function () {
            return this.audioNodeId;
        },
        enumerable: true,
        configurable: true
    });
    return AudioStreamNodeEvent;
}(AudioSourceEvent));
exports.AudioStreamNodeEvent = AudioStreamNodeEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioStreamNodeAttachingEvent = /** @class */ (function (_super) {
    __extends(AudioStreamNodeAttachingEvent, _super);
    function AudioStreamNodeAttachingEvent(audioSourceId, audioNodeId) {
        return _super.call(this, "AudioStreamNodeAttachingEvent", audioSourceId, audioNodeId) || this;
    }
    return AudioStreamNodeAttachingEvent;
}(AudioStreamNodeEvent));
exports.AudioStreamNodeAttachingEvent = AudioStreamNodeAttachingEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioStreamNodeAttachedEvent = /** @class */ (function (_super) {
    __extends(AudioStreamNodeAttachedEvent, _super);
    function AudioStreamNodeAttachedEvent(audioSourceId, audioNodeId) {
        return _super.call(this, "AudioStreamNodeAttachedEvent", audioSourceId, audioNodeId) || this;
    }
    return AudioStreamNodeAttachedEvent;
}(AudioStreamNodeEvent));
exports.AudioStreamNodeAttachedEvent = AudioStreamNodeAttachedEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioStreamNodeDetachedEvent = /** @class */ (function (_super) {
    __extends(AudioStreamNodeDetachedEvent, _super);
    function AudioStreamNodeDetachedEvent(audioSourceId, audioNodeId) {
        return _super.call(this, "AudioStreamNodeDetachedEvent", audioSourceId, audioNodeId) || this;
    }
    return AudioStreamNodeDetachedEvent;
}(AudioStreamNodeEvent));
exports.AudioStreamNodeDetachedEvent = AudioStreamNodeDetachedEvent;
// tslint:disable-next-line:max-classes-per-file
var AudioStreamNodeErrorEvent = /** @class */ (function (_super) {
    __extends(AudioStreamNodeErrorEvent, _super);
    function AudioStreamNodeErrorEvent(audioSourceId, audioNodeId, error) {
        var _this = _super.call(this, "AudioStreamNodeErrorEvent", audioSourceId, audioNodeId) || this;
        _this.error = error;
        return _this;
    }
    Object.defineProperty(AudioStreamNodeErrorEvent.prototype, "Error", {
        get: function () {
            return this.error;
        },
        enumerable: true,
        configurable: true
    });
    return AudioStreamNodeErrorEvent;
}(AudioStreamNodeEvent));
exports.AudioStreamNodeErrorEvent = AudioStreamNodeErrorEvent;



/***/ }),
/* 24 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var PlatformEvent_1 = __webpack_require__(8);
var ConnectionEvent = /** @class */ (function (_super) {
    __extends(ConnectionEvent, _super);
    function ConnectionEvent(eventName, connectionId, eventType) {
        if (eventType === void 0) { eventType = PlatformEvent_1.EventType.Info; }
        var _this = _super.call(this, eventName, eventType) || this;
        _this.connectionId = connectionId;
        return _this;
    }
    Object.defineProperty(ConnectionEvent.prototype, "ConnectionId", {
        get: function () {
            return this.connectionId;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectionEvent;
}(PlatformEvent_1.PlatformEvent));
exports.ConnectionEvent = ConnectionEvent;
// tslint:disable-next-line:max-classes-per-file
var ConnectionStartEvent = /** @class */ (function (_super) {
    __extends(ConnectionStartEvent, _super);
    function ConnectionStartEvent(connectionId, uri, headers) {
        var _this = _super.call(this, "ConnectionStartEvent", connectionId) || this;
        _this.uri = uri;
        _this.headers = headers;
        return _this;
    }
    Object.defineProperty(ConnectionStartEvent.prototype, "Uri", {
        get: function () {
            return this.uri;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionStartEvent.prototype, "Headers", {
        get: function () {
            return this.headers;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectionStartEvent;
}(ConnectionEvent));
exports.ConnectionStartEvent = ConnectionStartEvent;
// tslint:disable-next-line:max-classes-per-file
var ConnectionEstablishedEvent = /** @class */ (function (_super) {
    __extends(ConnectionEstablishedEvent, _super);
    function ConnectionEstablishedEvent(connectionId, metadata) {
        return _super.call(this, "ConnectionEstablishedEvent", connectionId) || this;
    }
    return ConnectionEstablishedEvent;
}(ConnectionEvent));
exports.ConnectionEstablishedEvent = ConnectionEstablishedEvent;
// tslint:disable-next-line:max-classes-per-file
var ConnectionClosedEvent = /** @class */ (function (_super) {
    __extends(ConnectionClosedEvent, _super);
    function ConnectionClosedEvent(connectionId, statusCode, reason) {
        var _this = _super.call(this, "ConnectionClosedEvent", connectionId, PlatformEvent_1.EventType.Debug) || this;
        _this.reason = reason;
        _this.statusCode = statusCode;
        return _this;
    }
    Object.defineProperty(ConnectionClosedEvent.prototype, "Reason", {
        get: function () {
            return this.reason;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionClosedEvent.prototype, "StatusCode", {
        get: function () {
            return this.statusCode;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectionClosedEvent;
}(ConnectionEvent));
exports.ConnectionClosedEvent = ConnectionClosedEvent;
// tslint:disable-next-line:max-classes-per-file
var ConnectionEstablishErrorEvent = /** @class */ (function (_super) {
    __extends(ConnectionEstablishErrorEvent, _super);
    function ConnectionEstablishErrorEvent(connectionId, statuscode, reason) {
        var _this = _super.call(this, "ConnectionEstablishErrorEvent", connectionId, PlatformEvent_1.EventType.Error) || this;
        _this.statusCode = statuscode;
        _this.reason = reason;
        return _this;
    }
    Object.defineProperty(ConnectionEstablishErrorEvent.prototype, "Reason", {
        get: function () {
            return this.reason;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionEstablishErrorEvent.prototype, "StatusCode", {
        get: function () {
            return this.statusCode;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectionEstablishErrorEvent;
}(ConnectionEvent));
exports.ConnectionEstablishErrorEvent = ConnectionEstablishErrorEvent;
// tslint:disable-next-line:max-classes-per-file
var ConnectionMessageReceivedEvent = /** @class */ (function (_super) {
    __extends(ConnectionMessageReceivedEvent, _super);
    function ConnectionMessageReceivedEvent(connectionId, networkReceivedTimeISO, message) {
        var _this = _super.call(this, "ConnectionMessageReceivedEvent", connectionId) || this;
        _this.networkReceivedTime = networkReceivedTimeISO;
        _this.message = message;
        return _this;
    }
    Object.defineProperty(ConnectionMessageReceivedEvent.prototype, "NetworkReceivedTime", {
        get: function () {
            return this.networkReceivedTime;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionMessageReceivedEvent.prototype, "Message", {
        get: function () {
            return this.message;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectionMessageReceivedEvent;
}(ConnectionEvent));
exports.ConnectionMessageReceivedEvent = ConnectionMessageReceivedEvent;
// tslint:disable-next-line:max-classes-per-file
var ConnectionMessageSentEvent = /** @class */ (function (_super) {
    __extends(ConnectionMessageSentEvent, _super);
    function ConnectionMessageSentEvent(connectionId, networkSentTimeISO, message) {
        var _this = _super.call(this, "ConnectionMessageSentEvent", connectionId) || this;
        _this.networkSentTime = networkSentTimeISO;
        _this.message = message;
        return _this;
    }
    Object.defineProperty(ConnectionMessageSentEvent.prototype, "NetworkSentTime", {
        get: function () {
            return this.networkSentTime;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionMessageSentEvent.prototype, "Message", {
        get: function () {
            return this.message;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectionMessageSentEvent;
}(ConnectionEvent));
exports.ConnectionMessageSentEvent = ConnectionMessageSentEvent;



/***/ }),
/* 25 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var ConnectionOpenResponse = /** @class */ (function () {
    function ConnectionOpenResponse(statusCode, reason) {
        this.statusCode = statusCode;
        this.reason = reason;
    }
    Object.defineProperty(ConnectionOpenResponse.prototype, "StatusCode", {
        get: function () {
            return this.statusCode;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(ConnectionOpenResponse.prototype, "Reason", {
        get: function () {
            return this.reason;
        },
        enumerable: true,
        configurable: true
    });
    return ConnectionOpenResponse;
}());
exports.ConnectionOpenResponse = ConnectionOpenResponse;



/***/ }),
/* 26 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var EventSource_1 = __webpack_require__(11);
var Events = /** @class */ (function () {
    function Events() {
    }
    Object.defineProperty(Events, "Instance", {
        get: function () {
            return Events.instance;
        },
        enumerable: true,
        configurable: true
    });
    Events.instance = new EventSource_1.EventSource();
    Events.SetEventSource = function (eventSource) {
        if (!eventSource) {
            throw new Error_1.ArgumentNullError("eventSource");
        }
        Events.instance = eventSource;
    };
    return Events;
}());
exports.Events = Events;



/***/ }),
/* 27 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var ConnectionState;
(function (ConnectionState) {
    ConnectionState[ConnectionState["None"] = 0] = "None";
    ConnectionState[ConnectionState["Connected"] = 1] = "Connected";
    ConnectionState[ConnectionState["Connecting"] = 2] = "Connecting";
    ConnectionState[ConnectionState["Disconnected"] = 3] = "Disconnected";
})(ConnectionState = exports.ConnectionState || (exports.ConnectionState = {}));



/***/ }),
/* 28 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var ConnectionMessage_1 = __webpack_require__(10);
var Error_1 = __webpack_require__(3);
var Guid_1 = __webpack_require__(5);
var RawWebsocketMessage = /** @class */ (function () {
    function RawWebsocketMessage(messageType, payload, id) {
        this.payload = null;
        if (!payload) {
            throw new Error_1.ArgumentNullError("payload");
        }
        if (messageType === ConnectionMessage_1.MessageType.Binary && !(payload instanceof ArrayBuffer)) {
            throw new Error_1.InvalidOperationError("Payload must be ArrayBuffer");
        }
        if (messageType === ConnectionMessage_1.MessageType.Text && !(typeof (payload) === "string")) {
            throw new Error_1.InvalidOperationError("Payload must be a string");
        }
        this.messageType = messageType;
        this.payload = payload;
        this.id = id ? id : Guid_1.CreateNoDashGuid();
    }
    Object.defineProperty(RawWebsocketMessage.prototype, "MessageType", {
        get: function () {
            return this.messageType;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RawWebsocketMessage.prototype, "Payload", {
        get: function () {
            return this.payload;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RawWebsocketMessage.prototype, "TextContent", {
        get: function () {
            if (this.messageType === ConnectionMessage_1.MessageType.Binary) {
                throw new Error_1.InvalidOperationError("Not supported for binary message");
            }
            return this.payload;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RawWebsocketMessage.prototype, "BinaryContent", {
        get: function () {
            if (this.messageType === ConnectionMessage_1.MessageType.Text) {
                throw new Error_1.InvalidOperationError("Not supported for text message");
            }
            return this.payload;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RawWebsocketMessage.prototype, "Id", {
        get: function () {
            return this.id;
        },
        enumerable: true,
        configurable: true
    });
    return RawWebsocketMessage;
}());
exports.RawWebsocketMessage = RawWebsocketMessage;



/***/ }),
/* 29 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var RiffPcmEncoder = /** @class */ (function () {
    function RiffPcmEncoder(actualSampleRate, desiredSampleRate) {
        var _this = this;
        this.channelCount = 1;
        this.Encode = function (needHeader, actualAudioFrame) {
            var audioFrame = _this.DownSampleAudioFrame(actualAudioFrame, _this.actualSampleRate, _this.desiredSampleRate);
            if (!audioFrame) {
                return null;
            }
            var audioLength = audioFrame.length * 2;
            if (!needHeader) {
                var buffer_1 = new ArrayBuffer(audioLength);
                var view_1 = new DataView(buffer_1);
                _this.FloatTo16BitPCM(view_1, 0, audioFrame);
                return buffer_1;
            }
            var buffer = new ArrayBuffer(44 + audioLength);
            var bitsPerSample = 16;
            var bytesPerSample = bitsPerSample / 8;
            // We dont know ahead of time about the length of audio to stream. So set to 0.
            var fileLength = 0;
            // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView
            var view = new DataView(buffer);
            /* RIFF identifier */
            _this.SetString(view, 0, "RIFF");
            /* file length */
            view.setUint32(4, fileLength, true);
            /* RIFF type & Format */
            _this.SetString(view, 8, "WAVEfmt ");
            /* format chunk length */
            view.setUint32(16, 16, true);
            /* sample format (raw) */
            view.setUint16(20, 1, true);
            /* channel count */
            view.setUint16(22, _this.channelCount, true);
            /* sample rate */
            view.setUint32(24, _this.desiredSampleRate, true);
            /* byte rate (sample rate * block align) */
            view.setUint32(28, _this.desiredSampleRate * _this.channelCount * bytesPerSample, true);
            /* block align (channel count * bytes per sample) */
            view.setUint16(32, _this.channelCount * bytesPerSample, true);
            /* bits per sample */
            view.setUint16(34, bitsPerSample, true);
            /* data chunk identifier */
            _this.SetString(view, 36, "data");
            /* data chunk length */
            view.setUint32(40, fileLength, true);
            _this.FloatTo16BitPCM(view, 44, audioFrame);
            return buffer;
        };
        this.SetString = function (view, offset, str) {
            for (var i = 0; i < str.length; i++) {
                view.setUint8(offset + i, str.charCodeAt(i));
            }
        };
        this.FloatTo16BitPCM = function (view, offset, input) {
            for (var i = 0; i < input.length; i++, offset += 2) {
                var s = Math.max(-1, Math.min(1, input[i]));
                view.setInt16(offset, s < 0 ? s * 0x8000 : s * 0x7FFF, true);
            }
        };
        this.DownSampleAudioFrame = function (srcFrame, srcRate, dstRate) {
            if (dstRate === srcRate || dstRate > srcRate) {
                return srcFrame;
            }
            var ratio = srcRate / dstRate;
            var dstLength = Math.round(srcFrame.length / ratio);
            var dstFrame = new Float32Array(dstLength);
            var srcOffset = 0;
            var dstOffset = 0;
            while (dstOffset < dstLength) {
                var nextSrcOffset = Math.round((dstOffset + 1) * ratio);
                var accum = 0;
                var count = 0;
                while (srcOffset < nextSrcOffset && srcOffset < srcFrame.length) {
                    accum += srcFrame[srcOffset++];
                    count++;
                }
                dstFrame[dstOffset++] = accum / count;
            }
            return dstFrame;
        };
        this.actualSampleRate = actualSampleRate;
        this.desiredSampleRate = desiredSampleRate;
    }
    return RiffPcmEncoder;
}());
exports.RiffPcmEncoder = RiffPcmEncoder;



/***/ }),
/* 30 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var InMemoryStorage_1 = __webpack_require__(12);
var Storage = /** @class */ (function () {
    function Storage() {
    }
    Object.defineProperty(Storage, "Session", {
        get: function () {
            return Storage.sessionStorage;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Storage, "Local", {
        get: function () {
            return Storage.localStorage;
        },
        enumerable: true,
        configurable: true
    });
    Storage.sessionStorage = new InMemoryStorage_1.InMemoryStorage();
    Storage.localStorage = new InMemoryStorage_1.InMemoryStorage();
    Storage.SetSessionStorage = function (sessionStorage) {
        if (!sessionStorage) {
            throw new Error_1.ArgumentNullError("sessionStorage");
        }
        Storage.sessionStorage = sessionStorage;
    };
    Storage.SetLocalStorage = function (localStorage) {
        if (!localStorage) {
            throw new Error_1.ArgumentNullError("localStorage");
        }
        Storage.localStorage = localStorage;
    };
    return Storage;
}());
exports.Storage = Storage;



/***/ }),
/* 31 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Error_1 = __webpack_require__(3);
var Guid_1 = __webpack_require__(5);
var Queue_1 = __webpack_require__(15);
var Stream = /** @class */ (function () {
    function Stream(streamId) {
        var _this = this;
        this.readerIdCounter = 1;
        this.isEnded = false;
        this.Write = function (buffer) {
            _this.ThrowIfClosed();
            _this.WriteStreamChunk({
                Buffer: buffer,
                IsEnd: false,
            });
        };
        this.GetReader = function () {
            var readerId = _this.readerIdCounter;
            _this.readerIdCounter++;
            var readerQueue = new Queue_1.Queue();
            var currentLength = _this.streambuffer.length;
            _this.readerQueues[readerId] = readerQueue;
            for (var i = 0; i < currentLength; i++) {
                readerQueue.Enqueue(_this.streambuffer[i]);
            }
            return new StreamReader(_this.id, readerQueue, function () {
                delete _this.readerQueues[readerId];
            });
        };
        this.Close = function () {
            if (!_this.isEnded) {
                _this.WriteStreamChunk({
                    Buffer: null,
                    IsEnd: true,
                });
                _this.isEnded = true;
            }
        };
        this.WriteStreamChunk = function (streamChunk) {
            _this.ThrowIfClosed();
            _this.streambuffer.push(streamChunk);
            for (var readerId in _this.readerQueues) {
                if (!_this.readerQueues[readerId].IsDisposed()) {
                    try {
                        _this.readerQueues[readerId].Enqueue(streamChunk);
                    }
                    catch (e) {
                        // Do nothing
                    }
                }
            }
        };
        this.ThrowIfClosed = function () {
            if (_this.isEnded) {
                throw new Error_1.InvalidOperationError("Stream closed");
            }
        };
        this.id = streamId ? streamId : Guid_1.CreateNoDashGuid();
        this.streambuffer = [];
        this.readerQueues = {};
    }
    Object.defineProperty(Stream.prototype, "IsClosed", {
        get: function () {
            return this.isEnded;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Stream.prototype, "Id", {
        get: function () {
            return this.id;
        },
        enumerable: true,
        configurable: true
    });
    return Stream;
}());
exports.Stream = Stream;
// tslint:disable-next-line:max-classes-per-file
var StreamReader = /** @class */ (function () {
    function StreamReader(streamId, readerQueue, onClose) {
        var _this = this;
        this.isClosed = false;
        this.Read = function () {
            if (_this.IsClosed) {
                throw new Error_1.InvalidOperationError("StreamReader closed");
            }
            return _this.readerQueue
                .Dequeue()
                .OnSuccessContinueWith(function (streamChunk) {
                if (streamChunk.IsEnd) {
                    _this.readerQueue.Dispose("End of stream reached");
                }
                return streamChunk;
            });
        };
        this.Close = function () {
            if (!_this.isClosed) {
                _this.isClosed = true;
                _this.readerQueue.Dispose("StreamReader closed");
                _this.onClose();
            }
        };
        this.readerQueue = readerQueue;
        this.onClose = onClose;
        this.streamId = streamId;
    }
    Object.defineProperty(StreamReader.prototype, "IsClosed", {
        get: function () {
            return this.isClosed;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(StreamReader.prototype, "StreamId", {
        get: function () {
            return this.streamId;
        },
        enumerable: true,
        configurable: true
    });
    return StreamReader;
}());
exports.StreamReader = StreamReader;



/***/ }),
/* 32 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Defines translation status.
 * @class TranslationStatus
 */
var TranslationStatus;
(function (TranslationStatus) {
    /**
     * @member TranslationStatus.Success
     */
    TranslationStatus[TranslationStatus["Success"] = 0] = "Success";
    /**
     * @member TranslationStatus.Error
     */
    TranslationStatus[TranslationStatus["Error"] = 1] = "Error";
})(TranslationStatus = exports.TranslationStatus || (exports.TranslationStatus = {}));



/***/ }),
/* 33 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var LocalStorage = /** @class */ (function () {
    function LocalStorage() {
        this.Get = function (key) {
            if (!key) {
                throw new Exports_1.ArgumentNullError("key");
            }
            return localStorage.getItem(key);
        };
        this.GetOrAdd = function (key, valueToAdd) {
            if (!key) {
                throw new Exports_1.ArgumentNullError("key");
            }
            var value = localStorage.getItem(key);
            if (value === null || value === undefined) {
                localStorage.setItem(key, valueToAdd);
            }
            return localStorage.getItem(key);
        };
        this.Set = function (key, value) {
            if (!key) {
                throw new Exports_1.ArgumentNullError("key");
            }
            localStorage.setItem(key, value);
        };
        this.Remove = function (key) {
            if (!key) {
                throw new Exports_1.ArgumentNullError("key");
            }
            localStorage.removeItem(key);
        };
    }
    return LocalStorage;
}());
exports.LocalStorage = LocalStorage;



/***/ }),
/* 34 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
var Exports_2 = __webpack_require__(1);
var MicAudioSource = /** @class */ (function () {
    function MicAudioSource(recorder, audioSourceId) {
        var _this = this;
        this.streams = {};
        this.TurnOn = function () {
            if (_this.initializeDeferral) {
                return _this.initializeDeferral.Promise();
            }
            _this.initializeDeferral = new Exports_2.Deferred();
            _this.CreateAudioContext();
            var nav = window.navigator;
            var getUserMedia = (nav.getUserMedia ||
                nav.webkitGetUserMedia ||
                nav.mozGetUserMedia ||
                nav.msGetUserMedia);
            if (!!nav.mediaDevices) {
                getUserMedia = function (constraints, successCallback, errorCallback) {
                    nav.mediaDevices
                        .getUserMedia(constraints)
                        .then(successCallback)
                        .catch(errorCallback);
                };
            }
            if (!getUserMedia) {
                var errorMsg = "Browser does not support getUserMedia.";
                _this.initializeDeferral.Reject(errorMsg);
                _this.OnEvent(new Exports_2.AudioSourceErrorEvent(errorMsg, "")); // mic initialized error - no streamid at this point
            }
            else {
                var next = function () {
                    _this.OnEvent(new Exports_2.AudioSourceInitializingEvent(_this.id)); // no stream id
                    getUserMedia({ audio: true, video: false }, function (mediaStream) {
                        _this.mediaStream = mediaStream;
                        _this.OnEvent(new Exports_2.AudioSourceReadyEvent(_this.id));
                        _this.initializeDeferral.Resolve(true);
                    }, function (error) {
                        var errorMsg = "Error occurred during microphone initialization: " + error;
                        var tmp = _this.initializeDeferral;
                        // HACK: this should be handled through onError callbacks of all promises up the stack.
                        // Unfortunately, the current implementation does not provide an easy way to reject promises
                        // without a lot of code replication.
                        // TODO: fix promise implementation, allow for a graceful reject chaining.
                        _this.initializeDeferral = null;
                        tmp.Reject(errorMsg); // this will bubble up through the whole chain of promises,
                        // with each new level adding extra "Unhandled callback error" prefix to the error message.
                        // The following line is not guaranteed to be executed.
                        _this.OnEvent(new Exports_2.AudioSourceErrorEvent(_this.id, errorMsg));
                    });
                };
                if (_this.context.state === "suspended") {
                    // NOTE: On iOS, the Web Audio API requires sounds to be triggered from an explicit user action.
                    // https://github.com/WebAudio/web-audio-api/issues/790
                    _this.context.resume().then(next, function (reason) {
                        _this.initializeDeferral.Reject("Failed to initialize audio context: " + reason);
                    });
                }
                else {
                    next();
                }
            }
            return _this.initializeDeferral.Promise();
        };
        this.Id = function () {
            return _this.id;
        };
        this.Attach = function (audioNodeId) {
            _this.OnEvent(new Exports_2.AudioStreamNodeAttachingEvent(_this.id, audioNodeId));
            return _this.Listen(audioNodeId).OnSuccessContinueWith(function (streamReader) {
                _this.OnEvent(new Exports_2.AudioStreamNodeAttachedEvent(_this.id, audioNodeId));
                return {
                    Detach: function () {
                        streamReader.Close();
                        delete _this.streams[audioNodeId];
                        _this.OnEvent(new Exports_2.AudioStreamNodeDetachedEvent(_this.id, audioNodeId));
                        _this.TurnOff();
                    },
                    Id: function () {
                        return audioNodeId;
                    },
                    Read: function () {
                        return streamReader.Read();
                    },
                };
            });
        };
        this.Detach = function (audioNodeId) {
            if (audioNodeId && _this.streams[audioNodeId]) {
                _this.streams[audioNodeId].Close();
                delete _this.streams[audioNodeId];
                _this.OnEvent(new Exports_2.AudioStreamNodeDetachedEvent(_this.id, audioNodeId));
            }
        };
        this.TurnOff = function () {
            for (var streamId in _this.streams) {
                if (streamId) {
                    var stream = _this.streams[streamId];
                    if (stream) {
                        stream.Close();
                    }
                }
            }
            _this.OnEvent(new Exports_2.AudioSourceOffEvent(_this.id)); // no stream now
            _this.initializeDeferral = null;
            _this.DestroyAudioContext();
            return Exports_2.PromiseHelper.FromResult(true);
        };
        this.Listen = function (audioNodeId) {
            return _this.TurnOn()
                .OnSuccessContinueWith(function (_) {
                var stream = new Exports_2.Stream(audioNodeId);
                _this.streams[audioNodeId] = stream;
                try {
                    _this.recorder.Record(_this.context, _this.mediaStream, stream);
                }
                catch (error) {
                    _this.OnEvent(new Exports_2.AudioStreamNodeErrorEvent(_this.id, audioNodeId, error));
                    throw error;
                }
                return stream.GetReader();
            });
        };
        this.OnEvent = function (event) {
            _this.events.OnEvent(event);
            Exports_2.Events.Instance.OnEvent(event);
        };
        this.CreateAudioContext = function () {
            if (!!_this.context) {
                return;
            }
            // https://developer.mozilla.org/en-US/docs/Web/API/AudioContext
            var AudioContext = (window.AudioContext)
                || (window.webkitAudioContext)
                || false;
            if (!AudioContext) {
                throw new Error("Browser does not support Web Audio API (AudioContext is not available).");
            }
            _this.context = new AudioContext();
        };
        this.DestroyAudioContext = function () {
            if (!_this.context) {
                return;
            }
            _this.recorder.ReleaseMediaResources(_this.context);
            if ("close" in _this.context) {
                _this.context.close();
                _this.context = null;
            }
            else if (_this.context.state === "running") {
                // Suspend actually takes a callback, but analogous to the
                // resume method, it'll be only fired if suspend is called
                // in a direct response to a user action. The later is not always
                // the case, as TurnOff is also called, when we receive an
                // end-of-speech message from the service. So, doing a best effort
                // fire-and-forget here.
                _this.context.suspend();
            }
        };
        this.id = audioSourceId ? audioSourceId : Exports_2.CreateNoDashGuid();
        this.events = new Exports_2.EventSource();
        this.recorder = recorder;
    }
    Object.defineProperty(MicAudioSource.prototype, "Format", {
        get: function () {
            return MicAudioSource.AUDIOFORMAT;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(MicAudioSource.prototype, "Events", {
        get: function () {
            return this.events;
        },
        enumerable: true,
        configurable: true
    });
    MicAudioSource.AUDIOFORMAT = Exports_1.AudioStreamFormat.getDefaultInputFormat();
    return MicAudioSource;
}());
exports.MicAudioSource = MicAudioSource;



/***/ }),
/* 35 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(6);
var Exports_2 = __webpack_require__(0);
var AudioInputStream_1 = __webpack_require__(16);
/**
 * Represents audio input configuration used for specifying what type of input to use (microphone, file, stream).
 * @class AudioConfig
 */
var AudioConfig = /** @class */ (function () {
    function AudioConfig() {
    }
    /**
     * Creates an AudioConfig object representing the default microphone on the system.
     * @member AudioConfig.fromDefaultMicrophoneInput
     * @function
     * @public
     * @returns {AudioConfig} The audio input configuration being created.
     */
    AudioConfig.fromDefaultMicrophoneInput = function () {
        var pcmRecorder = new Exports_1.PcmRecorder();
        return new AudioConfigImpl(new Exports_1.MicAudioSource(pcmRecorder));
    };
    /**
     * Creates an AudioConfig object representing the specified file.
     * @member AudioConfig.fromWavFileInput
     * @function
     * @public
     * @param {File} fileName - Specifies the audio input file. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
     * @returns {AudioConfig} The audio input configuration being created.
     */
    AudioConfig.fromWavFileInput = function (file) {
        return new AudioConfigImpl(new Exports_1.FileAudioSource(file));
    };
    /**
     * Creates an AudioConfig object representing the specified stream.
     * @member AudioConfig.fromStreamInput
     * @function
     * @public
     * @param {AudioInputStream | PullAudioInputStreamCallback} audioStream - Specifies the custom audio input stream. Currently, only WAV / PCM with 16-bit samples, 16 kHz sample rate, and a single channel (Mono) is supported.
     * @returns {AudioConfig} The audio input configuration being created.
     */
    AudioConfig.fromStreamInput = function (audioStream) {
        if (audioStream instanceof Exports_2.PullAudioInputStreamCallback) {
            return new AudioConfigImpl(new AudioInputStream_1.PullAudioInputStreamImpl(audioStream));
        }
        if (audioStream instanceof Exports_2.AudioInputStream) {
            return new AudioConfigImpl(audioStream);
        }
        throw new Error("Not Supported Type");
    };
    return AudioConfig;
}());
exports.AudioConfig = AudioConfig;
/**
 * Represents audio input stream used for custom audio input configurations.
 * @private
 * @class AudioConfigImpl
 */
// tslint:disable-next-line:max-classes-per-file
var AudioConfigImpl = /** @class */ (function (_super) {
    __extends(AudioConfigImpl, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {IAudioSource} source - An audio source.
     */
    function AudioConfigImpl(source) {
        var _this = _super.call(this) || this;
        _this.source = source;
        return _this;
    }
    Object.defineProperty(AudioConfigImpl.prototype, "Format", {
        /**
         * Format information for the audio
         */
        get: function () {
            return this.source.Format;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * @member AudioConfigImpl.prototype.close
     * @function
     * @public
     */
    AudioConfigImpl.prototype.close = function () {
        this.source.TurnOff();
    };
    /**
     * @member AudioConfigImpl.prototype.Id
     * @function
     * @public
     */
    AudioConfigImpl.prototype.Id = function () {
        return this.source.Id();
    };
    /**
     * @member AudioConfigImpl.prototype.TurnOn
     * @function
     * @public
     * @returns {Promise<boolean>} A promise.
     */
    AudioConfigImpl.prototype.TurnOn = function () {
        return this.source.TurnOn();
    };
    /**
     * @member AudioConfigImpl.prototype.Attach
     * @function
     * @public
     * @param {string} audioNodeId - The audio node id.
     * @returns {Promise<IAudioStreamNode>} A promise.
     */
    AudioConfigImpl.prototype.Attach = function (audioNodeId) {
        return this.source.Attach(audioNodeId);
    };
    /**
     * @member AudioConfigImpl.prototype.Detach
     * @function
     * @public
     * @param {string} audioNodeId - The audio node id.
     */
    AudioConfigImpl.prototype.Detach = function (audioNodeId) {
        return this.Detach(audioNodeId);
    };
    /**
     * @member AudioConfigImpl.prototype.TurnOff
     * @function
     * @public
     * @returns {Promise<boolean>} A promise.
     */
    AudioConfigImpl.prototype.TurnOff = function () {
        return this.source.TurnOff();
    };
    Object.defineProperty(AudioConfigImpl.prototype, "Events", {
        /**
         * @member AudioConfigImpl.prototype.Events
         * @function
         * @public
         * @returns {EventSource<AudioSourceEvent>} An event source for audio events.
         */
        get: function () {
            return this.source.Events;
        },
        enumerable: true,
        configurable: true
    });
    return AudioConfigImpl;
}(AudioConfig));
exports.AudioConfigImpl = AudioConfigImpl;



/***/ }),
/* 36 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Defines the possible reasons a recognition result might be canceled.
 * @class CancellationReason
 */
var CancellationReason;
(function (CancellationReason) {
    /**
     * Indicates that an error occurred during speech recognition. Use getErrorDetails() contains detailed error response.
     * @member CancellationReason.Error
     */
    CancellationReason[CancellationReason["Error"] = 0] = "Error";
    /**
     * Indicates that the end of the audio stream was reached.
     * @member CancellationReason.EndOfStream
     */
    CancellationReason[CancellationReason["EndOfStream"] = 1] = "EndOfStream";
})(CancellationReason = exports.CancellationReason || (exports.CancellationReason = {}));



/***/ }),
/* 37 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * An abstract base class that defines callback methods (read() and close()) for custom audio input streams).
 * @class PullAudioInputStreamCallback
 */
var PullAudioInputStreamCallback = /** @class */ (function () {
    function PullAudioInputStreamCallback() {
    }
    return PullAudioInputStreamCallback;
}());
exports.PullAudioInputStreamCallback = PullAudioInputStreamCallback;



/***/ }),
/* 38 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var Contracts_1 = __webpack_require__(4);
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
/**
 * Represents a keyword recognition model for recognizing when
 * the user says a keyword to initiate further speech recognition.
 * @class KeywordRecognitionModel
 */
var KeywordRecognitionModel = /** @class */ (function () {
    /**
     * Create and initializes a new instance.
     * @constructor
     */
    function KeywordRecognitionModel() {
        this.disposed = false;
    }
    /**
     * Creates a keyword recognition model using the specified filename.
     * @member KeywordRecognitionModel.fromFile
     * @function
     * @public
     * @param {string} fileName - A string that represents file name for the keyword recognition model.
     *                 Note, the file can point to a zip file in which case the model will be extracted from the zip.
     * @returns {KeywordRecognitionModel} The keyword recognition model being created.
     */
    KeywordRecognitionModel.fromFile = function (fileName) {
        Contracts_1.Contracts.throwIfFileDoesNotExist(fileName, "fileName");
        throw new Error("Not yet implemented.");
    };
    /**
     * Creates a keyword recognition model using the specified filename.
     * @member KeywordRecognitionModel.fromStream
     * @function
     * @public
     * @param {string} file - A File that represents file for the keyword recognition model.
     *                 Note, the file can point to a zip file in which case the model will be extracted from the zip.
     * @returns {KeywordRecognitionModel} The keyword recognition model being created.
     */
    KeywordRecognitionModel.fromStream = function (file) {
        Contracts_1.Contracts.throwIfNull(file, "file");
        throw new Error("Not yet implemented.");
    };
    /**
     * Dispose of associated resources.
     * @member KeywordRecognitionModel.prototype.close
     * @function
     * @public
     */
    KeywordRecognitionModel.prototype.close = function () {
        if (this.disposed) {
            return;
        }
        this.disposed = true;
    };
    return KeywordRecognitionModel;
}());
exports.KeywordRecognitionModel = KeywordRecognitionModel;



/***/ }),
/* 39 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Defines content for session events like SessionStarted/Stopped, SoundStarted/Stopped.
 * @class SessionEventArgs
 */
var SessionEventArgs = /** @class */ (function () {
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {string} sessionId - The session id.
     */
    function SessionEventArgs(sessionId) {
        this.privSessionId = sessionId;
    }
    Object.defineProperty(SessionEventArgs.prototype, "sessionId", {
        /**
         * Represents the session identifier.
         * @member SessionEventArgs.prototype.sessionId
         * @function
         * @public
         * @returns {string} Represents the session identifier.
         */
        get: function () {
            return this.privSessionId;
        },
        enumerable: true,
        configurable: true
    });
    return SessionEventArgs;
}());
exports.SessionEventArgs = SessionEventArgs;



/***/ }),
/* 40 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
/**
 * Defines payload for session events like Speech Start/End Detected
 * @class
 */
var RecognitionEventArgs = /** @class */ (function (_super) {
    __extends(RecognitionEventArgs, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {number} offset - The offset.
     * @param {string} sessionId - The session id.
     */
    function RecognitionEventArgs(offset, sessionId) {
        var _this = _super.call(this, sessionId) || this;
        _this.privOffset = offset;
        return _this;
    }
    Object.defineProperty(RecognitionEventArgs.prototype, "offset", {
        /**
         * Represents the message offset
         * @member RecognitionEventArgs.prototype.offset
         * @function
         * @public
         */
        get: function () {
            return this.privOffset;
        },
        enumerable: true,
        configurable: true
    });
    return RecognitionEventArgs;
}(Exports_1.SessionEventArgs));
exports.RecognitionEventArgs = RecognitionEventArgs;



/***/ }),
/* 41 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Define Speech Recognizer output formats.
 * @class OutputFormat
 */
var OutputFormat;
(function (OutputFormat) {
    /**
     * @member OutputFormat.Simple
     */
    OutputFormat[OutputFormat["Simple"] = 0] = "Simple";
    /**
     * @member OutputFormat.Detailed
     */
    OutputFormat[OutputFormat["Detailed"] = 1] = "Detailed";
})(OutputFormat = exports.OutputFormat || (exports.OutputFormat = {}));



/***/ }),
/* 42 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(0);
/**
 * Intent recognition result event arguments.
 * @class
 */
var IntentRecognitionEventArgs = /** @class */ (function (_super) {
    __extends(IntentRecognitionEventArgs, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param result - The result of the intent recognition.
     * @param offset - The offset.
     * @param sessionId - The session id.
     */
    function IntentRecognitionEventArgs(result, offset, sessionId) {
        var _this = _super.call(this, offset, sessionId) || this;
        _this.privResult = result;
        return _this;
    }
    Object.defineProperty(IntentRecognitionEventArgs.prototype, "result", {
        /**
         * Represents the intent recognition result.
         * @member IntentRecognitionEventArgs.prototype.result
         * @function
         * @public
         * @returns {IntentRecognitionResult} Represents the intent recognition result.
         */
        get: function () {
            return this.privResult;
        },
        enumerable: true,
        configurable: true
    });
    return IntentRecognitionEventArgs;
}(Exports_1.RecognitionEventArgs));
exports.IntentRecognitionEventArgs = IntentRecognitionEventArgs;



/***/ }),
/* 43 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Defines result of speech recognition.
 * @class RecognitionResult
 */
var RecognitionResult = /** @class */ (function () {
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {string} resultId - The result id.
     * @param {ResultReason} reason - The reason.
     * @param {string} text - The recognized text.
     * @param {number} duration - The duration.
     * @param {number} offset - The offset into the stream.
     * @param {string} errorDetails - Error details, if provided.
     * @param {string} json - Additional Json, if provided.
     * @param {PropertyCollection} properties - Additional properties, if provided.
     */
    function RecognitionResult(resultId, reason, text, duration, offset, errorDetails, json, properties) {
        this.privResultId = resultId;
        this.privReason = reason;
        this.privText = text;
        this.privDuration = duration;
        this.privOffset = offset;
        this.privErrorDetails = errorDetails;
        this.privJson = json;
        this.privProperties = properties;
    }
    Object.defineProperty(RecognitionResult.prototype, "resultId", {
        /**
         * Specifies the result identifier.
         * @member RecognitionResult.prototype.resultId
         * @function
         * @public
         * @returns {string} Specifies the result identifier.
         */
        get: function () {
            return this.privResultId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionResult.prototype, "reason", {
        /**
         * Specifies status of the result.
         * @member RecognitionResult.prototype.reason
         * @function
         * @public
         * @returns {ResultReason} Specifies status of the result.
         */
        get: function () {
            return this.privReason;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionResult.prototype, "text", {
        /**
         * Presents the recognized text in the result.
         * @member RecognitionResult.prototype.text
         * @function
         * @public
         * @returns {string} Presents the recognized text in the result.
         */
        get: function () {
            return this.privText;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionResult.prototype, "duration", {
        /**
         * Duration of recognized speech in 100 nano second incements.
         * @member RecognitionResult.prototype.duration
         * @function
         * @public
         * @returns {number} Duration of recognized speech in 100 nano second incements.
         */
        get: function () {
            return this.privDuration;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionResult.prototype, "offset", {
        /**
         * Offset of recognized speech in 100 nano second incements.
         * @member RecognitionResult.prototype.offset
         * @function
         * @public
         * @returns {number} Offset of recognized speech in 100 nano second incements.
         */
        get: function () {
            return this.privOffset;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionResult.prototype, "errorDetails", {
        /**
         * In case of an unsuccessful recognition, provides a brief description of an occurred error.
         * This field is only filled-out if the recognition status (@see RecognitionStatus) is set to Canceled.
         * @member RecognitionResult.prototype.errorDetails
         * @function
         * @public
         * @returns {string} a brief description of an error.
         */
        get: function () {
            return this.privErrorDetails;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionResult.prototype, "json", {
        /**
         * A string containing Json serialized recognition result as it was received from the service.
         * @member RecognitionResult.prototype.json
         * @function
         * @private
         * @returns {string} Json serialized representation of the result.
         */
        get: function () {
            return this.privJson;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognitionResult.prototype, "properties", {
        /**
         *  The set of properties exposed in the result.
         * @member RecognitionResult.prototype.properties
         * @function
         * @public
         * @returns {PropertyCollection} The set of properties exposed in the result.
         */
        get: function () {
            return this.privProperties;
        },
        enumerable: true,
        configurable: true
    });
    return RecognitionResult;
}());
exports.RecognitionResult = RecognitionResult;



/***/ }),
/* 44 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(0);
/**
 * Defines result of speech recognition.
 * @class SpeechRecognitionResult
 */
var SpeechRecognitionResult = /** @class */ (function (_super) {
    __extends(SpeechRecognitionResult, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @public
     * @param {string} resultId - The result id.
     * @param {ResultReason} reason - The reason.
     * @param {string} text - The recognized text.
     * @param {number} duration - The duration.
     * @param {number} offset - The offset into the stream.
     * @param {string} errorDetails - Error details, if provided.
     * @param {string} json - Additional Json, if provided.
     * @param {PropertyCollection} properties - Additional properties, if provided.
     */
    function SpeechRecognitionResult(resultId, reason, text, duration, offset, errorDetails, json, properties) {
        return _super.call(this, resultId, reason, text, duration, offset, errorDetails, json, properties) || this;
    }
    return SpeechRecognitionResult;
}(Exports_1.RecognitionResult));
exports.SpeechRecognitionResult = SpeechRecognitionResult;



/***/ }),
/* 45 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
/**
 * Intent recognition result.
 * @class
 */
var IntentRecognitionResult = /** @class */ (function (_super) {
    __extends(IntentRecognitionResult, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param intentId - The intent id.
     * @param resultId - The result id.
     * @param reason - The reason.
     * @param text - The recognized text.
     * @param duration - The duration.
     * @param offset - The offset into the stream.
     * @param errorDetails - Error details, if provided.
     * @param json - Additional Json, if provided.
     * @param properties - Additional properties, if provided.
     */
    function IntentRecognitionResult(intentId, resultId, reason, text, duration, offset, errorDetails, json, properties) {
        var _this = _super.call(this, resultId, reason, text, duration, offset, errorDetails, json, properties) || this;
        _this.privIntentId = intentId;
        return _this;
    }
    Object.defineProperty(IntentRecognitionResult.prototype, "intentId", {
        /**
         * A String that represents the intent identifier being recognized.
         * @member IntentRecognitionResult.prototype.intentId
         * @function
         * @public
         * @returns {string} A String that represents the intent identifier being recognized.
         */
        get: function () {
            return this.privIntentId;
        },
        enumerable: true,
        configurable: true
    });
    return IntentRecognitionResult;
}(Exports_1.SpeechRecognitionResult));
exports.IntentRecognitionResult = IntentRecognitionResult;



/***/ }),
/* 46 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Contracts_1 = __webpack_require__(4);
/**
 * Language understanding model
 * @class LanguageUnderstandingModel
 */
var LanguageUnderstandingModel = /** @class */ (function () {
    /**
     * Creates and initializes a new instance
     * @constructor
     */
    function LanguageUnderstandingModel() {
    }
    /**
     * Creates an language understanding model using the specified endpoint.
     * @member LanguageUnderstandingModel.fromEndpoint
     * @function
     * @public
     * @param {URL} uri - A String that represents the endpoint of the language understanding model.
     * @returns {LanguageUnderstandingModel} The language understanding model being created.
     */
    LanguageUnderstandingModel.fromEndpoint = function (uri) {
        Contracts_1.Contracts.throwIfNull(uri, "uri");
        Contracts_1.Contracts.throwIfNullOrWhitespace(uri.hostname, "uri");
        var langModelImp = new LanguageUnderstandingModelImpl();
        // Need to extract the app ID from the URL.
        // URL is in the format: https://<region>.api.cognitive.microsoft.com/luis/v2.0/apps/<Guid>?subscription-key=<key>&timezoneOffset=-360
        // Start tearing the string apart.
        // region can be extracted from the host name.
        var firstDot = uri.host.indexOf(".");
        if (-1 === firstDot) {
            throw new Error("Could not determine region from endpoint");
        }
        langModelImp.region = uri.host.substr(0, firstDot);
        // Now the app ID.
        var lastSegment = uri.pathname.lastIndexOf("/") + 1;
        if (-1 === lastSegment) {
            throw new Error("Could not determine appId from endpoint");
        }
        langModelImp.appId = uri.pathname.substr(lastSegment);
        // And finally the key.
        langModelImp.subscriptionKey = uri.searchParams.get("subscription-key");
        if (undefined === langModelImp.subscriptionKey) {
            throw new Error("Could not determine subscription key from endpoint");
        }
        return langModelImp;
    };
    /**
     * Creates an language understanding model using the application id of Language Understanding service.
     * @member LanguageUnderstandingModel.fromAppId
     * @function
     * @public
     * @param {string} appId - A String that represents the application id of Language Understanding service.
     * @returns {LanguageUnderstandingModel} The language understanding model being created.
     */
    LanguageUnderstandingModel.fromAppId = function (appId) {
        Contracts_1.Contracts.throwIfNullOrWhitespace(appId, "appId");
        var langModelImp = new LanguageUnderstandingModelImpl();
        langModelImp.appId = appId;
        return langModelImp;
    };
    /**
     * Creates a language understanding model using hostname, subscription key and application id of Language Understanding service.
     * @member LanguageUnderstandingModel.fromSubscription
     * @function
     * @public
     * @param {string} subscriptionKey - A String that represents the subscription key of Language Understanding service.
     * @param {string} appId - A String that represents the application id of Language Understanding service.
     * @param {LanguageUnderstandingModel} region - A String that represents the region of the Language Understanding service (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns {LanguageUnderstandingModel} The language understanding model being created.
     */
    LanguageUnderstandingModel.fromSubscription = function (subscriptionKey, appId, region) {
        Contracts_1.Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        Contracts_1.Contracts.throwIfNullOrWhitespace(appId, "appId");
        Contracts_1.Contracts.throwIfNullOrWhitespace(region, "region");
        var langModelImp = new LanguageUnderstandingModelImpl();
        langModelImp.appId = appId;
        langModelImp.region = region;
        langModelImp.subscriptionKey = subscriptionKey;
        return langModelImp;
    };
    return LanguageUnderstandingModel;
}());
exports.LanguageUnderstandingModel = LanguageUnderstandingModel;
/**
 * @private
 * @class LanguageUnderstandingModelImpl
 */
// tslint:disable-next-line:max-classes-per-file
var LanguageUnderstandingModelImpl = /** @class */ (function (_super) {
    __extends(LanguageUnderstandingModelImpl, _super);
    function LanguageUnderstandingModelImpl() {
        return _super !== null && _super.apply(this, arguments) || this;
    }
    return LanguageUnderstandingModelImpl;
}(LanguageUnderstandingModel));
exports.LanguageUnderstandingModelImpl = LanguageUnderstandingModelImpl;



/***/ }),
/* 47 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(0);
/**
 * Defines contents of speech recognizing/recognized event.
 * @class SpeechRecognitionEventArgs
 */
var SpeechRecognitionEventArgs = /** @class */ (function (_super) {
    __extends(SpeechRecognitionEventArgs, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {SpeechRecognitionResult} result - The speech recognition result.
     * @param {number} offset - The offset.
     * @param {string} sessionId - The session id.
     */
    function SpeechRecognitionEventArgs(result, offset, sessionId) {
        var _this = _super.call(this, offset, sessionId) || this;
        _this.privResult = result;
        return _this;
    }
    Object.defineProperty(SpeechRecognitionEventArgs.prototype, "result", {
        /**
         * Specifies the recognition result.
         * @member SpeechRecognitionEventArgs.prototype.result
         * @function
         * @public
         * @returns {SpeechRecognitionResult} the recognition result.
         */
        get: function () {
            return this.privResult;
        },
        enumerable: true,
        configurable: true
    });
    return SpeechRecognitionEventArgs;
}(Exports_1.RecognitionEventArgs));
exports.SpeechRecognitionEventArgs = SpeechRecognitionEventArgs;



/***/ }),
/* 48 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
/**
 * Defines content of a RecognitionErrorEvent.
 * @class SpeechRecognitionCanceledEventArgs
 */
var SpeechRecognitionCanceledEventArgs = /** @class */ (function (_super) {
    __extends(SpeechRecognitionCanceledEventArgs, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {CancellationReason} reason - The cancellation reason.
     * @param {string} errorDetails - Error details, if provided.
     * @param {number} offset - The offset.
     * @param {string} sessionId - The session id.
     */
    function SpeechRecognitionCanceledEventArgs(reason, errorDetails, offset, sessionId) {
        var _this = _super.call(this, offset, sessionId) || this;
        _this.privReason = reason;
        _this.privErrorDetails = errorDetails;
        return _this;
    }
    Object.defineProperty(SpeechRecognitionCanceledEventArgs.prototype, "reason", {
        /**
         * The reason the recognition was canceled.
         * @member SpeechRecognitionCanceledEventArgs.prototype.reason
         * @function
         * @public
         * @returns {CancellationReason} Specifies the reason canceled.
         */
        get: function () {
            return this.privReason;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechRecognitionCanceledEventArgs.prototype, "errorDetails", {
        /**
         * In case of an unsuccessful recognition, provides a details of why the occurred error.
         * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
         * @member SpeechRecognitionCanceledEventArgs.prototype.errorDetails
         * @function
         * @public
         * @returns {string} A String that represents the error details.
         */
        get: function () {
            return this.privErrorDetails;
        },
        enumerable: true,
        configurable: true
    });
    return SpeechRecognitionCanceledEventArgs;
}(Exports_1.RecognitionEventArgs));
exports.SpeechRecognitionCanceledEventArgs = SpeechRecognitionCanceledEventArgs;



/***/ }),
/* 49 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(0);
/**
 * Translation text result event arguments.
 * @class TranslationRecognitionEventArgs
 */
var TranslationRecognitionEventArgs = /** @class */ (function (_super) {
    __extends(TranslationRecognitionEventArgs, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {TranslationRecognitionResult} result - The translation recognition result.
     * @param {number} offset - The offset.
     * @param {string} sessionId - The session id.
     */
    function TranslationRecognitionEventArgs(result, offset, sessionId) {
        var _this = _super.call(this, offset, sessionId) || this;
        _this.privResult = result;
        return _this;
    }
    Object.defineProperty(TranslationRecognitionEventArgs.prototype, "result", {
        /**
         * Specifies the recognition result.
         * @member TranslationRecognitionEventArgs.prototype.result
         * @function
         * @public
         * @returns {TranslationRecognitionResult} the recognition result.
         */
        get: function () {
            return this.privResult;
        },
        enumerable: true,
        configurable: true
    });
    return TranslationRecognitionEventArgs;
}(Exports_1.RecognitionEventArgs));
exports.TranslationRecognitionEventArgs = TranslationRecognitionEventArgs;



/***/ }),
/* 50 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(0);
/**
 * Translation Synthesis event arguments
 * @class TranslationSynthesisEventArgs
 */
var TranslationSynthesisEventArgs = /** @class */ (function (_super) {
    __extends(TranslationSynthesisEventArgs, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {TranslationSynthesisResult} result - The translation synthesis result.
     * @param {string} sessionId - The session id.
     */
    function TranslationSynthesisEventArgs(result, sessionId) {
        var _this = _super.call(this, sessionId) || this;
        _this.privResult = result;
        return _this;
    }
    Object.defineProperty(TranslationSynthesisEventArgs.prototype, "result", {
        /**
         * Specifies the translation synthesis result.
         * @member TranslationSynthesisEventArgs.prototype.result
         * @function
         * @public
         * @returns {TranslationSynthesisResult} Specifies the translation synthesis result.
         */
        get: function () {
            return this.privResult;
        },
        enumerable: true,
        configurable: true
    });
    return TranslationSynthesisEventArgs;
}(Exports_1.SessionEventArgs));
exports.TranslationSynthesisEventArgs = TranslationSynthesisEventArgs;



/***/ }),
/* 51 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
/**
 * Translation text result.
 * @class TranslationRecognitionResult
 */
var TranslationRecognitionResult = /** @class */ (function (_super) {
    __extends(TranslationRecognitionResult, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {Translations} translations - The translations.
     * @param {string} resultId - The result id.
     * @param {ResultReason} reason - The reason.
     * @param {string} text - The recognized text.
     * @param {number} duration - The duration.
     * @param {number} offset - The offset into the stream.
     * @param {string} errorDetails - Error details, if provided.
     * @param {string} json - Additional Json, if provided.
     * @param {PropertyCollection} properties - Additional properties, if provided.
     */
    function TranslationRecognitionResult(translations, resultId, reason, text, duration, offset, errorDetails, json, properties) {
        var _this = _super.call(this, resultId, reason, text, duration, offset, errorDetails, json, properties) || this;
        _this.privTranslations = translations;
        return _this;
    }
    Object.defineProperty(TranslationRecognitionResult.prototype, "translations", {
        /**
         * Presents the translation results. Each item in the dictionary represents a translation result in one of target languages, where the key
         * is the name of the target language, in BCP-47 format, and the value is the translation text in the specified language.
         * @member TranslationRecognitionResult.prototype.translations
         * @function
         * @public
         * @returns {Translations} the current translation map that holds all translations requested.
         */
        get: function () {
            return this.privTranslations;
        },
        enumerable: true,
        configurable: true
    });
    return TranslationRecognitionResult;
}(Exports_1.SpeechRecognitionResult));
exports.TranslationRecognitionResult = TranslationRecognitionResult;



/***/ }),
/* 52 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
/**
 * Defines translation synthesis result, i.e. the voice output of the translated text in the target language.
 * @class TranslationSynthesisResult
 */
var TranslationSynthesisResult = /** @class */ (function () {
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {ResultReason} reason - The synthesis reason.
     * @param {ArrayBuffer} audio - The audio data.
     */
    function TranslationSynthesisResult(reason, audio) {
        this.privReason = reason;
        this.privAudio = audio;
    }
    Object.defineProperty(TranslationSynthesisResult.prototype, "audio", {
        /**
         * Translated text in the target language.
         * @member TranslationSynthesisResult.prototype.audio
         * @function
         * @public
         * @returns {ArrayBuffer} Translated audio in the target language.
         */
        get: function () {
            return this.privAudio;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(TranslationSynthesisResult.prototype, "reason", {
        /**
         * The synthesis status.
         * @member TranslationSynthesisResult.prototype.reason
         * @function
         * @public
         * @returns {ResultReason} The synthesis status.
         */
        get: function () {
            return this.privReason;
        },
        enumerable: true,
        configurable: true
    });
    return TranslationSynthesisResult;
}());
exports.TranslationSynthesisResult = TranslationSynthesisResult;



/***/ }),
/* 53 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Defines the possible reasons a recognition result might be generated.
 * @class ResultReason
 */
var ResultReason;
(function (ResultReason) {
    /**
     * Indicates speech could not be recognized. More details can be found in the NoMatchDetails object.
     * @member ResultReason.NoMatch
     */
    ResultReason[ResultReason["NoMatch"] = 0] = "NoMatch";
    /**
     * Indicates that the recognition was canceled. More details can be found using the CancellationDetails object.
     * @member ResultReason.Canceled
     */
    ResultReason[ResultReason["Canceled"] = 1] = "Canceled";
    /**
     * Indicates the speech result contains hypothesis text.
     * @member ResultReason.RecognizedSpeech
     */
    ResultReason[ResultReason["RecognizingSpeech"] = 2] = "RecognizingSpeech";
    /**
     * Indicates the speech result contains final text that has been recognized.
     * Speech Recognition is now complete for this phrase.
     * @member ResultReason.RecognizedSpeech
     */
    ResultReason[ResultReason["RecognizedSpeech"] = 3] = "RecognizedSpeech";
    /**
     * Indicates the intent result contains hypothesis text and intent.
     * @member ResultReason.RecognizingIntent
     */
    ResultReason[ResultReason["RecognizingIntent"] = 4] = "RecognizingIntent";
    /**
     * Indicates the intent result contains final text and intent.
     * Speech Recognition and Intent determination are now complete for this phrase.
     * @member ResultReason.RecognizedIntent
     */
    ResultReason[ResultReason["RecognizedIntent"] = 5] = "RecognizedIntent";
    /**
     * Indicates the translation result contains hypothesis text and its translation(s).
     * @member ResultReason.TranslatingSpeech
     */
    ResultReason[ResultReason["TranslatingSpeech"] = 6] = "TranslatingSpeech";
    /**
     * Indicates the translation result contains final text and corresponding translation(s).
     * Speech Recognition and Translation are now complete for this phrase.
     * @member ResultReason.TranslatedSpeech
     */
    ResultReason[ResultReason["TranslatedSpeech"] = 7] = "TranslatedSpeech";
    /**
     * Indicates the synthesized audio result contains a non-zero amount of audio data
     * @member ResultReason.SynthesizingAudio
     */
    ResultReason[ResultReason["SynthesizingAudio"] = 8] = "SynthesizingAudio";
    /**
     * Indicates the synthesized audio is now complete for this phrase.
     * @member ResultReason.SynthesizingAudioCompleted
     */
    ResultReason[ResultReason["SynthesizingAudioCompleted"] = 9] = "SynthesizingAudioCompleted";
})(ResultReason = exports.ResultReason || (exports.ResultReason = {}));



/***/ }),
/* 54 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(2);
var Contracts_1 = __webpack_require__(4);
var Exports_2 = __webpack_require__(0);
/**
 * Speech configuration.
 * @class SpeechConfig
 */
var SpeechConfig = /** @class */ (function () {
    /**
     * Creates and initializes an instance.
     * @constructor
     */
    function SpeechConfig() {
    }
    /**
     * Static instance of SpeechConfig returned by passing subscriptionKey and service region.
     * @member SpeechConfig.fromSubscription
     * @function
     * @public
     * @param {string} subscriptionKey - The subscription key.
     * @param {string} region - The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns {SpeechConfig} The speech factory
     */
    SpeechConfig.fromSubscription = function (subscriptionKey, region) {
        Contracts_1.Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        Contracts_1.Contracts.throwIfNullOrWhitespace(region, "region");
        var speechImpl = new SpeechConfigImpl();
        speechImpl.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Region, region);
        speechImpl.setProperty(Exports_2.PropertyId.SpeechServiceConnection_IntentRegion, region);
        speechImpl.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Key, subscriptionKey);
        return speechImpl;
    };
    /**
     * Creates an instance of the speech factory with specified endpoint and subscription key.
     * This method is intended only for users who use a non-standard service endpoint or paramters.
     * the language setting in uri takes precedence, and the effective language is "de-DE".
     * @member SpeechConfig.fromEndpoint
     * @function
     * @public
     * @param {URL} endpoint - The service endpoint to connect to.
     * @param {string} subscriptionKey - The subscription key.
     * @returns {SpeechConfig} A speech factory instance.
     */
    SpeechConfig.fromEndpoint = function (endpoint, subscriptionKey) {
        Contracts_1.Contracts.throwIfNull(endpoint, "endpoint");
        Contracts_1.Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        var speechImpl = new SpeechConfigImpl();
        speechImpl.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Endpoint, endpoint.href);
        speechImpl.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Key, subscriptionKey);
        return speechImpl;
    };
    /**
     * Creates an instance of the speech factory with specified initial authorization token and region.
     * @member SpeechConfig.fromAuthorizationToken
     * @function
     * @public
     * @param {string} authorizationToken - The initial authorization token.
     * @param {string} region - The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns {SpeechConfig} A speech factory instance.
     */
    SpeechConfig.fromAuthorizationToken = function (authorizationToken, region) {
        Contracts_1.Contracts.throwIfNull(authorizationToken, "authorizationToken");
        Contracts_1.Contracts.throwIfNullOrWhitespace(region, "region");
        var speechImpl = new SpeechConfigImpl();
        speechImpl.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Region, region);
        speechImpl.setProperty(Exports_2.PropertyId.SpeechServiceConnection_IntentRegion, region);
        speechImpl.authorizationToken = authorizationToken;
        return speechImpl;
    };
    /**
     * Closes the configuration.
     * @member SpeechConfig.prototype.close
     * @function
     * @public
     */
    /* tslint:disable:no-empty */
    SpeechConfig.prototype.close = function () { };
    return SpeechConfig;
}());
exports.SpeechConfig = SpeechConfig;
/**
 * @private
 * @class SpeechConfigImpl
 */
// tslint:disable-next-line:max-classes-per-file
var SpeechConfigImpl = /** @class */ (function (_super) {
    __extends(SpeechConfigImpl, _super);
    function SpeechConfigImpl() {
        var _this = _super.call(this) || this;
        _this.privProperties = new Exports_2.PropertyCollection();
        _this.speechRecognitionLanguage = "en-US"; // Should we have a default?
        _this.outputFormat = Exports_2.OutputFormat.Simple;
        return _this;
    }
    Object.defineProperty(SpeechConfigImpl.prototype, "properties", {
        get: function () {
            return this.privProperties;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConfigImpl.prototype, "endPoint", {
        get: function () {
            return new URL(this.privProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_Endpoint));
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConfigImpl.prototype, "subscriptionKey", {
        get: function () {
            return this.privProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_Key);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConfigImpl.prototype, "region", {
        get: function () {
            return this.privProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_Region);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConfigImpl.prototype, "authorizationToken", {
        get: function () {
            return this.privProperties.getProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token);
        },
        set: function (value) {
            this.privProperties.setProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token, value);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConfigImpl.prototype, "speechRecognitionLanguage", {
        get: function () {
            return this.privProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage);
        },
        set: function (value) {
            this.privProperties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage, value);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConfigImpl.prototype, "outputFormat", {
        get: function () {
            return Exports_2.OutputFormat[this.privProperties.getProperty(Exports_1.OutputFormatPropertyName, Exports_2.OutputFormat[Exports_2.OutputFormat.Simple])];
        },
        set: function (value) {
            this.privProperties.setProperty(Exports_1.OutputFormatPropertyName, Exports_2.OutputFormat[value]);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConfigImpl.prototype, "endpointId", {
        get: function () {
            return this.privProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_EndpointId);
        },
        set: function (value) {
            this.privProperties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_EndpointId, value);
        },
        enumerable: true,
        configurable: true
    });
    SpeechConfigImpl.prototype.setProperty = function (name, value) {
        Contracts_1.Contracts.throwIfNullOrWhitespace(value, "value");
        this.privProperties.setProperty(name, value);
    };
    SpeechConfigImpl.prototype.getProperty = function (name, def) {
        return this.privProperties.getProperty(name, def);
    };
    SpeechConfigImpl.prototype.clone = function () {
        var ret = new SpeechConfigImpl();
        ret.privProperties = this.privProperties.clone();
        return ret;
    };
    return SpeechConfigImpl;
}(SpeechConfig));
exports.SpeechConfigImpl = SpeechConfigImpl;



/***/ }),
/* 55 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var IAuthentication_1 = __webpack_require__(9);
var AuthHeader = "Ocp-Apim-Subscription-Key";
/**
 * @class
 */
var CognitiveSubscriptionKeyAuthentication = /** @class */ (function () {
    /**
     * Creates and initializes an instance of the CognitiveSubscriptionKeyAuthentication class.
     * @constructor
     * @param {string} subscriptionKey - The subscription key
     */
    function CognitiveSubscriptionKeyAuthentication(subscriptionKey) {
        var _this = this;
        /**
         * Fetches the subscription key.
         * @member
         * @function
         * @public
         * @param {string} authFetchEventId - The id to fetch.
         */
        this.Fetch = function (authFetchEventId) {
            return Exports_1.PromiseHelper.FromResult(_this.authInfo);
        };
        /**
         * Fetches the subscription key.
         * @member
         * @function
         * @public
         * @param {string} authFetchEventId - The id to fetch.
         */
        this.FetchOnExpiry = function (authFetchEventId) {
            return Exports_1.PromiseHelper.FromResult(_this.authInfo);
        };
        if (!subscriptionKey) {
            throw new Exports_1.ArgumentNullError("subscriptionKey");
        }
        this.authInfo = new IAuthentication_1.AuthInfo(AuthHeader, subscriptionKey);
    }
    return CognitiveSubscriptionKeyAuthentication;
}());
exports.CognitiveSubscriptionKeyAuthentication = CognitiveSubscriptionKeyAuthentication;



/***/ }),
/* 56 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var IAuthentication_1 = __webpack_require__(9);
var AuthHeader = "Authorization";
var CognitiveTokenAuthentication = /** @class */ (function () {
    function CognitiveTokenAuthentication(fetchCallback, fetchOnExpiryCallback) {
        var _this = this;
        this.Fetch = function (authFetchEventId) {
            return _this.fetchCallback(authFetchEventId).OnSuccessContinueWith(function (token) { return new IAuthentication_1.AuthInfo(AuthHeader, token); });
        };
        this.FetchOnExpiry = function (authFetchEventId) {
            return _this.fetchOnExpiryCallback(authFetchEventId).OnSuccessContinueWith(function (token) { return new IAuthentication_1.AuthInfo(AuthHeader, token); });
        };
        if (!fetchCallback) {
            throw new Exports_1.ArgumentNullError("fetchCallback");
        }
        if (!fetchOnExpiryCallback) {
            throw new Exports_1.ArgumentNullError("fetchOnExpiryCallback");
        }
        this.fetchCallback = fetchCallback;
        this.fetchOnExpiryCallback = fetchOnExpiryCallback;
    }
    return CognitiveTokenAuthentication;
}());
exports.CognitiveTokenAuthentication = CognitiveTokenAuthentication;



/***/ }),
/* 57 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(6);
var Exports_2 = __webpack_require__(1);
var Exports_3 = __webpack_require__(0);
var Exports_4 = __webpack_require__(2);
var TestHooksParamName = "testhooks";
var ConnectionIdHeader = "X-ConnectionId";
var IntentConnectionFactory = /** @class */ (function () {
    function IntentConnectionFactory() {
        var _this = this;
        this.Create = function (config, authInfo, connectionId) {
            var endpoint = config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_Endpoint);
            if (!endpoint) {
                var region = config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_IntentRegion);
                endpoint = _this.Host() + Exports_2.Storage.Local.GetOrAdd("TranslationRelativeUri", "/speech/" + _this.GetSpeechRegionFromIntentRegion(region) + "/recognition/interactive/cognitiveservices/v1");
            }
            var queryParams = {
                format: "simple",
                language: config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_RecoLanguage),
            };
            if (_this.IsDebugModeEnabled) {
                queryParams[TestHooksParamName] = "1";
            }
            var headers = {};
            headers[authInfo.HeaderName] = authInfo.Token;
            headers[ConnectionIdHeader] = connectionId;
            return new Exports_1.WebsocketConnection(endpoint, queryParams, headers, new Exports_4.WebsocketMessageFormatter(), connectionId);
        };
    }
    IntentConnectionFactory.prototype.Host = function () {
        return Exports_2.Storage.Local.GetOrAdd("Host", "wss://speech.platform.bing.com");
    };
    Object.defineProperty(IntentConnectionFactory.prototype, "IsDebugModeEnabled", {
        get: function () {
            var value = Exports_2.Storage.Local.GetOrAdd("IsDebugModeEnabled", "false");
            return value.toLowerCase() === "true";
        },
        enumerable: true,
        configurable: true
    });
    IntentConnectionFactory.prototype.GetSpeechRegionFromIntentRegion = function (intentRegion) {
        switch (intentRegion) {
            case "West US":
            case "US West":
            case "westus":
                return "uswest";
            case "West US 2":
            case "US West 2":
            case "westus2":
                return "uswest2";
            case "South Central US":
            case "US South Central":
            case "southcentralus":
                return "ussouthcentral";
            case "West Central US":
            case "US West Central":
            case "westcentralus":
                return "uswestcentral";
            case "East US":
            case "US East":
            case "eastus":
                return "useast";
            case "East US 2":
            case "US East 2":
            case "eastus2":
                return "useast2";
            case "West Europe":
            case "Europe West":
            case "westeurope":
                return "europewest";
            case "North Europe":
            case "Europe North":
            case "northeurope":
                return "europenorth";
            case "Brazil South":
            case "South Brazil":
            case "southbrazil":
                return "brazilsouth";
            case "Australia East":
            case "East Australia":
            case "eastaustralia":
                return "australiaeast";
            case "Southeast Asia":
            case "Asia Southeast":
            case "southeastasia":
                return "asiasoutheast";
            case "East Asia":
            case "Asia East":
            case "eastasia":
                return "asiaeast";
            default:
                return intentRegion;
        }
    };
    return IntentConnectionFactory;
}());
exports.IntentConnectionFactory = IntentConnectionFactory;



/***/ }),
/* 58 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var Exports_2 = __webpack_require__(0);
var Exports_3 = __webpack_require__(2);
var RecognitionEvents_1 = __webpack_require__(7);
var ServiceTelemetryListener_Internal_1 = __webpack_require__(59);
var SpeechConnectionMessage_Internal_1 = __webpack_require__(60);
var ServiceRecognizerBase = /** @class */ (function () {
    function ServiceRecognizerBase(authentication, connectionFactory, audioSource, recognizerConfig) {
        var _this = this;
        this.Recognize = function (onEventCallback, speechContextJson) {
            var requestSession = _this.GetSession(_this.audioSource.Id(), onEventCallback);
            requestSession.ListenForServiceTelemetry(_this.audioSource.Events);
            return _this.audioSource
                .Attach(requestSession.AudioNodeId)
                .ContinueWithPromise(function (result) {
                if (result.IsError) {
                    requestSession.OnAudioSourceAttachCompleted(null, true, result.Error);
                    throw new Error(result.Error);
                }
                else {
                    requestSession.OnAudioSourceAttachCompleted(result.Result, false);
                }
                var audioNode = result.Result;
                return _this.FetchConnection(requestSession)
                    .OnSuccessContinueWithPromise(function (connection) {
                    var messageRetrievalPromise = _this.ReceiveMessage(connection, requestSession);
                    var messageSendPromise = _this.SendSpeechConfig(requestSession.RequestId, connection, _this.recognizerConfig.PlatformConfig.Serialize())
                        .OnSuccessContinueWithPromise(function (_) {
                        return _this.SendSpeechContext(requestSession.RequestId, connection, speechContextJson)
                            .OnSuccessContinueWithPromise(function (_) {
                            return _this.SendAudio(requestSession.RequestId, connection, audioNode, requestSession);
                        });
                    });
                    var completionPromise = Exports_1.PromiseHelper.WhenAll([messageRetrievalPromise, messageSendPromise]);
                    completionPromise.On(function (r) {
                        requestSession.Dispose();
                        _this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                    }, function (error) {
                        requestSession.Dispose(error);
                        _this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                    });
                    return completionPromise;
                }).OnSuccessContinueWithPromise(function (_) {
                    return requestSession.CompletionPromise;
                });
            });
        };
        this.SendTelemetryData = function (requestId, connection, telemetryData) {
            return connection
                .Send(new SpeechConnectionMessage_Internal_1.SpeechConnectionMessage(Exports_1.MessageType.Text, "telemetry", requestId, "application/json", telemetryData));
        };
        this.FetchConnection = function (requestSession, isUnAuthorized) {
            if (isUnAuthorized === void 0) { isUnAuthorized = false; }
            if (_this.connectionFetchPromise) {
                if (_this.connectionFetchPromise.Result().IsError
                    || _this.connectionFetchPromise.Result().Result.State() === Exports_1.ConnectionState.Disconnected) {
                    _this.connectionId = null;
                    _this.connectionFetchPromise = null;
                    return _this.FetchConnection(requestSession);
                }
                else {
                    requestSession.OnPreConnectionStart(_this.authFetchEventId, _this.connectionId);
                    requestSession.OnConnectionEstablishCompleted(200);
                    requestSession.ListenForServiceTelemetry(_this.connectionFetchPromise.Result().Result.Events);
                    return _this.connectionFetchPromise;
                }
            }
            _this.authFetchEventId = Exports_1.CreateNoDashGuid();
            _this.connectionId = Exports_1.CreateNoDashGuid();
            requestSession.OnPreConnectionStart(_this.authFetchEventId, _this.connectionId);
            var authPromise = isUnAuthorized ? _this.authentication.FetchOnExpiry(_this.authFetchEventId) : _this.authentication.Fetch(_this.authFetchEventId);
            _this.connectionFetchPromise = authPromise
                .ContinueWithPromise(function (result) {
                if (result.IsError) {
                    requestSession.OnAuthCompleted(true, result.Error);
                    throw new Error(result.Error);
                }
                else {
                    requestSession.OnAuthCompleted(false);
                }
                if (_this.connection) {
                    _this.connection.Dispose();
                    _this.connection = undefined;
                }
                _this.connection = _this.connectionFactory.Create(_this.recognizerConfig, result.Result, _this.connectionId);
                requestSession.ListenForServiceTelemetry(_this.connection.Events);
                return _this.connection.Open().OnSuccessContinueWithPromise(function (response) {
                    if (response.StatusCode === 200) {
                        requestSession.OnConnectionEstablishCompleted(response.StatusCode);
                        return Exports_1.PromiseHelper.FromResult(_this.connection);
                    }
                    else if (response.StatusCode === 403 && !isUnAuthorized) {
                        return _this.FetchConnection(requestSession, true);
                    }
                    else {
                        requestSession.OnConnectionEstablishCompleted(response.StatusCode, response.Reason);
                        return Exports_1.PromiseHelper.FromError("Unable to contact server. StatusCode: " + response.StatusCode + ", Reason: " + response.Reason);
                    }
                });
            });
            return _this.connectionFetchPromise;
        };
        this.ReceiveMessage = function (connection, requestSession) {
            return connection
                .Read()
                .OnSuccessContinueWithPromise(function (message) {
                // indicates we are draining the queue and it came with no message;
                if (!message) {
                    return Exports_1.PromiseHelper.FromResult(true);
                }
                var connectionMessage = SpeechConnectionMessage_Internal_1.SpeechConnectionMessage.FromConnectionMessage(message);
                if (connectionMessage.RequestId.toLowerCase() === requestSession.RequestId.toLowerCase()) {
                    switch (connectionMessage.Path.toLowerCase()) {
                        case "turn.start":
                            requestSession.OnServiceTurnStartResponse(JSON.parse(connectionMessage.TextBody));
                            break;
                        case "speech.startdetected":
                            requestSession.OnServiceSpeechStartDetectedResponse(JSON.parse(connectionMessage.TextBody));
                            break;
                        case "speech.fragment":
                            requestSession.OnServiceSpeechFragmentResponse(JSON.parse(connectionMessage.TextBody));
                            break;
                        case "speech.enddetected":
                            if (connectionMessage.TextBody.length > 0) {
                                requestSession.OnServiceSpeechEndDetectedResponse(JSON.parse(connectionMessage.TextBody));
                            }
                            else {
                                // If the request was empty, the JSON returned is empty.
                                requestSession.OnServiceSpeechEndDetectedResponse({ Offset: 0 });
                            }
                            break;
                        case "turn.end":
                            requestSession.OnServiceTurnEndResponse();
                            return Exports_1.PromiseHelper.FromResult(true);
                        default:
                            _this.ProcessTypeSpecificMessages(connectionMessage, requestSession, connection);
                    }
                }
                return _this.ReceiveMessage(connection, requestSession);
            });
        };
        this.SendSpeechConfig = function (requestId, connection, speechConfigJson) {
            if (speechConfigJson && _this.connectionId !== _this.speechConfigConnectionId) {
                _this.speechConfigConnectionId = _this.connectionId;
                return connection
                    .Send(new SpeechConnectionMessage_Internal_1.SpeechConnectionMessage(Exports_1.MessageType.Text, "speech.config", requestId, "application/json", speechConfigJson));
            }
            return Exports_1.PromiseHelper.FromResult(true);
        };
        this.SendSpeechContext = function (requestId, connection, speechContextJson) {
            if (speechContextJson) {
                return connection
                    .Send(new SpeechConnectionMessage_Internal_1.SpeechConnectionMessage(Exports_1.MessageType.Text, "speech.context", requestId, "application/json", speechContextJson));
            }
            return Exports_1.PromiseHelper.FromResult(true);
        };
        this.SendAudio = function (requestId, connection, audioStreamNode, requestSession) {
            // NOTE: Home-baked promises crash ios safari during the invocation
            // of the error callback chain (looks like the recursion is way too deep, and
            // it blows up the stack). The following construct is a stop-gap that does not
            // bubble the error up the callback chain and hence circumvents this problem.
            // TODO: rewrite with ES6 promises.
            var deferred = new Exports_1.Deferred();
            // The time we last sent data to the service.
            var lastSendTime = 0;
            var audioFormat = _this.audioSource.Format;
            var readAndUploadCycle = function (_) {
                // If speech is done, stop sending audio.
                if (!_this.isDisposed && !requestSession.IsSpeechEnded && !requestSession.IsCompleted) {
                    audioStreamNode.Read().On(function (audioStreamChunk) {
                        // we have a new audio chunk to upload.
                        if (requestSession.IsSpeechEnded) {
                            // If service already recognized audio end then dont send any more audio
                            deferred.Resolve(true);
                            return;
                        }
                        var payload = (audioStreamChunk.IsEnd) ? null : audioStreamChunk.Buffer;
                        var uploaded = connection.Send(new SpeechConnectionMessage_Internal_1.SpeechConnectionMessage(Exports_1.MessageType.Binary, "audio", requestId, null, payload));
                        if (!audioStreamChunk.IsEnd) {
                            // Caculate any delay to the audio stream needed. /2 to allow 2x real time transmit rate max.
                            var minSendTime = ((payload.byteLength / audioFormat.avgBytesPerSec) / 2) * 1000;
                            var delay_1 = Math.max(0, (lastSendTime - Date.now() + minSendTime));
                            uploaded.OnSuccessContinueWith(function (result) {
                                setTimeout(function () {
                                    lastSendTime = Date.now();
                                    readAndUploadCycle(result);
                                }, delay_1);
                            });
                        }
                        else {
                            // the audio stream has been closed, no need to schedule next
                            // read-upload cycle.
                            deferred.Resolve(true);
                        }
                    }, function (error) {
                        if (requestSession.IsSpeechEnded) {
                            // For whatever reason, Reject is used to remove queue subscribers inside
                            // the Queue.DrainAndDispose invoked from DetachAudioNode down blow, which
                            // means that sometimes things can be rejected in normal circumstances, without
                            // any errors.
                            deferred.Resolve(true); // TODO: remove the argument, it's is completely meaningless.
                        }
                        else {
                            // Only reject, if there was a proper error.
                            deferred.Reject(error);
                        }
                    });
                }
            };
            readAndUploadCycle(true);
            return deferred.Promise();
        };
        if (!authentication) {
            throw new Exports_1.ArgumentNullError("authentication");
        }
        if (!connectionFactory) {
            throw new Exports_1.ArgumentNullError("connectionFactory");
        }
        if (!audioSource) {
            throw new Exports_1.ArgumentNullError("audioSource");
        }
        if (!recognizerConfig) {
            throw new Exports_1.ArgumentNullError("recognizerConfig");
        }
        this.authentication = authentication;
        this.connectionFactory = connectionFactory;
        this.audioSource = audioSource;
        this.recognizerConfig = recognizerConfig;
        this.isDisposed = false;
    }
    Object.defineProperty(ServiceRecognizerBase.prototype, "AudioSource", {
        get: function () {
            return this.audioSource;
        },
        enumerable: true,
        configurable: true
    });
    ServiceRecognizerBase.prototype.IsDisposed = function () {
        return this.isDisposed;
    };
    ServiceRecognizerBase.prototype.Dispose = function (reason) {
        this.isDisposed = true;
        if (this.connection) {
            this.connection.Dispose(reason);
            this.connection = undefined;
        }
    };
    return ServiceRecognizerBase;
}());
exports.ServiceRecognizerBase = ServiceRecognizerBase;
// tslint:disable-next-line:max-classes-per-file
var SpeechServiceRecognizer = /** @class */ (function (_super) {
    __extends(SpeechServiceRecognizer, _super);
    function SpeechServiceRecognizer(authentication, connectionFactory, audioSource, recognizerConfig) {
        return _super.call(this, authentication, connectionFactory, audioSource, recognizerConfig) || this;
    }
    SpeechServiceRecognizer.prototype.GetSession = function (audioSourceId, onEventCallback) {
        return new SpeechRequestSession(audioSourceId, onEventCallback);
    };
    SpeechServiceRecognizer.prototype.ProcessTypeSpecificMessages = function (connectionMessage, session, connection) {
        var requestSession = session;
        switch (connectionMessage.Path.toLowerCase()) {
            case "speech.hypothesis":
                requestSession.OnServiceSpeechHypothesisResponse(JSON.parse(connectionMessage.TextBody));
                break;
            case "speech.phrase":
                if (this.recognizerConfig.IsContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                }
                if (this.recognizerConfig.parameters.getProperty(Exports_3.OutputFormatPropertyName) === Exports_2.OutputFormat[Exports_2.OutputFormat.Simple]) {
                    requestSession.OnServiceSimpleSpeechPhraseResponse(JSON.parse(connectionMessage.TextBody));
                }
                else {
                    requestSession.OnServiceDetailedSpeechPhraseResponse(JSON.parse(connectionMessage.TextBody));
                }
                break;
            default:
                break;
        }
    };
    return SpeechServiceRecognizer;
}(ServiceRecognizerBase));
exports.SpeechServiceRecognizer = SpeechServiceRecognizer;
/**
 * @class SynthesisStatus
 * @private
 */
var SynthesisStatus;
(function (SynthesisStatus) {
    /**
     * The response contains valid audio data.
     * @member SynthesisStatus.Success
     */
    SynthesisStatus[SynthesisStatus["Success"] = 0] = "Success";
    /**
     * Indicates the end of audio data. No valid audio data is included in the message.
     * @member SynthesisStatus.SynthesisEnd
     */
    SynthesisStatus[SynthesisStatus["SynthesisEnd"] = 1] = "SynthesisEnd";
    /**
     * Indicates an error occurred during synthesis data processing.
     * @member SynthesisStatus.Error
     */
    SynthesisStatus[SynthesisStatus["Error"] = 2] = "Error";
})(SynthesisStatus || (SynthesisStatus = {}));
// tslint:disable-next-line:max-classes-per-file
var TranslationServiceRecognizer = /** @class */ (function (_super) {
    __extends(TranslationServiceRecognizer, _super);
    function TranslationServiceRecognizer(authentication, connectionFactory, audioSource, recognizerConfig) {
        return _super.call(this, authentication, connectionFactory, audioSource, recognizerConfig) || this;
    }
    TranslationServiceRecognizer.prototype.GetSession = function (audioSourceId, onEventCallback) {
        return new TranslationRequestSession(audioSourceId, onEventCallback);
    };
    TranslationServiceRecognizer.prototype.ProcessTypeSpecificMessages = function (connectionMessage, session, connection) {
        var requestSession = session;
        switch (connectionMessage.Path.toLowerCase()) {
            case "translation.hypothesis":
                requestSession.OnServiceTranslationHypothesisResponse(JSON.parse(connectionMessage.TextBody));
                break;
            case "translation.phrase":
                if (this.recognizerConfig.IsContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                }
                var translatedPhrase = JSON.parse(connectionMessage.TextBody);
                // Make sure the reason is an enumn and not a string.
                var status2 = "" + translatedPhrase.RecognitionStatus;
                var recstatus2 = Exports_3.RecognitionStatus2[status2];
                if (recstatus2 === Exports_3.RecognitionStatus2.Success) {
                    // OK, the recognition was successful. How'd the translation do?
                    if (Exports_1.TranslationStatus[translatedPhrase.Translation.TranslationStatus] === Exports_1.TranslationStatus.Success) {
                        requestSession.OnServiceTranslationPhraseResponse(translatedPhrase);
                    }
                    else {
                        requestSession.OnServiceTranslationErrorResponse(translatedPhrase);
                    }
                    break;
                }
                else {
                    // Hate to reparse it, but the data types are different.
                    requestSession.OnServiceRecognitionErrorResponse(JSON.parse(connectionMessage.TextBody));
                }
                break;
            case "translation.synthesis":
                if (this.recognizerConfig.IsContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                }
                requestSession.OnServiceTranslationSynthesis(connectionMessage.BinaryBody);
                break;
            case "translation.synthesis.end":
                if (this.recognizerConfig.IsContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                }
                var synthEnd = JSON.parse(connectionMessage.TextBody);
                var status3 = "" + synthEnd.SynthesisStatus;
                var synthStatus = SynthesisStatus[status3];
                switch (synthStatus) {
                    case SynthesisStatus.Error:
                        synthEnd.SynthesisStatus = Exports_2.ResultReason.Canceled;
                        requestSession.OnServiceTranslationSynthesisError(synthEnd);
                        break;
                    case SynthesisStatus.Success:
                        synthEnd.SynthesisStatus = Exports_2.ResultReason.SynthesizingAudio;
                        requestSession.OnServiceTranslationSynthesis(undefined);
                        break;
                    default:
                        synthEnd.SynthesisStatus = Exports_2.ResultReason.SynthesizingAudioCompleted;
                        break;
                }
                break;
            default:
                break;
        }
    };
    return TranslationServiceRecognizer;
}(ServiceRecognizerBase));
exports.TranslationServiceRecognizer = TranslationServiceRecognizer;
// tslint:disable-next-line:max-classes-per-file
var IntentServiceRecognizer = /** @class */ (function (_super) {
    __extends(IntentServiceRecognizer, _super);
    function IntentServiceRecognizer(authentication, connectionFactory, audioSource, recognizerConfig) {
        return _super.call(this, authentication, connectionFactory, audioSource, recognizerConfig) || this;
    }
    IntentServiceRecognizer.prototype.GetSession = function (audioSourceId, onEventCallback) {
        return new IntentRequestSession(audioSourceId, onEventCallback);
    };
    IntentServiceRecognizer.prototype.ProcessTypeSpecificMessages = function (connectionMessage, session, connection) {
        var requestSession = session;
        switch (connectionMessage.Path.toLowerCase()) {
            case "speech.hypothesis":
                requestSession.OnServiceSpeechHypothesisResponse(JSON.parse(connectionMessage.TextBody));
                break;
            case "speech.phrase":
                if (this.recognizerConfig.IsContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                }
                if (this.recognizerConfig.parameters.getProperty(Exports_3.OutputFormatPropertyName) === Exports_2.OutputFormat[Exports_2.OutputFormat.Simple]) {
                    requestSession.OnServiceSimpleSpeechPhraseResponse(JSON.parse(connectionMessage.TextBody));
                }
                else {
                    requestSession.OnServiceDetailedSpeechPhraseResponse(JSON.parse(connectionMessage.TextBody));
                }
                break;
            case "response":
                // Response from LUIS
                if (this.recognizerConfig.IsContinuousRecognition) {
                    // For continuous recognition telemetry has to be sent for every phrase as per spec.
                    this.SendTelemetryData(requestSession.RequestId, connection, requestSession.GetTelemetry());
                }
                if (connectionMessage.TextBody === "") {
                    // The query to LUIS failed, and nothing came back.
                    requestSession.OnServiceIntentResponse(null);
                }
                else {
                    requestSession.OnServiceIntentResponse(JSON.parse(connectionMessage.TextBody));
                }
                break;
            default:
                break;
        }
    };
    return IntentServiceRecognizer;
}(ServiceRecognizerBase));
exports.IntentServiceRecognizer = IntentServiceRecognizer;
// tslint:disable-next-line:max-classes-per-file
var RequestSessionBase = /** @class */ (function () {
    function RequestSessionBase(audioSourceId, onEventCallback) {
        var _this = this;
        this.isDisposed = false;
        this.detachables = new Array();
        this.isAudioNodeDetached = false;
        this.isCompleted = false;
        this.OnAudioSourceAttachCompleted = function (audioNode, isError, error) {
            _this.audioNode = audioNode;
            if (isError) {
                _this.OnComplete(RecognitionEvents_1.RecognitionCompletionStatus.AudioSourceError, error);
            }
            else {
                _this.OnEvent(new RecognitionEvents_1.ListeningStartedEvent(_this.requestId, _this.sessionId, _this.audioSourceId, _this.audioNodeId));
            }
        };
        this.OnPreConnectionStart = function (authFetchEventId, connectionId) {
            _this.authFetchEventId = authFetchEventId;
            _this.sessionId = connectionId;
            _this.OnEvent(new RecognitionEvents_1.ConnectingToServiceEvent(_this.requestId, _this.authFetchEventId, _this.sessionId));
        };
        this.OnAuthCompleted = function (isError, error) {
            if (isError) {
                _this.OnComplete(RecognitionEvents_1.RecognitionCompletionStatus.AuthTokenFetchError, error);
            }
        };
        this.OnConnectionEstablishCompleted = function (statusCode, reason) {
            if (statusCode === 200) {
                _this.OnEvent(new RecognitionEvents_1.RecognitionStartedEvent(_this.RequestId, _this.audioSourceId, _this.audioNodeId, _this.authFetchEventId, _this.sessionId));
                return;
            }
            else if (statusCode === 403) {
                _this.OnComplete(RecognitionEvents_1.RecognitionCompletionStatus.UnAuthorized, reason);
            }
            else {
                _this.OnComplete(RecognitionEvents_1.RecognitionCompletionStatus.ConnectError, reason);
            }
        };
        this.OnServiceTurnStartResponse = function (response) {
            if (response && response.context && response.context.serviceTag) {
                _this.serviceTag = response.context.serviceTag;
            }
        };
        this.OnServiceSpeechStartDetectedResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.SpeechStartDetectedEvent(_this.RequestId, _this.sessionId, result));
        };
        this.OnServiceSpeechFragmentResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.SpeechFragmentEvent(_this.RequestId, _this.sessionId, result));
        };
        this.OnServiceSpeechEndDetectedResponse = function (result) {
            _this.DetachAudioNode();
            _this.OnEvent(new RecognitionEvents_1.SpeechEndDetectedEvent(_this.RequestId, _this.sessionId, result));
        };
        this.OnServiceTurnEndResponse = function () {
            _this.OnComplete(RecognitionEvents_1.RecognitionCompletionStatus.Success);
        };
        this.OnConnectionError = function (error) {
            _this.OnComplete(RecognitionEvents_1.RecognitionCompletionStatus.UnknownError, error);
        };
        this.Dispose = function (error) {
            if (!_this.isDisposed) {
                // we should have completed by now. If we did not its an unknown error.
                _this.isDisposed = true;
                for (var _i = 0, _a = _this.detachables; _i < _a.length; _i++) {
                    var detachable = _a[_i];
                    detachable.Detach();
                }
                _this.serviceTelemetryListener.Dispose();
            }
        };
        this.GetTelemetry = function () {
            return _this.serviceTelemetryListener.GetTelemetry();
        };
        this.OnEvent = function (event) {
            _this.serviceTelemetryListener.OnEvent(event);
            Exports_1.Events.Instance.OnEvent(event);
            if (_this.onEventCallback) {
                _this.onEventCallback(event);
            }
        };
        this.OnComplete = function (status, error) {
            if (!_this.isCompleted) {
                _this.isCompleted = true;
                _this.DetachAudioNode();
                _this.OnEvent(new RecognitionEvents_1.RecognitionEndedEvent(_this.RequestId, _this.audioSourceId, _this.audioNodeId, _this.authFetchEventId, _this.sessionId, _this.serviceTag, status, error ? error : ""));
            }
        };
        this.DetachAudioNode = function () {
            if (!_this.isAudioNodeDetached) {
                _this.isAudioNodeDetached = true;
                if (_this.audioNode) {
                    _this.audioNode.Detach();
                }
            }
        };
        this.audioSourceId = audioSourceId;
        this.onEventCallback = onEventCallback;
        this.requestId = Exports_1.CreateNoDashGuid();
        this.audioNodeId = Exports_1.CreateNoDashGuid();
        this.requestCompletionDeferral = new Exports_1.Deferred();
        this.serviceTelemetryListener = new ServiceTelemetryListener_Internal_1.ServiceTelemetryListener(this.requestId, this.audioSourceId, this.audioNodeId);
        this.OnEvent(new RecognitionEvents_1.RecognitionTriggeredEvent(this.RequestId, this.sessionId, this.audioSourceId, this.audioNodeId));
    }
    Object.defineProperty(RequestSessionBase.prototype, "RequestId", {
        get: function () {
            return this.requestId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RequestSessionBase.prototype, "AudioNodeId", {
        get: function () {
            return this.audioNodeId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RequestSessionBase.prototype, "CompletionPromise", {
        get: function () {
            return this.requestCompletionDeferral.Promise();
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RequestSessionBase.prototype, "IsSpeechEnded", {
        get: function () {
            return this.isAudioNodeDetached;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RequestSessionBase.prototype, "IsCompleted", {
        get: function () {
            return this.isCompleted;
        },
        enumerable: true,
        configurable: true
    });
    RequestSessionBase.prototype.ListenForServiceTelemetry = function (eventSource) {
        this.detachables.push(eventSource.AttachListener(this.serviceTelemetryListener));
    };
    return RequestSessionBase;
}());
exports.RequestSessionBase = RequestSessionBase;
// tslint:disable-next-line:max-classes-per-file
var SpeechRequestSession = /** @class */ (function (_super) {
    __extends(SpeechRequestSession, _super);
    function SpeechRequestSession(audioSourceId, onEventCallback) {
        var _this = _super.call(this, audioSourceId, onEventCallback) || this;
        _this.OnServiceSpeechHypothesisResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.SpeechHypothesisEvent(_this.RequestId, _this.sessionId, result));
        };
        _this.OnServiceSimpleSpeechPhraseResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.SpeechSimplePhraseEvent(_this.RequestId, _this.sessionId, result));
        };
        _this.OnServiceDetailedSpeechPhraseResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.SpeechDetailedPhraseEvent(_this.RequestId, _this.sessionId, result));
        };
        return _this;
    }
    return SpeechRequestSession;
}(RequestSessionBase));
// tslint:disable-next-line:max-classes-per-file
var TranslationRequestSession = /** @class */ (function (_super) {
    __extends(TranslationRequestSession, _super);
    function TranslationRequestSession(audioSourceId, onEventCallback) {
        var _this = _super.call(this, audioSourceId, onEventCallback) || this;
        _this.OnServiceTranslationHypothesisResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.TranslationHypothesisEvent(_this.RequestId, _this.sessionId, result));
        };
        _this.OnServiceTranslationPhraseResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.TranslationPhraseEvent(_this.RequestId, _this.sessionId, result));
        };
        _this.OnServiceTranslationErrorResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.TranslationFailedEvent(_this.RequestId, _this.sessionId, result));
        };
        _this.OnServiceRecognitionErrorResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.RecognitionFailedEvent(_this.RequestId, _this.sessionId, result));
        };
        _this.OnServiceTranslationSynthesis = function (result) {
            _this.OnEvent(new RecognitionEvents_1.TranslationSynthesisEvent(_this.RequestId, _this.sessionId, result));
        };
        _this.OnServiceTranslationSynthesisError = function (result) {
            _this.OnEvent(new RecognitionEvents_1.TranslationSynthesisErrorEvent(_this.RequestId, _this.sessionId, result));
        };
        return _this;
    }
    return TranslationRequestSession;
}(RequestSessionBase));
// tslint:disable-next-line:max-classes-per-file
var IntentRequestSession = /** @class */ (function (_super) {
    __extends(IntentRequestSession, _super);
    function IntentRequestSession(audioSourceId, onEventCallback) {
        var _this = _super.call(this, audioSourceId, onEventCallback) || this;
        _this.OnServiceIntentResponse = function (result) {
            _this.OnEvent(new RecognitionEvents_1.IntentResponseEvent(_this.RequestId, _this.sessionId, result));
        };
        return _this;
    }
    return IntentRequestSession;
}(SpeechRequestSession));



/***/ }),
/* 59 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var RecognitionEvents_1 = __webpack_require__(7);
// tslint:disable-next-line:max-classes-per-file
var ServiceTelemetryListener = /** @class */ (function () {
    function ServiceTelemetryListener(requestId, audioSourceId, audioNodeId) {
        var _this = this;
        this.isDisposed = false;
        this.listeningTriggerMetric = null;
        this.micMetric = null;
        this.connectionEstablishMetric = null;
        this.OnEvent = function (e) {
            if (_this.isDisposed) {
                return;
            }
            if (e instanceof RecognitionEvents_1.RecognitionTriggeredEvent && e.RequestId === _this.requestId) {
                _this.listeningTriggerMetric = {
                    End: e.EventTime,
                    Name: "ListeningTrigger",
                    Start: e.EventTime,
                };
            }
            if (e instanceof Exports_1.AudioStreamNodeAttachingEvent && e.AudioSourceId === _this.audioSourceId && e.AudioNodeId === _this.audioNodeId) {
                _this.micStartTime = e.EventTime;
            }
            if (e instanceof Exports_1.AudioStreamNodeAttachedEvent && e.AudioSourceId === _this.audioSourceId && e.AudioNodeId === _this.audioNodeId) {
                _this.micStartTime = e.EventTime;
            }
            if (e instanceof Exports_1.AudioSourceErrorEvent && e.AudioSourceId === _this.audioSourceId) {
                if (!_this.micMetric) {
                    _this.micMetric = {
                        End: e.EventTime,
                        Error: e.Error,
                        Name: "Microphone",
                        Start: _this.micStartTime,
                    };
                }
            }
            if (e instanceof Exports_1.AudioStreamNodeErrorEvent && e.AudioSourceId === _this.audioSourceId && e.AudioNodeId === _this.audioNodeId) {
                if (!_this.micMetric) {
                    _this.micMetric = {
                        End: e.EventTime,
                        Error: e.Error,
                        Name: "Microphone",
                        Start: _this.micStartTime,
                    };
                }
            }
            if (e instanceof Exports_1.AudioStreamNodeDetachedEvent && e.AudioSourceId === _this.audioSourceId && e.AudioNodeId === _this.audioNodeId) {
                if (!_this.micMetric) {
                    _this.micMetric = {
                        End: e.EventTime,
                        Name: "Microphone",
                        Start: _this.micStartTime,
                    };
                }
            }
            if (e instanceof RecognitionEvents_1.ConnectingToServiceEvent && e.RequestId === _this.requestId) {
                _this.connectionId = e.SessionId;
            }
            if (e instanceof Exports_1.ConnectionStartEvent && e.ConnectionId === _this.connectionId) {
                _this.connectionStartTime = e.EventTime;
            }
            if (e instanceof Exports_1.ConnectionEstablishedEvent && e.ConnectionId === _this.connectionId) {
                if (!_this.connectionEstablishMetric) {
                    _this.connectionEstablishMetric = {
                        End: e.EventTime,
                        Id: _this.connectionId,
                        Name: "Connection",
                        Start: _this.connectionStartTime,
                    };
                }
            }
            if (e instanceof Exports_1.ConnectionEstablishErrorEvent && e.ConnectionId === _this.connectionId) {
                if (!_this.connectionEstablishMetric) {
                    _this.connectionEstablishMetric = {
                        End: e.EventTime,
                        Error: _this.GetConnectionError(e.StatusCode),
                        Id: _this.connectionId,
                        Name: "Connection",
                        Start: _this.connectionStartTime,
                    };
                }
            }
            if (e instanceof Exports_1.ConnectionMessageReceivedEvent && e.ConnectionId === _this.connectionId) {
                if (e.Message && e.Message.Headers && e.Message.Headers.path) {
                    if (!_this.receivedMessages[e.Message.Headers.path]) {
                        _this.receivedMessages[e.Message.Headers.path] = new Array();
                    }
                    _this.receivedMessages[e.Message.Headers.path].push(e.NetworkReceivedTime);
                }
            }
        };
        this.GetTelemetry = function () {
            var metrics = new Array();
            if (_this.listeningTriggerMetric) {
                metrics.push(_this.listeningTriggerMetric);
            }
            if (_this.micMetric) {
                metrics.push(_this.micMetric);
            }
            if (_this.connectionEstablishMetric) {
                metrics.push(_this.connectionEstablishMetric);
            }
            var telemetry = {
                Metrics: metrics,
                ReceivedMessages: _this.receivedMessages,
            };
            var json = JSON.stringify(telemetry);
            // We dont want to send the same telemetry again. So clean those out.
            _this.receivedMessages = {};
            _this.listeningTriggerMetric = null;
            _this.micMetric = null;
            _this.connectionEstablishMetric = null;
            return json;
        };
        this.Dispose = function () {
            _this.isDisposed = true;
        };
        this.GetConnectionError = function (statusCode) {
            /*
            -- Websocket status codes --
            NormalClosure = 1000,
            EndpointUnavailable = 1001,
            ProtocolError = 1002,
            InvalidMessageType = 1003,
            Empty = 1005,
            InvalidPayloadData = 1007,
            PolicyViolation = 1008,
            MessageTooBig = 1009,
            MandatoryExtension = 1010,
            InternalServerError = 1011
            */
            switch (statusCode) {
                case 400:
                case 1002:
                case 1003:
                case 1005:
                case 1007:
                case 1008:
                case 1009: return "BadRequest";
                case 401: return "Unauthorized";
                case 403: return "Forbidden";
                case 503:
                case 1001: return "ServerUnavailable";
                case 500:
                case 1011: return "ServerError";
                case 408:
                case 504: return "Timeout";
                default: return "statuscode:" + statusCode.toString();
            }
        };
        this.requestId = requestId;
        this.audioSourceId = audioSourceId;
        this.audioNodeId = audioNodeId;
        this.receivedMessages = {};
    }
    return ServiceTelemetryListener;
}());
exports.ServiceTelemetryListener = ServiceTelemetryListener;



/***/ }),
/* 60 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var PathHeaderName = "path";
var ContentTypeHeaderName = "content-type";
var RequestIdHeaderName = "x-requestid";
var RequestTimestampHeaderName = "x-timestamp";
var SpeechConnectionMessage = /** @class */ (function (_super) {
    __extends(SpeechConnectionMessage, _super);
    function SpeechConnectionMessage(messageType, path, requestId, contentType, body, additionalHeaders, id) {
        var _this = this;
        if (!path) {
            throw new Exports_1.ArgumentNullError("path");
        }
        if (!requestId) {
            throw new Exports_1.ArgumentNullError("requestId");
        }
        var headers = {};
        headers[PathHeaderName] = path;
        headers[RequestIdHeaderName] = requestId;
        headers[RequestTimestampHeaderName] = new Date().toISOString();
        if (contentType) {
            headers[ContentTypeHeaderName] = contentType;
        }
        if (additionalHeaders) {
            for (var headerName in additionalHeaders) {
                if (headerName) {
                    headers[headerName] = additionalHeaders[headerName];
                }
            }
        }
        if (id) {
            _this = _super.call(this, messageType, body, headers, id) || this;
        }
        else {
            _this = _super.call(this, messageType, body, headers) || this;
        }
        _this.path = path;
        _this.requestId = requestId;
        _this.contentType = contentType;
        _this.additionalHeaders = additionalHeaders;
        return _this;
    }
    Object.defineProperty(SpeechConnectionMessage.prototype, "Path", {
        get: function () {
            return this.path;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConnectionMessage.prototype, "RequestId", {
        get: function () {
            return this.requestId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConnectionMessage.prototype, "ContentType", {
        get: function () {
            return this.contentType;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechConnectionMessage.prototype, "AdditionalHeaders", {
        get: function () {
            return this.additionalHeaders;
        },
        enumerable: true,
        configurable: true
    });
    SpeechConnectionMessage.FromConnectionMessage = function (message) {
        var path = null;
        var requestId = null;
        var contentType = null;
        var requestTimestamp = null;
        var additionalHeaders = {};
        if (message.Headers) {
            for (var headerName in message.Headers) {
                if (headerName) {
                    if (headerName.toLowerCase() === PathHeaderName.toLowerCase()) {
                        path = message.Headers[headerName];
                    }
                    else if (headerName.toLowerCase() === RequestIdHeaderName.toLowerCase()) {
                        requestId = message.Headers[headerName];
                    }
                    else if (headerName.toLowerCase() === RequestTimestampHeaderName.toLowerCase()) {
                        requestTimestamp = message.Headers[headerName];
                    }
                    else if (headerName.toLowerCase() === ContentTypeHeaderName.toLowerCase()) {
                        contentType = message.Headers[headerName];
                    }
                    else {
                        additionalHeaders[headerName] = message.Headers[headerName];
                    }
                }
            }
        }
        return new SpeechConnectionMessage(message.MessageType, path, requestId, contentType, message.Body, additionalHeaders, message.Id);
    };
    return SpeechConnectionMessage;
}(Exports_1.ConnectionMessage));
exports.SpeechConnectionMessage = SpeechConnectionMessage;



/***/ }),
/* 61 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var RecognitionMode;
(function (RecognitionMode) {
    RecognitionMode[RecognitionMode["Interactive"] = 0] = "Interactive";
    RecognitionMode[RecognitionMode["Conversation"] = 1] = "Conversation";
    RecognitionMode[RecognitionMode["Dictation"] = 2] = "Dictation";
})(RecognitionMode = exports.RecognitionMode || (exports.RecognitionMode = {}));
var SpeechResultFormat;
(function (SpeechResultFormat) {
    SpeechResultFormat[SpeechResultFormat["Simple"] = 0] = "Simple";
    SpeechResultFormat[SpeechResultFormat["Detailed"] = 1] = "Detailed";
})(SpeechResultFormat = exports.SpeechResultFormat || (exports.SpeechResultFormat = {}));
var RecognizerConfig = /** @class */ (function () {
    function RecognizerConfig(platformConfig, recognitionMode, speechConfig) {
        if (recognitionMode === void 0) { recognitionMode = RecognitionMode.Interactive; }
        this.recognitionMode = RecognitionMode.Interactive;
        this.platformConfig = platformConfig ? platformConfig : new PlatformConfig(new Context(null, null));
        this.recognitionMode = recognitionMode;
        this.recognitionActivityTimeout = recognitionMode === RecognitionMode.Interactive ? 8000 : 25000;
        this.speechConfig = speechConfig;
    }
    Object.defineProperty(RecognizerConfig.prototype, "parameters", {
        get: function () {
            return this.speechConfig;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognizerConfig.prototype, "RecognitionMode", {
        get: function () {
            return this.recognitionMode;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognizerConfig.prototype, "PlatformConfig", {
        get: function () {
            return this.platformConfig;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognizerConfig.prototype, "RecognitionActivityTimeout", {
        get: function () {
            return this.recognitionActivityTimeout;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RecognizerConfig.prototype, "IsContinuousRecognition", {
        get: function () {
            return this.recognitionMode !== RecognitionMode.Interactive;
        },
        enumerable: true,
        configurable: true
    });
    return RecognizerConfig;
}());
exports.RecognizerConfig = RecognizerConfig;
// tslint:disable-next-line:max-classes-per-file
var PlatformConfig = /** @class */ (function () {
    function PlatformConfig(context) {
        var _this = this;
        this.Serialize = function () {
            return JSON.stringify(_this, function (key, value) {
                if (value && typeof value === "object") {
                    var replacement = {};
                    for (var k in value) {
                        if (Object.hasOwnProperty.call(value, k)) {
                            replacement[k && k.charAt(0).toLowerCase() + k.substring(1)] = value[k];
                        }
                    }
                    return replacement;
                }
                return value;
            });
        };
        this.context = context;
    }
    Object.defineProperty(PlatformConfig.prototype, "Context", {
        get: function () {
            return this.context;
        },
        enumerable: true,
        configurable: true
    });
    return PlatformConfig;
}());
exports.PlatformConfig = PlatformConfig;
// tslint:disable-next-line:max-classes-per-file
var Context = /** @class */ (function () {
    function Context(os, device) {
        this.system = new System();
        this.os = os;
        this.device = device;
    }
    Object.defineProperty(Context.prototype, "System", {
        get: function () {
            return this.system;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Context.prototype, "OS", {
        get: function () {
            return this.os;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Context.prototype, "Device", {
        get: function () {
            return this.device;
        },
        enumerable: true,
        configurable: true
    });
    return Context;
}());
exports.Context = Context;
// tslint:disable-next-line:max-classes-per-file
var System = /** @class */ (function () {
    function System() {
        // TODO: Tie this with the sdk Version somehow
        this.version = "1.0.00000";
    }
    Object.defineProperty(System.prototype, "Version", {
        get: function () {
            // Controlled by sdk
            return this.version;
        },
        enumerable: true,
        configurable: true
    });
    return System;
}());
exports.System = System;
// tslint:disable-next-line:max-classes-per-file
var OS = /** @class */ (function () {
    function OS(platform, name, version) {
        this.platform = platform;
        this.name = name;
        this.version = version;
    }
    Object.defineProperty(OS.prototype, "Platform", {
        get: function () {
            return this.platform;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(OS.prototype, "Name", {
        get: function () {
            return this.name;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(OS.prototype, "Version", {
        get: function () {
            return this.version;
        },
        enumerable: true,
        configurable: true
    });
    return OS;
}());
exports.OS = OS;
// tslint:disable-next-line:max-classes-per-file
var Device = /** @class */ (function () {
    function Device(manufacturer, model, version) {
        this.manufacturer = manufacturer;
        this.model = model;
        this.version = version;
    }
    Object.defineProperty(Device.prototype, "Manufacturer", {
        get: function () {
            return this.manufacturer;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Device.prototype, "Model", {
        get: function () {
            return this.model;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Device.prototype, "Version", {
        get: function () {
            return this.version;
        },
        enumerable: true,
        configurable: true
    });
    return Device;
}());
exports.Device = Device;



/***/ }),
/* 62 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var RecognitionStatus2;
(function (RecognitionStatus2) {
    RecognitionStatus2[RecognitionStatus2["Success"] = 0] = "Success";
    RecognitionStatus2[RecognitionStatus2["NoMatch"] = 1] = "NoMatch";
    RecognitionStatus2[RecognitionStatus2["InitialSilenceTimeout"] = 2] = "InitialSilenceTimeout";
    RecognitionStatus2[RecognitionStatus2["BabbleTimeout"] = 3] = "BabbleTimeout";
    RecognitionStatus2[RecognitionStatus2["Error"] = 4] = "Error";
    RecognitionStatus2[RecognitionStatus2["EndOfDictation"] = 5] = "EndOfDictation";
})(RecognitionStatus2 = exports.RecognitionStatus2 || (exports.RecognitionStatus2 = {}));



/***/ }),
/* 63 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var CRLF = "\r\n";
var WebsocketMessageFormatter = /** @class */ (function () {
    function WebsocketMessageFormatter() {
        var _this = this;
        this.ToConnectionMessage = function (message) {
            var deferral = new Exports_1.Deferred();
            try {
                if (message.MessageType === Exports_1.MessageType.Text) {
                    var textMessage = message.TextContent;
                    var headers = {};
                    var body = null;
                    if (textMessage) {
                        var headerBodySplit = textMessage.split("\r\n\r\n");
                        if (headerBodySplit && headerBodySplit.length > 0) {
                            headers = _this.ParseHeaders(headerBodySplit[0]);
                            if (headerBodySplit.length > 1) {
                                body = headerBodySplit[1];
                            }
                        }
                    }
                    deferral.Resolve(new Exports_1.ConnectionMessage(message.MessageType, body, headers, message.Id));
                }
                else if (message.MessageType === Exports_1.MessageType.Binary) {
                    var binaryMessage = message.BinaryContent;
                    var headers = {};
                    var body = null;
                    if (!binaryMessage || binaryMessage.byteLength < 2) {
                        throw new Error("Invalid binary message format. Header length missing.");
                    }
                    var dataView = new DataView(binaryMessage);
                    var headerLength = dataView.getInt16(0);
                    if (binaryMessage.byteLength < headerLength + 2) {
                        throw new Error("Invalid binary message format. Header content missing.");
                    }
                    var headersString = "";
                    for (var i = 0; i < headerLength; i++) {
                        headersString += String.fromCharCode((dataView).getInt8(i + 2));
                    }
                    headers = _this.ParseHeaders(headersString);
                    if (binaryMessage.byteLength > headerLength + 2) {
                        body = binaryMessage.slice(2 + headerLength);
                    }
                    deferral.Resolve(new Exports_1.ConnectionMessage(message.MessageType, body, headers, message.Id));
                }
            }
            catch (e) {
                deferral.Reject("Error formatting the message. Error: " + e);
            }
            return deferral.Promise();
        };
        this.FromConnectionMessage = function (message) {
            var deferral = new Exports_1.Deferred();
            try {
                if (message.MessageType === Exports_1.MessageType.Text) {
                    var payload = "" + _this.MakeHeaders(message) + CRLF + (message.TextBody ? message.TextBody : "");
                    deferral.Resolve(new Exports_1.RawWebsocketMessage(Exports_1.MessageType.Text, payload, message.Id));
                }
                else if (message.MessageType === Exports_1.MessageType.Binary) {
                    var headersString = _this.MakeHeaders(message);
                    var content = message.BinaryBody;
                    var headerInt8Array = new Int8Array(_this.StringToArrayBuffer(headersString));
                    var payload = new ArrayBuffer(2 + headerInt8Array.byteLength + (content ? content.byteLength : 0));
                    var dataView = new DataView(payload);
                    dataView.setInt16(0, headerInt8Array.length);
                    for (var i = 0; i < headerInt8Array.byteLength; i++) {
                        dataView.setInt8(2 + i, headerInt8Array[i]);
                    }
                    if (content) {
                        var bodyInt8Array = new Int8Array(content);
                        for (var i = 0; i < bodyInt8Array.byteLength; i++) {
                            dataView.setInt8(2 + headerInt8Array.byteLength + i, bodyInt8Array[i]);
                        }
                    }
                    deferral.Resolve(new Exports_1.RawWebsocketMessage(Exports_1.MessageType.Binary, payload, message.Id));
                }
            }
            catch (e) {
                deferral.Reject("Error formatting the message. " + e);
            }
            return deferral.Promise();
        };
        this.MakeHeaders = function (message) {
            var headersString = "";
            if (message.Headers) {
                for (var header in message.Headers) {
                    if (header) {
                        headersString += header + ": " + message.Headers[header] + CRLF;
                    }
                }
            }
            return headersString;
        };
        this.ParseHeaders = function (headersString) {
            var headers = {};
            if (headersString) {
                var headerMatches = headersString.match(/[^\r\n]+/g);
                if (headers) {
                    for (var _i = 0, headerMatches_1 = headerMatches; _i < headerMatches_1.length; _i++) {
                        var header = headerMatches_1[_i];
                        if (header) {
                            var separatorIndex = header.indexOf(":");
                            var headerName = separatorIndex > 0 ? header.substr(0, separatorIndex).trim().toLowerCase() : header;
                            var headerValue = separatorIndex > 0 && header.length > (separatorIndex + 1) ?
                                header.substr(separatorIndex + 1).trim() :
                                "";
                            headers[headerName] = headerValue;
                        }
                    }
                }
            }
            return headers;
        };
        this.StringToArrayBuffer = function (str) {
            var buffer = new ArrayBuffer(str.length);
            var view = new DataView(buffer);
            for (var i = 0; i < str.length; i++) {
                view.setUint8(i, str.charCodeAt(i));
            }
            return buffer;
        };
    }
    return WebsocketMessageFormatter;
}());
exports.WebsocketMessageFormatter = WebsocketMessageFormatter;



/***/ }),
/* 64 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(6);
var Exports_2 = __webpack_require__(1);
var Exports_3 = __webpack_require__(0);
var Exports_4 = __webpack_require__(2);
var TestHooksParamName = "testhooks";
var ConnectionIdHeader = "X-ConnectionId";
var TranslationConnectionFactory = /** @class */ (function () {
    function TranslationConnectionFactory() {
        var _this = this;
        this.Create = function (config, authInfo, connectionId) {
            var endpoint = config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_Endpoint, undefined);
            if (!endpoint) {
                var region = config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_Region, undefined);
                endpoint = _this.Host(region) + Exports_2.Storage.Local.GetOrAdd("TranslationRelativeUri", "/speech/translation/cognitiveservices/v1");
            }
            var queryParams = {
                from: config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_RecoLanguage),
                to: config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_TranslationToLanguages),
            };
            if (_this.IsDebugModeEnabled) {
                queryParams[TestHooksParamName] = "1";
            }
            var voiceName = "voice";
            var featureName = "features";
            if (config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_TranslationVoice, undefined) !== undefined) {
                queryParams[voiceName] = config.parameters.getProperty(Exports_3.PropertyId.SpeechServiceConnection_TranslationVoice);
                queryParams[featureName] = "texttospeech";
            }
            var headers = {};
            headers[authInfo.HeaderName] = authInfo.Token;
            headers[ConnectionIdHeader] = connectionId;
            return new Exports_1.WebsocketConnection(endpoint, queryParams, headers, new Exports_4.WebsocketMessageFormatter(), connectionId);
        };
    }
    TranslationConnectionFactory.prototype.Host = function (region) {
        return Exports_2.Storage.Local.GetOrAdd("Host", "wss://" + region + ".s2s.speech.microsoft.com");
    };
    Object.defineProperty(TranslationConnectionFactory.prototype, "IsDebugModeEnabled", {
        get: function () {
            var value = Exports_2.Storage.Local.GetOrAdd("IsDebugModeEnabled", "false");
            return value.toLowerCase() === "true";
        },
        enumerable: true,
        configurable: true
    });
    return TranslationConnectionFactory;
}());
exports.TranslationConnectionFactory = TranslationConnectionFactory;



/***/ }),
/* 65 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
var Exports_2 = __webpack_require__(2);
var EnumTranslation = /** @class */ (function () {
    function EnumTranslation() {
    }
    EnumTranslation.implTranslateRecognitionResult = function (recognitionStatus) {
        var reason = Exports_1.ResultReason.Canceled;
        var recognitionStatus2 = "" + recognitionStatus;
        var recstatus2 = Exports_2.RecognitionStatus2[recognitionStatus2];
        switch (recstatus2) {
            case Exports_2.RecognitionStatus2.Success:
                reason = Exports_1.ResultReason.RecognizedSpeech;
                break;
            case Exports_2.RecognitionStatus2.NoMatch:
            case Exports_2.RecognitionStatus2.InitialSilenceTimeout:
            case Exports_2.RecognitionStatus2.BabbleTimeout:
                reason = Exports_1.ResultReason.NoMatch;
                break;
            case Exports_2.RecognitionStatus2.EndOfDictation:
            case Exports_2.RecognitionStatus2.Error:
            default:
                reason = Exports_1.ResultReason.Canceled;
                break;
        }
        return reason;
    };
    EnumTranslation.implTranslateCancelResult = function (recognitionStatus) {
        var reason = Exports_1.CancellationReason.EndOfStream;
        var recognitionStatus2 = "" + recognitionStatus;
        var recstatus2 = Exports_2.RecognitionStatus2[recognitionStatus2];
        switch (recstatus2) {
            case Exports_2.RecognitionStatus2.Success:
            case Exports_2.RecognitionStatus2.EndOfDictation:
            case Exports_2.RecognitionStatus2.NoMatch:
                reason = Exports_1.CancellationReason.EndOfStream;
                break;
            case Exports_2.RecognitionStatus2.InitialSilenceTimeout:
            case Exports_2.RecognitionStatus2.BabbleTimeout:
            case Exports_2.RecognitionStatus2.Error:
            default:
                reason = Exports_1.CancellationReason.Error;
                break;
        }
        return reason;
    };
    return EnumTranslation;
}());
exports.EnumTranslation = EnumTranslation;



/***/ }),
/* 66 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(2);
var Contracts_1 = __webpack_require__(4);
var Exports_2 = __webpack_require__(0);
/**
 * Speech translation configuration.
 * @class SpeechTranslationConfig
 */
var SpeechTranslationConfig = /** @class */ (function (_super) {
    __extends(SpeechTranslationConfig, _super);
    /**
     * Creates an instance of recognizer config.
     */
    function SpeechTranslationConfig() {
        return _super.call(this) || this;
    }
    /**
     * Static instance of SpeechTranslationConfig returned by passing a subscription key and service region.
     * @member SpeechTranslationConfig.fromSubscription
     * @function
     * @public
     * @param {string} subscriptionKey - The subscription key.
     * @param {string} region - The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns {SpeechTranslationConfig} The speech translation config.
     */
    SpeechTranslationConfig.fromSubscription = function (subscriptionKey, region) {
        Contracts_1.Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        Contracts_1.Contracts.throwIfNullOrWhitespace(region, "region");
        var ret = new SpeechTranslationConfigImpl();
        ret.properties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Key, subscriptionKey);
        ret.properties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Region, region);
        return ret;
    };
    /**
     * Static instance of SpeechTranslationConfig returned by passing authorization token and service region.
     * Note: The caller needs to ensure that the authorization token is valid. Before the authorization token
     * expipres, the caller needs to refresh it by setting the property authorizationToken with a new valid token.
     * Otherwise, all the recognizers created by this SpeechTranslationConfig instance will encounter errors during recognition.
     * @member SpeechTranslationConfig.fromAuthorizationToken
     * @function
     * @public
     * @param {string} authorizationToken - The authorization token.
     * @param {string} region - The region name (see the <a href="https://aka.ms/csspeech/region">region page</a>).
     * @returns {SpeechTranslationConfig} The speech translation config.
     */
    SpeechTranslationConfig.fromAuthorizationToken = function (authorizationToken, region) {
        Contracts_1.Contracts.throwIfNullOrWhitespace(authorizationToken, "authorizationToken");
        Contracts_1.Contracts.throwIfNullOrWhitespace(region, "region");
        var ret = new SpeechTranslationConfigImpl();
        ret.properties.setProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token, authorizationToken);
        ret.properties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Region, region);
        return ret;
    };
    /**
     * Creates an instance of the speech translation config with specified endpoint and subscription key.
     * This method is intended only for users who use a non-standard service endpoint or paramters.
     * Note: The query properties specified in the endpoint URL are not changed, even if they are set by any other APIs.
     * For example, if language is defined in the uri as query parameter "language=de-DE", and also set by the speechRecognitionLanguage property,
     * the language setting in uri takes precedence, and the effective language is "de-DE".
     * Only the properties that are not specified in the endpoint URL can be set by other APIs.
     * @member SpeechTranslationConfig.fromEndpoint
     * @function
     * @public
     * @param {URL} endpoint - The service endpoint to connect to.
     * @param {string} subscriptionKey - The subscription key.
     * @returns {SpeechTranslationConfig} A speech config instance.
     */
    SpeechTranslationConfig.fromEndpoint = function (endpoint, subscriptionKey) {
        Contracts_1.Contracts.throwIfNull(endpoint, "endpoint");
        Contracts_1.Contracts.throwIfNullOrWhitespace(subscriptionKey, "subscriptionKey");
        var ret = new SpeechTranslationConfigImpl();
        ret.properties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Endpoint, endpoint.href);
        ret.properties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Key, subscriptionKey);
        return ret;
    };
    return SpeechTranslationConfig;
}(Exports_2.SpeechConfig));
exports.SpeechTranslationConfig = SpeechTranslationConfig;
/**
 * @private
 * @class SpeechTranslationConfigImpl
 */
// tslint:disable-next-line:max-classes-per-file
var SpeechTranslationConfigImpl = /** @class */ (function (_super) {
    __extends(SpeechTranslationConfigImpl, _super);
    function SpeechTranslationConfigImpl() {
        var _this = _super.call(this) || this;
        _this.speechProperties = new Exports_2.PropertyCollection();
        return _this;
    }
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "authorizationToken", {
        /**
         * Sets the authorization token.
         * If this is set, subscription key is ignored.
         * User needs to make sure the provided authorization token is valid and not expired.
         * @member SpeechTranslationConfigImpl.prototype.authorizationToken
         * @function
         * @public
         * @param {string} value - The authorization token.
         */
        set: function (value) {
            Contracts_1.Contracts.throwIfNullOrWhitespace(value, "value");
            this.properties.setProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token, value);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "speechRecognitionLanguage", {
        /**
         * Sets the authorization token.
         * If this is set, subscription key is ignored.
         * User needs to make sure the provided authorization token is valid and not expired.
         * @member SpeechTranslationConfigImpl.prototype.speechRecognitionLanguage
         * @function
         * @public
         * @param {string} value - The authorization token.
         */
        set: function (value) {
            Contracts_1.Contracts.throwIfNullOrWhitespace(value, "value");
            this.properties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage, value);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "subscriptionKey", {
        /**
         * @member SpeechTranslationConfigImpl.prototype.subscriptionKey
         * @function
         * @public
         */
        get: function () {
            return this.speechProperties.getProperty(Exports_2.PropertyId[Exports_2.PropertyId.SpeechServiceConnection_Key]);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "outputFormat", {
        /**
         * @member SpeechTranslationConfigImpl.prototype.outputFormat
         * @function
         * @public
         */
        get: function () {
            return Exports_2.OutputFormat[this.speechProperties.getProperty(Exports_1.OutputFormatPropertyName, Exports_2.OutputFormat[Exports_2.OutputFormat.Simple])];
        },
        /**
         * @member SpeechTranslationConfigImpl.prototype.outputFormat
         * @function
         * @public
         */
        set: function (value) {
            this.speechProperties.setProperty(Exports_1.OutputFormatPropertyName, Exports_2.OutputFormat[value]);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "endpointId", {
        /**
         * @member SpeechTranslationConfigImpl.prototype.endpointId
         * @function
         * @public
         */
        get: function () {
            return this.speechProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_EndpointId);
        },
        /**
         * @member SpeechTranslationConfigImpl.prototype.endpointId
         * @function
         * @public
         */
        set: function (value) {
            this.speechProperties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_Endpoint, value);
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Add a (text) target language to translate into.
     * @member SpeechTranslationConfigImpl.prototype.addTargetLanguage
     * @function
     * @public
     * @param {string} value - The language such as de-DE
     */
    SpeechTranslationConfigImpl.prototype.addTargetLanguage = function (value) {
        Contracts_1.Contracts.throwIfNullOrWhitespace(value, "value");
        var languages = this.targetLanguages;
        languages.push(value);
        this.properties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationToLanguages, languages.join(","));
    };
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "targetLanguages", {
        /**
         * Add a (text) target language to translate into.
         * @member SpeechTranslationConfigImpl.prototype.targetLanguages
         * @function
         * @public
         * @param {string} value - The language such as de-DE
         */
        get: function () {
            if (this.speechProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationToLanguages, undefined) !== undefined) {
                return this.speechProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationToLanguages).split(",");
            }
            else {
                return [];
            }
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "voiceName", {
        /**
         * @member SpeechTranslationConfigImpl.prototype.voiceName
         * @function
         * @public
         */
        get: function () {
            return this.getProperty(Exports_2.PropertyId[Exports_2.PropertyId.SpeechServiceConnection_TranslationVoice]);
        },
        /**
         * Sets voice of the translated language, enable voice synthesis output.
         * @member SpeechTranslationConfigImpl.prototype.voiceName
         * @function
         * @public
         * @param {string} value - The name of the voice.
         */
        set: function (value) {
            Contracts_1.Contracts.throwIfNullOrWhitespace(value, "value");
            this.properties.setProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationVoice, value);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "region", {
        /**
         * Provides the region.
         * @member SpeechTranslationConfigImpl.prototype.region
         * @function
         * @public
         * @returns {string} The region.
         */
        get: function () {
            return this.speechProperties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_Region);
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Allows for setting arbitrary properties.
     * @member SpeechTranslationConfigImpl.prototype.setProperty
     * @function
     * @public
     * @param {string} name - The name of the property.
     * @param {string} value - The value of the property.
     */
    SpeechTranslationConfigImpl.prototype.setProperty = function (name, value) {
        this.properties.setProperty(name, value);
    };
    /**
     * Allows for retrieving arbitrary property values.
     * @member SpeechTranslationConfigImpl.prototype.getProperty
     * @function
     * @public
     * @param {string} name - The name of the property.
     * @param {string} def - The default value of the property in case it is not set.
     * @returns {string} The value of the property.
     */
    SpeechTranslationConfigImpl.prototype.getProperty = function (name, def) {
        return this.speechProperties.getProperty(name, def);
    };
    Object.defineProperty(SpeechTranslationConfigImpl.prototype, "properties", {
        /**
         * Provides access to custom properties.
         * @member SpeechTranslationConfigImpl.prototype.properties
         * @function
         * @public
         * @returns {PropertyCollection} The properties.
         */
        get: function () {
            return this.speechProperties;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Dispose of associated resources.
     * @member SpeechTranslationConfigImpl.prototype.close
     * @function
     * @public
     */
    SpeechTranslationConfigImpl.prototype.close = function () {
        return;
    };
    return SpeechTranslationConfigImpl;
}(SpeechTranslationConfig));
exports.SpeechTranslationConfigImpl = SpeechTranslationConfigImpl;



/***/ }),
/* 67 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
/**
 * Represents collection of properties and their values.
 * @class PropertyCollection
 */
var PropertyCollection = /** @class */ (function () {
    function PropertyCollection() {
        this.keys = [];
        this.values = [];
    }
    /**
     * Returns the property value in type String. The parameter must have the same type as String.
     * Currently only String, int and bool are allowed.
     * If the name is not available, the specified defaultValue is returned.
     * @member PropertyCollection.prototype.getProperty
     * @function
     * @public
     * @param {string} key - The parameter name.
     * @param {string} def - The default value which is returned if the parameter is not available in the collection.
     * @returns {string} value of the parameter.
     */
    PropertyCollection.prototype.getProperty = function (key, def) {
        var keyToUse;
        if (typeof key === "string") {
            keyToUse = key;
        }
        else {
            keyToUse = Exports_1.PropertyId[key];
        }
        for (var n = 0; n < this.keys.length; n++) {
            if (this.keys[n] === keyToUse) {
                return this.values[n];
            }
        }
        return def;
    };
    /**
     * Sets the String value of the parameter specified by name.
     * @member PropertyCollection.prototype.setProperty
     * @function
     * @public
     * @param {string} key - The parameter name.
     * @param {string} value - The value of the parameter.
     */
    PropertyCollection.prototype.setProperty = function (key, value) {
        var keyToUse;
        if (typeof key === "string") {
            keyToUse = key;
        }
        else {
            keyToUse = Exports_1.PropertyId[key];
        }
        for (var n = 0; n < this.keys.length; n++) {
            if (this.keys[n] === keyToUse) {
                this.values[n] = value;
                return;
            }
        }
        this.keys.push(keyToUse);
        this.values.push(value);
    };
    /**
     * Clones the collection.
     * @member PropertyCollection.prototype.clone
     * @function
     * @public
     * @returns {PropertyCollection} A copy of the collection.
     */
    PropertyCollection.prototype.clone = function () {
        var clonedMap = new PropertyCollection();
        for (var n = 0; n < this.keys.length; n++) {
            clonedMap.keys.push(this.keys[n]);
            clonedMap.values.push(this.values[n]);
        }
        return clonedMap;
    };
    return PropertyCollection;
}());
exports.PropertyCollection = PropertyCollection;



/***/ }),
/* 68 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Defines speech property ids.
 * @class PropertyId
 */
var PropertyId;
(function (PropertyId) {
    /**
     * Subscription key.
     * @member PropertyId.SpeechServiceConnection_Key
     */
    PropertyId[PropertyId["SpeechServiceConnection_Key"] = 0] = "SpeechServiceConnection_Key";
    /**
     * Endpoint.
     * @member PropertyId.SpeechServiceConnection_Endpoint
     */
    PropertyId[PropertyId["SpeechServiceConnection_Endpoint"] = 1] = "SpeechServiceConnection_Endpoint";
    /**
     * Region.
     * @member PropertyId.SpeechServiceConnection_Region
     */
    PropertyId[PropertyId["SpeechServiceConnection_Region"] = 2] = "SpeechServiceConnection_Region";
    /**
     * Authorization token.
     * @member PropertyId.SpeechServiceAuthorization_Token
     */
    PropertyId[PropertyId["SpeechServiceAuthorization_Token"] = 3] = "SpeechServiceAuthorization_Token";
    /**
     * Authorization type.
     * @member PropertyId.SpeechServiceAuthorization_Type
     */
    PropertyId[PropertyId["SpeechServiceAuthorization_Type"] = 4] = "SpeechServiceAuthorization_Type";
    /**
     * Endpoint ID.
     * @member PropertyId.SpeechServiceConnection_EndpointId
     */
    PropertyId[PropertyId["SpeechServiceConnection_EndpointId"] = 5] = "SpeechServiceConnection_EndpointId";
    /**
     * Translation to languages.
     * @member PropertyId.SpeechServiceConnection_TranslationToLanguages
     */
    PropertyId[PropertyId["SpeechServiceConnection_TranslationToLanguages"] = 6] = "SpeechServiceConnection_TranslationToLanguages";
    /**
     * Translation output voice.
     * @member PropertyId.SpeechServiceConnection_TranslationVoice
     */
    PropertyId[PropertyId["SpeechServiceConnection_TranslationVoice"] = 7] = "SpeechServiceConnection_TranslationVoice";
    /**
     * Translation features.
     * @member PropertyId.SpeechServiceConnection_TranslationFeatures
     */
    PropertyId[PropertyId["SpeechServiceConnection_TranslationFeatures"] = 8] = "SpeechServiceConnection_TranslationFeatures";
    /**
     * Intent region.
     * @member PropertyId.SpeechServiceConnection_IntentRegion
     */
    PropertyId[PropertyId["SpeechServiceConnection_IntentRegion"] = 9] = "SpeechServiceConnection_IntentRegion";
    /**
     * Recognition mode.
     * @member PropertyId.SpeechServiceConnection_RecoMode
     */
    PropertyId[PropertyId["SpeechServiceConnection_RecoMode"] = 10] = "SpeechServiceConnection_RecoMode";
    /**
     * Recognition language.
     * @member PropertyId.SpeechServiceConnection_RecoLanguage
     */
    PropertyId[PropertyId["SpeechServiceConnection_RecoLanguage"] = 11] = "SpeechServiceConnection_RecoLanguage";
    /**
     * Session id.
     * @member PropertyId.Speech_SessionId
     */
    PropertyId[PropertyId["Speech_SessionId"] = 12] = "Speech_SessionId";
    /**
     * Detailed result required.
     * @member PropertyId.SpeechServiceResponse_RequestDetailedResultTrueFalse
     */
    PropertyId[PropertyId["SpeechServiceResponse_RequestDetailedResultTrueFalse"] = 13] = "SpeechServiceResponse_RequestDetailedResultTrueFalse";
    /**
     * Profanity filtering required.
     * @member PropertyId.SpeechServiceResponse_RequestProfanityFilterTrueFalse
     */
    PropertyId[PropertyId["SpeechServiceResponse_RequestProfanityFilterTrueFalse"] = 14] = "SpeechServiceResponse_RequestProfanityFilterTrueFalse";
    /**
     * JSON in result.
     * @member PropertyId.SpeechServiceResponse_JsonResult
     */
    PropertyId[PropertyId["SpeechServiceResponse_JsonResult"] = 15] = "SpeechServiceResponse_JsonResult";
    /**
     * Error details.
     * @member PropertyId.SpeechServiceResponse_JsonErrorDetails
     */
    PropertyId[PropertyId["SpeechServiceResponse_JsonErrorDetails"] = 16] = "SpeechServiceResponse_JsonErrorDetails";
    /**
     * Cancellation reason.
     * @member PropertyId.CancellationDetails_Reason
     */
    PropertyId[PropertyId["CancellationDetails_Reason"] = 17] = "CancellationDetails_Reason";
    /**
     * Cancellation text.
     * @member PropertyId.CancellationDetails_ReasonText
     */
    PropertyId[PropertyId["CancellationDetails_ReasonText"] = 18] = "CancellationDetails_ReasonText";
    /**
     * Cancellation detailed text.
     * @member PropertyId.CancellationDetails_ReasonDetailedText
     */
    PropertyId[PropertyId["CancellationDetails_ReasonDetailedText"] = 19] = "CancellationDetails_ReasonDetailedText";
    /**
     * JSON result of language understanding service.
     * @member PropertyId.LanguageUnderstandingServiceResponse_JsonResult
     */
    PropertyId[PropertyId["LanguageUnderstandingServiceResponse_JsonResult"] = 20] = "LanguageUnderstandingServiceResponse_JsonResult";
})(PropertyId = exports.PropertyId || (exports.PropertyId = {}));



/***/ }),
/* 69 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(2);
var RecognitionEvents_1 = __webpack_require__(7);
var Exports_2 = __webpack_require__(1);
var Contracts_1 = __webpack_require__(4);
var Exports_3 = __webpack_require__(0);
/**
 * Defines the base class Recognizer which mainly contains common event handlers.
 * @class Recognizer
 */
var Recognizer = /** @class */ (function () {
    /**
     * Creates and initializes an instance of a Recognizer
     * @constructor
     * @param {AudioConfig} audioInput - An optional audio input stream associated with the recognizer
     */
    function Recognizer(audioConfig) {
        this.audioConfig = (audioConfig !== undefined) ? audioConfig : Exports_3.AudioConfig.fromDefaultMicrophoneInput();
        this.disposed = false;
    }
    /**
     * Dispose of associated resources.
     * @member Recognizer.prototype.close
     * @function
     * @public
     */
    Recognizer.prototype.close = function () {
        Contracts_1.Contracts.throwIfDisposed(this.disposed);
        this.dispose(true);
    };
    /**
     * This method performs cleanup of resources.
     * The Boolean parameter disposing indicates whether the method is called from Dispose (if disposing is true) or from the finalizer (if disposing is false).
     * Derived classes should override this method to dispose resource if needed.
     * @member Recognizer.prototype.dispose
     * @function
     * @public
     * @param {boolean} disposing - Flag to request disposal.
     */
    Recognizer.prototype.dispose = function (disposing) {
        if (this.disposed) {
            return;
        }
        if (disposing) {
            // disconnect
        }
        this.disposed = true;
    };
    // Setup the recognizer
    Recognizer.prototype.implRecognizerSetup = function (recognitionMode, speechProperties, audioConfig, speechConnectionFactory) {
        var recognizerConfig = this.CreateRecognizerConfig(new Exports_1.PlatformConfig(new Exports_1.Context(new Exports_1.OS("navigator.userAgent", "Browser", null), new Exports_1.Device("Microsoft", "SpeechSDK", "1.0.0"))), // TODO: Need to get these values from the caller?
        recognitionMode); // SDK.SpeechResultFormat.Simple (Options - Simple/Detailed)
        var subscriptionKey = speechProperties.getProperty(Exports_3.PropertyId.SpeechServiceConnection_Key, undefined);
        var authentication = subscriptionKey ?
            new Exports_1.CognitiveSubscriptionKeyAuthentication(subscriptionKey) :
            new Exports_1.CognitiveTokenAuthentication(function (authFetchEventId) {
                var authorizationToken = speechProperties.getProperty(Exports_3.PropertyId.SpeechServiceAuthorization_Token, undefined);
                return Exports_2.PromiseHelper.FromResult(authorizationToken);
            }, function (authFetchEventId) {
                var authorizationToken = speechProperties.getProperty(Exports_3.PropertyId.SpeechServiceAuthorization_Token, undefined);
                return Exports_2.PromiseHelper.FromResult(authorizationToken);
            });
        return this.CreateServiceRecognizer(authentication, speechConnectionFactory, audioConfig, recognizerConfig);
    };
    // Start the recognition
    Recognizer.prototype.implRecognizerStart = function (recognizer, cb, speechContext) {
        var _this = this;
        recognizer.Recognize(function (event) {
            if (_this.disposed) {
                return;
            }
            var sessionStartStopEventArgs;
            var speechStartStopEventArgs;
            /*
                Alternative syntax for typescript devs.
                if (event instanceof SDK.RecognitionTriggeredEvent)
            */
            // TODO: The mapping of internal service events to API surface events is... bad. Needs to be cleaned up to have a common mapping
            // that's understandable.
            switch (event.Name) {
                case "RecognitionTriggeredEvent":
                case "ListeningStartedEvent":
                    // Internal events, ignore
                    break;
                case "RecognitionStartedEvent":// Fires when the client connects to the service successfuly.
                    sessionStartStopEventArgs = new Exports_3.SessionEventArgs(event.SessionId);
                    if (!!_this.sessionStarted) {
                        _this.sessionStarted(_this, sessionStartStopEventArgs);
                    }
                    break;
                case "RecognitionEndedEvent":
                    var recoEndedEvent = event;
                    sessionStartStopEventArgs = new Exports_3.SessionEventArgs(recoEndedEvent.SessionId);
                    if (recoEndedEvent.Status !== RecognitionEvents_1.RecognitionCompletionStatus.Success) {
                        if (cb) {
                            cb(event); // call continuation, if configured.
                        }
                    }
                    if (!!_this.sessionStopped) {
                        _this.sessionStopped(_this, sessionStartStopEventArgs);
                    }
                    break;
                case "SpeechStartDetectedEvent":
                    speechStartStopEventArgs = new Exports_3.RecognitionEventArgs(0 /*TODO*/, event.SessionId);
                    if (!!_this.speechStartDetected) {
                        _this.speechStartDetected(_this, speechStartStopEventArgs);
                    }
                    break;
                case "SpeechEndDetectedEvent":
                    speechStartStopEventArgs = new Exports_3.RecognitionEventArgs(0 /*TODO*/, event.SessionId);
                    if (!!_this.speechEndDetected) {
                        _this.speechEndDetected(_this, speechStartStopEventArgs);
                    }
                    break;
                default:
                    if (cb) {
                        cb(event); // call continuation, if configured.
                    }
            }
        }, speechContext).On(
        /* tslint:disable:no-empty */
        function (result) { }, function (error) {
            if (!!cb) {
                // Internal error with service communication.
                var errorEvent = new RecognitionEvents_1.InternalErrorEvent(undefined, undefined, "Runtime error: " + error);
                cb(errorEvent);
            }
        });
    };
    return Recognizer;
}());
exports.Recognizer = Recognizer;



/***/ }),
/* 70 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(2);
var SpeechConnectionFactory_1 = __webpack_require__(18);
var Contracts_1 = __webpack_require__(4);
var Exports_2 = __webpack_require__(0);
/**
 * Performs speech recognition from microphone, file, or other audio input streams, and gets transcribed text as result.
 * @class SpeechRecognizer
 */
var SpeechRecognizer = /** @class */ (function (_super) {
    __extends(SpeechRecognizer, _super);
    /**
     * SpeechRecognizer constructor.
     * @constructor
     * @param {SpeechConfig} speechConfig - An set of initial properties for this recognizer
     * @param {AudioConfig} audioConfig - An optional audio configuration associated with the recognizer
     */
    function SpeechRecognizer(speechConfig, audioConfig) {
        var _this = _super.call(this, audioConfig) || this;
        _this.disposedSpeechRecognizer = false;
        var speechConfigImpl = speechConfig;
        Contracts_1.Contracts.throwIfNull(speechConfigImpl, "speechConfig");
        _this.privProperties = speechConfigImpl.properties.clone();
        Contracts_1.Contracts.throwIfNullOrWhitespace(speechConfigImpl.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage), Exports_2.PropertyId[Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage]);
        return _this;
    }
    Object.defineProperty(SpeechRecognizer.prototype, "endpointId", {
        /**
         * Gets the endpoint id of a customized speech model that is used for speech recognition.
         * @member SpeechRecognizer.prototype.endpointId
         * @function
         * @public
         * @returns {string} the endpoint id of a customized speech model that is used for speech recognition.
         */
        get: function () {
            Contracts_1.Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_EndpointId, "00000000-0000-0000-0000-000000000000");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechRecognizer.prototype, "authorizationToken", {
        /**
         * Gets the authorization token used to communicate with the service.
         * @member SpeechRecognizer.prototype.authorizationToken
         * @function
         * @public
         * @returns {string} Authorization token.
         */
        get: function () {
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token);
        },
        /**
         * Sets the authorization token used to communicate with the service.
         * @member SpeechRecognizer.prototype.authorizationToken
         * @function
         * @public
         * @param {string} token - Authorization token.
         */
        set: function (token) {
            Contracts_1.Contracts.throwIfNullOrWhitespace(token, "token");
            this.properties.setProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token, token);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechRecognizer.prototype, "speechRecognitionLanguage", {
        /**
         * Gets the spoken language of recognition.
         * @member SpeechRecognizer.prototype.speechRecognitionLanguage
         * @function
         * @public
         * @returns {string} The spoken language of recognition.
         */
        get: function () {
            Contracts_1.Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechRecognizer.prototype, "outputFormat", {
        /**
         * Gets the output format of recognition.
         * @member SpeechRecognizer.prototype.outputFormat
         * @function
         * @public
         * @returns {OutputFormat} The output format of recognition.
         */
        get: function () {
            Contracts_1.Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
            if (this.properties.getProperty(Exports_1.OutputFormatPropertyName, Exports_2.OutputFormat[Exports_2.OutputFormat.Simple]) === Exports_2.OutputFormat[Exports_2.OutputFormat.Simple]) {
                return Exports_2.OutputFormat.Simple;
            }
            else {
                return Exports_2.OutputFormat.Detailed;
            }
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SpeechRecognizer.prototype, "properties", {
        /**
         * The collection of properties and their values defined for this SpeechRecognizer.
         * @member SpeechRecognizer.prototype.properties
         * @function
         * @public
         * @returns {PropertyCollection} The collection of properties and their values defined for this SpeechRecognizer.
         */
        get: function () {
            return this.privProperties;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Starts speech recognition, and stops after the first utterance is recognized. The task returns the recognition text as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition
     *       like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @member SpeechRecognizer.prototype.recognizeOnceAsync
     * @function
     * @public
     * @param cb - Callback that received the SpeechRecognitionResult.
     * @param err - Callback invoked in case of an error.
     */
    SpeechRecognizer.prototype.recognizeOnceAsync = function (cb, err) {
        var _this = this;
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
            this.implCloseExistingRecognizer();
            this.reco = this.implRecognizerSetup(Exports_1.RecognitionMode.Interactive, this.properties, this.audioConfig, new SpeechConnectionFactory_1.SpeechConnectionFactory());
            this.implRecognizerStart(this.reco, function (event) {
                if (_this.disposedSpeechRecognizer || !_this.reco) {
                    return;
                }
                _this.implDispatchMessageHandler(event, cb, err);
            });
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * Starts speech recognition, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @member SpeechRecognizer.prototype.startContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    SpeechRecognizer.prototype.startContinuousRecognitionAsync = function (cb, err) {
        var _this = this;
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
            this.implCloseExistingRecognizer();
            this.reco = this.implRecognizerSetup(Exports_1.RecognitionMode.Conversation, this.properties, this.audioConfig, new SpeechConnectionFactory_1.SpeechConnectionFactory());
            this.implRecognizerStart(this.reco, function (event) {
                if (_this.disposedSpeechRecognizer || !_this.reco) {
                    return;
                }
                _this.implDispatchMessageHandler(event, undefined, undefined);
            });
            // report result to promise.
            if (!!cb) {
                try {
                    cb();
                }
                catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
                cb = undefined;
            }
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * Stops continuous speech recognition.
     * @member SpeechRecognizer.prototype.stopContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    SpeechRecognizer.prototype.stopContinuousRecognitionAsync = function (cb, err) {
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
            this.implCloseExistingRecognizer();
            if (!!cb) {
                try {
                    cb();
                }
                catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
            }
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * Starts speech recognition with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Speech Devices SDK. This functionality is currently not included in the SDK itself.
     * @member SpeechRecognizer.prototype.startKeywordRecognitionAsync
     * @function
     * @public
     * @param {KeywordRecognitionModel} model The keyword recognition model that specifies the keyword to be recognized.
     * @param cb - Callback invoked once the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    SpeechRecognizer.prototype.startKeywordRecognitionAsync = function (model, cb, err) {
        Contracts_1.Contracts.throwIfNull(model, "model");
        if (!!err) {
            err("Not yet implemented.");
        }
    };
    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Speech Devices SDK. This functionality is currently not included in the SDK itself.
     * @member SpeechRecognizer.prototype.stopKeywordRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    SpeechRecognizer.prototype.stopKeywordRecognitionAsync = function (cb, err) {
        if (!!cb) {
            cb();
        }
    };
    /**
     * closes all external resources held by an instance of this class.
     * @member SpeechRecognizer.prototype.close
     * @function
     * @public
     */
    SpeechRecognizer.prototype.close = function () {
        Contracts_1.Contracts.throwIfDisposed(this.disposedSpeechRecognizer);
        this.dispose(true);
    };
    /**
     * Disposes any resources held by the object.
     * @member SpeechRecognizer.prototype.dispose
     * @function
     * @public
     * @param {boolean} disposing - true if disposing the object.
     */
    SpeechRecognizer.prototype.dispose = function (disposing) {
        if (this.disposedSpeechRecognizer) {
            return;
        }
        if (disposing) {
            this.implCloseExistingRecognizer();
            this.disposedSpeechRecognizer = true;
        }
        _super.prototype.dispose.call(this, disposing);
    };
    SpeechRecognizer.prototype.CreateRecognizerConfig = function (speechConfig, recognitionMode) {
        return new Exports_1.RecognizerConfig(speechConfig, recognitionMode, this.properties);
    };
    SpeechRecognizer.prototype.CreateServiceRecognizer = function (authentication, connectionFactory, audioConfig, recognizerConfig) {
        var configImpl = audioConfig;
        return new Exports_1.SpeechServiceRecognizer(authentication, connectionFactory, configImpl, recognizerConfig);
    };
    SpeechRecognizer.prototype.implCloseExistingRecognizer = function () {
        if (this.reco) {
            this.reco.AudioSource.TurnOff();
            this.reco.Dispose();
            this.reco = undefined;
        }
    };
    SpeechRecognizer.prototype.implDispatchMessageHandler = function (event, cb, err) {
        /*
         Alternative syntax for typescript devs.
         if (event instanceof SDK.RecognitionTriggeredEvent)
        */
        switch (event.Name) {
            case "RecognitionEndedEvent":
                {
                    var recoEndedEvent = event;
                    if (recoEndedEvent.Status !== Exports_1.RecognitionCompletionStatus.Success) {
                        var errorText = Exports_1.RecognitionCompletionStatus[recoEndedEvent.Status] + ": " + recoEndedEvent.Error;
                        var errorEvent = new Exports_2.SpeechRecognitionCanceledEventArgs(Exports_2.CancellationReason.Error, errorText, 0, /*todo*/ recoEndedEvent.SessionId);
                        if (this.canceled) {
                            try {
                                this.canceled(this, errorEvent);
                                /* tslint:disable:no-empty */
                            }
                            catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                        var result = new Exports_2.SpeechRecognitionResult(undefined, Exports_2.ResultReason.Canceled, undefined, undefined, undefined, errorText, undefined, undefined);
                        // report result to promise.
                        if (!!cb) {
                            try {
                                cb(result);
                                /* tslint:disable:no-empty */
                            }
                            catch (e) {
                                if (!!err) {
                                    err(e);
                                }
                            }
                        }
                    }
                }
                break;
            case "SpeechSimplePhraseEvent":
                {
                    var evResult = event;
                    var reason = Exports_1.EnumTranslation.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    var result = new Exports_2.SpeechRecognitionResult(undefined, reason, evResult.Result.DisplayText, evResult.Result.Duration, evResult.Result.Offset, undefined, JSON.stringify(evResult.Result), undefined);
                    if (reason === Exports_2.ResultReason.Canceled) {
                        var ev = new Exports_2.SpeechRecognitionCanceledEventArgs(Exports_1.EnumTranslation.implTranslateCancelResult(evResult.Result.RecognitionStatus), undefined, 0, /*todo*/ evResult.SessionId);
                        if (!!this.canceled) {
                            try {
                                this.canceled(this, ev);
                                /* tslint:disable:no-empty */
                            }
                            catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                    }
                    else {
                        var ev = new Exports_2.SpeechRecognitionEventArgs(result, 0 /*todo*/, evResult.SessionId);
                        if (!!this.recognized) {
                            try {
                                this.recognized(this, ev);
                                /* tslint:disable:no-empty */
                            }
                            catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                    }
                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(result);
                        }
                        catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke the
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
            case "SpeechDetailedPhraseEvent":
                {
                    var evResult = event;
                    var reason = Exports_1.EnumTranslation.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    var result = new Exports_2.SpeechRecognitionResult(undefined, reason, (reason === Exports_2.ResultReason.RecognizedSpeech) ? evResult.Result.NBest[0].Display : undefined, evResult.Result.Duration, evResult.Result.Offset, undefined, JSON.stringify(evResult.Result), undefined);
                    if (reason === Exports_2.ResultReason.Canceled) {
                        var ev = new Exports_2.SpeechRecognitionCanceledEventArgs(Exports_1.EnumTranslation.implTranslateCancelResult(evResult.Result.RecognitionStatus), undefined, 0, /*todo*/ evResult.SessionId);
                        if (!!this.canceled) {
                            try {
                                this.canceled(this, ev);
                                /* tslint:disable:no-empty */
                            }
                            catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                    }
                    else {
                        var ev = new Exports_2.SpeechRecognitionEventArgs(result, 0 /*todo*/, evResult.SessionId);
                        if (!!this.recognized) {
                            try {
                                this.recognized(this, ev);
                                /* tslint:disable:no-empty */
                            }
                            catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                    }
                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(result);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "SpeechHypothesisEvent":
                {
                    var evResult = event;
                    var result = new Exports_2.SpeechRecognitionResult(undefined, Exports_2.ResultReason.RecognizingSpeech, evResult.Result.Text, evResult.Result.Duration, evResult.Result.Offset, undefined, JSON.stringify(evResult.Result), undefined);
                    var ev = new Exports_2.SpeechRecognitionEventArgs(result, 0 /*todo*/, evResult.SessionId);
                    if (!!this.recognizing) {
                        try {
                            this.recognizing(this, ev);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "InternalErrorEvent":
                {
                    var evResult = event;
                    var result = new Exports_2.SpeechRecognitionResult(evResult.RequestId, Exports_2.ResultReason.Canceled, undefined, undefined, undefined, evResult.Result);
                    var canceledResult = new Exports_2.SpeechRecognitionCanceledEventArgs(Exports_2.CancellationReason.Error, result.errorDetails);
                    try {
                        this.canceled(this, canceledResult);
                        /* tslint:disable:no-empty */
                    }
                    catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }
                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(result);
                        }
                        catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke thebundle
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
        }
    };
    return SpeechRecognizer;
}(Exports_2.Recognizer));
exports.SpeechRecognizer = SpeechRecognizer;



/***/ }),
/* 71 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(2);
var Contracts_1 = __webpack_require__(4);
var Exports_2 = __webpack_require__(0);
/**
 * Intent recognizer.
 * @class
 */
var IntentRecognizer = /** @class */ (function (_super) {
    __extends(IntentRecognizer, _super);
    /**
     * Initializes an instance of the IntentRecognizer.
     * @constructor
     * @param {SpeechConfig} speechConfig - The set of configuration properties.
     * @param {AudioConfig} audioConfig - An optional audio input config associated with the recognizer
     */
    function IntentRecognizer(speechConfig, audioConfig) {
        var _this = this;
        Contracts_1.Contracts.throwIfNullOrUndefined(speechConfig, "speechConfig");
        var configImpl = speechConfig;
        Contracts_1.Contracts.throwIfNullOrUndefined(configImpl, "speechConfig");
        _this = _super.call(this, audioConfig) || this;
        _this.intentDataSent = false;
        _this.addedIntents = [];
        _this.addedLmIntents = {};
        _this.disposedIntentRecognizer = false;
        _this.privProperties = configImpl.properties;
        Contracts_1.Contracts.throwIfNullOrWhitespace(_this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage), Exports_2.PropertyId[Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage]);
        return _this;
    }
    Object.defineProperty(IntentRecognizer.prototype, "speechRecognitionLanguage", {
        /**
         * Gets the spoken language of recognition.
         * @member IntentRecognizer.prototype.speechRecognitionLanguage
         * @function
         * @public
         * @returns {string} the spoken language of recognition.
         */
        get: function () {
            Contracts_1.Contracts.throwIfDisposed(this.disposedIntentRecognizer);
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(IntentRecognizer.prototype, "authorizationToken", {
        /**
         * Gets the authorization token used to communicate with the service.
         * @member IntentRecognizer.prototype.authorizationToken
         * @function
         * @public
         * @returns {string} Authorization token.
         */
        get: function () {
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token);
        },
        /**
         * Sets the authorization token used to communicate with the service.
         * @member IntentRecognizer.prototype.authorizationToken
         * @function
         * @public
         * @param {string} value - Authorization token.
         */
        set: function (value) {
            this.properties.setProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token, value);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(IntentRecognizer.prototype, "properties", {
        /**
         * The collection of properties and their values defined for this IntentRecognizer.
         * @member IntentRecognizer.prototype.properties
         * @function
         * @public
         * @returns {PropertyCollection} The collection of properties and their values defined for this IntentRecognizer.
         */
        get: function () {
            return this.privProperties;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Starts intent recognition, and stops after the first utterance is recognized. The task returns the recognition text and intent as result.
     * Note: RecognizeOnceAsync() returns when the first utterance has been recognized, so it is suitable only for single shot recognition like command or query. For long-running recognition, use StartContinuousRecognitionAsync() instead.
     * @member IntentRecognizer.prototype.recognizeOnceAsync
     * @function
     * @public
     * @param cb - Callback that received the recognition has finished with an IntentRecognitionResult.
     * @param err - Callback invoked in case of an error.
     */
    IntentRecognizer.prototype.recognizeOnceAsync = function (cb, err) {
        var _this = this;
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedIntentRecognizer);
            this.implCloseExistingRecognizer();
            this.reco = this.implRecognizerSetup(Exports_1.RecognitionMode.Interactive, this.properties, this.audioConfig, new Exports_1.IntentConnectionFactory());
            var contextJson = void 0;
            if (Object.keys(this.addedLmIntents).length !== 0 || undefined !== this.umbrellaIntent) {
                contextJson = this.buildSpeechContext();
                this.intentDataSent = true;
            }
            this.implRecognizerStart(this.reco, function (event) {
                if (_this.disposedIntentRecognizer || !_this.reco) {
                    return;
                }
                _this.implDispatchMessageHandler(event, cb, err);
            }, contextJson);
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * Starts speech recognition, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * @member IntentRecognizer.prototype.startContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    IntentRecognizer.prototype.startContinuousRecognitionAsync = function (cb, err) {
        var _this = this;
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedIntentRecognizer);
            this.implCloseExistingRecognizer();
            this.reco = this.implRecognizerSetup(Exports_1.RecognitionMode.Conversation, this.properties, this.audioConfig, new Exports_1.IntentConnectionFactory());
            var contextJson = void 0;
            if (Object.keys(this.addedLmIntents).length !== 0) {
                contextJson = this.buildSpeechContext();
                this.intentDataSent = true;
            }
            this.implRecognizerStart(this.reco, function (event) {
                if (_this.disposedIntentRecognizer || !_this.reco) {
                    return;
                }
                _this.implDispatchMessageHandler(event, undefined, undefined);
            }, contextJson);
            // report result to promise.
            if (!!cb) {
                try {
                    cb();
                }
                catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
                cb = undefined;
            }
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * Stops continuous intent recognition.
     * @member IntentRecognizer.prototype.stopContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    IntentRecognizer.prototype.stopContinuousRecognitionAsync = function (cb, err) {
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedIntentRecognizer);
            this.implCloseExistingRecognizer();
            if (!!cb) {
                try {
                    cb();
                }
                catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
            }
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * Starts speech recognition with keyword spotting, until stopKeywordRecognitionAsync() is called.
     * User must subscribe to events to receive recognition results.
     * Note: Key word spotting functionality is only available on the Speech Devices SDK. This functionality is currently not included in the SDK itself.
     * @member IntentRecognizer.prototype.startKeywordRecognitionAsync
     * @function
     * @public
     * @param {KeywordRecognitionModel} model - The keyword recognition model that specifies the keyword to be recognized.
     * @param cb - Callback invoked once the recognition has started.
     * @param err - Callback invoked in case of an error.
     */
    IntentRecognizer.prototype.startKeywordRecognitionAsync = function (model, cb, err) {
        Contracts_1.Contracts.throwIfNull(model, "model");
        if (!!err) {
            err("Not yet implemented.");
        }
    };
    /**
     * Stops continuous speech recognition.
     * Note: Key word spotting functionality is only available on the Speech Devices SDK. This functionality is currently not included in the SDK itself.
     * @member IntentRecognizer.prototype.stopKeywordRecognitionAsync
     * @function
     * @public
     * @param cb - Callback invoked once the recognition has stopped.
     * @param err - Callback invoked in case of an error.
     */
    IntentRecognizer.prototype.stopKeywordRecognitionAsync = function (cb, err) {
        if (!!cb) {
            cb();
        }
    };
    /**
     * Adds a phrase that should be recognized as intent.
     * @member IntentRecognizer.prototype.addIntent
     * @function
     * @public
     * @param {string} intentId - A String that represents the identifier of the intent to be recognized.
     * @param {string} phrase - A String that specifies the phrase representing the intent.
     */
    IntentRecognizer.prototype.addIntent = function (simplePhrase, intentId) {
        Contracts_1.Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        Contracts_1.Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        Contracts_1.Contracts.throwIfNullOrWhitespace(simplePhrase, "simplePhrase");
        this.addedIntents.push([intentId, simplePhrase]);
    };
    /**
     * Adds an intent from Language Understanding service for recognition.
     * @member IntentRecognizer.prototype.addIntentWithLanguageModel
     * @function
     * @public
     * @param {string} intentId - A String that represents the identifier of the intent to be recognized. Ignored if intentName is empty.
     * @param {string} model - The intent model from Language Understanding service.
     * @param {string} intentName - The intent name defined in the intent model. If it is empty, all intent names defined in the model will be added.
     */
    IntentRecognizer.prototype.addIntentWithLanguageModel = function (intentId, model, intentName) {
        Contracts_1.Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        Contracts_1.Contracts.throwIfNullOrWhitespace(intentId, "intentId");
        Contracts_1.Contracts.throwIfNull(model, "model");
        var modelImpl = model;
        Contracts_1.Contracts.throwIfNullOrWhitespace(modelImpl.appId, "model.appId");
        this.addedLmIntents[intentId] = new AddedLmIntent(modelImpl, intentName);
    };
    /**
     * @summary Adds all intents from the specified Language Understanding Model.
     * @member IntentRecognizer.prototype.addAllIntents
     * @function
     * @public
     * @function
     * @public
     * @param {LanguageUnderstandingModel} model - The language understanding model containing the intents.
     * @param {string} intentId - A custom id String to be returned in the IntentRecognitionResult's getIntentId() method.
     */
    IntentRecognizer.prototype.addAllIntents = function (model, intentId) {
        Contracts_1.Contracts.throwIfNull(model, "model");
        var modelImpl = model;
        Contracts_1.Contracts.throwIfNullOrWhitespace(modelImpl.appId, "model.appId");
        this.umbrellaIntent = new AddedLmIntent(modelImpl, intentId);
    };
    /**
     * closes all external resources held by an instance of this class.
     * @member IntentRecognizer.prototype.close
     * @function
     * @public
     */
    IntentRecognizer.prototype.close = function () {
        Contracts_1.Contracts.throwIfDisposed(this.disposedIntentRecognizer);
        this.dispose(true);
    };
    IntentRecognizer.prototype.CreateRecognizerConfig = function (speecgConfig, recognitionMode) {
        return new Exports_1.RecognizerConfig(speecgConfig, recognitionMode, this.properties);
    };
    IntentRecognizer.prototype.CreateServiceRecognizer = function (authentication, connectionFactory, audioConfig, recognizerConfig) {
        var audioImpl = audioConfig;
        return new Exports_1.IntentServiceRecognizer(authentication, connectionFactory, audioImpl, recognizerConfig);
    };
    IntentRecognizer.prototype.dispose = function (disposing) {
        if (this.disposedIntentRecognizer) {
            return;
        }
        if (disposing) {
            this.disposedIntentRecognizer = true;
            _super.prototype.dispose.call(this, disposing);
        }
    };
    IntentRecognizer.prototype.implDispatchMessageHandler = function (event, cb, err) {
        var _this = this;
        /*
         Alternative syntax for typescript devs.
         if (event instanceof SDK.RecognitionTriggeredEvent)
        */
        switch (event.Name) {
            case "RecognitionEndedEvent":
                {
                    var recoEndedEvent = event;
                    if (recoEndedEvent.Status !== Exports_1.RecognitionCompletionStatus.Success) {
                        var result = new Exports_2.IntentRecognitionResult(undefined, undefined, Exports_2.ResultReason.Canceled, undefined, undefined, undefined, Exports_1.RecognitionCompletionStatus[recoEndedEvent.Status] + ": " + recoEndedEvent.Error, undefined, undefined);
                        var errorEvent = new Exports_2.IntentRecognitionCanceledEventArgs(Exports_2.CancellationReason.Error, recoEndedEvent.Error, result, 0, recoEndedEvent.SessionId);
                        if (this.canceled) {
                            try {
                                this.canceled(this, errorEvent);
                                /* tslint:disable:no-empty */
                            }
                            catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                        // report result to promise.
                        if (!!cb) {
                            try {
                                cb(result);
                                /* tslint:disable:no-empty */
                            }
                            catch (e) {
                                if (!!err) {
                                    err(e);
                                }
                            }
                        }
                    }
                    break;
                }
            case "SpeechSimplePhraseEvent":
                {
                    var evResult = event;
                    var reason = Exports_1.EnumTranslation.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                    var result = new Exports_2.IntentRecognitionResult(undefined, undefined, reason, evResult.Result.DisplayText, evResult.Result.Duration, evResult.Result.Offset, undefined, JSON.stringify(evResult.Result), undefined);
                    if (reason === Exports_2.ResultReason.Canceled) {
                        var ev = new Exports_2.IntentRecognitionCanceledEventArgs(Exports_1.EnumTranslation.implTranslateCancelResult(evResult.Result.RecognitionStatus), "", result, 0, evResult.SessionId);
                        if (!!this.canceled) {
                            try {
                                this.canceled(this, ev);
                                /* tslint:disable:no-empty */
                            }
                            catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                        // report result to promise.
                        if (!!cb) {
                            try {
                                cb(result);
                                /* tslint:disable:no-empty */
                            }
                            catch (e) {
                                if (!!err) {
                                    err(e);
                                }
                            }
                        }
                    }
                    else {
                        var ev_1 = new Exports_2.IntentRecognitionEventArgs(result, 0 /*TODO*/, evResult.SessionId);
                        var sendEvent = function () {
                            if (!!_this.recognized) {
                                try {
                                    _this.recognized(_this, ev_1);
                                    /* tslint:disable:no-empty */
                                }
                                catch (error) {
                                    // Not going to let errors in the event handler
                                    // trip things up.
                                }
                            }
                            // report result to promise.
                            if (!!cb) {
                                try {
                                    cb(ev_1.result);
                                }
                                catch (e) {
                                    if (!!err) {
                                        err(e);
                                    }
                                }
                                // Only invoke the call back once.
                                // and if it's successful don't invoke the
                                // error after that.
                                cb = undefined;
                                err = undefined;
                            }
                        };
                        var status_1 = Exports_1.RecognitionStatus2[evResult.Result.RecognitionStatus];
                        if (status_1 === Exports_1.RecognitionStatus2.InitialSilenceTimeout ||
                            status_1 === Exports_1.RecognitionStatus2.BabbleTimeout) {
                            ev_1 = new Exports_2.IntentRecognitionEventArgs(new Exports_2.IntentRecognitionResult(ev_1.result.intentId, ev_1.result.resultId, Exports_2.ResultReason.NoMatch, ev_1.result.text, ev_1.result.duration, ev_1.result.offset, ev_1.result.errorDetails, ev_1.result.json, ev_1.result.properties), ev_1.offset, ev_1.sessionId);
                        }
                        // If intent data was sent, the terminal result for this recognizer is an intent being found.
                        // If no intent data was sent, the terminal event is speech recognition being successful.
                        if (false === this.intentDataSent || Exports_2.ResultReason.NoMatch === ev_1.result.reason) {
                            sendEvent();
                        }
                        else {
                            // Squirrel away the args, when the response event arrives it will build upon them
                            // and then return
                            this.pendingIntentArgs = ev_1;
                            //// Also, set a one minute delay, if the response hasn't come back, or more speech events do, move on.
                            // setTimeout(() => {
                            //    if (undefined !== this.pendingIntentArgs && !this.disposedIntentRecognizer) {
                            //        sendEvent();
                            //        this.pendingIntentArgs = undefined;
                            //    }
                            // }, 60000);
                        }
                    }
                }
                break;
            case "SpeechHypothesisEvent":
                {
                    var evResult = event;
                    var result = new Exports_2.IntentRecognitionResult(undefined, undefined, Exports_2.ResultReason.RecognizingIntent, evResult.Result.Text, evResult.Result.Duration, evResult.Result.Offset, undefined, JSON.stringify(evResult.Result), undefined);
                    var ev = new Exports_2.IntentRecognitionEventArgs(result, 0 /*TODO*/, evResult.SessionId);
                    if (!!this.recognizing) {
                        try {
                            this.recognizing(this, ev);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "IntentResponseEvent":
                {
                    var ev = this.pendingIntentArgs;
                    this.pendingIntentArgs = undefined;
                    var evResult = event;
                    if (undefined === ev) {
                        // Odd... Not sure this can happen
                        ev = new Exports_2.IntentRecognitionEventArgs(new Exports_2.IntentRecognitionResult(), 0 /*TODO*/, evResult.SessionId);
                    }
                    // If LUIS didn't return anything, send the existing event, else
                    // modify it to show the match.
                    // See if the intent found is in the list of intents asked for.
                    var addedIntent = this.addedLmIntents[evResult.Result.topScoringIntent.intent];
                    if (this.umbrellaIntent !== undefined) {
                        addedIntent = this.umbrellaIntent;
                    }
                    if (null !== evResult.Result && addedIntent !== undefined) {
                        var intentId = addedIntent.intentName === undefined ? evResult.Result.topScoringIntent.intent : addedIntent.intentName;
                        var reason = ev.result.reason;
                        if (undefined !== intentId) {
                            reason = Exports_2.ResultReason.RecognizedIntent;
                        }
                        // make sure, properties is set.
                        var properties = (undefined !== ev.result.properties) ?
                            ev.result.properties : new Exports_2.PropertyCollection();
                        properties.setProperty(Exports_2.PropertyId.LanguageUnderstandingServiceResponse_JsonResult, JSON.stringify(evResult.Result));
                        ev = new Exports_2.IntentRecognitionEventArgs(new Exports_2.IntentRecognitionResult(intentId, ev.result.resultId, reason, ev.result.text, ev.result.duration, ev.result.offset, ev.result.errorDetails, ev.result.json, properties), ev.offset, ev.sessionId);
                    }
                    if (!!this.recognized) {
                        try {
                            this.recognized(this, ev);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(ev.result);
                        }
                        catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke the
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
            case "InternalErrorEvent":
                {
                    var evResult = event;
                    var result = new Exports_2.IntentRecognitionResult(undefined, undefined, Exports_2.ResultReason.Canceled, undefined, undefined, undefined, evResult.Result);
                    var canceledResult = new Exports_2.IntentRecognitionCanceledEventArgs(Exports_2.CancellationReason.Error, result.errorDetails, result);
                    try {
                        this.canceled(this, canceledResult);
                        /* tslint:disable:no-empty */
                    }
                    catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }
                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(result);
                        }
                        catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke thebundle
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
        }
    };
    IntentRecognizer.prototype.implCloseExistingRecognizer = function () {
        if (this.reco) {
            this.reco.AudioSource.TurnOff();
            this.reco.Dispose();
            this.reco = undefined;
        }
    };
    IntentRecognizer.prototype.buildSpeechContext = function () {
        var appId;
        var region;
        var subscriptionKey;
        var refGrammers = [];
        if (undefined !== this.umbrellaIntent) {
            appId = this.umbrellaIntent.modelImpl.appId;
            region = this.umbrellaIntent.modelImpl.region;
            subscriptionKey = this.umbrellaIntent.modelImpl.subscriptionKey;
        }
        // Build the reference grammer array.
        for (var _i = 0, _a = Object.keys(this.addedLmIntents); _i < _a.length; _i++) {
            var intentId = _a[_i];
            var addedLmIntent = this.addedLmIntents[intentId];
            // validate all the same model, region, and key...
            if (appId === undefined) {
                appId = addedLmIntent.modelImpl.appId;
            }
            else {
                if (appId !== addedLmIntent.modelImpl.appId) {
                    throw new Error("Intents must all be from the same LUIS model");
                }
            }
            if (region === undefined) {
                region = addedLmIntent.modelImpl.region;
            }
            else {
                if (region !== addedLmIntent.modelImpl.region) {
                    throw new Error("Intents must all be from the same LUIS model in a single region");
                }
            }
            if (subscriptionKey === undefined) {
                subscriptionKey = addedLmIntent.modelImpl.subscriptionKey;
            }
            else {
                if (subscriptionKey !== addedLmIntent.modelImpl.subscriptionKey) {
                    throw new Error("Intents must all use the same subscription key");
                }
            }
            var grammer = "luis/" + appId + "-PRODUCTION#" + intentId;
            refGrammers.push(grammer);
        }
        return JSON.stringify({
            dgi: {
                ReferenceGrammars: (undefined === this.umbrellaIntent) ? refGrammers : ["luis/" + appId + "-PRODUCTION"],
            },
            intent: {
                id: appId,
                key: (subscriptionKey === undefined) ? this.privProperties.getProperty(Exports_2.PropertyId[Exports_2.PropertyId.SpeechServiceConnection_Key]) : subscriptionKey,
                provider: "LUIS",
            },
        });
    };
    return IntentRecognizer;
}(Exports_2.Recognizer));
exports.IntentRecognizer = IntentRecognizer;
/**
 * @class AddedLmIntent
 */
// tslint:disable-next-line:max-classes-per-file
var AddedLmIntent = /** @class */ (function () {
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param modelImpl - The model.
     * @param intentName - The intent name.
     */
    function AddedLmIntent(modelImpl, intentName) {
        this.modelImpl = modelImpl;
        this.intentName = intentName;
    }
    return AddedLmIntent;
}());



/***/ }),
/* 72 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(2);
var Contracts_1 = __webpack_require__(4);
var Exports_2 = __webpack_require__(0);
/**
 * Translation recognizer
 * @class TranslationRecognizer
 */
var TranslationRecognizer = /** @class */ (function (_super) {
    __extends(TranslationRecognizer, _super);
    /**
     * Initializes an instance of the TranslationRecognizer.
     * @constructor
     * @param {SpeechTranslationConfig} speechConfig - Set of properties to configure this recognizer.
     * @param {AudioConfig} audioConfig - An optional audio config associated with the recognizer
     */
    function TranslationRecognizer(speechConfig, audioConfig) {
        var _this = this;
        var configImpl = speechConfig;
        Contracts_1.Contracts.throwIfNull(configImpl, "speechConfig");
        _this = _super.call(this, audioConfig) || this;
        _this.disposedTranslationRecognizer = false;
        _this.privProperties = configImpl.properties.clone();
        if (_this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationVoice, undefined) !== undefined) {
            Contracts_1.Contracts.throwIfNullOrWhitespace(_this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationVoice), Exports_2.PropertyId[Exports_2.PropertyId.SpeechServiceConnection_TranslationVoice]);
        }
        Contracts_1.Contracts.throwIfNullOrWhitespace(_this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationToLanguages), Exports_2.PropertyId[Exports_2.PropertyId.SpeechServiceConnection_TranslationToLanguages]);
        Contracts_1.Contracts.throwIfNullOrWhitespace(_this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage), Exports_2.PropertyId[Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage]);
        return _this;
    }
    Object.defineProperty(TranslationRecognizer.prototype, "speechRecognitionLanguage", {
        /**
         * Gets the language name that was set when the recognizer was created.
         * @member TranslationRecognizer.prototype.speechRecognitionLanguage
         * @function
         * @public
         * @returns {string} Gets the language name that was set when the recognizer was created.
         */
        get: function () {
            Contracts_1.Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_RecoLanguage);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(TranslationRecognizer.prototype, "targetLanguages", {
        /**
         * Gets target languages for translation that were set when the recognizer was created.
         * The language is specified in BCP-47 format. The translation will provide translated text for each of language.
         * @member TranslationRecognizer.prototype.targetLanguages
         * @function
         * @public
         * @returns {string[]} Gets target languages for translation that were set when the recognizer was created.
         */
        get: function () {
            Contracts_1.Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationToLanguages).split(",");
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(TranslationRecognizer.prototype, "voiceName", {
        /**
         * Gets the name of output voice.
         * @member TranslationRecognizer.prototype.voiceName
         * @function
         * @public
         * @returns {string} the name of output voice.
         */
        get: function () {
            Contracts_1.Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceConnection_TranslationVoice, undefined);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(TranslationRecognizer.prototype, "authorizationToken", {
        /**
         * Gets the authorization token used to communicate with the service.
         * @member TranslationRecognizer.prototype.authorizationToken
         * @function
         * @public
         * @returns {string} Authorization token.
         */
        get: function () {
            return this.properties.getProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token);
        },
        /**
         * Sets the authorization token used to communicate with the service.
         * @member TranslationRecognizer.prototype.authorizationToken
         * @function
         * @public
         * @param {string} value - Authorization token.
         */
        set: function (value) {
            this.properties.setProperty(Exports_2.PropertyId.SpeechServiceAuthorization_Token, value);
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(TranslationRecognizer.prototype, "properties", {
        /**
         * The collection of properties and their values defined for this TranslationRecognizer.
         * @member TranslationRecognizer.prototype.properties
         * @function
         * @public
         * @returns {PropertyCollection} The collection of properties and their values defined for this TranslationRecognizer.
         */
        get: function () {
            return this.privProperties;
        },
        enumerable: true,
        configurable: true
    });
    /**
     * Starts recognition and translation, and stops after the first utterance is recognized. The task returns the translation text as result.
     * Note: recognizeOnceAsync returns when the first utterance has been recognized, so it is suitableonly
     *       for single shot recognition like command or query. For long-running recognition, use startContinuousRecognitionAsync() instead.
     * @member TranslationRecognizer.prototype.recognizeOnceAsync
     * @function
     * @public
     * @param cb - Callback that received the result when the translation has completed.
     * @param err - Callback invoked in case of an error.
     */
    TranslationRecognizer.prototype.recognizeOnceAsync = function (cb, err) {
        var _this = this;
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
            this.implCloseExistingRecognizer();
            this.reco = this.implRecognizerSetup(Exports_1.RecognitionMode.Conversation, this.properties, this.audioConfig, new Exports_1.TranslationConnectionFactory());
            this.implRecognizerStart(this.reco, function (event) {
                if (_this.disposedTranslationRecognizer || !_this.reco) {
                    return;
                }
                _this.implDispatchMessageHandler(event, cb, err);
            });
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * Starts recognition and translation, until stopContinuousRecognitionAsync() is called.
     * User must subscribe to events to receive translation results.
     * @member TranslationRecognizer.prototype.startContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback that received the translation has started.
     * @param err - Callback invoked in case of an error.
     */
    TranslationRecognizer.prototype.startContinuousRecognitionAsync = function (cb, err) {
        var _this = this;
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
            this.implCloseExistingRecognizer();
            this.reco = this.implRecognizerSetup(Exports_1.RecognitionMode.Conversation, this.properties, this.audioConfig, new Exports_1.TranslationConnectionFactory());
            this.implRecognizerStart(this.reco, function (event) {
                if (_this.disposedTranslationRecognizer || !_this.reco) {
                    return;
                }
                _this.implDispatchMessageHandler(event, undefined, undefined);
            });
            // report result to promise.
            if (!!cb) {
                try {
                    cb();
                }
                catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
                cb = undefined;
            }
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * Stops continuous recognition and translation.
     * @member TranslationRecognizer.prototype.stopContinuousRecognitionAsync
     * @function
     * @public
     * @param cb - Callback that received the translation has stopped.
     * @param err - Callback invoked in case of an error.
     */
    TranslationRecognizer.prototype.stopContinuousRecognitionAsync = function (cb, err) {
        try {
            Contracts_1.Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
            this.implCloseExistingRecognizer();
            if (!!cb) {
                try {
                    cb();
                }
                catch (e) {
                    if (!!err) {
                        err(e);
                    }
                }
            }
        }
        catch (error) {
            if (!!err) {
                if (error instanceof Error) {
                    var typedError = error;
                    err(typedError.name + ": " + typedError.message);
                }
                else {
                    err(error);
                }
            }
        }
    };
    /**
     * closes all external resources held by an instance of this class.
     * @member TranslationRecognizer.prototype.close
     * @function
     * @public
     */
    TranslationRecognizer.prototype.close = function () {
        Contracts_1.Contracts.throwIfDisposed(this.disposedTranslationRecognizer);
        this.dispose(true);
    };
    TranslationRecognizer.prototype.dispose = function (disposing) {
        if (this.disposedTranslationRecognizer) {
            return;
        }
        if (disposing) {
            this.implCloseExistingRecognizer();
            this.disposedTranslationRecognizer = true;
            _super.prototype.dispose.call(this, disposing);
        }
    };
    TranslationRecognizer.prototype.CreateRecognizerConfig = function (speechConfig, recognitionMode) {
        return new Exports_1.RecognizerConfig(speechConfig, Exports_1.RecognitionMode.Conversation, this.properties);
    };
    TranslationRecognizer.prototype.CreateServiceRecognizer = function (authentication, connectionFactory, audioConfig, recognizerConfig) {
        var configImpl = audioConfig;
        return new Exports_1.TranslationServiceRecognizer(authentication, connectionFactory, configImpl, recognizerConfig);
    };
    TranslationRecognizer.prototype.implCloseExistingRecognizer = function () {
        if (this.reco) {
            this.reco.AudioSource.TurnOff();
            this.reco.Dispose();
            this.reco = undefined;
        }
    };
    TranslationRecognizer.prototype.implDispatchMessageHandler = function (event, cb, err) {
        if (!this.reco) {
            return;
        }
        switch (event.Name) {
            case "RecognitionEndedEvent":
                {
                    var recoEndedEvent = event;
                    if (recoEndedEvent.Status !== Exports_1.RecognitionCompletionStatus.Success) {
                        var result_1 = new Exports_2.TranslationRecognitionResult(undefined, undefined, Exports_2.ResultReason.Canceled, undefined, undefined, undefined, Exports_1.RecognitionCompletionStatus[recoEndedEvent.Status] + ": " + recoEndedEvent.Error, undefined, undefined);
                        var errorEvent = new Exports_2.TranslationRecognitionCanceledEventArgs(recoEndedEvent.SessionId, Exports_2.CancellationReason.Error, recoEndedEvent.Error, result_1);
                        if (!!this.canceled) {
                            try {
                                this.canceled(this, errorEvent);
                                /* tslint:disable:no-empty */
                            }
                            catch (error) {
                                // Not going to let errors in the event handler
                                // trip things up.
                            }
                        }
                        // report result to promise.
                        if (!!cb) {
                            try {
                                cb(errorEvent.result);
                            }
                            catch (e) {
                                if (!!err) {
                                    err(e);
                                }
                            }
                            // Only invoke the call back once.
                            // and if it's successful don't invoke the
                            // error after that.
                            cb = undefined;
                            err = undefined;
                        }
                    }
                }
                break;
            case "RecognitionFailedEvent":
                var evResult = event;
                var reason = Exports_1.EnumTranslation.implTranslateRecognitionResult(evResult.Result.RecognitionStatus);
                var result = new Exports_2.TranslationRecognitionResult(undefined, undefined, reason, evResult.Result.DisplayText, evResult.Result.Duration, evResult.Result.Offset, undefined, JSON.stringify(evResult.Result), undefined);
                if (reason === Exports_2.ResultReason.Canceled) {
                    var ev = new Exports_2.TranslationRecognitionCanceledEventArgs(evResult.SessionId, Exports_1.EnumTranslation.implTranslateCancelResult(evResult.Result.RecognitionStatus), null, result);
                    if (!!this.canceled) {
                        try {
                            this.canceled(this, ev);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                else {
                    var ev = new Exports_2.TranslationRecognitionEventArgs(result, 0 /*offset*/, evResult.SessionId);
                    if (!!this.recognized) {
                        try {
                            this.recognized(this, ev);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                // report result to promise.
                if (!!cb) {
                    try {
                        cb(result);
                    }
                    catch (e) {
                        if (!!err) {
                            err(e);
                        }
                    }
                    // Only invoke the call back once.
                    // and if it's successful don't invoke the
                    // error after that.
                    cb = undefined;
                    err = undefined;
                }
                break;
            case "TranslationPhraseEvent":
                {
                    var evResult_1 = event;
                    var result_2 = this.FireEventForResult(evResult_1);
                    if (!!this.recognized) {
                        try {
                            this.recognized(this, result_2);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(result_2.result);
                        }
                        catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke the
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
            case "TranslationHypothesisEvent":
                {
                    var evResult_2 = event;
                    var result_3 = this.FireEventForResult(evResult_2);
                    if (!!this.recognizing) {
                        try {
                            this.recognizing(this, result_3);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "TranslationFailedEvent":
                {
                    var evResult_3 = event;
                    var result_4 = new Exports_2.TranslationRecognitionResult(undefined, undefined, Exports_2.ResultReason.RecognizedSpeech, evResult_3.Result.Text, evResult_3.Result.Duration, evResult_3.Result.Offset, undefined, undefined, undefined);
                    var retEvent = new Exports_2.TranslationRecognitionEventArgs(result_4, 0 /*todo*/, evResult_3.SessionId);
                    if (!!this.recognized) {
                        try {
                            this.recognized(this, retEvent);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(retEvent.result);
                        }
                        catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke thebundle
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
            case "TranslationSynthesisEvent":
                {
                    var evResut = event;
                    var audio = evResut.Result;
                    var reason_1 = (undefined === audio) ? Exports_2.ResultReason.SynthesizingAudioCompleted : Exports_2.ResultReason.SynthesizingAudio;
                    var result_5 = new Exports_2.TranslationSynthesisResult(reason_1, audio);
                    var retEvent = new Exports_2.TranslationSynthesisEventArgs(result_5, evResut.SessionId);
                    if (!!this.synthesizing) {
                        try {
                            this.synthesizing(this, retEvent);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "TranslationSynthesisErrorEvent":
                {
                    var evResult_4 = event;
                    var result_6 = new Exports_2.TranslationSynthesisResult(evResult_4.Result.SynthesisStatus, undefined);
                    var retEvent = new Exports_2.TranslationSynthesisEventArgs(result_6, evResult_4.SessionId);
                    if (!!this.synthesizing) {
                        try {
                            this.synthesizing(this, retEvent);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                    if (!!this.canceled) {
                        // And raise a canceled event to send the rich(er) error message back.
                        var canceledResult = new Exports_2.TranslationRecognitionCanceledEventArgs(evResult_4.SessionId, Exports_2.CancellationReason.Error, evResult_4.Result.FailureReason, null);
                        try {
                            this.canceled(this, canceledResult);
                            /* tslint:disable:no-empty */
                        }
                        catch (error) {
                            // Not going to let errors in the event handler
                            // trip things up.
                        }
                    }
                }
                break;
            case "InternalErrorEvent":
                {
                    var evResult_5 = event;
                    var result_7 = new Exports_2.TranslationRecognitionResult(undefined, evResult_5.RequestId, Exports_2.ResultReason.Canceled, undefined, undefined, undefined, evResult_5.Result);
                    var canceledResult = new Exports_2.TranslationRecognitionCanceledEventArgs(evResult_5.SessionId, Exports_2.CancellationReason.Error, result_7.errorDetails, result_7);
                    try {
                        this.canceled(this, canceledResult);
                        /* tslint:disable:no-empty */
                    }
                    catch (error) {
                        // Not going to let errors in the event handler
                        // trip things up.
                    }
                    // report result to promise.
                    if (!!cb) {
                        try {
                            cb(result_7);
                        }
                        catch (e) {
                            if (!!err) {
                                err(e);
                            }
                        }
                        // Only invoke the call back once.
                        // and if it's successful don't invoke thebundle
                        // error after that.
                        cb = undefined;
                        err = undefined;
                    }
                }
                break;
        }
    };
    TranslationRecognizer.prototype.FireEventForResult = function (evResult) {
        var translations = new Exports_2.Translations();
        for (var _i = 0, _a = evResult.Result.Translation.Translations; _i < _a.length; _i++) {
            var translation = _a[_i];
            translations.set(translation.Language, translation.Text);
        }
        var result = new Exports_2.TranslationRecognitionResult(translations, undefined, evResult instanceof Exports_1.TranslationPhraseEvent ? Exports_2.ResultReason.TranslatedSpeech : Exports_2.ResultReason.TranslatingSpeech, evResult.Result.Text, evResult.Result.Duration, evResult.Result.Offset, undefined, JSON.stringify(evResult.Result), undefined);
        var ev = new Exports_2.TranslationRecognitionEventArgs(result, 0, evResult.SessionId);
        return ev;
    };
    return TranslationRecognizer;
}(Exports_2.Recognizer));
exports.TranslationRecognizer = TranslationRecognizer;



/***/ }),
/* 73 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
/**
 * Represents collection of parameters and their values.
 * @class Translation
 */
var Translations = /** @class */ (function () {
    function Translations() {
        // Use an PropertyCollection internally, just wrapping it to hide the | enum syntax it has.
        this.map = new Exports_1.PropertyCollection();
    }
    /**
     * Returns the parameter value in type String. The parameter must have the same type as String.
     * Currently only String, int and bool are allowed.
     * If the name is not available, the specified defaultValue is returned.
     * @member Translation.prototype.get
     * @function
     * @public
     * @param {string} key - The parameter name.
     * @param {string} def - The default value which is returned if the parameter is not available in the collection.
     * @returns {string} value of the parameter.
     */
    Translations.prototype.get = function (key, def) {
        return this.map.getProperty(key, def);
    };
    /**
     * Sets the String value of the parameter specified by name.
     * @member Translation.prototype.set
     * @function
     * @public
     * @param {string} key - The parameter name.
     * @param {string} value - The value of the parameter.
     */
    Translations.prototype.set = function (key, value) {
        this.map.setProperty(key, value);
    };
    return Translations;
}());
exports.Translations = Translations;



/***/ }),
/* 74 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Defines the possible reasons a recognition result might not be recognized.
 * @class NoMatchReason
 */
var NoMatchReason;
(function (NoMatchReason) {
    /**
     * Indicates that speech was detected, but not recognized.
     * @member NoMatchReason.NotRecognized
     */
    NoMatchReason[NoMatchReason["NotRecognized"] = 0] = "NotRecognized";
    /**
     * Indicates that the start of the audio stream contained only silence, and the service timed out waiting for speech.
     * @member NoMatchReason.InitialSilenceTimeout
     */
    NoMatchReason[NoMatchReason["InitialSilenceTimeout"] = 1] = "InitialSilenceTimeout";
    /**
     * Indicates that the start of the audio stream contained only noise, and the service timed out waiting for speech.
     * @member NoMatchReason.InitialBabbleTimeout
     */
    NoMatchReason[NoMatchReason["InitialBabbleTimeout"] = 2] = "InitialBabbleTimeout";
})(NoMatchReason = exports.NoMatchReason || (exports.NoMatchReason = {}));



/***/ }),
/* 75 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(2);
var Exports_2 = __webpack_require__(0);
/**
 * Contains detailed information for NoMatch recognition results.
 * @class NoMatchDetails
 */
var NoMatchDetails = /** @class */ (function () {
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {NoMatchReason} reason - The no-match reason.
     */
    function NoMatchDetails(reason) {
        this.privReason = reason;
    }
    /**
     * Creates an instance of NoMatchDetails object for the NoMatch SpeechRecognitionResults.
     * @member NoMatchDetails.fromResult
     * @function
     * @public
     * @param {SpeechRecognitionResult | IntentRecognitionResult | TranslationRecognitionResult} result - The recognition result that was not recognized.
     * @returns {NoMatchDetails} The no match details object being created.
     */
    NoMatchDetails.fromResult = function (result) {
        var simpleSpeech = JSON.parse(result.json);
        var reason = Exports_2.NoMatchReason.NotRecognized;
        var realReason = Exports_1.RecognitionStatus2[simpleSpeech.RecognitionStatus];
        switch (realReason) {
            case Exports_1.RecognitionStatus2.BabbleTimeout:
                reason = Exports_2.NoMatchReason.InitialBabbleTimeout;
                break;
            case Exports_1.RecognitionStatus2.InitialSilenceTimeout:
                reason = Exports_2.NoMatchReason.InitialSilenceTimeout;
                break;
            default:
                reason = Exports_2.NoMatchReason.NotRecognized;
                break;
        }
        return new NoMatchDetails(reason);
    };
    Object.defineProperty(NoMatchDetails.prototype, "reason", {
        /**
         * The reason the recognition was canceled.
         * @member NoMatchDetails.prototype.reason
         * @function
         * @public
         * @returns {NoMatchReason} Specifies the reason canceled.
         */
        get: function () {
            return this.privReason;
        },
        enumerable: true,
        configurable: true
    });
    return NoMatchDetails;
}());
exports.NoMatchDetails = NoMatchDetails;



/***/ }),
/* 76 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
/**
 * Define payload of speech recognition canceled result events.
 * @class TranslationRecognitionCanceledEventArgs
 */
var TranslationRecognitionCanceledEventArgs = /** @class */ (function () {
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {string} sessionid - The session id.
     * @param {CancellationReason} cancellationReason - The cancellation reason.
     * @param {string} errorDetails - Error details, if provided.
     * @param {TranslationRecognitionResult} result - The result.
     */
    function TranslationRecognitionCanceledEventArgs(sessionid, cancellationReason, errorDetails, result) {
        this.privCancelReason = cancellationReason;
        this.privErrorDetails = errorDetails;
        this.privResult = result;
        this.privSessionId = sessionid;
    }
    Object.defineProperty(TranslationRecognitionCanceledEventArgs.prototype, "result", {
        /**
         * Specifies the recognition result.
         * @member TranslationRecognitionCanceledEventArgs.prototype.result
         * @function
         * @public
         * @returns {TranslationRecognitionResult} the recognition result.
         */
        get: function () {
            return this.privResult;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(TranslationRecognitionCanceledEventArgs.prototype, "sessionId", {
        /**
         * Specifies the session identifier.
         * @member TranslationRecognitionCanceledEventArgs.prototype.sessionId
         * @function
         * @public
         * @returns {string} the session identifier.
         */
        get: function () {
            return this.privSessionId;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(TranslationRecognitionCanceledEventArgs.prototype, "reason", {
        /**
         * The reason the recognition was canceled.
         * @member TranslationRecognitionCanceledEventArgs.prototype.reason
         * @function
         * @public
         * @returns {CancellationReason} Specifies the reason canceled.
         */
        get: function () {
            return this.privCancelReason;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(TranslationRecognitionCanceledEventArgs.prototype, "errorDetails", {
        /**
         * In case of an unsuccessful recognition, provides a details of why the occurred error.
         * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
         * @member TranslationRecognitionCanceledEventArgs.prototype.errorDetails
         * @function
         * @public
         * @returns {string} A String that represents the error details.
         */
        get: function () {
            return this.privErrorDetails;
        },
        enumerable: true,
        configurable: true
    });
    return TranslationRecognitionCanceledEventArgs;
}());
exports.TranslationRecognitionCanceledEventArgs = TranslationRecognitionCanceledEventArgs;



/***/ }),
/* 77 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var __extends = (this && this.__extends) || (function () {
    var extendStatics = Object.setPrototypeOf ||
        ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
        function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
/**
 * Define payload of intent recognition canceled result events.
 * @class IntentRecognitionCanceledEventArgs
 */
var IntentRecognitionCanceledEventArgs = /** @class */ (function (_super) {
    __extends(IntentRecognitionCanceledEventArgs, _super);
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {CancellationReason} result - The result of the intent recognition.
     * @param {string} offset - The offset.
     * @param {IntentRecognitionResult} sessionId - The session id.
     */
    function IntentRecognitionCanceledEventArgs(reason, errorDetails, result, offset, sessionId) {
        var _this = _super.call(this, result, offset, sessionId) || this;
        _this.privReason = reason;
        _this.privErrorDetails = errorDetails;
        return _this;
    }
    Object.defineProperty(IntentRecognitionCanceledEventArgs.prototype, "reason", {
        /**
         * The reason the recognition was canceled.
         * @member IntentRecognitionCanceledEventArgs.prototype.reason
         * @function
         * @public
         * @returns {CancellationReason} Specifies the reason canceled.
         */
        get: function () {
            return this.privReason;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(IntentRecognitionCanceledEventArgs.prototype, "errorDetails", {
        /**
         * In case of an unsuccessful recognition, provides a details of why the occurred error.
         * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
         * @member IntentRecognitionCanceledEventArgs.prototype.errorDetails
         * @function
         * @public
         * @returns {string} A String that represents the error details.
         */
        get: function () {
            return this.privErrorDetails;
        },
        enumerable: true,
        configurable: true
    });
    return IntentRecognitionCanceledEventArgs;
}(Exports_1.IntentRecognitionEventArgs));
exports.IntentRecognitionCanceledEventArgs = IntentRecognitionCanceledEventArgs;



/***/ }),
/* 78 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(2);
var Exports_2 = __webpack_require__(0);
/**
 * Contains detailed information about why a result was canceled.
 * @class CancellationDetails
 */
var CancellationDetails = /** @class */ (function () {
    /**
     * Creates and initializes an instance of this class.
     * @constructor
     * @param {CancellationReason} reason - The cancellation reason.
     * @param {string} errorDetails - The error details, if provided.
     */
    function CancellationDetails(reason, errorDetails) {
        this.privReason = reason;
        this.privErrorDetails = errorDetails;
    }
    /**
     * Creates an instance of CancellationDetails object for the canceled RecognitionResult.
     * @member CancellationDetails.fromResult
     * @function
     * @public
     * @param {RecognitionResult} result - The result that was canceled.
     * @returns {CancellationDetails} The cancellation details object being created.
     */
    CancellationDetails.fromResult = function (result) {
        var reason = Exports_2.CancellationReason.Error;
        if (!!result.json) {
            var simpleSpeech = JSON.parse(result.json);
            var recognitionStatus2 = "" + simpleSpeech.RecognitionStatus;
            var recstatus2 = Exports_1.RecognitionStatus2[recognitionStatus2];
            switch (recstatus2) {
                case Exports_1.RecognitionStatus2.Success:
                case Exports_1.RecognitionStatus2.EndOfDictation:
                case Exports_1.RecognitionStatus2.NoMatch:
                    reason = Exports_2.CancellationReason.EndOfStream;
                    break;
                case Exports_1.RecognitionStatus2.InitialSilenceTimeout:
                case Exports_1.RecognitionStatus2.BabbleTimeout:
                case Exports_1.RecognitionStatus2.Error:
                default:
                    reason = Exports_2.CancellationReason.Error;
                    break;
            }
        }
        return new CancellationDetails(reason, result.errorDetails);
    };
    Object.defineProperty(CancellationDetails.prototype, "reason", {
        /**
         * The reason the recognition was canceled.
         * @member CancellationDetails.prototype.reason
         * @function
         * @public
         * @returns {CancellationReason} Specifies the reason canceled.
         */
        get: function () {
            return this.privReason;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(CancellationDetails.prototype, "errorDetails", {
        /**
         * In case of an unsuccessful recognition, provides a details of why the occurred error.
         * This field is only filled-out if the reason canceled (@see getReason) is set to Error.
         * @member CancellationDetails.prototype.errorDetails
         * @function
         * @public
         * @returns {string} A String that represents the error details.
         */
        get: function () {
            return this.privErrorDetails;
        },
        enumerable: true,
        configurable: true
    });
    return CancellationDetails;
}());
exports.CancellationDetails = CancellationDetails;



/***/ }),
/* 79 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var Exports_1 = __webpack_require__(0);
var Exports_2 = __webpack_require__(1);
var Exports_3 = __webpack_require__(6);
var FileAudioSource = /** @class */ (function () {
    function FileAudioSource(file, audioSourceId) {
        var _this = this;
        this.streams = {};
        this.TurnOn = function () {
            if (typeof FileReader === "undefined") {
                var errorMsg = "Browser does not support FileReader.";
                _this.OnEvent(new Exports_2.AudioSourceErrorEvent(errorMsg, "")); // initialization error - no streamid at this point
                return Exports_2.PromiseHelper.FromError(errorMsg);
            }
            else if (_this.file.name.lastIndexOf(".wav") !== _this.file.name.length - 4) {
                var errorMsg = _this.file.name + " is not supported. Only WAVE files are allowed at the moment.";
                _this.OnEvent(new Exports_2.AudioSourceErrorEvent(errorMsg, ""));
                return Exports_2.PromiseHelper.FromError(errorMsg);
            }
            else if (_this.file.size > FileAudioSource.MAX_SIZE) {
                var errorMsg = _this.file.name + " exceeds the maximum allowed file size (" + FileAudioSource.MAX_SIZE + ").";
                _this.OnEvent(new Exports_2.AudioSourceErrorEvent(errorMsg, ""));
                return Exports_2.PromiseHelper.FromError(errorMsg);
            }
            _this.OnEvent(new Exports_2.AudioSourceInitializingEvent(_this.id)); // no stream id
            _this.OnEvent(new Exports_2.AudioSourceReadyEvent(_this.id));
            return Exports_2.PromiseHelper.FromResult(true);
        };
        this.Id = function () {
            return _this.id;
        };
        this.Attach = function (audioNodeId) {
            _this.OnEvent(new Exports_2.AudioStreamNodeAttachingEvent(_this.id, audioNodeId));
            return _this.Upload(audioNodeId).OnSuccessContinueWith(function (streamReader) {
                _this.OnEvent(new Exports_2.AudioStreamNodeAttachedEvent(_this.id, audioNodeId));
                return {
                    Detach: function () {
                        streamReader.Close();
                        delete _this.streams[audioNodeId];
                        _this.OnEvent(new Exports_2.AudioStreamNodeDetachedEvent(_this.id, audioNodeId));
                        _this.TurnOff();
                    },
                    Id: function () {
                        return audioNodeId;
                    },
                    Read: function () {
                        return streamReader.Read();
                    },
                };
            });
        };
        this.Detach = function (audioNodeId) {
            if (audioNodeId && _this.streams[audioNodeId]) {
                _this.streams[audioNodeId].Close();
                delete _this.streams[audioNodeId];
                _this.OnEvent(new Exports_2.AudioStreamNodeDetachedEvent(_this.id, audioNodeId));
            }
        };
        this.TurnOff = function () {
            for (var streamId in _this.streams) {
                if (streamId) {
                    var stream = _this.streams[streamId];
                    if (stream && !stream.IsClosed) {
                        stream.Close();
                    }
                }
            }
            _this.OnEvent(new Exports_2.AudioSourceOffEvent(_this.id)); // no stream now
            return Exports_2.PromiseHelper.FromResult(true);
        };
        this.Upload = function (audioNodeId) {
            return _this.TurnOn()
                .OnSuccessContinueWith(function (_) {
                var stream = new Exports_2.Stream(audioNodeId);
                _this.streams[audioNodeId] = stream;
                var reader = new FileReader();
                var startOffset = 0;
                var endOffset = FileAudioSource.CHUNK_SIZE;
                var lastWriteTimestamp = 0;
                var processNextChunk = function (event) {
                    if (stream.IsClosed) {
                        return; // output stream was closed (somebody called TurnOff). We're done here.
                    }
                    if (lastWriteTimestamp !== 0) {
                        var delay = Date.now() - lastWriteTimestamp;
                        if (delay < FileAudioSource.UPLOAD_INTERVAL) {
                            // It's been less than the "upload interval" since we've uploaded the
                            // last chunk. Schedule the next upload to make sure that we're sending
                            // upstream roughly one chunk per upload interval.
                            new Exports_3.Timer(FileAudioSource.UPLOAD_INTERVAL - delay, processNextChunk).start();
                            return;
                        }
                    }
                    stream.Write(reader.result);
                    lastWriteTimestamp = Date.now();
                    if (endOffset < _this.file.size) {
                        startOffset = endOffset;
                        endOffset = Math.min(endOffset + FileAudioSource.CHUNK_SIZE, _this.file.size);
                        var chunk_1 = _this.file.slice(startOffset, endOffset);
                        reader.readAsArrayBuffer(chunk_1);
                    }
                    else {
                        // we've written the entire file to the output stream, can close it now.
                        stream.Close();
                    }
                };
                reader.onload = processNextChunk;
                reader.onerror = function (event) {
                    var errorMsg = "Error occurred while processing '" + _this.file.name + "'. " + event.error;
                    _this.OnEvent(new Exports_2.AudioStreamNodeErrorEvent(_this.id, audioNodeId, event.error));
                    throw new Error(errorMsg);
                };
                var chunk = _this.file.slice(startOffset, endOffset);
                reader.readAsArrayBuffer(chunk);
                return stream.GetReader();
            });
        };
        this.OnEvent = function (event) {
            _this.events.OnEvent(event);
            Exports_2.Events.Instance.OnEvent(event);
        };
        this.id = audioSourceId ? audioSourceId : Exports_2.CreateNoDashGuid();
        this.events = new Exports_2.EventSource();
        this.file = file;
    }
    Object.defineProperty(FileAudioSource.prototype, "Format", {
        get: function () {
            return FileAudioSource.FILEFORMAT;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(FileAudioSource.prototype, "Events", {
        get: function () {
            return this.events;
        },
        enumerable: true,
        configurable: true
    });
    // Recommended sample rate (bytes/second).
    FileAudioSource.SAMPLE_RATE = 16000 * 2; // 16 kHz * 16 bits
    // We should stream audio at no faster than 2x real-time (i.e., send five chunks
    // per second, with the chunk size == sample rate in bytes per second * 2 / 5).
    FileAudioSource.CHUNK_SIZE = FileAudioSource.SAMPLE_RATE * 2 / 5;
    FileAudioSource.UPLOAD_INTERVAL = 200; // milliseconds
    // 10 seconds of audio in bytes =
    // sample rate (bytes/second) * 600 (seconds) + 44 (size of the wave header).
    FileAudioSource.MAX_SIZE = FileAudioSource.SAMPLE_RATE * 600 + 44;
    FileAudioSource.FILEFORMAT = Exports_1.AudioStreamFormat.getWaveFormatPCM(16000, 16, 1);
    return FileAudioSource;
}());
exports.FileAudioSource = FileAudioSource;



/***/ }),
/* 80 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var OpusRecorder = /** @class */ (function () {
    function OpusRecorder(options) {
        var _this = this;
        this.Record = function (context, mediaStream, outputStream) {
            var mediaRecorder = new MediaRecorder(mediaStream, _this.mediaRecorderOptions);
            var timeslice = 100; // this is in ms - 100 ensures that the chunk doesn't exceed the max size of chunk allowed in WS connection
            mediaRecorder.ondataavailable = function (dataAvailableEvent) {
                if (outputStream) {
                    var reader_1 = new FileReader();
                    reader_1.readAsArrayBuffer(dataAvailableEvent.data);
                    reader_1.onloadend = function (event) {
                        outputStream.Write(reader_1.result);
                    };
                }
            };
            _this.mediaResources = {
                recorder: mediaRecorder,
                stream: mediaStream,
            };
            mediaRecorder.start(timeslice);
        };
        this.ReleaseMediaResources = function (context) {
            if (_this.mediaResources.recorder.state !== "inactive") {
                _this.mediaResources.recorder.stop();
            }
            _this.mediaResources.stream.getTracks().forEach(function (track) { return track.stop(); });
        };
        this.mediaRecorderOptions = options;
    }
    return OpusRecorder;
}());
exports.OpusRecorder = OpusRecorder;
/* Declaring this inline to avoid compiler warnings
declare class MediaRecorder {
    constructor(mediaStream: MediaStream, options: any);

    public state: string;

    public ondataavailable(dataAvailableEvent: any): void;
    public stop(): void;
}*/



/***/ }),
/* 81 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var PcmRecorder = /** @class */ (function () {
    function PcmRecorder() {
        var _this = this;
        this.Record = function (context, mediaStream, outputStream) {
            var desiredSampleRate = 16000;
            // https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createScriptProcessor
            var scriptNode = (function () {
                var bufferSize = 0;
                try {
                    return context.createScriptProcessor(bufferSize, 1, 1);
                }
                catch (error) {
                    // Webkit (<= version 31) requires a valid bufferSize.
                    bufferSize = 2048;
                    var audioSampleRate = context.sampleRate;
                    while (bufferSize < 16384 && audioSampleRate >= (2 * desiredSampleRate)) {
                        bufferSize <<= 1;
                        audioSampleRate >>= 1;
                    }
                    return context.createScriptProcessor(bufferSize, 1, 1);
                }
            })();
            var waveStreamEncoder = new Exports_1.RiffPcmEncoder(context.sampleRate, desiredSampleRate);
            var needHeader = true;
            var that = _this;
            scriptNode.onaudioprocess = function (event) {
                var inputFrame = event.inputBuffer.getChannelData(0);
                if (outputStream && !outputStream.IsClosed) {
                    var waveFrame = waveStreamEncoder.Encode(needHeader, inputFrame);
                    if (!!waveFrame) {
                        outputStream.Write(waveFrame);
                        needHeader = false;
                    }
                }
            };
            // https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/createMediaStreamSource
            var micInput = context.createMediaStreamSource(mediaStream);
            _this.mediaResources = {
                scriptProcessorNode: scriptNode,
                source: micInput,
                stream: mediaStream,
            };
            micInput.connect(scriptNode);
            scriptNode.connect(context.destination);
        };
        this.ReleaseMediaResources = function (context) {
            if (_this.mediaResources) {
                if (_this.mediaResources.scriptProcessorNode) {
                    _this.mediaResources.scriptProcessorNode.disconnect(context.destination);
                    _this.mediaResources.scriptProcessorNode = null;
                }
                if (_this.mediaResources.source) {
                    _this.mediaResources.source.disconnect();
                    _this.mediaResources.stream.getTracks().forEach(function (track) { return track.stop(); });
                    _this.mediaResources.source = null;
                }
            }
        };
    }
    return PcmRecorder;
}());
exports.PcmRecorder = PcmRecorder;



/***/ }),
/* 82 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var SessionStorage = /** @class */ (function () {
    function SessionStorage() {
        this.Get = function (key) {
            if (!key) {
                throw new Exports_1.ArgumentNullError("key");
            }
            return sessionStorage.getItem(key);
        };
        this.GetOrAdd = function (key, valueToAdd) {
            if (!key) {
                throw new Exports_1.ArgumentNullError("key");
            }
            var value = sessionStorage.getItem(key);
            if (value === null || value === undefined) {
                sessionStorage.setItem(key, valueToAdd);
            }
            return sessionStorage.getItem(key);
        };
        this.Set = function (key, value) {
            if (!key) {
                throw new Exports_1.ArgumentNullError("key");
            }
            sessionStorage.setItem(key, value);
        };
        this.Remove = function (key) {
            if (!key) {
                throw new Exports_1.ArgumentNullError("key");
            }
            sessionStorage.removeItem(key);
        };
    }
    return SessionStorage;
}());
exports.SessionStorage = SessionStorage;



/***/ }),
/* 83 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
var Timer = /** @class */ (function () {
    function Timer(delayInMillisec, successCallback) {
        var _this = this;
        this.start = function () {
            var params = [];
            for (var _i = 0; _i < arguments.length; _i++) {
                params[_i] = arguments[_i];
            }
            if (_this.timerId) {
                _this.stop();
            }
            _this.timerId = setTimeout(_this.successCallback, _this.delayInMillisec, params);
        };
        this.stop = function () {
            clearTimeout(_this.timerId);
        };
        this.delayInMillisec = delayInMillisec;
        this.successCallback = successCallback;
    }
    return Timer;
}());
exports.Timer = Timer;



/***/ }),
/* 84 */
/***/ (function(module, exports, __webpack_require__) {

"use strict";

Object.defineProperty(exports, "__esModule", { value: true });
//
// Copyright (c) Microsoft. All rights reserved.
// See https://aka.ms/csspeech/license201809 for the full license information.
//
var Exports_1 = __webpack_require__(1);
var WebsocketMessageAdapter_1 = __webpack_require__(19);
var WebsocketConnection = /** @class */ (function () {
    function WebsocketConnection(uri, queryParameters, headers, messageFormatter, connectionId) {
        var _this = this;
        this.isDisposed = false;
        this.Dispose = function () {
            _this.isDisposed = true;
            if (_this.connectionMessageAdapter) {
                _this.connectionMessageAdapter.Close();
            }
        };
        this.IsDisposed = function () {
            return _this.isDisposed;
        };
        this.State = function () {
            return _this.connectionMessageAdapter.State;
        };
        this.Open = function () {
            return _this.connectionMessageAdapter.Open();
        };
        this.Send = function (message) {
            return _this.connectionMessageAdapter.Send(message);
        };
        this.Read = function () {
            return _this.connectionMessageAdapter.Read();
        };
        if (!uri) {
            throw new Exports_1.ArgumentNullError("uri");
        }
        if (!messageFormatter) {
            throw new Exports_1.ArgumentNullError("messageFormatter");
        }
        this.messageFormatter = messageFormatter;
        var queryParams = "";
        var i = 0;
        if (queryParameters) {
            for (var paramName in queryParameters) {
                if (paramName) {
                    queryParams += i === 0 ? "?" : "&";
                    var val = encodeURIComponent(queryParameters[paramName]);
                    queryParams += paramName + "=" + val;
                    i++;
                }
            }
        }
        if (headers) {
            for (var headerName in headers) {
                if (headerName) {
                    queryParams += i === 0 ? "?" : "&";
                    var val = encodeURIComponent(headers[headerName]);
                    queryParams += headerName + "=" + val;
                    i++;
                }
            }
        }
        this.uri = uri + queryParams;
        this.id = connectionId ? connectionId : Exports_1.CreateNoDashGuid();
        this.connectionMessageAdapter = new WebsocketMessageAdapter_1.WebsocketMessageAdapter(this.uri, this.Id, this.messageFormatter);
    }
    Object.defineProperty(WebsocketConnection.prototype, "Id", {
        get: function () {
            return this.id;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(WebsocketConnection.prototype, "Events", {
        get: function () {
            return this.connectionMessageAdapter.Events;
        },
        enumerable: true,
        configurable: true
    });
    return WebsocketConnection;
}());
exports.WebsocketConnection = WebsocketConnection;



/***/ })
/******/ ]);
//# sourceMappingURL=microsoft.cognitiveservices.speech.sdk.bundle.js.map