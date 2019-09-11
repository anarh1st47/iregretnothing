#pragma once
#include <vector>

template<class T>
class TArray //: public std::vector<T>
{
public:
	T *val;
	int size;
	int max_size;
	T& at(int i) {
		if (i < size)
			return ((T*)val)[i];
		__debugbreak();
		return ((T*)val)[i];
	}
};