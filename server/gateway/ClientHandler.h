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

	virtual void OnMessage(const ChannelPtr& channel_ptr, Packet* packet) override;

private:
	bool Check(Packet *packet);

	void HandleHeartBeat(const ChannelPtr& channel_ptr, Packet *packet);

	void HandleUserLogin(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleUserLogout(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleDeviceToken(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleKickUser(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleMsgData(const ChannelPtr& channel_ptr, Packet *packet);

	void HandleMsgDataAck(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleMsgTime(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleMsgList(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleMsgByMsgId(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleUnreadCount(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleReadAck(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleLatestMsgId(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleP2PCommand(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleRecentContactSession(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleUserInfo(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleRemoveSession(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleAllUser(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleChangeAvatar(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleUsersStatus(const ChannelPtr& channel_ptr, Packet* packet);
	
	void HandleNormalList(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleGroupList(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleCreateGroup(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleChangeGroupMember(const ChannelPtr& channel_ptr, Packet* packet);

	void HandleShieldGroup(const ChannelPtr& channel_ptr, Packet* packet);

	int id_;
	bool logined_;
	IpAddress client_addr_;
	std::string user_name_;
	std::string user_password_;
	uint64_t user_id_;
	uint32_t client_type_;

};

} // namespace marz

#endif // __CLIENT_HANDLER_H__
