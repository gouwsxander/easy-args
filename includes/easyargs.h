#ifndef EASYARGS_H
#define EASYARGS_H

/*
    EasyArgs: A simple, single-header argument parser for C
    Version: October 20, 2025
    Author: Xander Gouws

    Provided under an MIT License. See end of file for details.

    See github.com/gouwsxander/easy-args for documentation and examples.
*/

#include <stdio.h>
#include <stdlib.h>  // used for parsing (atoi, atof)
#include <string.h>  // used for strcmp
#include <limits.h>  // used for type limits
#include <errno.h>   // used for errno
#include <stdint.h>  // used for SIZE_MAX
#include <ctype.h>   // used for isspace


#define ERROR(msg, ...) do { \
    fprintf(stderr, msg, ##__VA_ARGS__); \
    fflush(stderr); \
} while (0)

#define RETURN_ON(condition, ret_val, msg, ...) do { \
    if (condition) { \
        ERROR(msg, ##__VA_ARGS__); \
        return ret_val; \
    } \
} while (0)


// REQUIRED_ARG(type, name, label, description, parser)
// label and description should be strings, e.g. "contrast" and "Contrast applied to image"
#define REQUIRED_STRING_ARG(name, label, description) REQUIRED_ARG(char*, name, label, description, easyargs_parse_str)
#define REQUIRED_CHAR_ARG(name, label, description) REQUIRED_ARG(char, name, label, description, easyargs_parse_char)
#define REQUIRED_INT_ARG(name, label, description) REQUIRED_ARG(int, name, label, description, easyargs_parse_int)
#define REQUIRED_UINT_ARG(name, label, description) REQUIRED_ARG(unsigned int, name, label, description, easyargs_parse_uint)
#define REQUIRED_LONG_ARG(name, label, description) REQUIRED_ARG(long, name, label, description, easyargs_parse_long)
#define REQUIRED_ULONG_ARG(name, label, description) REQUIRED_ARG(unsigned long, name, label, description, easyargs_parse_ulong)
#define REQUIRED_LONG_LONG_ARG(name, label, description) REQUIRED_ARG(long long, name, label, description, easyargs_parse_llong)
#define REQUIRED_ULONG_LONG_ARG(name, label, description) REQUIRED_ARG(unsigned long long, name, label, description, easyargs_parse_ullong)
#define REQUIRED_SIZE_ARG(name, label, description) REQUIRED_ARG(size_t, name, label, description, easyargs_parse_size_t)
#define REQUIRED_FLOAT_ARG(name, label, description) REQUIRED_ARG(float, name, label, description, easyargs_parse_float)
#define REQUIRED_DOUBLE_ARG(name, label, description) REQUIRED_ARG(double, name, label, description, easyargs_parse_double)

// OPTIONAL_ARG(type, name, default, flag, label, description, formatter, parser)
#define OPTIONAL_STRING_ARG(name, default, flag, label, description) OPTIONAL_ARG(char*, name, default, flag, label, description, "%s", easyargs_parse_str)
#define OPTIONAL_CHAR_ARG(name, default, flag, label, description) OPTIONAL_ARG(char, name, default, flag, label, description, "%c", easyargs_parse_char)
#define OPTIONAL_INT_ARG(name, default, flag, label, description) OPTIONAL_ARG(int, name, default, flag, label, description, "%d", easyargs_parse_int)
#define OPTIONAL_UINT_ARG(name, default, flag, label, description) OPTIONAL_ARG(unsigned int, name, default, flag, label, description, "%u", easyargs_parse_uint)
#define OPTIONAL_LONG_ARG(name, default, flag, label, description) OPTIONAL_ARG(long, name, default, flag, label, description, "%ld", easyargs_parse_long)
#define OPTIONAL_ULONG_ARG(name, default, flag, label, description) OPTIONAL_ARG(unsigned long, name, default, flag, label, description, "%lu", easyargs_parse_ulong)
#define OPTIONAL_LONG_LONG_ARG(name, default, flag, label, description) OPTIONAL_ARG(long long, name, default, flag, label, description, "%lld", easyargs_parse_llong)
#define OPTIONAL_ULONG_LONG_ARG(name, default, flag, label, description) OPTIONAL_ARG(unsigned long long, name, default, flag, label, description, "%llu", easyargs_parse_ullong)
#define OPTIONAL_SIZE_ARG(name, default, flag, label, description) OPTIONAL_ARG(size_t, name, default, flag, label, description, "%zu", easyargs_parse_size_t)
#define OPTIONAL_FLOAT_ARG(name, default, flag, label, description, precision) OPTIONAL_ARG(float, name, default, flag, label, description, "%." #precision "g", easyargs_parse_float)
#define OPTIONAL_DOUBLE_ARG(name, default, flag, label, description, precision) OPTIONAL_ARG(double, name, default, flag, label, description, "%." #precision "g", easyargs_parse_double)

