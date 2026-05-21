#pragma once
#include <opencv2/opencv.hpp>

class CameraProvider {
private:
    cv::VideoCapture cap;
public:
    CameraProvider();
    ~CameraProvider();
    cv::Mat getFrame();
};
