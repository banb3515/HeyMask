//
//  Logger.cpp
//  로그를 관리합니다.
//
//  Created by BanB on 2021/06/27.
//

#include "Logger.hpp"

void Logger::Debug(string log) { Log("Debug", log); }

void Logger::Info(string log) { Log("Info", log); }

void Logger::Error(string log) { Log("Error", log); }

void Logger::Log(string type, string log)
{
    time_t cur = time(NULL);
    struct tm* curTime = localtime(&cur);
    
    ostringstream path;
    path << "Logs/" << type << "/" << setfill('0')
        << setw(4) << (curTime->tm_year + 1900) << "Y/"
        << setw(2) << curTime->tm_mon << "M/";
    
    ostringstream filename;
    filename << setfill('0')
        << setw(2) << curTime->tm_mday << "D.log";
    
    // 경로가 존재하는지 확인 후 경로가 없으면 경로 생성
    if (access(path.str().c_str(), F_OK) != 0)
        CreateDirectories(path.str().c_str());
    
    // 로그에 타임스탬프 추가
    ostringstream time;
    time << "[" << setfill('0')
          << setw(2) << curTime->tm_hour << ":"
          << setw(2) << curTime->tm_min << ":"
          << setw(2) << curTime->tm_sec << "]";
    
    string tsLog(time.str() + " ");
    tsLog.append(log);
    
    // 로그 저장
    ofstream file;
    file.open(path.str() + filename.str(), ios::out | ios::app);
    file << tsLog.c_str() << "\n";
    file.close();
}

void Logger::CreateDirectories(const char* path)
{
    char* fullPath = strdup(path);
    char* subPath = dirname(fullPath);
    if (strlen(subPath) > 1)
        CreateDirectories(subPath);
    mkdir(path, 0777);
    free(fullPath);
}
