//
//  main.cpp
//  HeyMask Main
//
//  Created by BanB on 2021/06/14.
//

#include <thread>
#include <time.h>
#include <iomanip>
#include <sstream>

#include "Logger.hpp"
#include "Sound.hpp"
#include "Sender.hpp"
#include "Detector.hpp"
#include "iniparser.hpp"
             
using namespace std;
using namespace cv;

#define HEYMASK_VERSION "1.0" // HeyMask 버전

int main(int argc, const char * argv[])
{
    /* 명령 인수 확인
        ./HeyMask help
        ./HeyMask [INI 파일 경로]
     */
    if ((argc == 2 && !strcmp(argv[1], "help")) || argc != 2)
    {
        cout << COLOR_YELLOW "# " COLOR_RESET "사용법: " << argv[0] << " [INI 파일 경로]\n";
        
        if (argc == 2 && !strcmp(argv[1], "help")) return 0;
        else return -1;
    }
    
    // INI 파일 읽기
    string iniFile = argv[1];
    INI::File ini;
    if (!ini.Load(iniFile))
    {
        cout << COLOR_RED "# " COLOR_GREEN << iniFile << COLOR_RESET " 파일을 찾을 수 없습니다.\n";
        Logger::Error(iniFile + " 파일을 찾을 수 없습니다.");
        return -1;
    }
    
    // 각각의 섹션 가져오기
    INI::Section* configSec = ini.GetSection("Config");
    INI::Section* soundSec = ini.GetSection("Sound");
    INI::Section* senderSec = ini.GetSection("Sender");
    INI::Section* personSec = ini.GetSection("Person");
    INI::Section* faceSec = ini.GetSection("Face");
    
    // 전역 설정
    string source = configSec->GetValue("source", "").AsString();
    int width = configSec->GetValue("width", 0).AsInt();
    int height = configSec->GetValue("height", 0).AsInt();
    string gpuStr = configSec->GetValue("gpu", "").AsString();
    bool useGPU;
    
    // 전열 설정 유효성 검사
    if (source == "")
    {
        cout << COLOR_RED "# " COLOR_GREEN "[Config] source" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error("[Config] source 값이 잘못되었습니다.");
        return -1;
    }
    else if (gpuStr == "")
    {
        cout << COLOR_RED "# " COLOR_GREEN "[Config] gpu" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error("[Config] gpu 값이 잘못되었습니다.");
        return -1;
    }
    istringstream(gpuStr.c_str()) >> useGPU;
    
    // No Mask Sound 설정
    SoundParam noMaskParam =
    {
        "no_mask",
        soundSec->GetValue("no_mask", "").AsString()
    };
    
    // Sender 설정
    SenderParam senderParam =
    {
        senderSec->GetValue("target", "").AsString(),
        senderSec->GetValue("port", 0).AsInt(),
        senderSec->GetValue("queue_size", 0).AsInt()
    };
    
    // Person Detector 설정
    DetectorParam personParam =
    {
        "Person",
        (float)personSec->GetValue("confidence-threshold", 0.0).AsDouble(),
        (float)personSec->GetValue("nms-threshold", 0.0).AsDouble(),
        personSec->GetValue("width", 0).AsInt(),
        personSec->GetValue("height", 0).AsInt(),
        personSec->GetValue("data", "").AsString(),
        useGPU
    };
    
    // Face Detector 설정
    DetectorParam faceParam =
    {
        "Face",
        (float)faceSec->GetValue("confidence-threshold", 0.0).AsDouble(),
        (float)faceSec->GetValue("nms-threshold", 0.0).AsDouble(),
        faceSec->GetValue("width", 0).AsInt(),
        faceSec->GetValue("height", 0).AsInt(),
        faceSec->GetValue("data", "").AsString(),
        useGPU
    };
    
    // INI 메모리 할당 해제
    ini.Unload();
    
    // 소스 별칭
    string sourceStr;
    
    VideoCapture capture;
    // 웹캠 사용
    if (atoi(source.c_str()) != 0 || source.compare("0") == 0)
    {
        capture.open(stoi(source.c_str()));
        if (!capture.isOpened())
        {
            cout << COLOR_RED "# " COLOR_GREEN << source << COLOR_RESET "번 웹캠을 찾을 수 없습니다.\n";
            Logger::Error(source + "번 웹캠을 찾을 수 없습니다.");
            return -1;
        }
        else if (width <= 0)
        {
            cout << COLOR_RED "# " COLOR_GREEN "[Config] width" COLOR_RESET " 값이 잘못되었습니다.\n";
            Logger::Error("[Config] width 값이 잘못되었습니다.");
            return -1;
        }
        else if (height <= 0)
        {
            cout << COLOR_RED "# " COLOR_GREEN "[Config] height" COLOR_RESET " 값이 잘못되었습니다.\n";
            Logger::Error("[Config] height 값이 잘못되었습니다.");
            return -1;
        }
        
        capture.set(CAP_PROP_FRAME_WIDTH, width); // 해상도 너비 설정
        capture.set(CAP_PROP_FRAME_HEIGHT, height); // 해상도 높이 설정
        
        sourceStr = "Webcam " + source;
    }
    // 비디오 파일 또는 URL 사용
    else
    {
        capture.open(source);
        if (!capture.isOpened())
        {
            // URL 형식이 아니면 비디오 파일로 인식
            if (source.find("://") == string::npos)
            {
                cout << COLOR_RED "# " COLOR_GREEN << source << COLOR_RESET " 파일을 찾을 수 없습니다.\n";
                Logger::Error(source + " 파일을 찾을 수 없습니다.");
            }
            // URL 형식으로 인식
            else
            {
                cout << COLOR_RED "# " COLOR_GREEN << source << COLOR_RESET "에 연결할 수 없습니다.\n";
                Logger::Error(source + "에 연결할 수 없습니다.");
            }
            return -1;
        }
        
        sourceStr = source;
    }
    
    width = (int)capture.get(CAP_PROP_FRAME_WIDTH); // 실제 해상도 너비 가져오기
    height = (int)capture.get(CAP_PROP_FRAME_HEIGHT); // 실제 해상도 높이 가져오기
    
    // 실행 환경 정보 표시
    cout << COLOR_YELLOW "----------\n" COLOR_RESET
        << COLOR_GREEN "# " COLOR_RESET "실행 환경 정보\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "영상 소스: " COLOR_GREEN << sourceStr << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "실제 해상도: " COLOR_GREEN << (to_string(width) + string("x") + to_string(height)) << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "GPU: " << (useGPU ? COLOR_GREEN "사용" : COLOR_RED "사용 안함") << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "HeyMask 버전: " COLOR_GREEN << HEYMASK_VERSION << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "OpenCV 버전: " COLOR_GREEN << CV_VERSION << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "mpg123 버전: " COLOR_GREEN << MPG123_API_VERSION << COLOR_RESET "\n";
    
    Sound* noMaskSound = new Sound(noMaskParam); // No Mask Sound 생성
    Sender* sender = new Sender(senderParam); // Sender 생성
    Detector* personDet = new Detector(personParam); // Person Detector 생성
    Detector* faceDet = new Detector(faceParam); // Face Detector 생성
    
    cout << COLOR_YELLOW "---------------" COLOR_RESET "\n";
    
    Logger::Info("HeyMask 실행");
    
    // 프레임
    Mat frame;
    
    while (true)
    {
        capture >> frame;

        // 프레임이 비어있으면 종료
        if (frame.empty()) break;
        
        // 기존 프레임
        Mat oriFrame = frame.clone();
        
        // 마스크를 착용하지 않은 사람이 발견되면 true, 발견되지 않으면 false
        bool isNoMask = false;
        
        // 사람 탐지
        vector<DetectorResult> persons = personDet->Detect(frame);
        
        for (DetectorResult person : persons)
        {
            // 탐지된 사람의 위치
            Mat personFrame = frame(person.rect);
            
            // 사람 위치에서 얼굴 탐지
            DetectorResult* face = Detector::GetGoodResult(faceDet->Detect(personFrame));
            // 탐지 정확도 텍스트 색상, 기본 값(탐지X): 하양, 얼굴: 검정
            Scalar textColor = Scalar::all(255);
            // 사람 위치 색상, 기본 값(탐지X): 회색, 마스크 착용: 연한 초록, 마스크 미착용: 연한 빨강
            Scalar rectColor = Scalar::all(93);
            
            // 탐지 정확도를 퍼센트로 변환 (2자리)
            std::ostringstream confiPer;
            confiPer.precision(2);
            
            // 사람 정확도
            confiPer << fixed << person.confi * 100;
            string text = "P: " + confiPer.str() + "%";
            
            // 얼굴이 탐지되었을 경우
            if (face != NULL)
            {
                textColor = Scalar::all(0); // 검정
                
                // 마스크 미착용 발견
                if (strcmp(face->label, "no_mask") == 0)
                {
                    isNoMask = true;
                    rectColor = Scalar(95, 95, 241); // 연한 빨강
                }
                else
                    rectColor = Scalar(127, 229, 134); // 연한 초록
                
                // 얼굴 정확도
                confiPer.str(string());
                confiPer << fixed << face->confi * 100;
                text.append(", F: " + confiPer.str() + "%");
            }
            
            // 사람의 위치를 사각형으로 표시, 정확도 표시
            Rect topRect(person.rect.x - 1, person.rect.y - 20, person.rect.width + 2, 20);
            Size textSize = getTextSize(text, FONT_HERSHEY_TRIPLEX, 0.5, 1, 0);
            int textX = topRect.x + (topRect.width - textSize.width) / 2;
            int textY = topRect.y + (topRect.height - textSize.height / 2) + 2;
            
            rectangle(frame, topRect, rectColor, FILLED, LINE_4);
            rectangle(frame, person.rect, rectColor, 2);
            putText(frame, text, Point(textX, textY), FONT_HERSHEY_TRIPLEX, 0.5, textColor, 1);
        }
        
        // 마스크 착용을 안 한 사람을 발견했을 경우 프레임을 전송 대기열에 추가
        if (isNoMask)
        {
            thread soundThread([noMaskSound]() { noMaskSound->Play(); });
            soundThread.detach();
            
            sender->AddQueue(oriFrame);
        }
        
        // 프레임 화면에 표시
        imshow("Hey! Mask!", frame);
        
        if (waitKey(1) == 27) break;
    }
    
    // 메모리 할당 해제
    capture.release();
    delete sender;
    delete noMaskSound;
    
    Logger::Info("HeyMask 종료");
    
    return 0;
}
