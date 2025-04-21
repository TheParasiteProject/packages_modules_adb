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

#include <stdlib.h>
#include <unistd.h>

#include <string>
#include <vector>

#include <android-base/logging.h>
#include <android-base/properties.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <com_android_modules_adb_flags.h>

#include "listen_addrs.h"

using com::android::modules::adb::flags::adbd_bind_tcp_and_listen_addrs;
using ::testing::ContainerEq;

class ListenAddrsTest : public ::testing::Test {
  protected:
    ListenAddrsTest() {
        android::base::SetProperty("service.adb.tcp.port", "");
        android::base::SetProperty("persist.adb.tcp.port", "");
        android::base::SetProperty("service.adb.listen_addrs", "");
        adbd_bind_tcp_and_listen_addrs(true);
    }
};

TEST_F(ListenAddrsTest, UsbOnlyLegacy) {
    adbd_bind_tcp_and_listen_addrs(false);

    auto addrs = get_listen_addrs(false);
    EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{}));
}

TEST_F(ListenAddrsTest, UsbOnly) {
    auto addrs = get_listen_addrs(false);
    EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{}));
}

TEST_F(ListenAddrsTest, TcpFallbackLegacy) {
    adbd_bind_tcp_and_listen_addrs(false);

    auto addrs = get_listen_addrs(true);
    EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                               "tcp:5555",
                               "vsock:5555",
                       }));
}

TEST_F(ListenAddrsTest, TcpFallback) {
    auto addrs = get_listen_addrs(true);
    EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                               "tcp:5555",
                               "vsock:5555",
                       }));
}

TEST_F(ListenAddrsTest, ListenAddrOnlyLegacy) {
    adbd_bind_tcp_and_listen_addrs(false);

    android::base::SetProperty("service.adb.listen_addrs", "tcp:localhost:5555,tcp:localhost:5556");

    for (bool enable_fallback : {true, false}) {
        auto addrs = get_listen_addrs(enable_fallback);
        EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                                   "tcp:localhost:5555",
                                   "tcp:localhost:5556",
                           }));
    }
}

TEST_F(ListenAddrsTest, ListenAddrOnly) {
    android::base::SetProperty("service.adb.listen_addrs", "tcp:localhost:5555,tcp:localhost:5556");

    for (bool enable_fallback : {true, false}) {
        auto addrs = get_listen_addrs(enable_fallback);
        EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                                   "tcp:localhost:5555",
                                   "tcp:localhost:5556",
                           }));
    }
}

TEST_F(ListenAddrsTest, ServicePortOnlyLegacy) {
    adbd_bind_tcp_and_listen_addrs(false);

    android::base::SetProperty("service.adb.tcp.port", "5545");
    android::base::SetProperty("persist.adb.tcp.port", "1234");

    for (bool enable_fallback : {true, false}) {
        auto addrs = get_listen_addrs(enable_fallback);
        EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                                   "tcp:5545",
                                   "vsock:5545",
                           }));
    }
}

TEST_F(ListenAddrsTest, ServicePortOnly) {
    android::base::SetProperty("service.adb.tcp.port", "5545");
    android::base::SetProperty("persist.adb.tcp.port", "1234");

    for (bool enable_fallback : {true, false}) {
        auto addrs = get_listen_addrs(enable_fallback);
        EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                                   "tcp:5545",
                                   "vsock:5545",
                           }));
    }
}

TEST_F(ListenAddrsTest, PersistPortOnlyLegacy) {
    adbd_bind_tcp_and_listen_addrs(false);

    android::base::SetProperty("persist.adb.tcp.port", "1234");

    for (bool enable_fallback : {true, false}) {
        auto addrs = get_listen_addrs(enable_fallback);
        EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                                   "tcp:1234",
                                   "vsock:1234",
                           }));
    }
}

TEST_F(ListenAddrsTest, PersistPortOnly) {
    android::base::SetProperty("persist.adb.tcp.port", "1234");

    for (bool enable_fallback : {true, false}) {
        auto addrs = get_listen_addrs(enable_fallback);
        EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                                   "tcp:1234",
                                   "vsock:1234",
                           }));
    }
}

TEST_F(ListenAddrsTest, PortAndListenAddrLegacy) {
    adbd_bind_tcp_and_listen_addrs(false);

    android::base::SetProperty("service.adb.tcp.port", "5544");
    android::base::SetProperty("service.adb.listen_addrs", "tcp:localhost:5555,tcp:localhost:5556");

    for (bool enable_fallback : {true, false}) {
        auto addrs = get_listen_addrs(enable_fallback);
        EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                                   "tcp:localhost:5555",
                                   "tcp:localhost:5556",
                           }));
    }
}

TEST_F(ListenAddrsTest, PortAndListenAddr) {
    android::base::SetProperty("service.adb.tcp.port", "5544");
    android::base::SetProperty("service.adb.listen_addrs", "tcp:localhost:5555,tcp:localhost:5556");

    for (bool enable_fallback : {true, false}) {
        auto addrs = get_listen_addrs(enable_fallback);
        EXPECT_THAT(addrs, ContainerEq(std::vector<std::string>{
                                   "tcp:5544",
                                   "vsock:5544",
                                   "tcp:localhost:5555",
                                   "tcp:localhost:5556",
                           }));
    }
}
