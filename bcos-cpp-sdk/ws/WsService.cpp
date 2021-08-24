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
 * @file WsService.cpp
 * @author: octopus
 * @date 2021-07-28
 */
#include <bcos-cpp-sdk/ws/Common.h>
#include <bcos-cpp-sdk/ws/WsMessageType.h>
#include <bcos-cpp-sdk/ws/WsService.h>
#include <bcos-cpp-sdk/ws/WsSession.h>
#include <bcos-framework/interfaces/crypto/KeyInterface.h>
#include <bcos-framework/interfaces/protocol/CommonError.h>
#include <bcos-framework/libutilities/Common.h>
#include <bcos-framework/libutilities/DataConvertUtility.h>
#include <bcos-framework/libutilities/Log.h>
#include <bcos-framework/libutilities/ThreadPool.h>
#include <json/json.h>
#include <boost/core/ignore_unused.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

using namespace bcos;
using namespace bcos::ws;

void WsService::start()
{
    if (m_running)
    {
        WEBSOCKET_SERVICE(INFO) << LOG_BADGE("start") << LOG_DESC("websocket service is running");
        return;
    }
    m_running = true;
    reconnect();
    WEBSOCKET_SERVICE(INFO) << LOG_BADGE("start")
                            << LOG_DESC("start websocket service successfully");
}

void WsService::stop()
{
    if (!m_running)
    {
        WEBSOCKET_SERVICE(INFO) << LOG_BADGE("stop")
                                << LOG_DESC("websocket service has been stopped");
        return;
    }
    m_running = false;

    if (m_reconnect)
    {
        m_reconnect->cancel();
    }

    WEBSOCKET_SERVICE(INFO) << LOG_BADGE("stop") << LOG_DESC("stop websocket service successfully");
}

void WsService::reconnect()
{
    auto ss = sessions();
    auto peers = m_config->peers();
    for (auto const& peer : peers)
    {
        std::string connectedEndPoint = peer.host + ":" + std::to_string(peer.port);
        auto session = getSession(connectedEndPoint);
        if (session)
        {
            // TODO: add heartbeat logic, ping/pong message to be send
            continue;
        }

        WEBSOCKET_SERVICE(DEBUG) << LOG_BADGE("reconnect") << LOG_DESC("try to connect to peer")
                                 << LOG_KV("connectedEndPoint", connectedEndPoint);

        std::string host = peer.host;
        uint16_t port = peer.port;
        auto self = std::weak_ptr<WsService>(shared_from_this());
        m_tools->connectToWsServer(m_resolver, m_ioc, host, port,
            [self, connectedEndPoint](
                std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>>
                    _stream) {
                auto service = self.lock();
                if (!service)
                {
                    return;
                }

                auto session = service->newSession(_stream);
                session->setConnectedEndPoint(connectedEndPoint);
                service->addSession(session);
            });
    }

    WEBSOCKET_SERVICE(INFO) << LOG_BADGE("heartbeat") << LOG_DESC("connected server")
                            << LOG_KV("count", ss.size());

    m_reconnect = std::make_shared<boost::asio::deadline_timer>(boost::asio::make_strand(*m_ioc),
        boost::posix_time::milliseconds(m_config->reconnectPeriod()));
    auto self = std::weak_ptr<WsService>(shared_from_this());
    m_reconnect->async_wait([self](const boost::system::error_code&) {
        auto service = self.lock();
        if (!service)
        {
            return;
        }
        service->reconnect();
    });
}

