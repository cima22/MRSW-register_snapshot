//
// Created by AK Gunter on 10.06.23.
//

#include "mrmwREG.h"

void initREG(mrmwREG * reg, int value, int pid, int sn){
    atomic_store(&reg->value,value);
    atomic_store(&reg->pid,pid);
    atomic_store(&reg->sn,sn);
}

void updateREG(mrmwREG * reg, int value, int sn){
    atomic_store(&reg->value,value);
    atomic_store(&reg->sn,sn);
}

bool compareREG(mrmwREG aa, mrmwREG bb){
    return getValue(aa) == getValue(bb)
           && getPid(aa) == getPid(bb)
           && getSn(aa) == getSn(bb);
}

void copyREG(mrmwREG*aa,mrmwREG* bb) {
    aa->pid = bb->pid;
    aa->sn = bb->sn;
    aa->value = bb->value;
}

int getValue(mrmwREG reg){
    return atomic_load(&reg.value);
}

int getPid(mrmwREG reg){
    return atomic_load(&reg.pid);
}

int getSn(mrmwREG reg){
    return atomic_load(&reg.sn);
}