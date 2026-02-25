#include <assert.h>
#include <stdio.h>
#include <stdbool.h>

#if defined(_WIN32)
#  define NULL_DEVICE "NUL"
#else
#  define NULL_DEVICE "/dev/null"
#endif

static FILE *saved_stderr = NULL;

static bool passed = true;
static int failed = 0;
static int succeeded = 0;

void suppress_err_begin(void) {
    fflush(stderr);

    saved_stderr = stderr;

    freopen(NULL_DEVICE, "w", stderr);
}

void suppress_stderr_end(void) {
    fflush(stderr);

    if (saved_stderr) {
        freopen(NULL_DEVICE, "w", stderr);
        stderr = saved_stderr;
        saved_stderr = NULL;
    }
}

#define ASSERT(a, b, operation) do { \
    suppress_err_begin(); \
    if (!((a) operation (b))) { \
        suppress_stderr_end(); \
        fprintf(stderr, "Assertion failed: " #a " " #operation " " #b "\n"); \
        fflush(stderr); \
        passed = false; \
        failed++; \
    } else { \
        printf("Assertion passed: " #a " " #operation " " #b "\n"); \
        fflush(stdout); \
        suppress_stderr_end(); \
        succeeded++; \
    } \
} while (0)

#define FINISH() do { printf("\n%d tests failed.\n%d tests succeeded.\n", failed, succeeded); if (!passed) abort(); } while (0)