// BOOLEAN_ARG(name, flag, description)

// HELPER FUNCTIONS
static inline const char* easyargs_skip_leading(const char *s) {
    if (!s) return s;
    while (isspace((unsigned char)*s)) ++s;
    return s;
}

// PARSERS
static inline char* easyargs_parse_str(const char* text, int* ok) {
    *ok = 0;

    RETURN_ON(!text, NULL, "Error: null string value.\n");
    RETURN_ON(text[0] == '\0', NULL, "Error: empty string value not allowed.\n");

    *ok = 1;
    return (char*) text;
}

static inline char easyargs_parse_char(const char* text, int* ok) {
    *ok = 0;

    RETURN_ON(!text, 0, "Error: null input for character argument.\n");
    RETURN_ON(text[0] == '\0' || text[1] != '\0', 0, "Error: '%s' is not a valid character.\n", text);

    *ok = 1;
    return text[0];
}

#define DEFINE_UNSIGNED_INTEGER_PARSER(funcname, rettype, maxval, typename) \
static inline rettype funcname(const char* text, int* ok) { \
    *ok = 0; \
    RETURN_ON(!text, 0, "Error: null input for %s.\n", typename); \
    text = easyargs_skip_leading(text); \
    RETURN_ON(text[0] == '\0', 0, "Error: empty input for %s.\n", typename); \
    RETURN_ON(text[0] == '-', 0, "Error: '%s' negative value not allowed for %s.\n", text, typename); \
    char* end; \
    errno = 0; \
    unsigned long long val = strtoull(text, &end, 0); \
    RETURN_ON(*end != '\0', 0, "Error: '%s' is not a valid %s.\n", text, typename); \
    RETURN_ON(errno == ERANGE || val > (unsigned long long)(maxval), 0, "Error: '%s' is out of range for %s.\n", text, typename); \
    *ok = 1; \
    return (rettype) val; \
}

DEFINE_UNSIGNED_INTEGER_PARSER(easyargs_parse_uint, unsigned int, UINT_MAX, "unsigned int")
DEFINE_UNSIGNED_INTEGER_PARSER(easyargs_parse_ulong, unsigned long, ULONG_MAX, "unsigned long")
DEFINE_UNSIGNED_INTEGER_PARSER(easyargs_parse_ullong, unsigned long long, ULLONG_MAX, "unsigned long long")
DEFINE_UNSIGNED_INTEGER_PARSER(easyargs_parse_size_t, size_t, SIZE_MAX, "size_t")

#undef DEFINE_UNSIGNED_INTEGER_PARSER

#define DEFINE_SIGNED_INTEGER_PARSER(funcname, rettype, minval, maxval, typename) \
static inline rettype funcname(const char* text, int* ok) { \
    *ok = 0; \
    RETURN_ON(!text, 0, "Error: null input for %s.\n", typename); \
    text = easyargs_skip_leading(text); \
    RETURN_ON(text[0] == '\0', 0, "Error: empty input for %s.\n", typename); \
    char* end; \
    errno = 0; \
    long long val = strtoll(text, &end, 0); \
    RETURN_ON(*end != '\0', 0, "Error: '%s' is not a valid %s.\n", text, typename); \
    RETURN_ON(errno == ERANGE || val < (long long)(minval) || val > (long long)(maxval), 0, "Error: '%s' is out of range for %s.\n", text, typename); \
    *ok = 1; \
    return (rettype) val; \
}

DEFINE_SIGNED_INTEGER_PARSER(easyargs_parse_int, int, INT_MIN, INT_MAX, "int")
DEFINE_SIGNED_INTEGER_PARSER(easyargs_parse_long, long, LONG_MIN, LONG_MAX, "long")
DEFINE_SIGNED_INTEGER_PARSER(easyargs_parse_llong, long long, LLONG_MIN, LLONG_MAX, "long long")

#undef DEFINE_SIGNED_INTEGER_PARSER

