#ifndef GRAPHICALDISPLAY_H
#define GRAPHICALDISPLAY_H

#include "display.h"

// Placeholder for a GUI implementation (later)
class GraphicalDisplay : public Display {
public:
    GraphicalDisplay() = default;
    ~GraphicalDisplay() override = default;

    void notify(Subject &whoFrom) override;
};

#endif
