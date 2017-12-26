/* Source taken from Zed A. Shaw */
#ifndef __debug_macros_h__
#define __debug_macros_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
/// Print info to stderr about variable location
#define debug(M, ...)\
            fprintf(\
                stderr,\
                "[DEBUG] (%s:%d): " M "\n",\
                __FILE__,\
                __LINE__,\
                ##__VA_ARGS__)
#endif

/// Get clean error number
#define clean_errno()\
            (errno == 0 ? "None" : strerror(errno))

/// Log an error to stderr
#define log_err(M, ...)\
            fprintf(\
                stderr,\
                "[ERROR] (%s:%d: errno: %s) " M "\n",\
                __FILE__,\
                __LINE__,\
                clean_errno(),\
                ##__VA_ARGS__)

/// Log a warning to stderr
#define log_warn(M, ...)\
            fprintf(\
                stderr,\
                "[WARN] (%s:%d: errno: %s) " M "\n",\
                __FILE__,\
                __LINE__,\
                clean_errno(),\
                ##__VA_ARGS__)

/// Log info to stderr
#define log_info(M, ...)\
            fprintf(\
                stderr,\
                "[INFO] (%s:%d) " M "\n",\
                __FILE__, __LINE__,\
                ##__VA_ARGS__)

/// Check if variable is true. If not, log an error
#define check(A, M, ...)\
            if(!(A)) {\
                log_err(M, ##__VA_ARGS__);\
                errno=0;\
                goto error;\
            }

/// Log and throw an error if called
#define sentinel(M, ...)\
            log_err(M, ##__VA_ARGS__);\
            errno=0;\
            goto error;\

/// Check if memory points to something meaningful
#define check_mem(A)\
            check((A), "Out of memory.")

/// Check if variable is true. If not, throw an error and debug other variables
#define check_debug(A, M, ...)\
            if(!(A)) {\
                debug(M, ##__VA_ARGS__);\
                errno=0;\
                goto error;\
            }

#endif
