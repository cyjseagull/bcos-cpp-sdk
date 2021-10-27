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
 * @file JsonRpcImpl.cpp
 * @author: octopus
 * @date 2021-08-10
 */

#include "libutilities/Common.h"
#include <bcos-cpp-sdk/rpc/Common.h>
#include <bcos-cpp-sdk/rpc/JsonRpcImpl.h>
#include <boost/core/ignore_unused.hpp>
#include <fstream>
#include <memory>
#include <string>

using namespace bcos;
using namespace cppsdk;
using namespace jsonrpc;

void JsonRpcImpl::start()
{
    if (m_service)
    {  // start websocket service
        m_service->start();
    }
    else
    {
        RPCIMPL_LOG(WARNING) << LOG_BADGE("start")
                             << LOG_DESC("websocket service is not uninitialized");
    }
    RPCIMPL_LOG(INFO) << LOG_BADGE("start") << LOG_DESC("start rpc");
}

void JsonRpcImpl::stop()
{
    // TODO:
    RPCIMPL_LOG(INFO) << LOG_BADGE("stop") << LOG_DESC("stop rpc");
}

void JsonRpcImpl::genericMethod(const std::string& _data, RespFunc _respFunc)
{
    m_sender("", "", _data, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("genericMethod") << LOG_KV("request", _data);
}

void JsonRpcImpl::genericMethod(
    const std::string& _groupID, const std::string& _data, RespFunc _respFunc)
{
    m_sender(_groupID, "", _data, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("genericMethod") << LOG_KV("group", _groupID)
                       << LOG_KV("request", _data);
}

void JsonRpcImpl::genericMethod(const std::string& _groupID, const std::string& _nodeName,
    const std::string& _data, RespFunc _respFunc)
{
    boost::ignore_unused(_nodeName);
    m_sender(_groupID, _nodeName, _data, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("genericMethod") << LOG_KV("group", _groupID)
                       << LOG_KV("nodeName", _nodeName) << LOG_KV("request", _data);
}

void JsonRpcImpl::call(const std::string& _groupID, const std::string& _nodeName,
    const std::string& _to, const std::string& _data, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);
    params.append(_to);
    params.append(_data);

    auto request = m_factory->buildRequest("call", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("call") << LOG_KV("request", s);
}

void JsonRpcImpl::sendTransaction(const std::string& _groupID, const std::string& _nodeName,
    const std::string& _data, bool _requireProof, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);
    params.append(_data);
    params.append(_requireProof);

    auto request = m_factory->buildRequest("sendTransaction", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("sendTransaction") << LOG_KV("request", s);
}

void JsonRpcImpl::getTransaction(const std::string& _groupID, const std::string& _nodeName,
    const std::string& _txHash, bool _requireProof, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);
    params.append(_txHash);
    params.append(_requireProof);

    auto request = m_factory->buildRequest("getTransaction", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getTransaction") << LOG_KV("request", s);
}

void JsonRpcImpl::getTransactionReceipt(const std::string& _groupID, const std::string& _nodeName,
    const std::string& _txHash, bool _requireProof, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);
    params.append(_txHash);
    params.append(_requireProof);

    auto request = m_factory->buildRequest("getTransactionReceipt", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getTransactionReceipt") << LOG_KV("request", s);
}

void JsonRpcImpl::getBlockByHash(const std::string& _groupID, const std::string& _nodeName,
    const std::string& _blockHash, bool _onlyHeader, bool _onlyTxHash, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);
    params.append(_blockHash);
    params.append(_onlyHeader);
    params.append(_onlyTxHash);

    auto request = m_factory->buildRequest("getBlockByHash", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getBlockByHash") << LOG_KV("request", s);
}

void JsonRpcImpl::getBlockByNumber(const std::string& _groupID, const std::string& _nodeName,
    int64_t _blockNumber, bool _onlyHeader, bool _onlyTxHash, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    // params.append(_groupID);
    params.append(_blockNumber);
    params.append(_onlyHeader);
    params.append(_onlyTxHash);

    auto request = m_factory->buildRequest("getBlockByNumber", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getBlockByNumber") << LOG_KV("request", s);
}

void JsonRpcImpl::getBlockHashByNumber(const std::string& _groupID, const std::string& _nodeName,
    int64_t _blockNumber, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);
    params.append(_blockNumber);

    auto request = m_factory->buildRequest("getBlockHashByNumber", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getBlockHashByNumber") << LOG_KV("request", s);
}

void JsonRpcImpl::getBlockNumber(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getBlockNumber", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getBlockNumber") << LOG_KV("request", s);
}

void JsonRpcImpl::getCode(const std::string& _groupID, const std::string& _nodeName,
    const std::string _contractAddress, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);
    params.append(_contractAddress);

    auto request = m_factory->buildRequest("getCode", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getCode") << LOG_KV("request", s);
}

void JsonRpcImpl::getSealerList(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getSealerList", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getSealerList") << LOG_KV("request", s);
}

void JsonRpcImpl::getObserverList(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getObserverList", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getObserverList") << LOG_KV("request", s);
}

void JsonRpcImpl::getPbftView(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getPbftView", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getPbftView") << LOG_KV("request", s);
}

