#include "../includes/easyargs.h"
#include "test.h"

int main() {
    int ok = 1;
    const char* var1 = "a";
    ASSERT('a', easyargs_parse_char(var1, &ok), ==);
    ASSERT(ok, 1, ==);
    ok = 1;
    const char* var2 = "";
    ASSERT('\0', easyargs_parse_char(var2, &ok), ==);
    ASSERT(ok, 0, ==);
    ok = 1;
    const char* var3 = "ab";
    ASSERT('\0', easyargs_parse_char(var3, &ok), ==);
    ASSERT(ok, 0, ==);
    ok = 1;

    FINISH();
}
