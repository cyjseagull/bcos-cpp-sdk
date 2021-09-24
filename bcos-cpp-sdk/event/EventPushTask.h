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
 * @file EvenPushTask.h
 * @author: octopus
 * @date 2021-09-01
 */

#pragma once

#include <bcos-cpp-sdk/event/Common.h>
#include <bcos-cpp-sdk/event/EventPushInterface.h>
#include <bcos-cpp-sdk/event/EventPushParams.h>
#include <bcos-cpp-sdk/ws/WsSession.h>
#include <bcos-framework/libutilities/Log.h>
#include <atomic>

namespace bcos
{
namespace ws
{
class WsSession;
}

namespace cppsdk
{
namespace event
{
class EventPushTaskState
{
public:
    using Ptr = std::shared_ptr<EventPushTaskState>;
    using ConstPtr = std::shared_ptr<const EventPushTaskState>;

public:
    int64_t currentBlockNumber() const { return m_currentBlockNumber.load(); }
    void setCurrentBlockNumber(int64_t _currentBlockNumber)
    {
        m_currentBlockNumber.store(_currentBlockNumber);
    }

private:
    std::atomic<int64_t> m_currentBlockNumber = -1;
};

class EventPushTask
{
public:
    using Ptr = std::shared_ptr<EventPushTask>;
    EventPushTask() { EVENT_TASK(DEBUG) << LOG_KV("[NEWOBJ][EventPushTask]", this); }
    ~EventPushTask() { EVENT_TASK(DEBUG) << LOG_KV("[DELOBJ][EventPushTask]", this); }

public:
    void setSession(std::shared_ptr<ws::WsSession> _session) { m_session = _session; }
    std::shared_ptr<ws::WsSession> session() const { return m_session; }

    void setId(const std::string& _id) { m_id = _id; }
    std::string id() const { return m_id; }

    void setGroup(const std::string& _group) { m_group = _group; }
    std::string group() const { return m_group; }

    void setParams(std::shared_ptr<const EventPushParams> _params) { m_params = _params; }
    std::shared_ptr<const EventPushParams> params() const { return m_params; }

    void setState(std::shared_ptr<EventPushTaskState> _state) { m_state = _state; }
    std::shared_ptr<EventPushTaskState> state() const { return m_state; }

    void setCallback(Callback _callback) { m_callback = _callback; }
    Callback callback() const { return m_callback; }

private:
    std::string m_id;
    std::string m_group;
    Callback m_callback;
    std::shared_ptr<ws::WsSession> m_session;
    std::shared_ptr<const EventPushParams> m_params;
    std::shared_ptr<EventPushTaskState> m_state;
};

using EventPushTaskPtrs = std::vector<EventPushTask::Ptr>;
}  // namespace event
}  // namespace cppsdk
}  // namespace bcos
