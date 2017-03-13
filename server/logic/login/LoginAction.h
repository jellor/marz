#ifndef __LOGIN_ACTION_H__
#define __LOGIN_ACTION_H__

#include <string>
#include "PacketHandler.h"

namespace marz {

namespace Action {

void Login(const ChannelPtr& channel_ptr, Packet *packet);

} // namespace action

} // namespace marz

#endif // __LOGIN_ACTION_H__