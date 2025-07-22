#ifndef TEXTDISPLAY_H
#define TEXTDISPLAY_H

#include "display.h"
#include <iostream>

class TextDisplay : public Display {
public:
    TextDisplay() = default;
    ~TextDisplay() override = default;

    // Updates ASCII representation when board notifies
    void notify(Subject &whoFrom) override;
};

#endif
