#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <condition_variable>

class FaceDetector {
private:
    cv::dnn::Net net;
    std::thread workerThread;
    std::mutex mtx;
    std::condition_variable cv;
    
    std::atomic<bool> running;
    bool hasNewFrame;
    
    cv::Mat currentFrame;
    std::vector<cv::Rect> detectedFaces;
    
    void detectLoop();
public:
    FaceDetector(const std::string& prototxt, const std::string& model);
    ~FaceDetector();
    
    void start();
    void stop();
    void updateFrame(const cv::Mat& frame);
    std::vector<cv::Rect> getFaces();
};
