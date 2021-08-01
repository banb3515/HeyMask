//
//  Sound.cpp
//  사운드 출력을 관리합니다.
//
//  Created by BanB on 2021/06/26.
//

#include "Sound.hpp"

Sound::Sound(SoundParam param)
{
    // 매개 변수 유효성 확인
    if (param.path == "")
    {
        cout << COLOR_RED "# " COLOR_GREEN "[Sound] " << param.alias << COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error(string("[Sound] ") + param.alias + " 값이 잘못되었습니다.");
        exit(-1);
    }
    
    this->param = param;
    
    // 사운드 파일이 존재하는지 확인
    if (access(param.path.c_str(), F_OK) != 0)
    {
        cout << COLOR_RED "# " COLOR_GREEN << param.path << COLOR_RESET " 파일을 찾을 수 없습니다.\n";
        Logger::Error(param.path + " 파일을 찾을 수 없습니다.");
        exit(-1);
    }
    
    ao_sample_format format;
    int error, channels, encoding;
    long rate;
    
    // 초기화
    ao_initialize();
    int driver = ao_default_driver_id();
    mpg123_init();
    handle = mpg123_new(NULL, &error);
    bufferSize = mpg123_outblock(handle);
    buffer = (unsigned char*) malloc(bufferSize * sizeof(unsigned char));
    
    // 사운드 오픈, 파일 포맷 가져오기
    mpg123_open(handle, param.path.c_str());
    mpg123_getformat(handle, &rate, &channels, &encoding);
    
    // 포맷 설정
    format.bits = mpg123_encsize(encoding) * 8; // 8 Bits
    format.rate = (int)rate;
    format.channels = channels;
    format.byte_format = AO_FMT_NATIVE;
    format.matrix = 0;
    
    // 오디오 장치
    device = ao_open_live(driver, &format, NULL);
    
    // Sound 정보 표시
    string cLine = "";
    for (int i = 0; i < string(param.alias).size() + 10; i ++)
        cLine.append("-");
    
    cout << COLOR_YELLOW << cLine << COLOR_RESET "\n"
        << COLOR_GREEN "# " COLOR_RESET "Sound " COLOR_GREEN "(" << param.alias << ")" COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "사운드 파일: " COLOR_GREEN << param.path << COLOR_RESET << "\n";
}

Sound::~Sound()
{
    free(buffer);
    ao_close(device);
    mpg123_close(handle);
    mpg123_delete(handle);
    mpg123_exit();
    ao_shutdown();
}

void Sound::Play()
{
    if (isPlaying) return;
    
    isPlaying = true;
    
    // 파일 오픈 & 재생
    while (mpg123_read(handle, buffer, bufferSize, &done) == MPG123_OK)
        ao_play(device, reinterpret_cast<char*>(buffer), (uint_32)done);
    
    isPlaying = false;
    
    // 사운드 재 오픈
    mpg123_open(handle, param.path.c_str());
}
