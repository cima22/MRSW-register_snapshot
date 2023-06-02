#ifndef STAMPEDVALUE_H
#define STAMPEDVALUE_H

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    long stamp;
    void* value;
} StampedValue;

int createStampedValue(StampedValue* stampedValue, long stamp, void* value);
int initStampedValue(StampedValue* stampedValue, void* init);
int isFirstBigger(StampedValue* x, StampedValue* y);
int copyStampedValueSecondIntoFirst(StampedValue* x, StampedValue* y);
int freeStampedValue(StampedValue* stampedValue);
int duplicateStampedValue(StampedValue* duplicate, StampedValue* original);

#endif //STAMPEDVALUE_H
