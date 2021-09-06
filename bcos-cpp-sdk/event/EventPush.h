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
 * @file EvenPush.h
 * @author: octopus
 * @date 2021-09-01
 */

#pragma once
#include <bcos-cpp-sdk/event/EventPushInterface.h>
#include <bcos-cpp-sdk/event/EventPushTask.h>
#include <bcos-cpp-sdk/ws/WsService.h>
#include <shared_mutex>
#include <utility>

namespace bcos
{
namespace cppsdk
{
namespace event
{
class EventPush : public EventPushInterface, public std::enable_shared_from_this<EventPush>
{
public:
    using Ptr = std::shared_ptr<EventPush>;
    EventPush() {}
    virtual ~EventPush() override { stop(); }

public:
    virtual void start() override;
    virtual void stop() override;

    virtual void subscribeEvent(
        const std::string& _group, EventParams::Ptr _params, Callback _callback) override;
    virtual void unsubscribeEvent(const std::string& _id, Callback _callback) override;
    virtual void doLoop();

public:
    void onRecvEventPushMessage(
        std::shared_ptr<ws::WsMessage> _msg, std::shared_ptr<ws::WsSession> _session);
    void onRecvSubEventRespMessage(const std::string& _id, std::shared_ptr<ws::WsMessage> _msg,
        std::shared_ptr<ws::WsSession> _session);

public:
    void addTask(const std::string& _id, EventPushTask::Ptr _task);
    void removeTask(const std::string& _id);
    EventPushTask::Ptr getTask(const std::string& _id);
    EventPushTask::Ptr getTaskAndRemove(const std::string& _id);
    void endOfPush(const std::string& _id);

    void interruptTasks(std::shared_ptr<ws::WsSession> _session);

    void setWsService(ws::WsService::Ptr _wsService) { m_wsService = _wsService; }
    ws::WsService::Ptr wsService() const { return m_wsService; }

    void setTimer(std::shared_ptr<boost::asio::deadline_timer> _timer) { m_timer = _timer; }
    std::shared_ptr<boost::asio::deadline_timer> timer() const { return m_timer; }

    void setIoc(std::shared_ptr<boost::asio::io_context> _ioc) { m_ioc = _ioc; }
    std::shared_ptr<boost::asio::io_context> ioc() const { return m_ioc; }

    void setFactory(std::shared_ptr<ws::WsMessageFactory> _factory) { m_factory = _factory; }
    std::shared_ptr<ws::WsMessageFactory> factory() const { return m_factory; }

private:
    bool m_running = false;
    mutable std::shared_mutex x_tasks;
    std::unordered_map<std::string, EventPushTask::Ptr> m_tasks;
    std::unordered_map<std::string, EventPushTask::Ptr> m_interruptTasks;
    std::set<std::string> m_waitRespTasks;
    // timer
    std::shared_ptr<boost::asio::deadline_timer> m_timer;
    // io context
    std::shared_ptr<boost::asio::io_context> m_ioc;
    // message factory
    std::shared_ptr<ws::WsMessageFactory> m_factory;
    // websocket service
    ws::WsService::Ptr m_wsService;
};
}  // namespace event
}  // namespace cppsdk
}  // namespace bcos
