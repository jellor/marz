#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include "PacketHandler.h"

namespace marz {

class ClientHandler : public PacketHandler {
public:
	ClientHandler(const ChannelPtr& channel_ptr);
	virtual ~ClientHandler();

	virtual void OnInactive(const ChannelPtr& channel_ptr) override;

	virtual void OnActive(const ChannelPtr& channel_ptr) override;

	virtual void OnMessage(const ChannelPtr& channel_ptr) override;

private:
	bool Check(Packet *packet);

	void HandleHeartBeat(const ChannelPtr& channel_ptr, Packet *packet);

	void HandleUserLogin(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleUserLogout(const ChannelPtr& channel_ptr, Packet* packet);

	void handleDeviceToken(const ChannelPtr& channel_ptr, Packet* packet);

	void handleKickUser(const ChannelPtr& channel_ptr, Packet* packet);

	void handleMsgData(const ChannelPtr& channel_ptr, Packet *packet);

	void handleMsgDataAck(const ChannelPtr& channel_ptr, Packet* packet);

	void handleMsgTime(const ChannelPtr& channel_ptr, Packet* packet);

	void handleMsgList(const ChannelPtr& channel_ptr, Packet* packet);

	void handleMsgByMsgId(const ChannelPtr& channel_ptr, Packet* packet);

	void handleUnreadCount(const ChannelPtr& channel_ptr, Packet* packet);

	void handleReadAck(const ChannelPtr& channel_ptr, Packet* packet);

	void handleLatestMsgId(const ChannelPtr& channel_ptr, Packet* packet);

	void handleP2PCommand(const ChannelPtr& channel_ptr, Packet* packet);

	void handleRecentContactSession(const ChannelPtr& channel_ptr, Packet* packet);

	void handleUserInfo(const ChannelPtr& channel_ptr, Packet* packet);

	void handleRemoveSession(const ChannelPtr& channel_ptr, Packet* packet);

	void handleAllUser(const ChannelPtr& channel_ptr, Packet* packet);

	void handleChangeAvatar(const ChannelPtr& channel_ptr, Packet* packet);

	void handleUsersStatus(const ChannelPtr& channel_ptr, Packet* packet);
	
	void handleNormalList(const ChannelPtr& channel_ptr, Packet* packet);

	void handleGroupList(const ChannelPtr& channel_ptr, Packet* packet);

	void handleCreateGroup(const ChannelPtr& channel_ptr, Packet* packet);

	void handleChangeGroupMember(const ChannelPtr& channel_ptr, Packet* packet);

	void handleShieldGroup(const ChannelPtr& channel_ptr, Packet* packet);

	int id_;
	bool logined_;
	std::string user_name_;
	std::string user_password_;
	uint64_t user_id_;
	uint32_t client_type_;

};

} // namespace marz

#endif // __CLIENT_HANDLER_H__
