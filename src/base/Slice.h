#ifndef __SLICE_H__
#define __SLICE_H__

#include <string.h>
#include <assert.h>
#include <string>

namespace marz {

class Slice {
public:
	Slice(): data_(""), size_(0) {}
	Slice(const char *str): data_(str), size_(strlen(str)) {}
	Slice(const char *data, size_t size): data_(data), size_(size) {}
	Slice(const std::string& str): data_(str.c_str()), size_(str.size()) {}

	const char *Data() const { return data_; }
	size_t Size() const { return size_; }
	bool Empty() const { return size_ == 0; }
	void Clear() {	data_ = ""; size_ = 0; }
	std::string ToString() const { return std::string(data_, size_); }

	char operator[](size_t index) const {
		assert(index >= 0 && index < size_);
		return data_[index];
	}

private:
	const char *data_;
	size_t size_;
	
};

} // namespace marz

#endif // __SLICE_H__
