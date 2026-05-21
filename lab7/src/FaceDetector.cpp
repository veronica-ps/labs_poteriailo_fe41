#include "FaceDetector.hpp"
#include <iostream>
#include <chrono>

FaceDetector::FaceDetector(const std::string& prototxt, const std::string& model) {
    try {
        net = cv::dnn::readNetFromCaffe(prototxt, model);
        std::cout << "Face Detector successfuly improted.\n";
    } catch (const cv::Exception& e) {
        std::cerr << "Error occured: " << e.what() << "\n";
    }
    running = false;
    hasNewFrame = false;
}

FaceDetector::~FaceDetector() {
    stop();
}

void FaceDetector::start() {
    running = true;
    workerThread = std::thread(&FaceDetector::detectLoop, this);
}

void FaceDetector::stop() {
    running = false;
    cv.notify_one();
    if (workerThread.joinable()) {
        workerThread.join();
    }
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    frame.copyTo(currentFrame);
    hasNewFrame = true;
    cv.notify_one();
}

std::vector<cv::Rect> FaceDetector::getFaces() {
    std::lock_guard<std::mutex> lock(mtx);
    return detectedFaces;
}

void FaceDetector::detectLoop() {
    while (running) {
        cv::Mat frame;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this]() { return hasNewFrame || !running; });
            
            if (!running) break;
            
            frame = currentFrame.clone();
            hasNewFrame = false;
        }
        
        if (frame.empty() || net.empty()) continue;

        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));
        net.setInput(blob);
        cv::Mat detections = net.forward(); 
        std::vector<cv::Rect> faces;
        cv::Mat detectionMat(detections.size[2], detections.size[3], CV_32F, detections.ptr<float>());

        for (int i = 0; i < detectionMat.rows; i++) {
            float confidence = detectionMat.at<float>(i, 2);
            if (confidence > 0.5) {
                int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
                int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
                int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
                int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);
                faces.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
            }
        }
        
        {
            std::lock_guard<std::mutex> lock(mtx);
            detectedFaces = faces;
        }
    }
}
