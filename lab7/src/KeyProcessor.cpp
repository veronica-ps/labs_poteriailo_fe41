#include "KeyProcessor.hpp"
#include <iostream>

KeyProcessor::KeyProcessor() : currentMode(ProcessMode::NORMAL), overlayActive(false) {}

void KeyProcessor::processKey(int key) {
    if (key == -1) return;
    
    switch (key) {
        case '1': currentMode = ProcessMode::NORMAL; std::cout << "Normal mode\n"; break;
        case '2': currentMode = ProcessMode::INVERT; std::cout << "Invert mode\n"; break;
        case '3': currentMode = ProcessMode::BLUR; std::cout << "Blur mode\n"; break;
        case '4': currentMode = ProcessMode::CANNY; std::cout << "Canny mode\n"; break;
        case '5': currentMode = ProcessMode::PIP; overlayActive = !overlayActive;
            std::cout << "Image: " << (overlayActive ? "On" : "Off") << "\n"; break;
	case 'f': case 'F': currentMode = ProcessMode::FACE; std::cout << "Face detection\n"; break;
	default: break;
    }
}

ProcessMode KeyProcessor::getMode() const {
    return currentMode;
}
bool KeyProcessor::isOverlayActive() const { return overlayActive; }
