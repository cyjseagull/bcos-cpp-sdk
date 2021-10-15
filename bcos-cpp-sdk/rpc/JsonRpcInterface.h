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
 * @file JsonRPCInterface.h
 * @author: octopus
 * @date 2021-05-24
 */

#pragma once
#include <bcos-framework/interfaces/protocol/CommonError.h>
#include <bcos-framework/libutilities/Common.h>
#include <bcos-framework/libutilities/Error.h>
#include <functional>
#include <memory>

namespace bcos
{
namespace cppsdk
{
namespace jsonrpc
{
using RespFunc = std::function<void(bcos::Error::Ptr, std::shared_ptr<bcos::bytes>)>;

class JsonRpcInterface
{
public:
    using Ptr = std::shared_ptr<JsonRpcInterface>;

    JsonRpcInterface() = default;
    virtual ~JsonRpcInterface() {}

public:
    virtual void start() = 0;
    virtual void stop() = 0;

public:
    //-------------------------------------------------------------------------------------
    virtual void genericMethod(const std::string& _data, RespFunc _respFunc) = 0;
    virtual void genericMethod(
        const std::string& _groupID, const std::string& _data, RespFunc _respFunc) = 0;
    virtual void genericMethod(const std::string& _groupID, const std::string& _nodeName,
        const std::string& _data, RespFunc _respFunc) = 0;
    //-------------------------------------------------------------------------------------

    virtual void call(const std::string& _groupID, const std::string& _to, const std::string& _data,
        RespFunc _respFunc) = 0;

    virtual void sendTransaction(const std::string& _groupID, const std::string& _data,
        bool _requireProof, RespFunc _respFunc) = 0;

    virtual void getTransaction(const std::string& _groupID, const std::string& _txHash,
        bool _requireProof, RespFunc _respFunc) = 0;

    virtual void getTransactionReceipt(const std::string& _groupID, const std::string& _txHash,
        bool _requireProof, RespFunc _respFunc) = 0;

    virtual void getBlockByHash(const std::string& _groupID, const std::string& _blockHash,
        bool _onlyHeader, bool _onlyTxHash, RespFunc _respFunc) = 0;

    virtual void getBlockByNumber(const std::string& _groupID, int64_t _blockNumber,
        bool _onlyHeader, bool _onlyTxHash, RespFunc _respFunc) = 0;

    virtual void getBlockHashByNumber(
        const std::string& _groupID, int64_t _blockNumber, RespFunc _respFunc) = 0;

    virtual void getBlockNumber(const std::string& _groupID, RespFunc _respFunc) = 0;

    virtual void getCode(
        const std::string& _groupID, const std::string _contractAddress, RespFunc _respFunc) = 0;

    virtual void getSealerList(const std::string& _groupID, RespFunc _respFunc) = 0;

    virtual void getObserverList(const std::string& _groupID, RespFunc _respFunc) = 0;

    virtual void getPbftView(const std::string& _groupID, RespFunc _respFunc) = 0;

    virtual void getPendingTxSize(const std::string& _groupID, RespFunc _respFunc) = 0;

    virtual void getSyncStatus(const std::string& _groupID, RespFunc _respFunc) = 0;

    virtual void getConsensusStatus(const std::string& _groupID, RespFunc _respFunc) = 0;

    virtual void getSystemConfigByKey(
        const std::string& _groupID, const std::string& _keyValue, RespFunc _respFunc) = 0;

    virtual void getTotalTransactionCount(const std::string& _groupID, RespFunc _respFunc) = 0;

    virtual void getPeers(RespFunc _respFunc) = 0;

    // create a new group
    virtual void createGroup(std::string const& _groupInfo, RespFunc _respFunc) = 0;
    // expand new node for the given group
    virtual void expandGroupNode(
        std::string const& _groupID, std::string const& _nodeInfo, RespFunc _respFunc) = 0;
    // remove the given group from the given chain
    virtual void removeGroup(std::string const& _groupID, RespFunc _respFunc) = 0;
    // remove the given node from the given group
    virtual void removeGroupNode(
        std::string const& _groupID, std::string const& _nodeName, RespFunc _respFunc) = 0;
    // recover the given group
    virtual void recoverGroup(std::string const& _groupID, RespFunc _respFunc) = 0;
    // recover the given node of the given group
    virtual void recoverGroupNode(
        std::string const& _groupID, std::string const& _nodeName, RespFunc _respFunc) = 0;
    // start the given node
    virtual void startNode(
        std::string const& _groupID, std::string const& _nodeName, RespFunc _respFunc) = 0;
    // stop the given node
    virtual void stopNode(
        std::string const& _groupID, std::string const& _nodeName, RespFunc _respFunc) = 0;
    // get all the groupID list
    virtual void getGroupList(RespFunc _respFunc) = 0;
    // get all the group informations
    virtual void getGroupInfoList(RespFunc _respFunc) = 0;
    // get the group information of the given group
    virtual void getGroupInfo(std::string const& _groupID, RespFunc _respFunc) = 0;
    // get the information of a given node
    virtual void getGroupNodeInfo(
        std::string const& _groupID, std::string const& _nodeName, RespFunc _respFunc) = 0;

    // TODO: temp interface , should be removed in the end
    virtual void getNodeInfo(RespFunc _respFunc) = 0;
};

}  // namespace jsonrpc
}  // namespace cppsdk
}  // namespace bcos