//
//  Sender.cpp
//  TCP 프로토콜을 이용하여 프레임을 서버로 전송합니다.
//
//  Created by BanB on 2021/06/21.
//

#include "Sender.hpp"

Sender::Sender(SenderParam param)
{
    // 매개 변수 유효성 확인
    if (param.target == "")
    {
        cout << COLOR_RED "# " COLOR_GREEN "[Sender] address" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error("[Sender] address 값이 잘못되었습니다.");
        exit(-1);
    }
    else if (param.port <= 0 && param.port != -1)
    {
        cout << COLOR_RED "# " COLOR_GREEN "[Sender] port" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error("[Sender] port 값이 잘못되었습니다.");
        exit(-1);
    }
    else if (param.queueSize <= 0)
    {
        cout << COLOR_RED "# " COLOR_GREEN "[Sender] queue_size" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error("[Sender] queue_size 값이 잘못되었습니다.");
        exit(-1);
    }
    else if (param.port == -1)
        param.port = HEYMASK_PORT;
    
    queueSize = param.queueSize;
    
    // 주소 타입, 주소, Port 설정
    if (inet_pton(AF_INET, param.target.c_str(), &addr.sin_addr) <= 0)
    {
        cout << COLOR_RED "# " COLOR_GREEN "[Sender] address" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error("[Sender] address 값이 잘못되었습니다.");
        exit(-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(param.port);
    addrSize = sizeof addr;
    
    /* 소켓 생성
        서버와 통신이 가능한지 확인하기 위해 통신 상태만 확인하고 소켓을 닫습니다. (Ping)
     */
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bool isConnected = sockfd >= 0 && connect(sockfd, (struct sockaddr*)&addr, addrSize) >= 0;
    close(sockfd);
    
    // Sender 정보 표시
    cout << COLOR_YELLOW "--------------------" COLOR_RESET "\n"
        << COLOR_GREEN "# " COLOR_RESET "Sender " << (isConnected ? COLOR_GREEN "(연결 성공)" : COLOR_RED "(연결 실패)") << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "IPv4 주소: " COLOR_GREEN << param.target << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "Port: " COLOR_GREEN << param.port << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "전송 대기열 크기: " COLOR_GREEN << param.queueSize << "개" COLOR_RESET "\n";
    
    // 전송 스레드 생성
    sendThread = thread([this]()
    {
        while (queueSize != -1)
        {
            if (sendQueue.Size() > 0)
            {
                ssize_t sentLen = Send(sendQueue.Front());
                sendQueue.PopFront();
                
                time_t cur = time(NULL);
                struct tm* curTime = localtime(&cur);

                // 결과에 타임스탬프 추가
                ostringstream time;
                time << "[" << setfill('0')
                      << setw(2) << curTime->tm_hour << ":"
                      << setw(2) << curTime->tm_min << ":"
                      << setw(2) << curTime->tm_sec << "]";

                // 결과 출력
                if (sentLen > 0)
                {
                    cout << COLOR_CYAN << time.str() << COLOR_RESET " 전송 결과: " COLOR_GREEN "성공 " COLOR_YELLOW "[" << sentLen << " Bytes]ㅤㅤㅤㅤㅤㅤㅤㅤㅤ" COLOR_RESET "\n";
                    Logger::Info(string("전송 결과: 성공 [") + to_string(sentLen) + string(" Bytes]"));
                }
                else
                {
                    cout << COLOR_CYAN << time.str() << COLOR_RESET " 전송 결과: " COLOR_RED "실패" COLOR_RESET "\n";
                    Logger::Info("전송 결과: 실패");
                }
            }
            this_thread::sleep_for(chrono::milliseconds(1));
        }
    });
    sendThread.detach();
}

Sender::~Sender()
{
    queueSize = -1;
}

void Sender::AddQueue(Mat frame)
{
    if (sendQueue.Size() > queueSize) return;
    
    sendQueue.Push(frame);
}

ssize_t Sender::Send(Mat frame)
{
    // TCP 소켓 생성
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0 || connect(sockfd, (struct sockaddr*)&addr, addrSize) < 0) return 0;
    
    // 프레임 인코딩 (JPG)
    vector<uchar> buffer; // 인코딩된 Bytes
    vector<int> encParam = vector<int>(2); // 인코딩 매개 변수
    encParam[0] = IMWRITE_JPEG_QUALITY; // JPEG 품질 설정
    encParam[1] = 75; // 품질: 75
    imencode(".jpg", frame, buffer, encParam); // 프레임 인코딩
    
    // 프레임 전송 후 소켓 닫기
    ssize_t sentLen = send(sockfd, buffer.data(), buffer.size(), 0);
    close(sockfd);
    
    return sentLen;
}
