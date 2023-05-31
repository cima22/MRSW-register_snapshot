#include <stdlib.h>
#include <stdio.h>

typedef struct {
    long stamp;
    void* value;
} StampedValue;

int createStampedValue(StampedValue* stampedValue, long stamp, void* value) {
    if (stampedValue == NULL) {
        fprintf(stderr,"Stamped value was null.");
        return EXIT_FAILURE;
    }
    stampedValue->stamp = stamp;
    stampedValue->value = value;
    return EXIT_SUCCESS;
}

int initStampedValue(StampedValue* stampedValue, void* init) {
    if (stampedValue == NULL) {
        fprintf(stdout,"Stamped value was null.");
        return EXIT_FAILURE;
    }
    stampedValue->stamp = 0;
    stampedValue->value = init;
    return EXIT_SUCCESS;
}


// Maximum of two StampedValues based on timestamp,
// goal is to use x only after the method call (should be modified)
int max(StampedValue* x, StampedValue* y) {
    if (x == NULL) {
        fprintf(stderr,"First stamped value was null.");
        return EXIT_FAILURE;
    }
    if (y == NULL) {
        fprintf(stderr,"Second stamped value was null.");
        return EXIT_FAILURE;
    }
    if (x->stamp <= y->stamp) {
        x->stamp = y->stamp;
        x->value = y->value;
    }
    return EXIT_SUCCESS;
}

int freeStampedValue(StampedValue* stampedValue) {
    if (stampedValue == NULL) {
        fprintf(stderr,"Stamped value was null.");
        return EXIT_FAILURE;
    }
    free(stampedValue);
    return EXIT_SUCCESS;
}



