#include "mbed.h"

DigitalIn encoderA(A0); // A相のピン A0からA5
DigitalIn encoderB(A1); // B相のピン A0からA5
int counter = 0;
bool prevStateA = 0;
bool prevStateB = 0;

int koppepan = 'a';
int dorayaki = 'a';

BufferedSerial pc(USBTX, USBRX, 250000); // パソコンとのシリアル通信
// pwm[0] = 0;         左前タイヤ
// pwm[1] = 0;         左後ろタイヤ
// pwm[2] = 0;         右前タイヤ
// pwm[3] = 0;         右後ろタイヤ
CAN can(PA_11, PA_12, (int)1e6);
int16_t pwm[4] = {0, 0, 0, 0};  // pwm制御
int16_t pwm1[4] = {0, 0, 0, 0}; // pwm制御

CANMessage msg;
int bekon = 'a';
int gohan = 'a';
int tyuusyoku = 'b';
int mikan = 'a'; // ←a方向とb方向がある

DigitalIn button1(D4); // 塗機構スイッチ1
DigitalIn button2(D5); // 塗機構スイッチ2

int main()
{
    while (1) // ここからロリコン
    {
        int button1State = button1.read();
        int button2State = button2.read();
        bool stateA = encoderA.read();
        bool stateB = encoderB.read();

        if (prevStateA == 0 && prevStateB == 0)
        {
            if (stateA == 1 && stateB == 0)
            {
                counter++;
            }
            if (stateA == 0 && stateB == 1)
            {
                counter--;
            }
        }
        if (prevStateA == 1 && prevStateB == 0)
        {
            if (stateA == 1 && stateB == 1)
            {
                counter++;
            }
            if (stateA == 0 && stateB == 0)
            {
                counter--;
            }
        }
        if (prevStateA == 1 && prevStateB == 1)
        {
            if (stateA == 0 && stateB == 1)
            {
                counter++;
            }
            if (stateA == 1 && stateB == 0)
            {
                counter--;
            }
        }
        if (prevStateA == 0 && prevStateB == 1)
        {
            if (stateA == 0 && stateB == 0)
            {
                counter++;
            }
            if (stateA == 1 && stateB == 1)
            {
                counter--;
            }
        }
        prevStateA = stateA;
        prevStateB = stateB;
        wait_ns(100); // ロリコンここまで

        if (pc.readable()) // 足回り
        {
            char buf;
            pc.read(&buf, sizeof(buf));

            if (buf == 'W' || buf == 'w') // 前進
            {
                pwm[0] = 8000;
                pwm[1] = 8000;
                pwm[2] = 8000;
                pwm[3] = 8000;
                pwm1[1] = 8000;
            }
            else if (buf == 'S' || buf == 's') // 後進
            {
                pwm[0] = -8000;
                pwm[1] = -8000;
                pwm[2] = -8000;
                pwm[3] = -8000;
            }
            else if (buf == 'A' || buf == 'a') // 左回転
            {
                pwm[0] = -2000;
                pwm[1] = -2000;
                pwm[2] = 2000;
                pwm[3] = 2000;
            }
            else if (buf == 'D' || buf == 'd') // 右回転
            {
                pwm[0] = 2000;
                pwm[1] = 2000;
                pwm[2] = -2000;
                pwm[3] = -2000;
            }
            else if (buf == 'Q' || buf == 'q') // 微弱前進
            {
                pwm[0] = 4000;
                pwm[1] = 4000;
                pwm[2] = 4000;
                pwm[3] = 4000;
            }
            else if (buf == 'E' || buf == 'e') // 微弱後進
            {
                pwm[0] = -4000;
                pwm[1] = -4000;
                pwm[2] = -4000;
                pwm[3] = -4000;
            }
            else if (buf == 'V' || buf == 'v') // Vだっしゅ！！
            {
                pwm[0] = 15000;
                pwm[1] = 15000;
                pwm[2] = 15000;
                pwm[3] = 15000;
            } // 足回り

            else if (buf == 'Z' || buf == 'z')
            { // 停止
                pwm[0] = 0;
                pwm[1] = 0;
                pwm[2] = 0;
                pwm[3] = 0;
                pwm1[1] = 0;
            }

            if (buf == 'O')
            {
                dorayaki = 'a';
            }
            else if (buf == 'L')
            {
                dorayaki = 'b';
            }
            else if (buf == 'k')
            {
                gohan = 'b';
            }
            else if (buf == 'M')
            {
                tyuusyoku = 'b';
            }
            else if (buf == 'N')
            {
                tyuusyoku = 'a';
            }
            if (buf == 'y')
            {
                bekon = 'b';
            }
            else if (buf == 'h')
            {
                bekon = 'a';
            }

            // キーに対する処理がわったらCAN通信を行う
            // msg.id = 3;
            // msg.len = 8;

            // ThisThread::sleep_for(1000ms);
        }
        if (button1State == 1 || button2State == 1)
        {
            // if(koppepan=='a'){
            //     koppepan='b';
            //     osDelay(100);
            // }
            // else if(koppepan=='b'){
            //     koppepan='a';
            //     osDelay(100);
            // }
            pwm[1] = 1000;
        }

        if (dorayaki == 'a')
        {
            if (counter <= -3900)
            {
                pwm[0] = 0;
                pwm[1] = 0;
                pwm[2] = 0;
                pwm[3] = 0;
                pwm1[1] = 0;
            }
        }

        if (gohan == 'b')
        {
            counter = 0;
            gohan = 'a';
        }
        if (tyuusyoku == 'b')
        {
            if (counter > 0)
            {
                pwm[0] = 0;
                pwm[1] = 0;
                pwm[2] = 0;
                pwm[3] = 0;
                pwm1[1] = 0;
            }
        }
        // もしbekonがaならラックアンドピニオンをとめる。bなら動かす。
        if (bekon == 'b')
        {
            if (koppepan == 'a')
            {
                pwm[1] = 6000;
            }
            else if (koppepan == 'b')
            {
                pwm[1] = -6000;
            }
        }
        else if (bekon == 'a')
        {
            pwm[1] = 0;
        }

        CANMessage msg(1, (const uint8_t *)pwm, 8);
        CANMessage msg1(5, (const uint8_t *)pwm1, 8);

        can.write(msg);
        can.write(msg1);
        printf("counter: %d\n", counter);
    }
}
