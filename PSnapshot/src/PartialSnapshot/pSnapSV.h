//
// Created by AK Gunter on 10.06.23.
//

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    long stamp;
    int* value;
} StampedValue;

int createStampedValue(StampedValue* stampedValue, long stamp, int* value);
int initStampedValue(StampedValue* stampedValue, int* init);
int isFirstBigger(StampedValue* x, StampedValue* y);
int copyStampedValueSecondIntoFirst(StampedValue* x, StampedValue* y);
int freeStampedValue(StampedValue* stampedValue);
int duplicateStampedValue(StampedValue* duplicate, StampedValue* original);

