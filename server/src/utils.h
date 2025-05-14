#ifndef __UTILS_H__
#define __UTILS_H__

#define UNUSED(x) (void)x;

#ifdef DEBUG
#define PRINT_DEBUG(format, ...)                                               \
    do {                                                                       \
        fprintf(stderr, "[%s] ", __FUNCTION__);                                \
        fprintf(stderr, format, ##__VA_ARGS__);                                \
    } while (0)
#else
#define PRINT_DEBUG(format, ...) ((void)0)
#endif

#endif // __UTILS_H__
