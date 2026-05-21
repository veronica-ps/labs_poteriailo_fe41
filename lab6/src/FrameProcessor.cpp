#include "FrameProcessor.hpp"
#include <iostream>
#include <vector>

FrameProcessor::FrameProcessor() : brightnessOffset(50) {
    overlayImage = cv::imread("image01.png", cv::IMREAD_UNCHANGED);
    if (overlayImage.empty()) {
        std::cout << "image01.png not found. Import it in build dir.\n";
    } else {
        cv::resize(overlayImage, overlayImage, cv::Size(150, 150));
    }
}

cv::Mat FrameProcessor::process(const cv::Mat& input, ProcessMode mode, bool showOverlay) {
    cv::Mat output;
    input.convertTo(output, -1, 1, brightnessOffset - 50);

    switch (mode) {
        case ProcessMode::INVERT:
            cv::bitwise_not(output, output);
            break;
        case ProcessMode::BLUR:
            cv::GaussianBlur(output, output, cv::Size(15, 15), 0);
            break;
        case ProcessMode::CANNY:
            cv::cvtColor(output, output, cv::COLOR_BGR2GRAY);
            cv::Canny(output, output, 50, 150);
            cv::cvtColor(output, output, cv::COLOR_GRAY2BGR);
            break;
	case ProcessMode::PIP:
    		if (showOverlay && !overlayImage.empty()) {
        		int margin = 20;
        		int x = output.cols - overlayImage.cols - margin;
        		int y = output.rows - overlayImage.rows - margin;
	        	if (x >= 0 && y >= 0) {
        			cv::Rect roi(x, y, overlayImage.cols, overlayImage.rows);
            			if (overlayImage.channels() == 4) {
                			std::vector<cv::Mat> channels;
                			cv::split(overlayImage, channels);
                			cv::Mat rgb;
                			cv::merge(std::vector<cv::Mat>{channels[0], channels[1], channels[2]}, rgb);
                			rgb.copyTo(output(roi), channels[3]);
            			} else {
                			overlayImage.copyTo(output(roi));
            			}
        		}
    		}
    		break;
        case ProcessMode::NORMAL:
        default:
            break; 
    }
    return output;
}
