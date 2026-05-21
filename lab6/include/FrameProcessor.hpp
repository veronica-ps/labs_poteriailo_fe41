#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
private:
    cv::Mat overlayImage;
public:
    int brightnessOffset; 
    FrameProcessor();
    cv::Mat process(const cv::Mat& input, ProcessMode mode, bool showOverlay);
};
