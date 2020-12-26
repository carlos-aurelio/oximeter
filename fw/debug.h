#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#ifdef HOST
#define PSTR(str) str
#define printf_P(...) printf(__VA_ARGS__)
#else
#include <avr/pgmspace.h>
#endif

#ifdef DEBUG
#	define LOG_P(...) do {\
		printf_P(PSTR("["));\
		printf_P(PSTR(__FILE__));\
		printf_P(PSTR(":%d] "), __LINE__);\
		printf_P(__VA_ARGS__);\
		printf_P(PSTR("\r\n"));\
	} while (0);
#	define LOGLV_P(lv, ...) if (lv < DEBUG) LOG_P(__VA_ARGS__)
#else
#	define LOG_P(...) do { } while (0)
#	define LOGLV_P(lv, ...) do { } while (0)
#endif

#endif/*__DEBUG_H__*/