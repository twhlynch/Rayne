//
//  RNEOSHost.cpp
//  Rayne-EOS
//
//  Copyright 2021 by Überpixel. All rights reserved.
//  Unauthorized use is punishable by torture, mutilation, and vivisection.
//

#include "RNEOSHost.h"

#include <RayneConfig.h>

#include "RNEOSWorld.h"

#include "eos_platform_prereqs.h"
#include "eos_sdk.h"
#include "eos_common.h"
#include "eos_p2p.h"
#include "eos_p2p_types.h"

#define MAX_PACKET_SIZE 1000 //Max packet size: 1170, but seems to have issues, so trying 1000

namespace RN
{
	RNDefineMeta(EOSHost, Object)

	EOSHost::EOSHost() : _pingTimer(10.0), _status(Status::Disconnected)
	{
		
	}
		
	EOSHost::~EOSHost()
	{
		
	}

	bool EOSHost::IsPacketInOrder(EOSHost::ProtocolPacketType packetType, uint16 senderID, uint8 packetID, uint8 channel)
	{
		EOSWorld *world = EOSWorld::GetInstance();
		Peer &peer = _peers[senderID];
		
		//Send ack for reliable data. EOS has something like this internally, but does not expose any of it :(
		if(packetType == ProtocolPacketTypeReliableData)
		{
			EOS_P2P_SocketId socketID = {0};
			socketID.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
			socketID.SocketName[0] = 'F';
			socketID.SocketName[1] = 'u';
			socketID.SocketName[2] = 'c';
			socketID.SocketName[3] = 'k';
			socketID.SocketName[4] = 'Y';
			socketID.SocketName[5] = 'e';
			socketID.SocketName[6] = 'a';
			socketID.SocketName[7] = 'h';
			
			ProtocolPacketHeader packetHeader;
			packetHeader.packetType = ProtocolPacketTypeReliableDataAck;
			packetHeader.packetID = packetID;
			packetHeader.dataLength = 0;
			
			EOS_P2P_SendPacketOptions sendPacketOptions = {0};
			sendPacketOptions.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
			sendPacketOptions.Channel = channel;
			sendPacketOptions.LocalUserId = world->GetUserID();
			sendPacketOptions.RemoteUserId = peer.internalID;
			sendPacketOptions.SocketId = &socketID;
			sendPacketOptions.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
			sendPacketOptions.bAllowDelayedDelivery = false;
			sendPacketOptions.Data = &packetHeader;
			sendPacketOptions.DataLengthBytes = sizeof(packetHeader);
			EOS_P2P_SendPacket(world->GetP2PHandle(), &sendPacketOptions);
		}
		
		//Handle reliable data ack
		if(packetType == ProtocolPacketTypeReliableDataAck)
		{
			//TODO: need to somehow flag reliable data in transit per channel instead of global for peer!?
			if(peer._hasReliableInTransit && packetID == peer._lastReliableIDForChannel[channel])
			{
				peer._hasReliableInTransit = false;
			}
		}
		
		//This assumes that less than 127 packets are ever lost at once...
		if(packetType == ProtocolPacketTypeReliableData || peer._receivedIDForChannel[channel] < packetID || (peer._receivedIDForChannel[channel] > 127 && packetID < 127))
		{
			peer._receivedIDForChannel[channel] = packetID;
			return true;
		}
		
		return false;
	}

