#ifndef __GROUP_ACTION_H__
#define __GROUP_ACTION_H__

#include <string>
#include "PacketHandler.h"

namespace marz {

namespace Action {

void CreateGroup(const ChannelPtr& channel_ptr, Packet* packet);
void GetNormalGroupList(const ChannelPtr& channel_ptr, Packet* packet);
void GetGroupInfo(const ChannelPtr& channel_ptr, Packet* packet);
void ModifyMember(const ChannelPtr& channel_ptr, Packet* packet);
void SetGroupPush(const ChannelPtr& channel_ptr, Packet* packet);
void GetGroupPush(const ChannelPtr& channel_ptr, Packet* packet);

} // namespace action

} // namespace marz

#endif // __GROUP_ACTION_H__