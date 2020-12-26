#ifndef __ASSERT_H__
#define __ASSERT_H__

#include "debug.h"

#define assert(call) do {\
	if (call) {\
		LOG_P(PSTR(#call " FAILED"));\
		goto exit;\
	}\
} while (0)

#endif/*__ASSERT_H__*/