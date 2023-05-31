//
// Created by AK Gunter on 31.05.23.
//

#ifndef PROJECT_STAMPEDVALUE_H
#define PROJECT_STAMPEDVALUE_H

#endif //PROJECT_STAMPEDVALUE_H

typedef struct {
    long stamp;
    void* value;
} StampedValue;

int initStampedValue(StampedValue* stampedValue, void* init);

int createStampedValue(StampedValue* stampedValue, long stamp, void* value);

int max(StampedValue* x, StampedValue* y);

int freeStampedValue(StampedValue* stampedValue);