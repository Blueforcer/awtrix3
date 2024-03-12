/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
 ****************************************************/

#ifndef _MeanFilterLIB_h
#define _MeanFilterLIB_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

template <typename T>
class MeanFilter
{
public:
	MeanFilter<T>(const size_t windowSize);
	T AddValue(const T value);
	T GetFiltered();

private:
	T* _items;
	T* _accessor;
	int _windowSize;
	int _count;
	T _sum;

	void addToBuffer(const T value);
	void incCounter();
};

template<typename T>
MeanFilter<T>::MeanFilter(const size_t windowSize)
{
	_items = new T[windowSize];
	_accessor = _items;
	_windowSize = windowSize;
	_count = 0;
}

template<typename T>
T MeanFilter<T>::AddValue(const T value)
{
	_sum += value;

	if (_count >= _windowSize)
		_sum -= *_accessor;

	addToBuffer(value);
	incCounter();

	return GetFiltered();
}

template<typename T>
T MeanFilter<T>::GetFiltered()
{
	return (_sum / _count);
}

template<typename T>
inline void MeanFilter<T>::addToBuffer(const T value)
{
	*_accessor = value;
	
	++_accessor;
	if (_accessor >= _items + _windowSize)
		_accessor = _items;

}

template<typename T>
inline void MeanFilter<T>::incCounter()
{
	if (_count < _windowSize)
		++_count;
}

#endif