void WsService::initMethod()
{
    m_msgType2Method.clear();

    auto self = std::weak_ptr<WsService>(shared_from_this());
    m_msgType2Method[WsMessageType::BLOCK_NOTIFY] = [self](std::shared_ptr<WsMessage> _msg,
                                                        std::shared_ptr<WsSession> _session) {
        auto service = self.lock();
        if (service)
        {
            service->onRecvBlkNotify(_msg, _session);
        }
    };
    m_msgType2Method[WsMessageType::AMOP_REQUEST] = [self](std::shared_ptr<WsMessage> _msg,
                                                        std::shared_ptr<WsSession> _session) {
        auto service = self.lock();
        if (service)
        {
            service->onRecvAMOPRequest(_msg, _session);
        }
    };
    m_msgType2Method[WsMessageType::AMOP_RESPONSE] = [self](std::shared_ptr<WsMessage> _msg,
                                                         std::shared_ptr<WsSession> _session) {
        auto service = self.lock();
        if (service)
        {
            service->onRecvAMOPResponse(_msg, _session);
        }
    };
    m_msgType2Method[WsMessageType::AMOP_BROADCAST] = [self](std::shared_ptr<WsMessage> _msg,
                                                          std::shared_ptr<WsSession> _session) {
        auto service = self.lock();
        if (service)
        {
            service->onRecvAMOPBroadcast(_msg, _session);
        }
    };

    WEBSOCKET_SERVICE(INFO) << LOG_BADGE("initMethod")
                            << LOG_KV("methods", m_msgType2Method.size());
    for (const auto& method : m_msgType2Method)
    {
        WEBSOCKET_SERVICE(INFO) << LOG_BADGE("initMethod") << LOG_KV("type", method.first);
    }
}

std::shared_ptr<WsSession> WsService::newSession(
    std::shared_ptr<boost::beast::websocket::stream<boost::beast::tcp_stream>> _stream)
{
    auto remoteEndPoint = _stream->next_layer().socket().remote_endpoint();
    std::string endPoint =
        remoteEndPoint.address().to_string() + ":" + std::to_string(remoteEndPoint.port());

    auto wsSession = std::make_shared<bcos::ws::WsSession>(std::move(*_stream));
    wsSession->setThreadPool(threadPool());
    wsSession->setMessageFactory(messageFactory());
    wsSession->setEndPoint(endPoint);

    auto self = std::weak_ptr<bcos::ws::WsService>(shared_from_this());
    wsSession->setRecvMessageHandler([self](std::shared_ptr<bcos::ws::WsMessage> _msg,
                                         std::shared_ptr<bcos::ws::WsSession> _session) {
        auto wsService = self.lock();
        if (wsService)
        {
            wsService->onRecvMessage(_msg, _session);
        }
    });
    wsSession->setDisconnectHandler(
        [self](bcos::Error::Ptr _error, std::shared_ptr<ws::WsSession> _session) {
            auto wsService = self.lock();
            if (wsService)
            {
                wsService->onDisconnect(_error, _session);
            }
        });

    WEBSOCKET_SERVICE(INFO) << LOG_BADGE("newSession") << LOG_KV("endPoint", endPoint);
    wsSession->run();
    return wsSession;
}

void WsService::addSession(std::shared_ptr<WsSession> _session)
{
    auto connectedEndPoint = _session->connectedEndPoint();
    auto endpoint = _session->endPoint();
    bool ok = false;
    {
        std::unique_lock lock(x_mutex);
        auto it = m_sessions.find(connectedEndPoint);
        if (it == m_sessions.end())
        {
            m_sessions[connectedEndPoint] = _session;
            ok = true;
        }
    }

    WEBSOCKET_SERVICE(INFO) << LOG_BADGE("addSession")
                            << LOG_KV("connectedEndPoint", connectedEndPoint)
                            << LOG_KV("endPoint", endpoint) << LOG_KV("result", ok);
}

void WsService::removeSession(const std::string& _endPoint)
{
    {
        std::unique_lock lock(x_mutex);
        m_sessions.erase(_endPoint);
    }

    WEBSOCKET_SERVICE(INFO) << LOG_BADGE("removeSession") << LOG_KV("endpoint", _endPoint);
}

std::shared_ptr<WsSession> WsService::getSession(const std::string& _endPoint)
{
    std::shared_lock lock(x_mutex);
    auto it = m_sessions.find(_endPoint);
    if (it != m_sessions.end())
    {
        return it->second;
    }
    return nullptr;
}

WsSessions WsService::sessions()
{
    WsSessions sessions;
    {
        std::shared_lock lock(x_mutex);
        for (const auto& session : m_sessions)
        {
            if (session.second && session.second->isConnected())
            {
                sessions.push_back(session.second);
            }
        }
    }

    WEBSOCKET_SERVICE(TRACE) << LOG_BADGE("sessions") << LOG_KV("size", sessions.size());
    return sessions;
}

