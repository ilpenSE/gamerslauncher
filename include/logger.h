#ifndef LOGGER_API_H
#define LOGGER_API_H

#if defined(_WIN32)
#if defined(LOGGER_BUILD)
#define LOGGER_API __declspec(dllexport)
#else
#define LOGGER_API __declspec(dllimport)
#endif
#else
#define LOGGER_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

LOGGER_API int  logger_init(const char* logs_dir, int use_local_time);
LOGGER_API int  logger_log_info(const char* msg);
LOGGER_API int  logger_log_error(const char* msg);
LOGGER_API int  logger_log_warning(const char* msg);
LOGGER_API void logger_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
