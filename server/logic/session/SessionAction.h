#ifndef __SESSION_ACTION_H__
#define __SESSION_ACTION_H__

#include <string>
#include "PacketHandler.h"

namespace marz {

namespace Action {

void GetRecentSession(const ChannelPtr& channel_ptr, Packet *packet);
void DelRecentSession(const ChannelPtr& channel_ptr, Packet *packet);

} // namespace action

} // namespace marz

#endif // __SESSION_ACTION_H__