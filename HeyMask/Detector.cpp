//
//  Detector.cpp
//  YOLO를 이용하여 객체를 탐지합니다.
//
//  Created by BanB on 2021/06/14.
//

#include "Detector.hpp"

Detector::Detector(DetectorParam param)
{
    // 매개 변수 유효성 확인
    if (param.confThreshold <= 0.0)
    {
        cout << COLOR_RED "# " COLOR_GREEN "[" << param.alias << "] confidence-threshold" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error(string("[") + param.alias + "] confidence-threshold 값이 잘못되었습니다.");
        exit(-1);
    }
    else if (param.nmsThreshold <= 0.0)
    {
        cout << COLOR_RED "# " COLOR_GREEN "[" << param.alias << "] nms-threshold" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error(string("[") + param.alias + "] nms-threshold 값이 잘못되었습니다.");
        exit(-1);
    }
    else if (param.width <= 0)
    {
        cout << COLOR_RED "# " COLOR_GREEN "[" << param.alias << "] width" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error(string("[") + param.alias + "] width 값이 잘못되었습니다.");
        exit(-1);
    }
    else if (param.height <= 0)
    {
        cout << COLOR_RED "# " COLOR_GREEN "[" << param.alias << "] height" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error(string("[") + param.alias + "] height 값이 잘못되었습니다.");
        exit(-1);
    }
    else if (param.data == "")
    {
        cout << COLOR_RED "# " COLOR_GREEN "[" << param.alias << "] data" COLOR_RESET " 값이 잘못되었습니다.\n";
        Logger::Error(string("[") + param.alias + "] data 값이 잘못되었습니다.");
        exit(-1);
    }
    
    this->param = param;
    
    // 데이터 파일 확장자 설정
    string classesFile = param.data;
    classesFile.append(".names");
    string configFile = param.data;
    configFile.append(".cfg");
    string weightsFile = param.data;
    weightsFile.append(".weights");
    
    // 데이터 파일들이 존재하는지 확인
    if (access(classesFile.c_str(), F_OK) != 0)
    {
        cout << COLOR_RED "# " COLOR_GREEN << classesFile << COLOR_RESET " 파일을 찾을 수 없습니다.\n";
        Logger::Error(classesFile + " 파일을 찾을 수 없습니다.");
        exit(-1);
    }
    if (access(configFile.c_str(), F_OK) != 0)
    {
        cout << COLOR_RED "# " COLOR_GREEN << configFile << COLOR_RESET " 파일을 찾을 수 없습니다.\n";
        Logger::Error(configFile + " 파일을 찾을 수 없습니다.");
        exit(-1);
    }
    if (access(weightsFile.c_str(), F_OK) != 0)
    {
        cout << COLOR_RED "# " COLOR_GREEN << weightsFile << COLOR_RESET " 파일을 찾을 수 없습니다.\n";
        Logger::Error(weightsFile + " 파일을 찾을 수 없습니다.");
        exit(-1);
    }
    
    // 클래스 배열에 클래스 추가
    ifstream ifs(classesFile);
    string line;
    while (getline(ifs, line))
        classes.push_back(line);
    
    // Darknet(YOLO) 설정
    net = readNetFromDarknet(configFile, weightsFile);
    if (param.gpu)
    {
#if USE_CUDA == 1
        net.setPreferableBackend(DNN_BACKEND_CUDA);
        net.setPreferableTarget(DNN_TARGET_CUDA);
#elif USE_CUDA == 0
        net.setPreferableBackend(DNN_BACKEND_OPENCV);
        net.setPreferableTarget(DNN_TARGET_OPENCL);
#endif
    }
    else net.setPreferableBackend(DNN_TARGET_CPU);

    // Detector 정보 표시
    string cLine = "";
    for (int i = 0; i < string(param.alias).size() + 13; i ++)
        cLine.append("-");
    
    cout << COLOR_YELLOW << cLine << COLOR_RESET "\n"
        << COLOR_GREEN "# " COLOR_RESET "Detector " COLOR_GREEN "(" << param.alias << ")" COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "데이터 파일: " COLOR_GREEN << param.data << COLOR_RESET << "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "신뢰 임계 값: " COLOR_GREEN << ceilf((double)param.confThreshold * 100 * 100) / 100 << "%" COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "비 최대 억제 임계 값(NMS): " COLOR_GREEN << ceilf((double)param.nmsThreshold * 100 * 100) / 100 << "%" COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "네트워크의 입력 이미지 너비: " COLOR_GREEN << param.width << COLOR_RESET "\n"
        << "  " COLOR_CYAN "- " COLOR_RESET "네트워크의 입력 이미지 높이: " COLOR_GREEN << param.height << COLOR_RESET "\n";
}

vector<DetectorResult> Detector::Detect(Mat frame)
{
    Mat blob;
    blobFromImage(frame, blob, 1 / 255.0, Size(param.width, param.height), Scalar(0, 0, 0), true, false);
    net.setInput(blob);
    
    vector<Mat> outs;
    net.forward(outs, GetOutputsNames());
    
    Mat detectedFrame;
    frame.convertTo(detectedFrame, CV_8U);
    
    vector<int> classIds; // 탐지된 클래스 ID 배열
    vector<float> confis; // 탐지된 정확도 배열
    vector<Rect> rects; // 탐지된 위치 배열

    for (int idx = 0; idx < outs.size(); ++idx)
    {
        float* data = (float*)outs[idx].data;
        
        for (int j = 0; j < outs[idx].rows; ++j, data += outs[idx].cols)
        {
            Mat scores = outs[idx].row(j).colRange(5, outs[idx].cols);
            Point classIdP;
            double confi;
            
            minMaxLoc(scores, 0, &confi, 0, &classIdP);
            
            if (confi > param.confThreshold)
            {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                
                if (left < 0) left = 0;
                if (top < 0) top = 0;
                if (left + width > frame.cols) width = frame.cols - left;
                if (top + height > frame.rows) height = frame.rows - top;
                
                classIds.push_back(classIdP.x);
                confis.push_back((float)confi);
                rects.push_back(Rect(left, top, width, height));
            }
        }
    }

    vector<DetectorResult> results;
    vector<int> indices;
    NMSBoxes(rects, confis, param.confThreshold, param.nmsThreshold, indices);
    
    for (size_t i = 0; i < indices.size(); ++i)
    {
        int idx = indices[i];
        Rect rect = rects[idx];
        DetectorResult result = { classes[classIds[idx]].c_str(), confis[idx], rect };
        results.push_back(result);
    }
    
    return results;
}

DetectorResult* Detector::GetGoodResult(vector<DetectorResult> results)
{
    // 결과 배열이 비어있을 경우 NULL 반환
    if (results.size() == 0) return NULL;
    
    // 정확도가 높은 결과의 인덱스
    int goodIdx = 0;
    
    for (int i = 1; i < results.size(); i ++)
    {
        if (results[goodIdx].confi < results[i].confi)
            goodIdx = i;
    }
    
    // 정확도가 높은 결과로 다시 결과 생성
    DetectorResult* result = new DetectorResult
    {
        results[goodIdx].label,
        results[goodIdx].confi,
        results[goodIdx].rect
    };
    
    return result;
}

vector<String> Detector::GetOutputsNames()
{
    vector<String> names;
    
    vector<int> outLayers = net.getUnconnectedOutLayers();
    vector<String> layersNames = net.getLayerNames();
        
    names.resize(outLayers.size());
    for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    
    return names;
}
