/*
 * Error.cpp
 *
 *  Created on: May 23, 2019
 *      Author: nathan
 */

#include "error.h"

namespace qkernel {

Error lastError;

Error::Error()
		: errorType(ErrorType::none), message("") {}

Error::Error(ErrorType errorType)
		: errorType(errorType), message("") {}

Error::Error(ErrorType errorType, const char* message)
		: errorType(errorType), message(message) {}

ErrorType Error::getType()
{
	return errorType;
}

const char* Error::getMessage()
{
	return message;
}

} /* namespace qkernel */