static inline float easyargs_parse_float(const char* text, int* ok) {
    *ok = 0;

    RETURN_ON(!text, 0.0f, "Error: null input for float.\n");

    text = easyargs_skip_leading(text);

    RETURN_ON(text[0] == '\0', 0.0f, "Error: empty input for float.\n");

    char* end;
    errno = 0;
    float value = strtof(text, &end);

    RETURN_ON(errno == ERANGE, 0.0f, "Error: '%s' is out of range for type float.\n", text);
    RETURN_ON(*end != '\0', 0.0f, "Error: '%s' is not a valid float.\n", text);

    *ok = 1;
    return value;
}

static inline double easyargs_parse_double(const char* text, int* ok) {
    *ok = 0;

    RETURN_ON(!text, 0.0, "Error: null input for double.\n");

    text = easyargs_skip_leading(text);

    RETURN_ON(text[0] == '\0', 0.0, "Error: empty input for double.\n");

    char* end;
    errno = 0;
    double value = strtod(text, &end);

    RETURN_ON(errno == ERANGE, 0.0, "Error: '%s' is out of range for type double.\n", text);
    RETURN_ON(*end != '\0', 0.0, "Error: '%s' is not a valid double.\n", text);

    *ok = 1;
    return value;
}


// COUNT ARGUMENTS
#ifdef REQUIRED_ARGS
#define REQUIRED_ARG(...) + 1
static const int REQUIRED_ARG_COUNT = 0 REQUIRED_ARGS;
#undef REQUIRED_ARG
#else
static const int REQUIRED_ARG_COUNT = 0;
#endif

#ifdef OPTIONAL_ARGS
#define OPTIONAL_ARG(...) + 1
static const int OPTIONAL_ARG_COUNT = 0 OPTIONAL_ARGS;
#undef OPTIONAL_ARG
#else
static const int OPTIONAL_ARG_COUNT = 0;
#endif

#ifdef BOOLEAN_ARGS
#define BOOLEAN_ARG(...) + 1
static const int BOOLEAN_ARG_COUNT = 0 BOOLEAN_ARGS;
#undef BOOLEAN_ARG
#else
static const int BOOLEAN_ARG_COUNT = 0;
#endif


// ARG_T STRUCT
#define REQUIRED_ARG(type, name, ...) type name;
#define OPTIONAL_ARG(type, name, ...) type name;
#define BOOLEAN_ARG(name, ...) _Bool name;
// Stores argument values
typedef struct {
    #ifdef REQUIRED_ARGS
    REQUIRED_ARGS
    #endif
    #ifdef OPTIONAL_ARGS
    OPTIONAL_ARGS
    #endif
    #ifdef BOOLEAN_ARGS
    BOOLEAN_ARGS
    #endif
} args_t;
#undef REQUIRED_ARG
#undef OPTIONAL_ARG
#undef BOOLEAN_ARG


// Build an args_t struct with assigned default values
static inline args_t make_default_args() {
    args_t args = {
        #define REQUIRED_ARG(type, name, ...) .name = (type) 0,
        #define OPTIONAL_ARG(type, name, default, ...) .name = default,
        #define BOOLEAN_ARG(name, ...) .name = 0,

        #ifdef REQUIRED_ARGS
        REQUIRED_ARGS
        #endif

        #ifdef OPTIONAL_ARGS
        OPTIONAL_ARGS
        #endif

        #ifdef BOOLEAN_ARGS
        BOOLEAN_ARGS
        #endif

        #undef REQUIRED_ARG
        #undef OPTIONAL_ARG
        #undef BOOLEAN_ARG
    };

    return args;
}


// Parse arguments. Returns 0 if failed.
static inline int parse_args(int argc, char* argv[], args_t* args) {
    RETURN_ON(!argc, 0, "Internal error: null argc.\n");
    RETURN_ON(!argv, 0, "Internal error: null argv.\n");

    // If not enough required arguments
    RETURN_ON(argc < 1 + REQUIRED_ARG_COUNT, 0, "Not all required arguments included.\n");

    int ok;
    int i = 1;

    // Get required arguments
    #ifdef REQUIRED_ARGS
    #define REQUIRED_ARG(type, name, label, description, parser) \
    ok = 0; \
    args->name = (type) parser(argv[i++], &ok); \
    if (!ok) \
        return 0;

    REQUIRED_ARGS
    #undef REQUIRED_ARG
    #endif

    // Get optional and boolean arguments
    #define OPTIONAL_ARG(type, name, default, flag, label, description, formatter, parser) \
    if (!strcmp(argv[i], flag)) { \
        RETURN_ON(i + 1 >= argc, 0, "Error: option '%s' requires a value.\n", flag); \
        ok = 0; \
        args->name = (type) parser(argv[++i], &ok); \
        if (!ok) \
            return 0; \
        continue; \
    }

    #define BOOLEAN_ARG(name, flag, description) \
    if (!strcmp(argv[i], flag)) { \
        args->name = 1; \
        continue; \
    }

    for (int i = 1 + REQUIRED_ARG_COUNT; i < argc; i++) {
        #ifdef OPTIONAL_ARGS
        OPTIONAL_ARGS
        #endif

        #ifdef BOOLEAN_ARGS
        BOOLEAN_ARGS
        #endif

        fprintf(stderr, "Warning: Ignoring invalid argument '%s'\n", argv[i]);
    }

    #undef OPTIONAL_ARG
    #undef BOOLEAN_ARG

    return 1;
}


