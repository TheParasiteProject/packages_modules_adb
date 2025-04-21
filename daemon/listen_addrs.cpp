/*
 * Copyright (C) 2025 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define TRACE_TAG ADB

#include <stdio.h>
#include <stdlib.h>

#include <optional>
#include <vector>

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>
#include <android-base/strings.h>

#include <com_android_modules_adb_flags.h>

#include "adb.h"
#include "daemon/listen_addrs.h"

static std::optional<uint16_t> get_tcp_port() {
    std::string prop_port = android::base::GetProperty("service.adb.tcp.port", "");
    if (prop_port.empty()) {
        prop_port = android::base::GetProperty("persist.adb.tcp.port", "");
    }

#if !defined(__ANDROID__)
    if (prop_port.empty() && getenv("ADBD_PORT")) {
        prop_port = getenv("ADBD_PORT");
    }
#endif

    int port;
    if (sscanf(prop_port.c_str(), "%d", &port) == 1 && port > 0 && port <= UINT16_MAX) {
        D("using tcp port=%d", port);
        return port;
    } else {
        return std::nullopt;
    }
}

static void push_tcp_addrs(std::vector<std::string>& addrs, uint16_t port) {
    // Listen on TCP and VSOCK port.
    addrs.push_back(android::base::StringPrintf("tcp:%d", port));
    addrs.push_back(android::base::StringPrintf("vsock:%d", port));
}

static void push_listen_addrs(std::vector<std::string>& addrs) {
    std::string prop_addr = android::base::GetProperty("service.adb.listen_addrs", "");

    if (prop_addr.empty()) {
        return;
    }

    for (const auto& addr : android::base::Split(prop_addr, ",")) {
        // TODO: Strip whitespace?
        addrs.push_back(addr);
    }
}

std::vector<std::string> get_listen_addrs(bool enable_fallback) {
    std::vector<std::string> addrs;

    if (com::android::modules::adb::flags::adbd_bind_tcp_and_listen_addrs()) {
        // Push the wildcard bind first since it's the port mDNS will advertise.
        // If we were to push the `listen_addrs` first, they could be bound to
        // `localhost`, so the port mDNS is advertising might not work.
        //
        // For example:
        //   listen_addrs: localhost:4444
        //   tcp.addr: 5555
        //
        // If listen_addrs was pushed first then mDNS would advertise 4444 which
        // is bound to `localhost` so no external clients could connect.
        auto port = get_tcp_port();
        if (port.has_value()) {
            push_tcp_addrs(addrs, *port);
        }

        push_listen_addrs(addrs);
    } else {
        push_listen_addrs(addrs);
        if (addrs.empty()) {
            auto port = get_tcp_port();
            if (port.has_value()) {
                push_tcp_addrs(addrs, *port);
            }
        }
    }

    if (addrs.empty() && enable_fallback) {
        // Listen on default port.
        push_tcp_addrs(addrs, DEFAULT_ADB_LOCAL_TRANSPORT_PORT);
    }

    return addrs;
}
