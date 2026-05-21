#include "Display.hpp"

Display::Display(const std::string& name) : windowName(name) {
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
}

void Display::show(const cv::Mat& frame) {
    cv::imshow(windowName, frame);
}

std::string Display::getWindowName() const {
    return windowName;
}