void JsonRpcImpl::getPendingTxSize(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getPendingTxSize", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getPendingTxSize") << LOG_KV("request", s);
}

void JsonRpcImpl::getSyncStatus(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getSyncStatus", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getSyncStatus") << LOG_KV("request", s);
}

void JsonRpcImpl::getConsensusStatus(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getConsensusStatus", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getConsensusStatus") << LOG_KV("request", s);
}

void JsonRpcImpl::getSystemConfigByKey(const std::string& _groupID, const std::string& _nodeName,
    const std::string& _keyValue, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);
    params.append(_keyValue);

    auto request = m_factory->buildRequest("getSystemConfigByKey", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getSystemConfigByKey") << LOG_KV("request", s);
}

void JsonRpcImpl::getTotalTransactionCount(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getTotalTransactionCount", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getTotalTransactionCount") << LOG_KV("request", s);
}

void JsonRpcImpl::getPeers(RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    auto request = m_factory->buildRequest("getPeers", params);
    auto s = request->toJson();
    m_sender("", "", s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getPeers") << LOG_KV("request", s);
}

void JsonRpcImpl::getGroupList(RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    auto request = m_factory->buildRequest("getGroupList", params);
    auto s = request->toJson();
    m_sender("", "", s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getGroupList") << LOG_KV("request", s);
}

void JsonRpcImpl::getGroupInfo(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    auto groupInfo = m_service->getGroupInfo(_groupID);
    if (groupInfo)
    {  // get group info from cache
        JsonResponse jsonResp;
        jsonResp.jsonrpc = "2.0";
        jsonResp.id = m_factory->nextId();
        jsonResp.result = groupInfo->serialize();

        auto jsonString = toStringResponse(jsonResp);
        auto jsonData = std::make_shared<bcos::bytes>(jsonString.begin(), jsonString.end());
        _respFunc(nullptr, jsonData);

        RPCIMPL_LOG(DEBUG) << LOG_BADGE("getGroupInfo") << LOG_BADGE("get group info from cache")
                           << LOG_KV("response", jsonString);
    }
    else
    {
        // call remote rpc
        Json::Value params = Json::Value(Json::arrayValue);
        params.append(_groupID);
        params.append(_nodeName);
        auto request = m_factory->buildRequest("getGroupInfo", params);
        auto s = request->toJson();
        m_sender(_groupID, _nodeName, s, _respFunc);
        RPCIMPL_LOG(DEBUG) << LOG_BADGE("getGroupInfo") << LOG_KV("request", s);
    }
}

void JsonRpcImpl::getGroupInfoList(RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);

    auto request = m_factory->buildRequest("getGroupInfoList", params);
    auto s = request->toJson();
    m_sender("", "", s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getGroupNodeInfo") << LOG_KV("request", s);
}

void JsonRpcImpl::getGroupNodeInfo(
    const std::string& _groupID, const std::string& _nodeName, RespFunc _respFunc)
{
    Json::Value params = Json::Value(Json::arrayValue);
    params.append(_groupID);
    params.append(_nodeName);

    auto request = m_factory->buildRequest("getGroupNodeInfo", params);
    auto s = request->toJson();
    m_sender(_groupID, _nodeName, s, _respFunc);
    RPCIMPL_LOG(DEBUG) << LOG_BADGE("getGroupNodeInfo") << LOG_KV("request", s);
}