#ifndef EASYARGS_H
#define EASYARGS_H

/*
    EasyArgs: A simple, single-header argument parser for C
    Version: September 27, 2025
    Author: Xander Gouws

    See README.md for documentation and examples.
*/

#include <stdio.h>
#include <stdlib.h>  // used for parsing (atoi, atof)

#ifndef EASYARGS_STRCMP
#include <string.h>
#define EASYARGS_STRCMP(str1, str2) strcmp(str1, str2)
#endif

// REQUIRED_ARG(type, name, label, description, parser)
// label and description should be strings, e.g. "contrast" and "Contrast applied to image"
#define REQUIRED_STRING_ARG(name, label, description) REQUIRED_ARG(char*, name, label, description, )
#define REQUIRED_CHAR_ARG(name, label, description) REQUIRED_ARG(char, name, label, description, parse_char)
#define REQUIRED_INT_ARG(name, label, description) REQUIRED_ARG(int, name, label, description, atoi)
#define REQUIRED_UINT_ARG(name, label, description) REQUIRED_ARG(unsigned int, name, label, description, atoi)
#define REQUIRED_LONG_ARG(name, label, description) REQUIRED_ARG(long, name, label, description, atol)
#define REQUIRED_ULONG_ARG(name, label, description) REQUIRED_ARG(unsigned long, name, label, description, parse_ul)
#define REQUIRED_LONG_LONG_ARG(name, label, description) REQUIRED_ARG(long long, name, label, description, atoll)
#define REQUIRED_ULONG_LONG_ARG(name, label, description) REQUIRED_ARG(unsigned long long, name, label, description, parse_ull)
#define REQUIRED_SIZE_ARG(name, label, description) REQUIRED_ARG(size_t, name, label, description, parse_ull)
#define REQUIRED_FLOAT_ARG(name, label, description) REQUIRED_ARG(float, name, label, description, atof)
#define REQUIRED_DOUBLE_ARG(name, label, description) REQUIRED_ARG(double, name, label, description, atof)

// OPTIONAL_ARG(type, name, default, flag, label, description, formatter, parser)
#define OPTIONAL_STRING_ARG(name, default, flag, label, description) OPTIONAL_ARG(char*, name, default, flag, label, description, "%s", )
#define OPTIONAL_CHAR_ARG(name, default, flag, label, description) OPTIONAL_ARG(char, name, default, flag, label, description, "%c", parse_char)
#define OPTIONAL_INT_ARG(name, default, flag, label, description) OPTIONAL_ARG(int, name, default, flag, label, description, "%d", atoi)
#define OPTIONAL_UINT_ARG(name, default, flag, label, description) OPTIONAL_ARG(unsigned int, name, default, flag, label, description, "%u", atoi)
#define OPTIONAL_LONG_ARG(name, default, flag, label, description) OPTIONAL_ARG(long, name, default, flag, label, description, "%ld", atol)
#define OPTIONAL_ULONG_ARG(name, default, flag, label, description) OPTIONAL_ARG(unsigned long, name, default, flag, label, description, "%lu", parse_ul)
#define OPTIONAL_LONG_LONG_ARG(name, default, flag, label, description) OPTIONAL_ARG(long long, name, default, flag, label, description, "%lld", atoll)
#define OPTIONAL_ULONG_LONG_ARG(name, default, flag, label, description) OPTIONAL_ARG(unsigned long long, name, default, flag, label, description, "%llu", parse_ull)
#define OPTIONAL_SIZE_ARG(name, default, flag, label, description) OPTIONAL_ARG(size_t, name, default, flag, label, description, "%zu", parse_ull)
#define OPTIONAL_FLOAT_ARG(name, default, flag, label, description, precision) OPTIONAL_ARG(float, name, default, flag, label, description, "%." #precision "g", atof)
#define OPTIONAL_DOUBLE_ARG(name, default, flag, label, description, precision) OPTIONAL_ARG(double, name, default, flag, label, description, "%." #precision "g", atof)

// BOOLEAN_ARG(name, flag, description)


// PARSERS
static inline char parse_char(const char* text) {
    return text[0];
}

static inline unsigned long parse_ul(const char* text) {
    return strtoul(text, NULL, 10);
}

static inline unsigned long long parse_ull(const char* text) {
    return strtoull(text, NULL, 10);
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
    if (!argc || !argv) {
        fprintf(stderr, "Internal error: null args or argv.\n");
        return 0;
    }

    // If not enough required arguments
    if (argc < 1 + REQUIRED_ARG_COUNT) {
        fprintf(stderr, "Not all required arguments included.\n");
        return 0;
    }

    // Get required arguments
    #ifdef REQUIRED_ARGS
    #define REQUIRED_ARG(type, name, label, description, parser) args->name = (type) parser(argv[i++]);
    int i = 1;
    REQUIRED_ARGS
    #undef REQUIRED_ARG
    #endif

    // Get optional and boolean arguments
    #define OPTIONAL_ARG(type, name, default, flag, label, description, formatter, parser) \
    if (!EASYARGS_STRCMP(argv[i], flag)) { \
        if (i + 1 >= argc) { \
            fprintf(stderr, "Error: option '%s' requires a value.\n", flag); \
            return 0; \
        } \
        args->name = (type) parser(argv[++i]); \
        continue; \
    }

    #define BOOLEAN_ARG(name, flag, description) \
    if (!EASYARGS_STRCMP(argv[i], flag)) { \
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
