#include <stdatomic.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct {
    atomic_int value;
} SRSW_Register;

void SRSW_Write(SRSW_Register *reg, int value) {
    atomic_store(&reg->value, value);
}

int SRSW_Read(SRSW_Register *reg) {
    return atomic_load(&reg->value);
}

SRSW_Register* SRSW_Register_Create() {
    SRSW_Register *reg = malloc(sizeof(SRSW_Register));
    atomic_init(&reg->value, 0);
    return reg;
}

void SRSW_Register_Destroy(SRSW_Register *reg) {
    free(reg);
}
int main(){
    SRSW_Register* reg1 = SRSW_Register_Create();
    SRSW_Write(reg1, 12);
    printf("%d\n",SRSW_Read(reg1));
}