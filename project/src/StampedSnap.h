//
// Created by Gabriele on 31/05/2023.
//

#ifndef AMP_TESTS2_STAMPEDSNAP_H
#define AMP_TESTS2_STAMPEDSNAP_H

#include <atomic>

template <typename T>
class StampedSnap {
public:
    long stamp;
    std::atomic<T> value;
    T* snap;

    StampedSnap(T value) {
        stamp = 0;
        this->value = value;
        snap = nullptr;
    }

    StampedSnap(long label, T value, T* snap) {
        this->stamp = label;
        this->value = value;
        this->snap = snap;
    }
};


#endif //AMP_TESTS2_STAMPEDSNAP_H
