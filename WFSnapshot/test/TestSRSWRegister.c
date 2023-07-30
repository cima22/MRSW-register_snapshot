//
// Created by Gabriele on 15/06/2023.
//

#include "../src/srswRegisterOpenMP.h"

int main() {
    AtomicSRSWRegister srswRegister;
    createAtomicSRSWRegister(&srswRegister,0);
    StampedValue result = readSRSW(&srswRegister);
    printf("%ld -- %ld - %d\n",srswRegister.lastStamp,result.stamp,result.value);
    writeSRSW(&srswRegister,(StampedValue) {1,10});
    result = readSRSW(&srswRegister);
    printf("%ld -- %ld - %d\n",srswRegister.lastStamp,result.stamp,result.value);
}
