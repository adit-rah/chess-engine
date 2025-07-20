#ifndef SUBJECT_H
#define SUBJECT_H


class Observer;

class Subject {
    std::vector<Observer*> observers;

    public:
        void detach(Observer o);
        void attach(Observer o);
};





#endif