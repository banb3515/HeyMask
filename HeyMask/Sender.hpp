//
//  Sender.hpp
//  TCP 프로토콜을 이용하여 프레임을 서버로 전송합니다.
//
//  Created by BanB on 2021/06/21.
//

#ifndef Sender_hpp
#define Sender_hpp

#include <iostream>
#include <string.h>
#include <vector>
#include <queue>
#include <thread>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>

#include "Logger.hpp"
#include "Color.hpp"
#include "SharedQueue.h"

using namespace std;
using namespace cv;

#define HEYMASK_PORT 34726 // HeyMask, Sender 기본 Port

/* Sender의 매개 변수입니다.
 @param target: 연결할 서버의 IPv4 주소입니다.
 @param port: 연결할 서버의 Port 번호입니다.
 @param queueSize: 전송 대기열의 크기입니다.
 */
struct SenderParam
{
    string target;
    int port;
    int queueSize;
};

// TCP 프로토콜을 이용하여 프레임을 서버로 전송합니다.
class Sender
{
public:
    /* Sender를 생성 후 초기화합니다.
     @param param: Sender의 매개 변수입니다.
     */
    Sender(SenderParam param);
    
    // 스레드를 중지하고 Sender를 삭제합니다.
    ~Sender();
    
    /* 프레임을 전송 대기열에 추가합니다.
     @param frame: 대기열에 추가할 프레임입니다.
     */
    void AddQueue(Mat frame);
    
private:
    /* 프레임을 서버로 전송합니다.
     @param frame: 전송할 프레임입니다.
     @return 전송된 Bytes의 길이입니다. 전송에 실패했을 경우 0이 반환됩니다.
     */
    ssize_t Send(Mat frame);
    
    // 서버 주소 구조체
    struct sockaddr_in addr;
    // 서버 주소 구조체의 크기
    int addrSize;
    
    // 전송 스레드
    thread sendThread;
    
    // 전송 대기열
    SharedQueue<Mat> sendQueue;
    // 전송 대기열의 크기
    int queueSize;
};

#endif /* Sender_hpp */
