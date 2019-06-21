//
// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE.md file in the project root for full license information.
//

#pragma once

#include <iostream>
#include <random>
#include <signal.h>
#include <fstream>
#include <algorithm>
#include <string>

#ifdef _DEBUG
#define SPX_CONFIG_INCLUDE_ALL_DBG 1
#define SPX_CONFIG_INCLUDE_ALL 1
#else
#define SPX_CONFIG_INCLUDE_ALL 1
#endif

#include "trace_message.h"

#ifndef __SPX_DO_TRACE_IMPL
#define __SPX_DO_TRACE_IMPL SpxTraceMessage
#endif

#include "string_utils.h"
#include "debug_utils.h"
#include "json.h"

#ifdef _MSC_VER
#pragma warning( push )
// disable: (9754,94): error 6330:  : 'const char' passed as _Param_(1) when 'unsigned char' is required in call to 'isalnum'.
#pragma warning( disable : 6330 )
#include "catch.hpp"
#pragma warning( pop )
#else
#include "catch.hpp"
#endif

#if !defined(CATCH_CONFIG_RUNNER)
#define EXTERN extern
#else
#define EXTERN
#endif

#define TEST_SETTINGS_FILE "test.settings.json"

namespace Keys
{
    EXTERN std::string Speech;
    EXTERN std::string LUIS;
    EXTERN std::string Dialog;
    EXTERN std::string ConversationTranscriber;
}

namespace Config
{
    EXTERN std::string Endpoint;
    EXTERN std::string Region;
    EXTERN std::string LuisRegion;
    EXTERN std::string LuisAppId;
    EXTERN std::string InputDir;
    EXTERN std::string DialogRegion;
    EXTERN std::string DialogBotSecret;
    EXTERN std::string InroomEndpoint;
    EXTERN std::string OnlineEndpoint;
    EXTERN std::string OfflineModelPathRoot;
    EXTERN std::string OfflineModelLanguage;
    EXTERN bool DoDiscover;
}

inline bool exists(const std::string& name) {
    return std::ifstream(name.c_str()).good();
}

inline std::ifstream get_stream(const std::string& name) {
    return std::ifstream(name.c_str(), std::ifstream::binary);
}

class ConfigSettings {
private:
    static nlohmann::json getJson(std::string path)
    {
        nlohmann::json nlohmanJson = nullptr;

        if (exists(path))
        {
            std::ifstream testSettingsFile(path);
            testSettingsFile >> nlohmanJson;
        }

        return nlohmanJson;
    }

public:
    static void LoadFromJsonFile(const char* rootPathString)
    {
        std::string rootPath(rootPathString);
        std::replace(rootPath.begin(), rootPath.end(), '\\', '/');
        std::string rootPathOnly = rootPath.substr(0, rootPath.find_last_of('/') + 1);

        nlohmann::json data = getJson(rootPathOnly + TEST_SETTINGS_FILE);

        if (data != nullptr)
        {
            Keys::Speech = data.at("keySpeech").get<std::string>();
            Keys::LUIS = data.at("keyLUIS").get<std::string>();
            Keys::Dialog = data.at("keyDialog").get<std::string>();
            Keys::ConversationTranscriber = data.at("keyConversationTranscriber").get<std::string>();

            Config::Endpoint = data.at("endPoint").get<std::string>();
            Config::Region = data.at("regionId").get<std::string>();
            Config::LuisRegion = data.at("regionIdLUIS").get<std::string>();
            Config::LuisAppId = data.at("luisAppId").get<std::string>();
            Config::DialogRegion = data.at("dialogRegionId").get<std::string>();
            Config::DialogBotSecret = data.at("dialogBotSecret").get<std::string>();
            Config::InroomEndpoint = data.at("InRoomEndPoint").get<std::string>();
            Config::OnlineEndpoint = data.at("OnlineEndPoint").get<std::string>();

            Config::InputDir = data.at("inputDir").get<std::string>();
            if (Config::InputDir.length() != 0)
            {
                Config::InputDir = rootPathOnly + Config::InputDir;
            }
        }
    }
};

inline bool checkForDiscovery(int argc, char*argv[])
{
    for (int index = 0; index < argc; index++)
    {
        if (!strcmp(argv[index], "--discovery"))
        {
            return true;
        }
    }

    return false;
}

