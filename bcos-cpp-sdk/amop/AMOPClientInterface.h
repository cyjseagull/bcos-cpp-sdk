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
 * @file AMOPClientInterface.h
 * @author: octopus
 * @date 2021-08-23
 */
#pragma once

#include <bcos-cpp-sdk/ws/Common.h>
#include <bcos-cpp-sdk/ws/WsMessage.h>
#include <bcos-framework/libutilities/Common.h>
#include <bcos-framework/libutilities/Error.h>
#include <functional>
#include <set>

namespace bcos
{
namespace ws
{
class WsMessage;
class WsSession;
}  // namespace ws

namespace cppsdk
{
namespace amop
{
using AMOPCallback = std::function<void(
    bcos::Error::Ptr, std::shared_ptr<bcos::ws::WsMessage>, std::shared_ptr<bcos::ws::WsSession>)>;
class AMOPClientInterface
{
public:
    using Ptr = std::shared_ptr<AMOPClientInterface>;
    virtual ~AMOPClientInterface() {}
    /*
    // recv message
    virtual void onRecvMessage(
        std::shared_ptr<ws::WsMessage> _msg, std::shared_ptr<ws::WsSession> _session);
    */
    // subscribe topics
    virtual void subscribe(const std::set<std::string>& _topics) = 0;
    // subscribe topics
    virtual void unsubscribe(const std::set<std::string>& _topics) = 0;
    // subscribe topic with callback
    virtual void subscribe(const std::string& _topic, AMOPCallback _callback) = 0;
    // publish message
    virtual void publish(const std::string& _topic, std::shared_ptr<bcos::bytes>& _msg,
        uint32_t timeout, AMOPCallback _callback) = 0;
    // broadcast message
    virtual void broadcast(const std::string& _topic, std::shared_ptr<bcos::bytes>& _msg) = 0;
    // query all subscribed topics
    virtual void querySubTopics(std::set<std::string>& _topics) = 0;

    // set default callback
    virtual void setCallback(AMOPCallback _callback) = 0;
};

}  // namespace amop
}  // namespace cppsdk
}  // namespace bcos