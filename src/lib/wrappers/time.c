#include <lib/time.h>
#include <lib/nos.h>

char* timestr(time_t time, bool military, bool seconds) { return __timestr(time, military, seconds); }

time_t timenow() { return __timenow(); }