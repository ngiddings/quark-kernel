/*
 * Bitmap.h
 *
 *  Created on: Jun 1, 2019
 *      Author: nathan
 */

#ifndef SRC_BITMAP_H_
#define SRC_BITMAP_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

namespace qkernel {

/**
 * Represents a fixed array of N bits. Somewhat analogous to std::bitset. Can
 * be manipulated using standard arithmetic operators (&, |, ^, ~).
 */
template <size_t N>
class Bitmap {
public:

	/**
	 * A proxy object used to reference an individual bit contained in a Bitmap object.
	 */
	class Reference
	{
	public:

		/**
		 * Construct a reference to a bit contained in a Bitmap object.
		 *
		 * @param data A reference to the byte in which the referenced bit is
		 * contained
		 * @param position The position of the referenced bit inside 'data.' 0
		 * denotes the least significant bit; 7 denoted the most significant
		 * bit.
		 */
		Reference(uint8_t& data, size_t position);

		/**
		 * Sets the bit referenced by this object to 'b'
		 *
		 * @returns a reference to this object
		 */
		Reference& operator=(const bool b);

		/**
		 * Sets the bit referenced by this object to the value of the bit
		 * referenced by 'r.'
		 *
		 * @returns a reference to this object
		 */
		Reference& operator=(const Reference& r);

		/**
		 * @returns the inverse of the value of the bit referenced by this
		 * object.
		 */
		bool operator~() const;

		/**
		 * Converts the bit referenced by this object to a boolean, whose value
		 * is 'true' if the bit referenced it set, and false if it is clear.
		 */
		operator bool() const;

	private:

		uint8_t& data;

		size_t position;

	};

	/**
	 * Constructs a bitmap containing 'N' bits, containing uninitialized
	 * data.
	 */
	Bitmap();

	/**
	 * Constructs a bitmap containing 'N' bits, copying data from 'bitmap' into
	 * the new object.
	 */
	Bitmap(const Bitmap<N>& bitmap);

	/**
	 * Constructs a bitmap containing 'N' bits, initializing each bit to 'v.'
	 */
	Bitmap(const bool value);

	/**
	 * Constructs a bitmap containing 'N' bits, initializing the first 32 bits
	 * to the bits contained in 'value.' If the bitmap contains more than 32
	 * bits, the rest are initialized to 0.
	 */
	Bitmap(const uint8_t value);

	/**
	 * Constructs a bitmap containing 'N' bits, initializing the first 32 bits
	 * to the bits contained in 'value.' If the bitmap contains more than 32
	 * bits, the rest are initialized to 0.
	 */
	Bitmap(const uint16_t value);

	/**
	 * Constructs a bitmap containing 'N' bits, initializing the first 32 bits
	 * to the bits contained in 'value.' If the bitmap contains more than 32
	 * bits, the rest are initialized to 0.
	 */
	Bitmap(const uint32_t value);

	/**
	 * @returns the number of bits stored in this bitmap
	 */
	size_t size();

	/**
	 * @returns the number of bits that are set
	 */
	size_t count();

	/**
	 * @returns true if all bits are set; otherwise false.
	 */
	bool all();

	/**
	 * @returns true if at least one bit is set; otherwise false.
	 */
	bool any();

	/**
	 * @returns true if all bits are cleared; otherwise false.
	 */
	bool none();

	/**
	 * Sets all bits in this bitmap.
	 *
	 * @returns a reference to this bitmap
	 */
	Bitmap<N>& set();

	/**
	 * Clears all bits in this bitmap.
	 *
	 * @returns a reference to this bitmap
	 */
	Bitmap<N>& clear();

	/**
	 * Compares the contents of 'bitmap' and this bitmap.
	 *
	 * @param bitmap The bitmap to compare this object to.
	 *
	 * @returns true only if each bit in 'other' is equal to each bit in this
	 * bitmap; otherwise false.
	 */
	bool operator==(const Bitmap<N>& other) const;

	/**
	 * Compares the contents of 'bitmap' and this bitmap.
	 *
	 * @param bitmap The bitmap to compare this object to.
	 *
	 * @returns false only if each bit in 'other' is equal to each bit in this
	 * bitmap; otherwise true.
	 */
	bool operator!=(const Bitmap<N>& other) const;

	/**
	 * Accesses the bit at 'index.' Does not perform bounds checking;
	 * out-of-bounds access will result in problems.
	 *
	 * @param index The position in the bitmap to access
	 *
	 * @returns a reference to the bit at 'index'
	 */
	Reference operator[](const size_t index);

	/**
	 * Accesses the bit at 'index.' Does not perform bounds checking;
	 * out-of-bounds access will result in problems.
	 *
	 * @param index The position in the bitmap to access
	 *
	 * @returns the value of the bit at position 'index'
	 */
	bool operator[](const size_t index) const;

	/**
	 * @returns a bitmap containing the bitwise AND of this bitmap and 'other.'
	 */
	Bitmap<N> operator&(const Bitmap<N>& other) const;

