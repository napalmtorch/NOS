#include <services/vesa/vesa.h>
#include <lib/nos.h>

uint32_t exit_code;

int _start(char** argv, int argc)
{
    lib_init();
    exit_code = 0;

    vesa_init();

    time_t now;
    int time, time_last;

    while (true)
    {
        vesa_clear(0xFFFF0000);

        now = timenow();
        time = now.second;
        if (time != time_last)
        {
            printf("TIME: %s\n", timestr(now, false, true));
            time_last = time;
        }

        if (exit_code > 0) { return exit_code; }
    }   
    return 0;
}