	void EOSHost::SendPing(uint16 receiverID, bool isResponse, uint8 responseID)
	{
		Lock();
		EOSWorld *world = EOSWorld::GetInstance();
		
		EOS_P2P_SocketId socketID = {0};
		socketID.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		socketID.SocketName[0] = 'F';
		socketID.SocketName[1] = 'u';
		socketID.SocketName[2] = 'c';
		socketID.SocketName[3] = 'k';
		socketID.SocketName[4] = 'Y';
		socketID.SocketName[5] = 'e';
		socketID.SocketName[6] = 'a';
		socketID.SocketName[7] = 'h';
		
		ProtocolPacketHeader packetHeader;
		if(isResponse)
		{
			packetHeader.packetType = ProtocolPacketTypePingResponse;
			packetHeader.packetID = responseID;
			packetHeader.dataLength = 0;
		}
		else
		{
			packetHeader.packetType = ProtocolPacketTypePingRequest;
			packetHeader.packetID = _peers[receiverID]._lastPingID++;
			packetHeader.dataLength = 0;

			_peers[receiverID]._sentPingTime = Clock::now();
		}
		
		EOS_P2P_SendPacketOptions sendPacketOptions = {0};
		sendPacketOptions.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
		sendPacketOptions.Channel = 255;
		sendPacketOptions.LocalUserId = world->GetUserID();
		sendPacketOptions.RemoteUserId = _peers[receiverID].internalID;
		sendPacketOptions.SocketId = &socketID;
		sendPacketOptions.Reliability = EOS_EPacketReliability::EOS_PR_UnreliableUnordered;
		sendPacketOptions.bAllowDelayedDelivery = false;
		sendPacketOptions.Data = &packetHeader;
		sendPacketOptions.DataLengthBytes = sizeof(packetHeader);
		EOS_P2P_SendPacket(world->GetP2PHandle(), &sendPacketOptions);
		Unlock();
	}

	void EOSHost::SendPacket(Data *data, uint16 receiverID, uint32 channel, bool reliable)
	{
		if(_peers[receiverID]._wantsDisconnect) return; //Don't allow sending more data to users that are about to be disconnected.
		
		//Only reliable packets can be split up, unreliable packets need to be small enough to fit a single networking packet
		RN_DEBUG_ASSERT(data->GetLength() < MAX_PACKET_SIZE || reliable, "Packet too big!");
		
		if(!reliable && data->GetLength() >= MAX_PACKET_SIZE) return; //Don't send if unreliable packet is too big. Since it is unreliable, not sending it is acceptable.
		
		Lock();
		if(_peers.size() == 0 || _peers.find(receiverID) == _peers.end())
		{
			Unlock();
			return;
		}
		
		if(_peers[receiverID]._scheduledPackets.find(channel) != _peers[receiverID]._scheduledPackets.end())
		{
			_peers[receiverID]._scheduledPackets.insert(std::pair<uint32, std::queue<Packet> >(channel, std::queue<Packet>()));
		}
		
		_peers[receiverID]._scheduledPackets[channel].push({receiverID, channel, reliable, data->Retain()});
		
		Unlock();
	}

