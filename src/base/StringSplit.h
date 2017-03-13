#ifndef __STRING_SPLIT_H__
#define __STRING_SPLIT_H__

#include <string.h>
#include <stdint.h>

namespace marz {

class StringSplit {
public:
	StringSplit(const char* str, const char seperator);
	~StringSplit();

	uint32_t GetItemCount() const { return item_count_; }
	char* GetItem(uint32_t index) const { return item_list_[index]; }

private:
	uint32_t item_count_;
	char** item_list_;

};

} // namespace marz

#endif // __STRING_SPLIT_H__
