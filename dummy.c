#include <stdio.h>
#include <stdlib.h>

#define UTF8_IMPLEMENTATION
#include "./src/utf8.h"

int main(void) {
    const char* msg = "Žan Sovič";

    printf("%s\n", msg);

    utf8codepoint* codepoints = utf8_to_codepoints((utf8*)msg);

    utf8* out = codepoints_to_utf8(codepoints);
    printf("%s\n", out);

    free(out);
    free(codepoints);
    return 0;
}
