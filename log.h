#ifndef LOG_H
#define LOG_H

#include <string>
#include <spdlog/spdlog.h>


void initLogger(bool clog=true, bool flog=false);

/*
 * 控制台日志默认开启，文件日志默认关闭
 * 文件日志输出路径在exe程序目录下log/当前日期时间.txt
 */
void setControlLog(bool flag);
void setFileLog(bool flag);

/*
 * 设置日志输出等级，目前有info,warn,error三个等级，默认输出等级为info
 * 默认输出格式为[级别-时-分-秒-毫秒-线程]
 */
void setLevel(spdlog::level::level_enum level);
void setPattern(const std::string& pattern);


extern bool isControl;
extern bool isFile;

extern std::shared_ptr<spdlog::logger> consoleLogger;
extern std::shared_ptr<spdlog::logger> fileLogger;


template<typename... Args>
void log_info(Args&&... args){
    if(isControl){
        consoleLogger->info(std::forward<Args>(args)...);
    }
    if(isFile){
        fileLogger->info(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void log_warn(Args&&... args){
    if(isControl){
        consoleLogger->warn(std::forward<Args>(args)...);
    }
    if(isFile){
        fileLogger->warn(std::forward<Args>(args)...);
    }
}

template<typename... Args>
void log_error(Args&&... args){
    if(isControl){
        consoleLogger->error(std::forward<Args>(args)...);
    }
    if(isFile){
        fileLogger->error(std::forward<Args>(args)...);
    }
}

#endif // LOG_H
