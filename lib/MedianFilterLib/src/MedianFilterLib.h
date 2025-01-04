/***************************************************
Copyright (c) 2017 Luis Llamas
(www.luisllamas.es)

Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License
 ****************************************************/

#ifndef _MedianFilterLib_h
#define _MedianFilterLib_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

template <typename T>
class MedianFilter
{

typedef T (MedianFilter::*Action)(T);

public:
	MedianFilter<T>(const size_t windowSize);
	T AddValue(T item);
	T GetFiltered() const;

private:
	struct node
	{
		struct node *next;      // Apunta al siguiente elemento en orden
		T value;
	};

	Action addValue;			// Puntero para permitir cambiar entra algoritmo 3 o N
	T addValue3(T item);
	T addValueN(T item);
	T median3(T a, T b, T c);

	int _windowSize;
	T _lastFiltered;

	const static int _stopper = 0;
	struct node *_buffer;		// Buffer para los elementos por orden de llegada
	struct node *_iterator; 

	struct node _smaller;
	struct node _bigger;
};


template<typename T>
MedianFilter<T>::MedianFilter(const size_t windowSize)
{
	_windowSize = windowSize;
	_buffer = new node[windowSize];
	_iterator = _buffer;

	_smaller = { nullptr, _stopper };
	_bigger = { &_smaller, 0 };

	if(_windowSize == 3)
		addValue = &MedianFilter::addValue3;
	else
		addValue = &MedianFilter::addValueN;
}

template<typename T>
T MedianFilter<T>::AddValue(T value)
{
	return (*this.*addValue)(value);
}

template<typename T>
T MedianFilter<T>::GetFiltered() const
{
	return _lastFiltered;
}

template<typename T>
T MedianFilter<T>::addValueN(T value)
{
	struct node *_successor;
	struct node *_accessor; 
	struct node *_accessorPrev;
	struct node *_median;

	if (value == _stopper)
		++value;

	if ((++_iterator - _buffer) >= _windowSize)
		_iterator = _buffer;

	_iterator->value = value;
	_successor = _iterator->next;
	_median = &_bigger;
	_accessor = &_bigger;
	_accessorPrev = nullptr;

	if (_accessor->next == _iterator)
		_accessor->next = _successor;

	_accessorPrev = _accessor; 
	_accessor = _accessor->next;

	for (int iCount = 0; iCount < _windowSize; ++iCount)
	{
		// Gestion de elementos impares
		if (_accessor->next == _iterator)
			_accessor->next = _successor;

		if (_accessor->value < value)
		{
			_iterator->next = _accessorPrev->next;
			_accessorPrev->next = _iterator;
			value = _stopper;
		};

		_median = _median->next;
		if (_accessor == &_smaller)
			break;

		_accessorPrev = _accessor;  
		_accessor = _accessor->next;

		// Gestion de elementos pares
		if (_accessor->next == _iterator)
			_accessor->next = _successor;

		if (_accessor->value < value)
		{
			_iterator->next = _accessorPrev->next;
			_accessorPrev->next = _iterator;
			value = _stopper;
		}

		if (_accessor == &_smaller)
			break;

		_accessorPrev = _accessor;
		_accessor = _accessor->next;
	}

	_lastFiltered = _median->value;
	return _lastFiltered;
}

template<typename T>
T MedianFilter<T>::addValue3(T value)
{
	if ((++_iterator - _buffer) >= 3)
		_iterator = _buffer;
	_iterator->value = value;

	return median3(_buffer->value, (_buffer + 1)->value, (_buffer + 2)->value);
}
	
template<typename T>
T MedianFilter<T>::median3(T a, T b, T c)
{
	if ((a <= b) && (a <= c))
		return (b <= c) ? b : c;
	if ((b <= a) && (b <= c))
		return (a <= c) ? a : c;
	return (a <= b) ? a : b;
}

#endif

