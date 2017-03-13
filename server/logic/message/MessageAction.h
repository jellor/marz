#ifndef __MESSAGE_ACTION_H__
#define __MESSAGE_ACTION_H__

#include <string>
#include "PacketHandler.h"

namespace marz {

namespace Action {

void Login(const ChannelPtr& channel_ptr, Packet *packet);

} // namespace Action

} // namespace marz

#endif // __MESSAGE_ACTION_H__