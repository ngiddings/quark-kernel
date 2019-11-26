/*
 * Error.h
 *
 *  Created on: May 23, 2019
 *      Author: nathan
 */

#ifndef SRC_ERROR_H_
#define SRC_ERROR_H_

#include "errortype.h"

namespace qkernel {

class Error {
public:

	Error();

	Error(ErrorType errorType);

	Error(ErrorType errorType, const char* message);

	ErrorType getType();

	const char* getMessage();

private:

	ErrorType errorType;

	const char* message;

};

extern Error lastError;

} /* namespace qkernel */

#endif /* SRC_ERROR_H_ */