typedef std::linear_congruential_engine<uint_fast32_t, 1664525, 1013904223, UINT_FAST32_MAX> random_engine;

inline void add_signal_handlers()
{
    Debug::HookSignalHandlers();
}

#if defined(CATCH_CONFIG_RUNNER)
inline int parse_cli_args(Catch::Session& session, int argc, char* argv[])
{
    // Build a new parser on top of Catch's
    using namespace Catch::clara;

    auto cli
        = session.cli() // Get Catch's composite command line parser
        | Opt(Keys::Speech, "SpeechSubscriptionKey") // bind variable to a new option, with a hint string
        ["--keySpeech"]    // the option names it will respond to
    ("The subscription key for speech")
        | Opt(Keys::LUIS, "LuisSubscriptionKey")
        ["--keyLUIS"]
    ("The subscription key for language understanding")
        | Opt(Keys::Dialog, "keyDialog")
        ["--keyDialog"]
    ("The subscription key for the Speech Channel")
        | Opt(Config::Endpoint, "endpoint")
        ["--endpoint"]
    ("The endpoint url to test against.")
        | Opt(Config::Region, "RegionId")
        ["--regionId"]
    ("The region id to be used for subscription and authorization requests")
        | Opt(Config::LuisRegion, "LuisRegionId")
        ["--regionIdLUIS"]
    ("The region id to be used for language understanding subscription and authorization requests")
        | Opt(Config::LuisAppId, "LuisAppId")
        ["--luisAppId"]
    ("The language understanding app id to be used intent recognition tests")
        | Opt(Config::InroomEndpoint, "InroomEndpoint")
        ["--InroomEndpoint"]
    ("The endpoint that in-room tests in intelligent meeting recognizer talks to")
        | Opt(Config::OnlineEndpoint, "OnlineEndpoint")
        ["--OnlineEndpoint"]
    ("The endpoint that on-line tests in intelligent meeting recognizer talks to")
        | Opt(Keys::ConversationTranscriber, "ConversationTranscriber")
        ["--keyConversationTranscriberPPE"]
    ("The conversation transcriber key")
        | Opt(Config::InputDir, "InputDir")
        ["--inputDir"]
    ("The directory where test input files are placed")
        | Opt(Config::DialogRegion, "DialogRegion")
        ["--dialogRegionId"]
    ("The region id to be used for the Speech Channel Service")
        | Opt(Config::DialogBotSecret, "DialogBotSecret")
        ["--dialogBotSecret"]
    ("Secret for the functional test bot")
        | Opt(Config::OfflineModelPathRoot, "OfflineModelPathRoot")
        ["--offlineModelPathRoot"]
    ("The root path under which offline speech recognition models are located.")
        | Opt(Config::OfflineModelLanguage, "OfflineModelLanguage")
        ["--offlineModelLanguage"]
    ("The language code of the offline speech recognition model used in tests.")
        | Opt(Config::DoDiscover)
        ["--discovery"]
    ("Perform VS Test Adaptor discovery");
        ;

    // Now pass the new composite back to Catch so it uses that
    session.cli(cli);

    // Let Catch (using Clara) parse the command line
    return session.applyCommandLine(argc, argv);
}
#endif

#define SPXTEST_SECTION(msg) SECTION(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_SECTION('%s') %s(%d)", msg, __FILE__, __LINE__);  \
    return 1; }())

#define SPXTEST_GIVEN(msg) GIVEN(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_GIVEN('%s') %s(%d):", msg, __FILE__, __LINE__); \
    return 1; }())

#define SPXTEST_WHEN(msg) WHEN(msg) if ([=](){ \
    SPX_TRACE_INFO("SPXTEST_WHEN('%s') %s(%d):", msg, __FILE__, __LINE__); \
    return 1; }())

#define SPXTEST_REQUIRE(expr) \
    SPX_TRACE_INFO("SPXTEST_REQUIRE('%s'): %s(%d):", __SPX_EXPR_AS_STRING(expr), __FILE__, __LINE__); \
    SPX_TRACE_ERROR_IF(!(expr), "SPXTEST_REQUIRE('%s') FAILED: %s(%d):", __SPX_EXPR_AS_STRING(expr), __FILE__, __LINE__); \
    REQUIRE(expr)