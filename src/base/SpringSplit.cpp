#include "StringSplit.h"

namespace marz {

StringSplit::StringSplit(const char* str, const char seperator):
item_count_(0),
item_list_(nullptr)
{
	if (str == nullptr) {
		return;
	}

	item_count_ = 1;
	char* pos = const_cast<char*>(str);
	while (*pos != '\0') {
		if (*pos == seperator) {
			item_count_ ++;
		}

		pos ++;
	}

	item_list_ = new char* [item_count_];
	uint32_t index = 0;
	char* start = pos = const_cast<char*>(str);
	while (*pos != '\0') {
		if (*pos == seperator && start != pos) {
			uint32_t len = pos - start;
			item_list_[index] = new char[len + 1];
			memcpy(item_list_[index], start, len);
			item_list_[index][len] = '\0';

			index ++;
			start = pos + 1;
		}

		pos ++;
	}

	uint32_t len = pos - start;
	if (len != 0) {
		item_list_[index] = new char[len + 1];
		memcpy(item_list_[index], start, len);
		item_list_[index][len] = '\0';
	}
}

StringSplit::~StringSplit()
{
	for (uint32_t i = 0; i < item_count_; i ++) {
		delete[] item_list_[i];
	}

	delete[] item_list_;
}

} // namespace marz
