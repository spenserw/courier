#pragma once

#include <cstdio>
#include <string>
#include <ctime>

#define LOG_STDOUT(msg, args...) printf(msg, args)
#define LOG(msg, args...) LOG_STDOUT(msg, args)

#define STAMP(level) \
    ">>> LOG (" level ") [%s]: "
//enddef STAMP

#define LOG_IF_LEVEL(level, msg, ...) \
    if(g_debug_options->log_level >= LogLevel::level) \
	LOG(msg, timestamp().c_str() __VA_OPT__(,) __VA_ARGS__)
//enddef LOG_IF_LEVEL

#define LOG_DEBUG(msg, args...) LOG_IF_LEVEL(DEBUG, STAMP("debug") msg, args)
#define LOG_INFO(msg, args...) LOG_IF_LEVEL(INFO, "\033[33;1m" STAMP("info") "\033[0m" msg, args)
#define LOG_ERROR(msg, args...) LOG_IF_LEVEL(ERROR, "\033[31;1;4m" STAMP("error") "\033[0m" msg, args)

enum LogLevel {
	       SILENT=0,
	       ERROR,
	       INFO,
	       DEBUG
};

extern struct GlobalDebugOptions {
    GlobalDebugOptions(LogLevel log_level)
	: log_level(log_level) {}

    LogLevel log_level;
} *g_debug_options;

inline std::string timestamp() {
    time_t rawtime;
    struct tm *timeinfo = {0};
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char buffer[80];
    strftime(buffer, 80, "%F %T", timeinfo);
    return std::string(buffer);
}
