#ifndef _GUARD_H_UTILS_H_
#define _GUARD_H_UTILS_H_
#include <osapi.h>

#define AT_DEBUG 2

#if AT_DEBUG == 1
#define AT_DBG(fmt, ...) do {	\
	char _tmp_out_dbg[64];	\
	os_sprintf(_tmp_out_dbg, fmt, ##__VA_ARGS__);	\
    at_port_print(_tmp_out_dbg);	\
	}while(0)

#elif AT_DEBUG == 0

#define AT_DBG

#elif AT_DEBUG == 2

#define AT_DBG(fmt, ...) do {	\
	char _tmp_out_dbg[64];	\
	os_sprintf(_tmp_out_dbg, fmt, ##__VA_ARGS__);	\
    espush_debug_output(_tmp_out_dbg);	\
	}while(0)

#endif


#endif
