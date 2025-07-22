#ifndef DISPLAY_H
#define DISPLAY_H

#include "observer.h"
#include "board.h"

// Abstract base display
class Display : public Observer {
public:
    virtual ~Display() {}
    
    // observer interface --> called when Board (Subject) notifies
    virtual void notify(Subject &whoFrom) override = 0;
};

#endif
