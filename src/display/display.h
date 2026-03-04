#ifndef DISPLAY_H
#define DISPLAY_H

#include "observer.h"
#include "board.h"

// Abstract base display
class AbstractDisplay : public Observer {
public:
    const int BOARD_SIZE = 8; 
    // this is here so that the observers know that the subject (the board) 
    // has size 8 across all displays/observers.
    
    virtual ~AbstractDisplay() {}
    
    // observer interface --> called when Board (Subject) notifies
    virtual void notify(Subject &whoFrom) override = 0;
};

#endif
