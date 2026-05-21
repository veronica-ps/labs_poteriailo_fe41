#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"
#include "FaceDetector.hpp"

cv::Point startPt(-1, -1);
cv::Point endPt(-1, -1);
bool isDrawing = false;
bool showArrow = false;

void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == cv::EVENT_LBUTTONDOWN) {
        startPt = cv::Point(x, y);
        endPt = startPt;
        isDrawing = true;
        showArrow = true;
    } else if (event == cv::EVENT_MOUSEMOVE) {
        if (isDrawing) {
            endPt = cv::Point(x, y);
        }
    } else if (event == cv::EVENT_LBUTTONUP) {
        if (isDrawing) {
            endPt = cv::Point(x, y);
            isDrawing = false;
        }
    } else if (event == cv::EVENT_RBUTTONDOWN) {
        showArrow = false;
        isDrawing = false;
    }
}

int main() {
    std::cout << "Ultra camera app\n";
    std::cout << "1 - Normal | 2 - Invert | 3 - Blur | 4 - Canny | 5 - PIP (58) | F - Face detector\n";
    std::cout << "Left mouse button - Draw line | Right mouse button - clear line\n";
    std::cout << "ESC - exit\n";

    CameraProvider camera;
    KeyProcessor keyProcessor;
    FrameProcessor frameProcessor;
    Display display("FE-41 Lab 7");

    FaceDetector faceDetector("deploy.prototxt", "res10_300x300_ssd_iter_140000.caffemodel");
    faceDetector.start();

    cv::setMouseCallback(display.getWindowName(), onMouse, nullptr);

    int trackbarVal = 50;
    cv::createTrackbar("Brightness", display.getWindowName(), &trackbarVal, 100);

    double fps = 0.0;
    int frames = 0;
    int64 startTick = cv::getTickCount();

    while (true) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;
	ProcessMode currentMode = keyProcessor.getMode();
        frameProcessor.brightnessOffset = cv::getTrackbarPos("Brightness", display.getWindowName());
        cv::Mat processedFrame = frameProcessor.process(frame, currentMode, keyProcessor.isOverlayActive());
	if (currentMode == ProcessMode::FACE) {
            faceDetector.updateFrame(frame);
            std::vector<cv::Rect> faces = faceDetector.getFaces();

            for (const auto& face : faces) {
                cv::rectangle(processedFrame, face, cv::Scalar(0, 255, 0), 2);
            }
        }
        // FPS
        frames++;
        int64 currentTick = cv::getTickCount();
        double timeElapsed = (currentTick - startTick) / cv::getTickFrequency();
        if (timeElapsed >= 1.0) {
            fps = frames / timeElapsed;
            frames = 0;
            startTick = currentTick;
        }

        cv::putText(processedFrame, "FPS: " + std::to_string((int)fps), cv::Point(10, 30), 
                    cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
        if (showArrow) {
            if (startPt != endPt) {
                cv::arrowedLine(processedFrame, startPt, endPt, cv::Scalar(255, 50, 50), 3, 8, 0, 0.05);
            } else {
                cv::circle(processedFrame, startPt, 3, cv::Scalar(255, 50, 50), -1);
            }
        }

        display.show(processedFrame);

        int key = cv::waitKey(30);
        if (key == 27) break; 
        keyProcessor.processKey(key);
    }
    faceDetector.stop();
    return 0;
}
