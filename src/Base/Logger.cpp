#include "Logger.h"

namespace SoftGL
{
    void *Logger::logContext_ = nullptr;
    LogFunc Logger::logFunc_ = nullptr;

    LogLevel Logger::minLevel_ = LOG_INFO;

    char Logger::buf_[MAX_lOG_LENGTH] = {};
    std::mutex Logger::mutex_;

    void Logger::setLogFunc(void *ctx, LogFunc func)
    {
        logContext_ = ctx;
        logFunc_ = func;
    }

    void Logger::setLogLevel(LogLevel level)
    {
        minLevel_ = level;
    }

    void Logger::log(LogLevel level, const char *file, int line, const char *message, ...)
    {
        std::lock_guard<std::mutex> lock_guard(mutex_);
        if (level < minLevel_) return;
        va_list argPtr;
        va_start(argPtr, message);
        vsnprintf(buf_, MAX_lOG_LENGTH - 1, message, argPtr);
        va_end(argPtr);
        buf_[MAX_lOG_LENGTH - 1] = '\0';
        if (logFunc_ != nullptr)
        {
            logFunc_(logContext_, level, buf_);
            return;
        }
        switch (level)
        {
 #ifdef LOG_SOURCE_LINE
            case LOG_INFO:    fprintf(stdout, "[INFO] %s:%d: %s\n", file, line, buf_);    break;
            case LOG_DEBUG:   fprintf(stdout, "[DEBUG] %s:%d: %s\n", file, line, buf_);   break;
            case LOG_WARNING: fprintf(stdout, "[Warning] %s:%d: %s\n", file, line, buf_); break;
            case LOG_ERROR:   fprintf(stderr, "[Error] %s:%d: %s\n", file, line, buf_);   break;
 #else
            case LOG_INFO:    fprintf(stdout, "[INFO] : %s\n", buf_);    break;
            case LOG_DEBUG:   fprintf(stdout, "[DEBUG] : %s\n", buf_);   break;
            case LOG_WARNING: fprintf(stdout, "[Warning] : %s\n", buf_); break;
            case LOG_ERROR:   fprintf(stderr, "[Error] : %s\n", buf_);   break;
 #endif
        }
        fflush(stdout);
        fflush(stderr);
    }
}