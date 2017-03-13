#ifndef __SELECTOR_H__
#define __SELECTOR_H__

#include <stdlib.h>
#include "base/NonCopyable.h"
#include "Handle.h"

namespace marz {

class Handle;

class Selector : public NonCopyable {
public:
	Selector()  		= default;
	virtual ~Selector() = default;

	virtual void AddHandle(Handle* handle) 	    = 0;
	virtual void UpdateHandle(Handle* handle)   = 0;
	virtual void RemoveHandle(Handle* handle)   = 0;
	virtual void Dispatch(int64_t timeout_usec) = 0;

	static Selector* NewSelector();

};

} // namespace marz

#endif // __SELECTOR_H__
