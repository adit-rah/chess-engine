#ifndef OBSERVER_H
#define OBSERVER_H

static const int BOARD_SIZE = 8; // standard chess board size
class Subject;

class Observer {
  public:
    // Pass the Subject that called the notify method.
    virtual ~Observer() = default;
    virtual void notify(Subject &whoFrom) = 0; 
};

#endif