	void EOSHost::Update(float delta)
	{
		Lock();
		EOSWorld *world = EOSWorld::GetInstance();
		
		_pingTimer += delta;
		if(_pingTimer > 5.0)
		{
			_pingTimer = 0.0f;
			for(auto &pair : _peers)
			{
				SendPing(pair.first, false, 0);
			}
		}
		
		EOS_P2P_SocketId socketID = {0};
		socketID.ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		socketID.SocketName[0] = 'F';
		socketID.SocketName[1] = 'u';
		socketID.SocketName[2] = 'c';
		socketID.SocketName[3] = 'k';
		socketID.SocketName[4] = 'Y';
		socketID.SocketName[5] = 'e';
		socketID.SocketName[6] = 'a';
		socketID.SocketName[7] = 'h';
		
		uint32 nextPacketSize = 0;
		uint8 pingChannel = 255;
		EOS_P2P_GetNextReceivedPacketSizeOptions nextPacketSizeOptions = {0};
		nextPacketSizeOptions.ApiVersion = EOS_P2P_GETNEXTRECEIVEDPACKETSIZE_API_LATEST;
		nextPacketSizeOptions.LocalUserId = world->GetUserID();
		nextPacketSizeOptions.RequestedChannel = &pingChannel;
		while(EOS_P2P_GetNextReceivedPacketSize(world->GetP2PHandle(), &nextPacketSizeOptions, &nextPacketSize) == EOS_EResult::EOS_Success)
		{
			if(nextPacketSize < sizeof(ProtocolPacketHeader))
			{
				RNDebug("Packet too small, this is not supposed to ever happen...");
				continue;
			}
			
			EOS_P2P_ReceivePacketOptions receiveOptions = {0};
			receiveOptions.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
			receiveOptions.LocalUserId = world->GetUserID();
			receiveOptions.MaxDataSizeBytes = nextPacketSize;
			receiveOptions.RequestedChannel = &pingChannel;
			
			EOS_ProductUserId senderUserID;
			EOS_P2P_SocketId socketID;
			uint8 channel = 0;
			uint32 bytesWritten = 0;
			
			uint8 *rawData = new uint8[nextPacketSize];
			if(EOS_P2P_ReceivePacket(world->GetP2PHandle(), &receiveOptions, &senderUserID, &socketID, &channel, rawData, &bytesWritten) != EOS_EResult::EOS_Success)
			{
				RNDebug("Failed receiving Data");
				break;
			}
			
			uint16 dataIndex = 0;
			while(dataIndex < bytesWritten)
			{
				ProtocolPacketHeader packetHeader;
				packetHeader.packetType = static_cast<ProtocolPacketType>(rawData[dataIndex + 0]);
				packetHeader.packetID = rawData[dataIndex + 1];
				packetHeader.dataLength = rawData[dataIndex + 2] | (rawData[dataIndex + 3] << 8); //These are pings, so this should always be 0!?
				
				uint16 id = GetUserIDForInternalID(senderUserID);
				if(packetHeader.packetType == ProtocolPacketTypePingRequest)
				{
					SendPing(id, true, packetHeader.packetID);
				}
				else if(packetHeader.packetType == ProtocolPacketTypePingResponse)
				{
					if(_peers[id]._lastPingID-1 == packetHeader.packetID)
					{
						Clock::time_point receivedPingTime = Clock::now();
						auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(receivedPingTime - _peers[id]._sentPingTime).count();
						double timeElapsed = milliseconds / 1000.0;
						
						//RNDebug("Ping time for " << id << ": " << timeElapsed);
						
						_peers[id].smoothedRoundtripTime = _peers[id].smoothedRoundtripTime * 0.75 + timeElapsed * 0.25;
					}
					else
					{
						RNDebug("Missed a ping!");
					}
				}
				
				dataIndex += packetHeader.dataLength + 4;
			}
			
			delete[] rawData;
		}

		//size_t scheduled_count = 0;
		//size_t sent_count = 0;
		for(auto& peer : _peers)
		{
			for(auto& pair : peer.second._scheduledPackets)
			{
				auto& scheduledPackets = pair.second;
				while(scheduledPackets.size() > 0)
				{
					Data *data = new Data();
					bool isReliable = false;
					
					if(scheduledPackets.front().data->GetLength() + 4 >= MAX_PACKET_SIZE) //If it does not fit into a single networking packet
					{
						RN_DEBUG_ASSERT(scheduledPackets.front().isReliable, "Large packets (>= 1000 byte) need to be reliable!");
						
						uint8 packetID = peer.second._packetIDForChannel[pair.first]++;
						peer.second._hasReliableInTransit = true;
						peer.second._lastReliableIDForChannel[pair.first] = packetID;
						isReliable = true;
						
						Data *packetData = scheduledPackets.front().data;
						scheduledPackets.pop();
						
						uint16 totalParts = std::ceil(packetData->GetLength() / static_cast<float>(MAX_PACKET_SIZE - 6)); //Get total number of parts to split the data up to
						uint32 dataOffset = 0;
						for(uint16 currentPart = 0; currentPart < totalParts; currentPart += 1)
						{
							uint8 headerData[2];
							headerData[0] = ProtocolPacketTypeReliableDataMultipart;
							headerData[1] = packetID;
							
							uint16 multiPartHeaderData[2];
							multiPartHeaderData[0] = currentPart;
							multiPartHeaderData[1] = totalParts;
							
							RNDebug("Sending multipart data (" << packetID <<  "), part " << currentPart << " of " << totalParts);
							
							data->Append(headerData, 2);
							data->Append(multiPartHeaderData, 4);
							
							uint32 dataLength = std::min(static_cast<uint32>(MAX_PACKET_SIZE - 6), static_cast<uint32>(packetData->GetLength() - dataOffset));
							data->Append(packetData->GetDataInRange(Range(dataOffset, dataLength)));
							dataOffset += dataLength;
							
							EOS_P2P_SendPacketOptions sendPacketOptions = {0};
							sendPacketOptions.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
							sendPacketOptions.Channel = pair.first;
							sendPacketOptions.LocalUserId = world->GetUserID();
							sendPacketOptions.RemoteUserId = peer.second.internalID;
							sendPacketOptions.SocketId = &socketID;
							sendPacketOptions.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
							sendPacketOptions.bAllowDelayedDelivery = false;
							sendPacketOptions.Data = data->GetBytes();
							sendPacketOptions.DataLengthBytes = data->GetLength();
							EOS_P2P_SendPacket(world->GetP2PHandle(), &sendPacketOptions);
							data->Release(); //Should keep data around and just clear it somehow to not reallocate all the time
							data = new Data();
						}
						
						data->Release();
						packetData->Release();
					}
					else
					{
						//Combine as many packets into one as possible to improve performance
						while(scheduledPackets.size() > 0 && data->GetLength() + scheduledPackets.front().data->GetLength() + 4 < MAX_PACKET_SIZE)
						{
							uint8 headerData[2];
							headerData[1] = peer.second._packetIDForChannel[pair.first]++;
							
							ProtocolPacketType packetType = ProtocolPacketTypeData;
							if(scheduledPackets.front().isReliable)
							{
								isReliable = true;
								packetType = ProtocolPacketTypeReliableData;
								peer.second._hasReliableInTransit = true;
								peer.second._lastReliableIDForChannel[pair.first] = headerData[1];
							}
							headerData[0] = packetType;

							data->Append(headerData, 2);
							uint16 dataLength = scheduledPackets.front().data->GetLength();
							data->Append(&dataLength, 2); //Data length is actually part of the header, but much easier to just set here
							data->Append(scheduledPackets.front().data);
							
							scheduledPackets.front().data->Release();
							scheduledPackets.pop();
							
							//scheduled_count += 1;
						}
						
						EOS_P2P_SendPacketOptions sendPacketOptions = {0};
						sendPacketOptions.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
						sendPacketOptions.Channel = pair.first;
						sendPacketOptions.LocalUserId = world->GetUserID();
						sendPacketOptions.RemoteUserId = peer.second.internalID;
						sendPacketOptions.SocketId = &socketID;
						sendPacketOptions.Reliability = isReliable?EOS_EPacketReliability::EOS_PR_ReliableOrdered:EOS_EPacketReliability::EOS_PR_UnreliableUnordered;
						sendPacketOptions.bAllowDelayedDelivery = false;
						sendPacketOptions.Data = data->GetBytes();
						sendPacketOptions.DataLengthBytes = data->GetLength();
						EOS_P2P_SendPacket(world->GetP2PHandle(), &sendPacketOptions);
						data->Release(); //Should keep data around and just clear it somehow to not reallocate all the time
						
						//sent_count += 1;
					}
				}
			}
		}

		Unlock();
		
		//if(scheduled_count + sent_count > 0) RNDebug("Did send " << scheduled_count << " scheduled packets as " << sent_count << " packets to " << _peers.size() << " peers.");
	}

