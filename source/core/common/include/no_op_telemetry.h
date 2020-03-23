//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//
// no_op_telemetry.h: An implementation of the telemetry interface that does nothing
//

#pragma once

#include <i_telemetry.h>

namespace Microsoft {
namespace CognitiveServices {
namespace Speech {
namespace USP {

    /// <summary>
    /// Telemetry implementation that does nothing
    /// </summary>
    struct NoOpTelemetry : public ITelemetry
    {
        /// <summary>
        /// Gets the singleton instance of the telemetry implementation that does nothing
        /// </summary>
        /// <returns>The singleton instance</returns>
        static ITelemetry& Instance()
        {
            static NoOpTelemetry _instance;
            return _instance;
        }

        virtual void Flush(const std::string &) override {}
        virtual void InbandEventTimestampPopulate(const std::string &, const std::string &, const std::string &, const std::string &) override {}
        virtual void InbandConnectionTelemetry(const std::string &, const std::string &, const nlohmann::json &) override {}
        virtual void InbandEventKeyValuePopulate(const std::string &, const std::string &, const std::string &, const std::string &, const nlohmann::json &) override {}
        virtual void RecordReceivedMsg(const std::string &, const std::string &) override {}
        virtual void RecordResultLatency(const std::string &, uint64_t, bool) override {}
        virtual void RegisterNewRequestId(const std::string &) override {}

    private:
        NoOpTelemetry() {}
        NoOpTelemetry(const NoOpTelemetry&) = delete;
        NoOpTelemetry& operator=(const NoOpTelemetry&) = delete;
        NoOpTelemetry(NoOpTelemetry&&) = delete;
        NoOpTelemetry& operator=(NoOpTelemetry&&) = delete;
    };

} } } }
