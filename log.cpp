#include "log.h"

#include <iostream>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/details/os.h>

#include <QDateTime>

bool isControl=true;
bool isFile=false;

std::shared_ptr<spdlog::logger> consoleLogger;
std::shared_ptr<spdlog::logger> fileLogger;

spdlog::level::level_enum logLevel=spdlog::level::level_enum::info;
std::string logPattern="[%l %H:%M:%S:%e %P]: %v";

void setControlLog(bool flag)
{
    isControl=flag;
}

void setFileLog(bool flag)
{
    isFile=flag;
}

void initLogger(bool clog, bool flog)
{
    setControlLog(clog);
    setFileLog(flog);

    if(isControl){
        consoleLogger = spdlog::stdout_color_mt("console");
        consoleLogger->set_level(logLevel);
        consoleLogger->set_pattern(logPattern);
        std::cout<<"终端颜色输出支持："<<std::boolalpha<<spdlog::details::os::is_color_terminal()<<std::endl;
    }

    if(isFile){
        QDateTime dt=QDateTime::currentDateTime();
        std::string fname = dt.toString().toLocal8Bit().toStdString()+ ".txt";
        std::string fpath = "./log/"+fname;

        fileLogger=spdlog::basic_logger_mt("fileLog", fpath);
        fileLogger->set_level(logLevel);
        fileLogger->set_pattern(logPattern);
    }
}

void setLevel(spdlog::level::level_enum level)
{
    logLevel=level;
    if(consoleLogger){
        consoleLogger->set_level(logLevel);
    }
    if(fileLogger){
        fileLogger->set_level(logLevel);
    }
}

void setPattern(const std::string &pattern)
{
    logPattern=pattern;
    if(consoleLogger){
        consoleLogger->set_pattern(logPattern);
    }
    if(fileLogger){
        fileLogger->set_pattern(logPattern);
    }
}
