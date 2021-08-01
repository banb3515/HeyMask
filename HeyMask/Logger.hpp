//
//  Logger.hpp
//  로그를 관리합니다.
//
//  Created by BanB on 2021/06/27.
//

#ifndef Logger_hpp
#define Logger_hpp

#include <iostream>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <libgen.h>
#include <iomanip>
#include <sstream>
#include <fstream>

using namespace std;

// 로그를 관리합니다.
class Logger
{
public:
    /* 개발 시 디버그 용도로 사용합니다.
     @param log: 출력할 로그 메시지입니다.
     */
    static void Debug(string log);
    
    /* 정보성 메시지를 나타낼 때 사용합니다.
     @param log: 출력할 로그 메시지입니다.
     */
    static void Info(string log);
    
    /* 처리하는 도중 문제가 발생하였을 때 사용합니다.
     @param log: 출력할 로그 메시지입니다.
     */
    static void Error(string log);
    
private:
    /* 로그를 저장합니다.
     @param path: 로그의 타입입니다.
     @param log: 저장할 로그 메시지입니다.
     */
    static void Log(string type, string log);
    
    /* 디렉터리를 생성합니다. (하위 디렉터리 포함)
     @param path: 생성할 디렉터리의 경로입니다.
     */
    static void CreateDirectories(const char* path);
};

#endif /* Logger_hpp */
