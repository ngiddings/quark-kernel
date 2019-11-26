/*
 * math.h
 *
 *  Created on: May 23, 2019
 *      Author: nathan
 */

#ifndef SRC_MATH_H_
#define SRC_MATH_H_

#include <stdint.h>

/**
 * Computes the logarithm in base 2 of a given integer, returning an integer as
 * the result. For n = -1, returns unsigned -1.
 *
 * @returns floor(log2(n)), where n > 0. Else unsigned -1.
 */
uint32_t ilog2(uint32_t n, bool roundUp = false);


#endif /* SRC_MATH_H_ */
