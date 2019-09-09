#pragma once
#include <vector>

template<class T>
class TArray //: public std::vector<T>
{
public:
	T *val;
	int size;
	T& at(int i) {
		if (i < size)
			return val[i];
		__debugbreak();
		return val[0];
	}

	//TArray()
	//	: std::vector<T>() {}
	//TArray(TArray<T>&& Other)
	//	: std::vector<T>(std::move(Other)) {}
	//TArray(const TArray<T>& Other)
	//	: std::vector<T>(Other) {}
	//TArray(std::initializer_list<T> l)
	//	: std::vector<T>(l) {}
	//void operator=(TArray<T>&& Other)
	//{
	//	std::vector<T>::operator=(std::move(Other));
	//}
	//void SetNum(const int32_t Size)
	//{
	//	std::vector<T>::resize(Size);
	//}
	//int32_t Num() const
	//{
	//	return static_cast<int32_t>(std::vector<T>::size());
	//}
	//void Add(const T& Elem)
	//{
	//	std::vector<T>::push_back(Elem);
	//}
	///*void Sort()
	//{
	//	std::sort(begin(), end());
	//}*/
};