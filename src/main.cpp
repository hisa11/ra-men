#include "mbed.h"

BufferedSerial pc(USBTX, USBRX, 115200); // パソコンとのシリアル通信
CAN can(PA_11, PA_12, (int)1e6);
uint8_t DATA[8] = {};
int main(){
    while(1){
        int16_t output = 9000;
        DATA[0] = output >> 1;
        DATA[1] = output;
        CANMessage msg0(0x200, DATA, 8);
        can.write(msg0);

        for(auto e: DATA) {
            printf("%2x ", e);
        }
        printf("\n");
    }
}
