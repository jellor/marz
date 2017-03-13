#include "Selector.h"
#include "selector/EpollSelector.h"
#include "selector/KqueueSelector.h"

namespace marz {

Selector* Selector::NewSelector() {
#ifdef __linux__
	return new EpollSelector();
#elif __APPLE__
	return new KqueueSelector();
#else
#error "Platform Not Supported"
#endif
}

} // namespace marz 
