/*
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @file EvenPushInterface.h
 * @author: octopus
 * @date 2021-09-01
 */

#pragma once
#include <bcos-cpp-sdk/event/EventPushParams.h>
#include <bcos-framework/libutilities/Common.h>
#include <bcos-framework/libutilities/Error.h>

namespace bcos
{
namespace cppsdk
{
namespace event
{
using Callback = std::function<void(bcos::Error::Ptr, const std::string&, const std::string&)>;

class EventPushInterface
{
public:
    using Ptr = std::shared_ptr<EventPushInterface>;

    virtual ~EventPushInterface() {}

public:
    virtual void start() = 0;
    virtual void stop() = 0;

public:
    virtual void subscribeEvent(
        const std::string& _group, EventPushParams::Ptr _params, Callback _callback) = 0;
    virtual void unsubscribeEvent(const std::string& _id, Callback _callback) = 0;
};
}  // namespace event
}  // namespace cppsdk
}  // namespace bcos