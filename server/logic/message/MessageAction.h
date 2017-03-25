#ifndef __MESSAGE_ACTION_H__
#define __MESSAGE_ACTION_H__

#include <string>
#include "PacketHandler.h"

namespace marz {

namespace Action {

void SendMessage(const ChannelPtr& channel_ptr, Packet* packet);
void GetMessage(const ChannelPtr& channel_ptr, Packet* packet);
void GetMsgById(const ChannelPtr& channel_ptr, Packet* packet);
void GetLatestMsgId(const ChannelPtr& channel_ptr, Packet* packet);
void GetUnreadMsgCounter(const ChannelPtr& channel_ptr, Packet* packet);
void ClearUnreadMsgCounter(const ChannelPtr& channel_ptr, Packet* packet);
void SetDeviceToken(const ChannelPtr& channel_ptr, Packet* packet);
void GetDeviceToken(const ChannelPtr& channel_ptr, Packet* packet);

} // namespace Action

} // namespace marz

#endif // __MESSAGE_ACTION_H__