	EOSHost::Peer EOSHost::CreatePeer(uint16 userID, EOS_ProductUserId internalID)
	{
		Peer peer;
		peer.userID = userID;
		peer.internalID = internalID;
		peer.smoothedRoundtripTime = 0.05;
		peer._lastPingID = 0;
		peer._hasReliableInTransit = false;
		peer._wantsDisconnect = false;
		peer._disconnectDelay = 0.0f;
		
		for(int i = 0; i < 254; i++)
		{
			peer._packetIDForChannel[i] = 0;
			peer._receivedIDForChannel[i] = 255;
			peer._lastReliableIDForChannel[i] = 0;
		}
		
		return peer;
	}

	uint16 EOSHost::GetUserIDForInternalID(EOS_ProductUserId internalID)
	{
		for(auto &pair : _peers)
		{
			if(pair.second.internalID == internalID)
			{
				return pair.first;
			}
		}

		return 0;
	}

	bool EOSHost::HasReliableDataInTransit()
	{
		for(auto &iter : _peers)
		{
			if(iter.second._hasReliableInTransit)
			{
				return true;
			}
		}
		
		return false;
	}

	double EOSHost::GetLastRoundtripTime(uint16 peerID)
	{
		return _peers[peerID].smoothedRoundtripTime;
	}
}
