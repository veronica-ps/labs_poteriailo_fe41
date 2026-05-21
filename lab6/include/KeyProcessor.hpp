#pragma once

enum class ProcessMode {
    NORMAL,
    INVERT,
    BLUR,
    CANNY,
    PIP
};

class KeyProcessor {
private:
    ProcessMode currentMode;
    bool overlayActive;
public:
    KeyProcessor();
    void processKey(int key);
    ProcessMode getMode() const;
    bool isOverlayActive() const;
};
