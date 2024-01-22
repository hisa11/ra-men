#include "mbed.h"
BufferedSerial pc(USBTX, USBRX, 250000); // パソコンとのシリアル通信
CANMessage msg;
CAN can1(PA_11, PA_12, (int)1e6);
int16_t pwm0[4] = {0, 0, 0, 0}; // モタドラ1
int sokudo = 1000;

int main()
{
    while (1)
    {
        if (pc.readable())
        {
            char buf;
            pc.read(&buf, sizeof(buf));
            if (buf == 'w')
            {
                pwm0[0] = sokudo; // 右前
                pwm0[1] = sokudo; // 右後ろ
                pwm0[2] = sokudo; // 左前
                pwm0[3] = sokudo; // 左後ろ
            }
            else if (buf == 's')
            {
                pwm0[0] = -sokudo;
                pwm0[1] = -sokudo;
                pwm0[2] = -sokudo;
                pwm0[3] = -sokudo;
            }
            else if (buf == 'd')
            {
                pwm0[0] = sokudo;
                pwm0[1] = -sokudo;
                pwm0[2] = -sokudo;
                pwm0[3] = sokudo;
            }
            else if (buf == 'a')
            {
                pwm0[0] = -sokudo;
                pwm0[1] = sokudo;
                pwm0[2] = sokudo;
                pwm0[3] = -sokudo;
            }
            else if (buf == 'z')
            {
                pwm0[0] = 0;
                pwm0[1] = 0;
                pwm0[2] = 0;
                pwm0[3] = 0;
            }
        }
        CANMessage msg0(2, (const uint8_t *)pwm0, 8);
        can1.write(msg0);
    }
}
