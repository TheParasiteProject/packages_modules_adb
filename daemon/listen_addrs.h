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

#pragma once

#include <string>
#include <vector>

/**
 * Returns the bind specs to listen on.
 *
 * The `service.adb.listen_addrs` property can be used to provide a list of
 * bind specs. It is a comma-separated list of bind specs.
 *     e.g., tcp:localhost:1234,tcp:4567
 *
 * The TCP port to listen on can be specified using one of the following
 * (in priority order):
 * * `service.adb.tcp.port` property.
 * * `persist.adb.tcp.port` property.
 * * `ADBD_PORT` environment variable. Only on non-Android builds.
 *
 * If none of the above properties are set, and `enable_fallback` is true, then
 * the TCP port will default to 5555.
 *
 * When a TCP port is specified, then it will return both a `tcp:<port>` and
 * `vsock:<port>` bind spec.
 *
 * If both `service.adb.listen_addrs` and a TCP port are set, then the
 * `adbd_bind_tcp_and_listen_addrs` flag controls the behavior. If the flag is
 * true, then both `service.adb.listen_addrs` and the TCP port are used to
 * generate the list of bind specs. Otherwise, only `service.adb.listen_addrs`
 * is used.
 *
 * @return A list of bind specs to listen to.
 */
std::vector<std::string> get_listen_addrs(bool enable_fallback);
