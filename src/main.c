#include <stdio.h>
#include "cmdc.h"
#include <stdlib.h>

int main(void) {
    const char *debug_file = getenv("CMDC_DEBUG");
    if (debug_file) {
        // перенаправить stdin из файла
        freopen(debug_file, "r", stdin);
    }
    parser();
    return 0;
}