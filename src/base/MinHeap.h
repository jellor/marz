#ifndef __MIN_HEAP_H__
#define __MIN_HEAP_H__

#include <stdlib.h>
#include <iostream>
#include "NonCopyable.h"

namespace marz {

template <class T>
inline bool GREATER(const T& x, const T& y) {
	return *x > *y;
}

template<class T>
class MinHeapInterface {
public:
	virtual ~MinHeapInterface()           = 0;
	virtual bool operator >(const T& rhs) = 0;
	virtual int GetIndex() const          = 0;
	virtual void SetIndex(int index)      = 0;
};

template<class T>
MinHeapInterface<T>::~MinHeapInterface() {}

template<class T>
class MinHeap : public NonCopyable {
public:
	MinHeap();
	explicit MinHeap(unsigned int size);
	~MinHeap();

	bool Empty() const { return size_ == 0; }
	unsigned int Size() const { return size_; }
	unsigned int Capacity() const { return capacity_; }
	const T* Top() const { return Empty() ? nullptr : base_[0]; }
	T* Pop();
	void Push(T* element); // ? element already in base_;
	bool Erase(T* element);
	void Resize(unsigned int size);

	void Print() const {
		for (int i = 0 ; i < size_; i ++ ) {
			std::cout << " Index " << i << " " << base_[i]->GetIndex() << " " << base_[i]->Timeout() << std::endl;
		}
	}

private:
	void ShiftUp(unsigned int holeIndex, T* element);
	void ShiftDown(unsigned int holeIndex, T* element);

	T** base_;
	unsigned int size_;
	unsigned int capacity_;

	static const unsigned int kInitSize = 4;
};

template<class T>
MinHeap<T>::MinHeap():
size_(0)
{
	base_ = static_cast<T**>(malloc(kInitSize * sizeof(T*))); // ? base_ == NULL
	capacity_ = kInitSize;
}

template<class T>
MinHeap<T>::MinHeap(unsigned int size):
size_(0)
{
	base_ = static_cast<T**>(malloc(size * sizeof(T*))); // ? base_ == NULL
	capacity_ = size;
}

template<class T>
MinHeap<T>::~MinHeap() {
	if (!Empty()) {
		for (int i = 0; i < size_; i ++) {
			delete base_[i];
		}
	}
	if (base_ != NULL) free(base_);
}

template<class T>
T* MinHeap<T>::Pop() {
	if (__builtin_expect(size_ == 0, 0)) {
		return nullptr;
	}
	T* top = base_[0];
	ShiftDown(0u, base_[--size_]);
	return top;
}

template<class T>
void MinHeap<T>::Push(T* element) {
	int index = element->GetIndex();
	if (0 <= index && index < size_) {
		// element is already pushed in MinHeap.
		return ;
	}
	if (__builtin_expect(size_ >= capacity_, 0)) {
		unsigned int size = capacity_ << 1;
		Resize(size);
	}
	ShiftUp(size_, element);
	size_ ++;
}

template<class T>
bool MinHeap<T>::Erase(T* element) {
	int index = element->GetIndex();
	if (__builtin_expect(0 <= index && index < size_, 1)) {
		if (index != 0 && GREATER(base_[(index - 1) / 2], base_[index])) {
			ShiftUp(index, base_[-- size_]);
		} else {
			ShiftDown(index, base_[-- size_]);
		}
		return true;
	} else {
		// element is not in MinHeap.
		return false;
	}
}

template<class T>
void MinHeap<T>::Resize(unsigned int size) {
	T** new_base = static_cast<T**>(realloc(base_, size * sizeof(T*)));
	if (new_base == NULL) {
		// realloc() failed.
		free(base_);
		capacity_ = 0;
	} else { 
		capacity_ = size;
	}

	base_ = new_base;
}

template<class T>
void MinHeap<T>::ShiftUp(unsigned int holeIndex, T* element) {
	unsigned int parentIndex = (holeIndex - 1) / 2; // ? when holeIndex is zero, what will happen
	while (parentIndex < holeIndex && GREATER(base_[parentIndex], element)) {
		(base_[holeIndex] = base_[parentIndex])->SetIndex(holeIndex);
		holeIndex = parentIndex;
		parentIndex = (holeIndex - 1) / 2;
	}
	(base_[holeIndex] = element)->SetIndex(holeIndex);
}

template<class T>
void MinHeap<T>::ShiftDown(unsigned int holeIndex, T* element) {
	unsigned int minIndex = holeIndex * 2 + 1;
	while (minIndex < size_) {
		minIndex += (minIndex + 1) < size_ && GREATER(base_[minIndex], base_[minIndex + 1]);
		if (GREATER(base_[minIndex], element)) {
			break;
		}
		(base_[holeIndex] = base_[minIndex])->SetIndex(holeIndex);
		holeIndex = minIndex;
		minIndex = holeIndex * 2 + 1;
	}
	(base_[holeIndex] = element)->SetIndex(holeIndex);
}

} // namespace marz

#endif // __MIN_HEAP_H__