// Display help string, given command used to launch program, e.g., argv[0]
static inline void print_help(char* exec_alias) {
    // USAGE SECTION
    printf("USAGE:\n");
    printf("    %s ", exec_alias);

    #ifdef REQUIRED_ARGS
    if (REQUIRED_ARG_COUNT > 0 && REQUIRED_ARG_COUNT <= 3) {
        #define REQUIRED_ARG(type, name, label, ...) "<" label "> "
        printf(REQUIRED_ARGS);
        #undef REQUIRED_ARG
    } else {
        printf("<ARGUMENTS> ");
    }
    #endif

    if (OPTIONAL_ARG_COUNT + BOOLEAN_ARG_COUNT <= 3) {
        #ifdef OPTIONAL_ARGS
        #define OPTIONAL_ARG(type, name, default, flag, label, ...) "[" flag " <" label ">" "] "
        printf(OPTIONAL_ARGS);
        #undef OPTIONAL_ARG
        #endif

        #ifdef BOOLEAN_ARGS
        #define BOOLEAN_ARG(name, flag, ...) "[" flag "] "
        printf(BOOLEAN_ARGS);
        #undef BOOLEAN_ARG
        #endif
    } else {
        printf("[OPTIONS]");
    }

    printf("\n\n");

    // Get maximum width of labels for spacing
    int max_width = 0;
    (void) max_width; // suppress unused variable warning
    #ifdef REQUIRED_ARGS
    #define REQUIRED_ARG(type, name, label, ...) \
        { int len = strlen(label) + 2; if (len > max_width) max_width = len; }
    REQUIRED_ARGS
    #undef REQUIRED_ARG
    #endif

    #ifdef OPTIONAL_ARGS
    #define OPTIONAL_ARG(type, name, default, flag, label, ...) \
        { int len = strlen(flag) + 1 + strlen(label) + 2; if (len > max_width) max_width = len; }
    OPTIONAL_ARGS
    #undef OPTIONAL_ARG
    #endif

    #ifdef BOOLEAN_ARGS
    #define BOOLEAN_ARG(name, flag, ...) \
        { int len = strlen(flag); if (len > max_width) max_width = len; }
    BOOLEAN_ARGS
    #undef BOOLEAN_ARG
    #endif

    // ARGUMENTS SECTION
    #ifdef REQUIRED_ARGS
    printf("ARGUMENTS:\n");

    #define REQUIRED_ARG(type, name, label, description, ...) \
        printf("    <" label ">%*s    " description "\n", max_width - (int)strlen(label) - 2, "");
    REQUIRED_ARGS
    #undef REQUIRED_ARG

    printf("\n");
    #endif

    #if defined(OPTIONAL_ARGS) || defined(BOOLEAN_ARGS)
    printf("OPTIONS:\n");

    #ifdef OPTIONAL_ARGS

    #define OPTIONAL_ARG(type, name, default, flag, label, description, formatter, ...) \
        printf("    " flag " <" label ">%*s    " description " (default: " formatter ")\n", max_width - (int)strlen(label) - (int)strlen(flag) - 3, "", default);
    OPTIONAL_ARGS
    #undef OPTIONAL_ARG
    #endif

    #ifdef BOOLEAN_ARGS
    #define BOOLEAN_ARG(name, flag, description) \
        printf("    " flag "%*s    " description "\n", max_width - (int)strlen(flag), "");
    BOOLEAN_ARGS
    #undef BOOLEAN_ARG
    #endif

    #endif
}

#endif

#undef ERROR
#undef RETURN_ON

/*
    MIT License

    Copyright (c) 2025 Xander Gouws

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
