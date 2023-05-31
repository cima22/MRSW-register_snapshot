//
// Created by Gabriele on 31/05/2023.
//

#ifndef AMP_TESTS2_WFSNAPSHOT_H
#define AMP_TESTS2_WFSNAPSHOT_H


#include "StampedSnap.h"
#include "omp.h"

template<typename T>
class WFSnapshot {
private:
    StampedSnap<T>** a_table;
    unsigned int num_threads;

public:
    WFSnapshot(int capacity, T init) {
        a_table = new StampedSnap<T>*[capacity];
        for (int i = 0; i < capacity; i++) {
            a_table[i] = new StampedSnap<T>(init);
        }
        num_threads = capacity;
    }

    StampedSnap<T>** collect() {
        auto** copy = new StampedSnap<T>*[num_threads];
        for (int j = 0; j < num_threads; j++) {
            copy[j] = a_table[j];
        }
        return copy;
    }

    void update(T value) {
        int me = omp_get_thread_num();
        T* snap = scan();
        StampedSnap<T>* oldValue = a_table[me];
        auto* newValue = new StampedSnap<T>(oldValue->stamp + 1, value, snap);
        a_table[me] = newValue;
    }

    T* scan() {
        StampedSnap<T>** oldCopy;
        StampedSnap<T>** newCopy;
        bool* moved = new bool[num_threads];
        oldCopy = collect();
        while (true) {
            newCopy = collect();
            for (int j = 0; j < num_threads; j++) {
                if (oldCopy[j]->stamp != newCopy[j]->stamp) {
                    if (moved[j]) {
                        return oldCopy[j]->snap;
                    } else {
                        moved[j] = true;
                        oldCopy = newCopy;
                    }
                }
            }
            T* result = new T[num_threads];
            for (int j = 0; j < num_threads; j++) {
                result[j] = newCopy[j]->value.load();
            }
            return result;
        }
    }
};


#endif //AMP_TESTS2_WFSNAPSHOT_H
