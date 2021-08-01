//
//  Sound.hpp
//  사운드 출력을 관리합니다.
//
//  Created by BanB on 2021/06/26.
//

#ifndef Sound_hpp
#define Sound_hpp

#include <iostream>
#include <string.h>
#include <thread>
#include <unistd.h>
#include <ao/ao.h>
#include <mpg123.h>

#include "Logger.hpp"
#include "Color.hpp"

using namespace std;

/* Sound의 매개 변수입니다.
 @param alias: Sound의 별칭입니다.
 @param path: 재생할 사운드 파일의 경로입니다.
 */
struct SoundParam
{
    string alias;
    string path;
};

// 사운드 출력을 관리합니다.
class Sound
{
public:
    /* Sound를 생성 후 초기화합니다.
     @param param: Sound의 매개 변수입니다.
     */
    Sound(SoundParam param);
    
    // 메모리 할당을 해제하고 Sound를 삭제합니다.
    ~Sound();
    
    // 사운드 파일을 재생합니다.
    void Play();
    
private:
    mpg123_handle* handle; // MPG123
    ao_device* device; // AO 디바이스
    unsigned char* buffer; // 사운드 파일 버퍼
    size_t bufferSize; // 사운드 파일 버퍼의 사이즈
    size_t done; // 완료 상태
    
    bool isPlaying = false; // 재생 상태
    
    SoundParam param;
};

#endif /* Sound_hpp */