	/**
	 * Sets the value of this bitmap to the bitwise AND of this bitmap and
	 * 'other.'
	 *
	 * @returns a reference to this bitmap
	 */
	Bitmap<N>& operator&=(const Bitmap<N>& other);

	/**
	 * @returns a bitmap containing the bitwise OR of this bitmap and 'other.'
	 */
	Bitmap<N> operator|(const Bitmap<N>& other) const;

	/**
	 * Sets the value of this bitmap to the bitwise OR of this bitmap and
	 * 'other.'
	 *
	 * @returns a reference to this bitmap
	 */
	Bitmap<N>& operator|=(const Bitmap<N>& other);

	/**
	 * @returns a bitmap containing the bitwise XOR of this bitmap and 'other.'
	 */
	Bitmap<N> operator^(const Bitmap<N>& other) const;

	/**
	 * Sets the value of this bitmap to the bitwise OR of this bitmap and
	 * 'other.'
	 *
	 * @returns a reference to this bitmap
	 */
	Bitmap<N>& operator^=(const Bitmap<N>& other);

	/**
	 * Shifts this bitmap 'n' bits left.
	 *
	 * @returns a new Bitmap containing the result
	 */
	Bitmap<N> operator<<(const size_t n) const;

	/**
	 * Shifts this bitmap 'n' bits left.
	 *
	 * @returns a reference to this bitmap
	 */
	Bitmap<N>& operator<<=(const size_t n);

	/**
	 * Shifts this bitmap 'n' bits right.
	 *
	 * @returns a new Bitmap containing the result
	 */
	Bitmap<N> operator>>(const size_t n) const;

	/**
	 * Shifts the bitmap 'n' bits right.
	 *
	 * @returns a reference to this bitmap
	 */
	Bitmap<N>& operator>>=(const size_t n);

	/**
	 * Computes the bitwise NOT of this bitmap
	 *
	 * @returns a new bitmap containing the result
	 */
	Bitmap<N> operator~() const;

private:

	uint8_t data[(N / 8) + 1];

};

} /* namespace qkernel */

template<size_t N>
inline qkernel::Bitmap<N>::Reference::Reference(uint8_t& data, size_t position)
	: data(data)
{
	this->position = position;
}

template<size_t N>
inline typename qkernel::Bitmap<N>::Reference& qkernel::Bitmap<N>::Reference::operator =(const bool b) {
	if(b)
	{
		data |= 1 << position;
	}
	else
	{
		data &= ~(1 << position);
	}
	return *this;
}

template<size_t N>
inline typename qkernel::Bitmap<N>::Reference& qkernel::Bitmap<N>::Reference::operator =(const Reference& r) {
	if((bool) r)
	{
		data |= 1 << position;
	}
	else
	{
		data &= ~(1 << position);
	}
}

template<size_t N>
inline bool qkernel::Bitmap<N>::Reference::operator ~() const {
	return !((bool) (*this));
}

