#ifndef SUBJECT_H
#define SUBJECT_H

class Observer;

class Subject {
    std::vector<Observer*> observers;
    protected:
    void setState(State newS);
    public:
        void detach(Observer *o);
        void attach(Observer *o);
};

#endif
