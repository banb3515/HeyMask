//
//  Detector.hpp
//  YOLO를 이용하여 객체를 탐지합니다.
//
//  Created by BanB on 2021/06/14.
//

#ifndef Detector_hpp
#define Detector_hpp

#include <math.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <unistd.h>

#include "Logger.hpp"
#include "Color.hpp"

using namespace std;
using namespace cv;
using namespace cv::dnn;

/* Detector의 매개 변수입니다.
 @param alias: Detector의 별칭입니다.
 @param confThreshold: 신뢰 임계 값 입니다.
 @param nmsThreshold: 비 최대 억제 임계 값(NMS) 입니다.
 @param width: 네트워크의 입력 이미지 너비입니다.
 @param height: 네트워크의 입력 이미지 높이입니다.
 @param data: 데이터 파일입니다.
 @param gpu: GPU 사용 여부입니다.
 */
struct DetectorParam
{
    string alias;
    float confThreshold;
    float nmsThreshold;
    int width;
    int height;
    string data;
    bool gpu;
};

/* Detector의 탐지 결과입니다.
 @param label: 탐지된 클래스 이름입니다.
 @param confi: 정확도입니다.
 @param rect: 탐지된 위치입니다.
 */
struct DetectorResult
{
    const char *label;
    float confi;
    Rect rect;
};

// YOLO를 이용하여 객체를 탐지합니다.
class Detector
{
public:
    /* Detector를 생성 후 초기화합니다.
     @param param: Detector의 매개 변수입니다.
     */
    Detector(DetectorParam param);

    /* 여러개의 결과 중 정확도가 가장 높은 결과만 가져옵니다.
     @param results: 탐지된 결과 배열입니다.
     @return 정확도가 가장 높은 결과입니다. 결과 배열이 비어있을 경우 NULL을 반환합니다.
     */
    static DetectorResult *GetGoodResult(vector<DetectorResult> results);

    /* 프레임에서 객체를 탐지합니다.
     @param frame: 탐지할 프레임입니다.
     @return 탐지된 결과 배열입니다.
     */
    vector<DetectorResult> Detect(Mat frame);

private:
    vector<String> GetOutputsNames();

    // 클래스 배열입니다.
    vector<string> classes;

    Net net;

    // Detector의 매개 변수입니다.
    DetectorParam param;
};

#endif /* Detector_hpp */