/**
 * @brief: websocket session disconnect
 * @param _msg: received message
 * @param _error:
 * @param _session: websocket session
 * @return void:
 */
void WsService::onDisconnect(Error::Ptr _error, std::shared_ptr<WsSession> _session)
{
    boost::ignore_unused(_error);
    std::string endpoint = "";
    std::string connectedEndPoint = "";
    if (_session)
    {
        endpoint = _session->endPoint();
        connectedEndPoint = _session->connectedEndPoint();
    }

    // clear the session
    removeSession(connectedEndPoint);
    // Add additional disconnect logic

    WEBSOCKET_SERVICE(INFO) << LOG_BADGE("onDisconnect") << LOG_KV("endpoint", endpoint)
                            << LOG_KV("connectedEndPoint", connectedEndPoint);
}

void WsService::onRecvMessage(std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session)
{
    auto seq = std::string(_msg->seq()->begin(), _msg->seq()->end());

    WEBSOCKET_SERVICE(TRACE) << LOG_BADGE("onRecvMessage") << LOG_KV("type", _msg->type())
                             << LOG_KV("seq", seq) << LOG_KV("endpoint", _session->endPoint())
                             << LOG_KV("data size", _msg->data()->size());

    auto it = m_msgType2Method.find(_msg->type());
    if (it != m_msgType2Method.end())
    {
        auto callback = it->second;
        callback(_msg, _session);
    }
    else
    {
        WEBSOCKET_SERVICE(ERROR) << LOG_BADGE("onRecvMessage")
                                 << LOG_DESC("unrecognized message type")
                                 << LOG_KV("type", _msg->type())
                                 << LOG_KV("endpoint", _session->endPoint()) << LOG_KV("seq", seq)
                                 << LOG_KV("data size", _msg->data()->size());
    }
}

void WsService::onRecvAMOPRequest(
    std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session)
{
    auto request = m_requestFactory->buildRequest();
    request->decode(bytesConstRef(_msg->data()->data(), _msg->data()->size()));
    auto data = std::string(request->data().begin(), request->data().end());
    WEBSOCKET_VERSION(INFO) << LOG_DESC("onRecvAMOPRequest")
                            << LOG_KV("endpoint", _session->endPoint()) << LOG_KV("message", data);

    _msg->setType(WsMessageType::AMOP_RESPONSE);
    _msg->setData(std::make_shared<bcos::bytes>(request->data().begin(), request->data().end()));
    // NOTE: just send the message response
    _session->asyncSendMessage(_msg);
}

void WsService::asyncSendMessage(
    std::shared_ptr<WsMessage> _msg, Options _options, RespCallBack _respFunc)
{
    auto seq = std::string(_msg->seq()->begin(), _msg->seq()->end());
    auto ss = sessions();
    class Retry : public std::enable_shared_from_this<Retry>
    {
    public:
        WsSessions ss;
        std::shared_ptr<WsMessage> msg;
        Options options;
        RespCallBack respFunc;

    public:
        void sendMessage()
        {
            if (ss.empty())
            {
                // TODO: error code define
                auto error = std::make_shared<Error>(
                    bcos::protocol::CommonError::TIMEOUT, "send message to server failed");
                respFunc(error, nullptr, nullptr);
                return;
            }

            auto seed = std::chrono::system_clock::now().time_since_epoch().count();
            std::default_random_engine e(seed);
            std::shuffle(ss.begin(), ss.end(), e);

            auto session = *ss.begin();
            ss.erase(ss.begin());

            auto self = shared_from_this();
            session->asyncSendMessage(msg, options,
                [self](bcos::Error::Ptr _error, std::shared_ptr<WsMessage> _msg,
                    std::shared_ptr<WsSession> _session) {
                    if (_error && _error->errorCode() != bcos::protocol::CommonError::SUCCESS)
                    {
                        WEBSOCKET_VERSION(WARNING)
                            << LOG_BADGE("asyncSendMessage") << LOG_DESC("callback error")
                            << LOG_KV("endpoint", _session->endPoint())
                            << LOG_KV("errorCode", _error ? _error->errorCode() : -1)
                            << LOG_KV("errorMessage",
                                   _error ? _error->errorMessage() : std::string(""));
                        return self->sendMessage();
                    }

                    self->respFunc(_error, _msg, _session);
                });
        }
    };

    std::size_t size = ss.size();
    auto retry = std::make_shared<Retry>();
    retry->ss = ss;
    retry->msg = _msg;
    retry->options = _options;
    retry->respFunc = _respFunc;
    retry->sendMessage();

    WEBSOCKET_VERSION(DEBUG) << LOG_BADGE("asyncSendMessage") << LOG_KV("seq", seq)
                             << LOG_KV("size", size);
}