template<size_t N>
inline qkernel::Bitmap<N>::Reference::operator bool() const {
	uint8_t value = data & (1 << position);
	if((data & (1 << position)) != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

template<size_t N>
inline qkernel::Bitmap<N>::Bitmap() {
}

template<size_t N>
inline qkernel::Bitmap<N>::Bitmap(const Bitmap<N>& bitmap) {
	for(size_t i = 0; i < N; i++)
	{
		(*this)[i] = bitmap[i];
	}
}

template<size_t N>
inline qkernel::Bitmap<N>::Bitmap(const bool value) {
	for(size_t i = 0; i < N; i++)
	{
		(*this)[i] = value;
	}
}

template<size_t N>
inline qkernel::Bitmap<N>::Bitmap(uint8_t value) {
	size_t max = N >= 8 ? 8 : N;
	for(size_t i = 0; i < max; i++)
	{
		(*this)[i] = value & 1;
		value >>= 1;
	}
}

template<size_t N>
inline qkernel::Bitmap<N>::Bitmap(uint16_t value) {
	size_t max = N >= 16 ? 16 : N;
	for(size_t i = 0; i < max; i++)
	{
		(*this)[i] = value & 1;
		value >>= 1;
	}
}

template<size_t N>
inline qkernel::Bitmap<N>::Bitmap(uint32_t value) {
	size_t max = N >= 32 ? 32 : N;
	for(size_t i = 0; i < max; i++)
	{
		(*this)[i] = value & 1;
		value >>= 1;
	}
}

template<size_t N>
inline size_t qkernel::Bitmap<N>::size() {
	return N;
}

template<size_t N>
inline size_t qkernel::Bitmap<N>::count() {
	size_t count = 0;
	for(size_t i = 0; i < N; i++)
	{
		if((*this)[i] == true)
		{
			count++;
		}
	}
	return count;
}

template<size_t N>
inline bool qkernel::Bitmap<N>::all() {
	for(size_t i = 0; i < N; i++)
	{
		if((*this)[i] == false)
		{
			return false;
		}
	}
	return true;
}

template<size_t N>
inline bool qkernel::Bitmap<N>::any() {
	for(size_t i = 0; i < N; i++)
	{
		if((*this)[i] == true)
		{
			return true;
		}
	}
	return false;
}

template<size_t N>
inline bool qkernel::Bitmap<N>::none() {
	for(size_t i = 0; i < N; i++)
	{
		if((*this)[i] == true)
		{
			return false;
		}
	}
	return true;
}

template<size_t N>
inline qkernel::Bitmap<N>& qkernel::Bitmap<N>::set() {
	for(size_t i = 0; i < N; i++)
	{
		(*this)[i] = true;
	}
	return *this;
}

template<size_t N>
inline qkernel::Bitmap<N>& qkernel::Bitmap<N>::clear() {
	for(size_t i = 0; i < N; i++)
	{
		(*this)[i] = false;
	}
	return *this;
}

template<size_t N>
inline bool qkernel::Bitmap<N>::operator ==(const Bitmap<N>& other) const {
	for(size_t i = 0; i < N; i++)
	{
		if((*this)[i] != other[i])
		{
			return false;
		}
	}
	return true;
}

template<size_t N>
inline bool qkernel::Bitmap<N>::operator !=(const Bitmap<N>& other) const {
	return !((*this) == other);
}

template<size_t N>
inline typename qkernel::Bitmap<N>::Reference qkernel::Bitmap<N>::operator [](const size_t index) {
	return Reference(data[index / 8], index % 8);
}

template<size_t N>
inline bool qkernel::Bitmap<N>::operator [](const size_t index) const {
	return (data[index/8] & (1 << (index % 8))) == 0 ? false : true;
}

template<size_t N>
inline qkernel::Bitmap<N> qkernel::Bitmap<N>::operator &(const Bitmap<N>& other) const {
	Bitmap<N> result;
	for(size_t i = 0; i < N; i++)
	{
		result[i] = (*this)[i] && other[i];
	}
	return result;
}

template<size_t N>
inline qkernel::Bitmap<N>& qkernel::Bitmap<N>::operator &=(const Bitmap<N>& other) {
	for(size_t i = 0; i < N; i++)
	{
		(*this)[i] = (*this)[i] && other[i];
	}
	return *this;
}

template<size_t N>
inline qkernel::Bitmap<N> qkernel::Bitmap<N>::operator |(const Bitmap<N>& other) const {
	Bitmap<N> result;
	for(size_t i = 0; i < N; i++)
	{
		result[i] = (*this)[i] || other[i];
	}
	return result;
}

template<size_t N>
inline qkernel::Bitmap<N>& qkernel::Bitmap<N>::operator |=(const Bitmap<N>& other) {
	for(size_t i = 0; i < N; i++)
	{
		(*this)[i] = (*this)[i] || other[i];
	}
	return *this;
}

template<size_t N>
inline qkernel::Bitmap<N> qkernel::Bitmap<N>::operator ^(const Bitmap<N>& other) const {
	Bitmap<N> result;
	for(size_t i = 0; i < N; i++)
	{
		result[i] = (*this)[i] ^ other[i];
	}
	return result;
}

template<size_t N>
inline qkernel::Bitmap<N>& qkernel::Bitmap<N>::operator ^=(const Bitmap<N>& other) {
	for(size_t i = 0; i < N; i++)
	{
		(*this)[i] = (*this)[i] ^ other[i];
	}
	return *this;
}

template<size_t N>
inline qkernel::Bitmap<N> qkernel::Bitmap<N>::operator <<(const size_t n) const {
	Bitmap<N> result;
	for(size_t i = 0; i < N; i++)
	{
		if(i < n)
		{
			result[i] = false;
		}
		else
		{
			result[i] = (*this)[i-n];
		}
	}
	return result;
}

template<size_t N>
inline qkernel::Bitmap<N>& qkernel::Bitmap<N>::operator <<=(const size_t n) {
	for(size_t i = 0; i < N; i++)
	{
		if(i == 0)
		{
			(*this)[i] = 0;
		}
		else
		{
			(*this)[i] = (*this)[i-1];
		}
	}
	return *this;
}

template<size_t N>
inline qkernel::Bitmap<N> qkernel::Bitmap<N>::operator >>(const size_t n) const {
	Bitmap<N> result;
	for(size_t i = 0; i < N; i++)
	{
		if(i > N - n)
		{
			result[i] = false;
		}
		else
		{
			result[i] = (*this)[i+n];
		}
	}
	return result;
}

template<size_t N>
inline qkernel::Bitmap<N>& qkernel::Bitmap<N>::operator >>=(const size_t n) {
	for(size_t i = N - 1; i >= 0; i--)
	{
		if(i == N - 1)
		{
			(*this)[i] = 0;
		}
		else
		{
			(*this)[i] = (*this)[i+1];
		}
	}
	return *this;
}

template<size_t N>
inline qkernel::Bitmap<N> qkernel::Bitmap<N>::operator ~() const {
	Bitmap<N> result;
	for(size_t i = 0; i < N; i++)
	{
		result[i] = !(*this)[i];
	}
	return result;
}

#endif /* SRC_BITMAP_H_ */