void WsService::onRecvAMOPResponse(
    std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session)
{
    auto strMsg = std::string(_msg->data()->begin(), _msg->data()->end());
    WEBSOCKET_VERSION(INFO) << LOG_DESC("onRecvAMOPResponse")
                            << LOG_KV("endpoint", _session->endPoint())
                            << LOG_KV("message", strMsg);
}

void WsService::onRecvAMOPBroadcast(
    std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session)
{
    auto strMsg = std::string(_msg->data()->begin(), _msg->data()->end());
    WEBSOCKET_VERSION(INFO) << LOG_DESC("onRecvAMOPBroadcast")
                            << LOG_KV("endpoint", _session->endPoint())
                            << LOG_KV("message", strMsg);
}

void WsService::onRecvBlkNotify(
    std::shared_ptr<WsMessage> _msg, std::shared_ptr<WsSession> _session)
{
    auto jsonValue = std::string(_msg->data()->begin(), _msg->data()->end());
    WEBSOCKET_VERSION(INFO) << LOG_DESC("onRecvBlkNotify") << LOG_KV("blockNumber", jsonValue)
                            << LOG_KV("endpoint", _session->endPoint());
}

void WsService::subscribe(const std::set<std::string> _topics, std::shared_ptr<WsSession> _session)
{
    Json::Value jTopics(Json::arrayValue);
    for (const auto& topic : _topics)
    {
        jTopics.append(topic);
    }
    Json::Value jReq;
    jReq["topics"] = jTopics;
    Json::FastWriter writer;
    std::string request = writer.write(jReq);

    auto msg = m_messageFactory->buildMessage();
    msg->setType(bcos::ws::WsMessageType::AMOP_SUBTOPIC);
    msg->setData(std::make_shared<bcos::bytes>(request.begin(), request.end()));

    WEBSOCKET_VERSION(INFO) << LOG_DESC("subscribe") << LOG_KV("topics", request);

    _session->asyncSendMessage(msg);
}

void WsService::publish(const std::string& _topic, std::shared_ptr<bcos::bytes> _msg,
    std::shared_ptr<WsSession> _session,
    std::function<void(Error::Ptr, std::shared_ptr<bcos::bytes>)> _callback)
{
    auto requestFactory = std::make_shared<bcos::ws::AMOPRequestFactory>();
    auto request = requestFactory->buildRequest();
    request->setTopic(_topic);
    request->setData(bytesConstRef(_msg->data(), _msg->size()));
    auto buffer = std::make_shared<bcos::bytes>();
    request->encode(*buffer);

    auto message = m_messageFactory->buildMessage();
    message->setType(bcos::ws::WsMessageType::AMOP_REQUEST);
    message->setData(buffer);

    _session->asyncSendMessage(message);

    boost::ignore_unused(_callback);
    //   if (_callback) {
    //     // TODO:
    //   }
}

void WsService::broadcast(const std::string& _topic, std::shared_ptr<bcos::bytes> _msg,
    std::shared_ptr<WsSession> _session)
{
    auto requestFactory = std::make_shared<bcos::ws::AMOPRequestFactory>();
    auto request = requestFactory->buildRequest();
    request->setTopic(_topic);
    request->setData(bytesConstRef(_msg->data(), _msg->size()));
    auto buffer = std::make_shared<bcos::bytes>();
    request->encode(*buffer);

    auto message = m_messageFactory->buildMessage();
    message->setType(bcos::ws::WsMessageType::AMOP_BROADCAST);
    message->setData(buffer);

    _session->asyncSendMessage(message);
}