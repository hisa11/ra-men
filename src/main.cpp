#include "mbed.h"

DigitalIn encoderA(A0); // A相のピン A0からA5
DigitalIn encoderB(A1); // B相のピン A0からA5
PwmOut my_servo(D3);    // サーボ
AnalogIn ain(A5);       // サーボ
int counter = 0;
bool prevStateA = 0;
bool prevStateB = 0;

BufferedSerial pc(USBTX, USBRX, 250000); // パソコンとのシリアル通信
CANMessage msg;
// pwm0[0]  左前タイヤ
// pwm0[1]  左後ろタイヤ
// pwm0[2]  右前タイヤ
// pwm0[3]  右後ろタイヤ
CAN can(PA_11, PA_12, (int)1e6);
int16_t pwm0[4] = {0, 0, 0, 0}; // モタドラ1
int16_t pwm1[4] = {0, 0, 0, 0}; // モダドラ2
int16_t pwm2[4] = {0, 0, 0, 0}; // モダドラ3

int16_t bekon = 0;     // ラックアンドピニオン制御
int16_t gohan = 0;     // ロリコンカウンターリセット
int16_t tyuusyoku = 1; // ロジャー安全装置（↓）
int16_t dorayaki = 0;  // ロジャー安全装置（↑）
int16_t koppepan = 0;  // ラックアンドピニオン方向

int16_t kukki = 2;  // タイヤ動作の有無
int16_t tamago = 0; // クローラー動作の有無

DigitalIn button1(D4); // 塗機構スイッチ1
DigitalIn button2(D5); // 塗機構スイッチ2

int main()
{
    button1.mode(PullUp);
    button2.mode(PullUp);
    while (1) // ここからロリコン
    {

        bool stateA = encoderA.read();
        bool stateB = encoderB.read();

        // ここからサーボ

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

        if (pc.readable()) // サーボ
        {
            char buf;
            if (buf == 'U') // 左
            {
                for (int i = 0; i < 200; i++)
                {
                    my_servo = i / 650.0;
                    ThisThread::sleep_for(20ms);
                }
            }
            if (buf == 'I') // 右
            {
                for (int i = 150; i > 0; i--)
                {
                    my_servo = i / 600.0;
                    ThisThread::sleep_for(20ms);
                }
            }

            if (buf == 'u') // 左
            {
                for (int i = 0; i < 200; i++)
                {
                    my_servo = i / 100.0;
                    ThisThread::sleep_for(20ms);
                }
            }
            if (buf == 'i') // 右
            {
                for (int i = 150; i > 0; i--)
                {
                    my_servo = i / 50.0;
                    ThisThread::sleep_for(20ms);
                }
            }
            }
            if (buf == '=')
            {
                kukki = 0;
            }
            else if (buf == '~')
            {
                kukki = 1;
            }
            else if (buf == '|')
            {
                kukki = 2;
            }

            if (buf == 'W' || buf == 'w') // 前進
            {
                if (kukki == 0)
                {
                    pwm0[0] = 16000;
                    pwm0[1] = 16000;
                    pwm0[2] = 16000;
                    pwm0[3] = 16000;
                    pwm1[3] = 13000;
                }
                else if (kukki == 2)
                {
                    pwm0[0] = -16000;
                    pwm0[1] = -16000;
                    pwm0[2] = -16000;
                    pwm0[3] = -16000;
                    pwm1[3] = -13000;
                }
            }
            else if (buf == 'S' || buf == 's') // 後進
            {
                if (kukki == 0)
                {
                    pwm0[0] = -16000;
                    pwm0[1] = -16000;
                    pwm0[2] = -16000;
                    pwm0[3] = -16000;
                    pwm1[3] = -13000;
                }
                else if (kukki == 2)
                {
                    pwm0[0] = 16000;
                    pwm0[1] = 16000;
                    pwm0[2] = 16000;
                    pwm0[3] = 16000;
                    pwm1[3] = 13000;
                }
            }
            else if (buf == 'a') // 左回転
            {
                if (kukki == 0)
                {
                    pwm0[0] = 12000;
                    pwm0[1] = 12000;
                    pwm0[2] = -12000;
                    pwm0[3] = -12000;
                }
                else if (kukki == 2)
                {
                    pwm0[0] = -12000;
                    pwm0[1] = -12000;
                    pwm0[2] = 12000;
                    pwm0[3] = 12000;
                }
            }
            else if (buf == 'd') // 右回転
            {
                if (kukki == 0)
                {
                    pwm0[0] = -12000;
                    pwm0[1] = -12000;
                    pwm0[2] = 12000;
                    pwm0[3] = 12000;
                }
                else if (kukki == 2)
                {
                    pwm0[0] = 12000;
                    pwm0[1] = 12000;
                    pwm0[2] = -12000;
                    pwm0[3] = -12000;
                }
            }
            else if (buf == 'q') // 微弱前進
            {
                if (kukki == 0)
                {
                    pwm0[0] = 8000;
                    pwm0[1] = 8000;
                    pwm0[2] = 8000;
                    pwm0[3] = 8000;
                    pwm1[3] = 6500;
                }
                else if (kukki == 2)
                {
                    pwm0[0] = -8000;
                    pwm0[1] = -8000;
                    pwm0[2] = -8000;
                    pwm0[3] = -8000;
                    pwm1[3] = -6500;
                }
            }
            else if (buf == 'e') // 微弱後進
            {
                if (kukki == 0)
                {
                    pwm0[0] = -8000;
                    pwm0[1] = -8000;
                    pwm0[2] = -8000;
                    pwm0[3] = -8000;
                    pwm1[3] = -6500;
                }
                else if (kukki == 0)
                {
                    pwm0[0] = 8000;
                    pwm0[1] = 8000;
                    pwm0[2] = 8000;
                    pwm0[3] = 8000;
                    pwm1[3] = 6500;
                }
            }
            else if (buf == 'V' || buf == 'v') // Vだっしゅ！！
            {
                if (kukki == 0)
                {
                    pwm0[0] = 27500;
                    pwm0[1] = 27500;
                    pwm0[2] = 27500;
                    pwm0[3] = 27500;
                    pwm1[3] = 8000;
                }
                else if (kukki == 0)
                {
                    pwm0[0] = 27500;
                    pwm0[1] = 27500;
                    pwm0[2] = 27500;
                    pwm0[3] = 27500;
                    pwm1[3] = 8000;
                }

            } // 足回り

            else if (buf == 'z' || buf == 'A' || buf == 'D')
            { // 足回り停止
                pwm0[0] = 0;
                pwm0[1] = 0;
                pwm0[2] = 0;
                pwm0[3] = 0;
                pwm1[3] = 0;
            }
            else if (buf == 'Z')
            { // 　全体停止
                pwm0[0] = 0;
                pwm0[1] = 0;
                pwm0[2] = 0;
                pwm0[3] = 0;

                pwm1[0] = 0;
                pwm1[1] = 0;
                pwm1[2] = 0;
                pwm1[3] = 0;

                pwm2[0] = 0;
                pwm2[1] = 0;
            }
            else if (buf == 'o')
            {
                pwm1[0] = -10000;
            }
            else if (buf == 'l')
            {
                pwm1[0] = 4000;
            }
            else if (buf == 'U') // ドラム手動　出す
            {
                pwm1[3] = 10000;
            }
            else if (buf == 'J')
            {
                pwm1[3] = -10000; // ドラム手動　巻き取る
            }
            else if (buf == 'M') // ドラム手動　停止
            {
                pwm1[3] = 0;
            }

            if (buf == 'O')
            {
                dorayaki = 0;
            }
            else if (buf == 'L')
            {
                dorayaki = 1;
            }
            else if (buf == 'k')
            {
                gohan = 1;
            }
            else if (buf == 'N')
            {
                tyuusyoku = 1;
            }
            else if (buf == 'B')
            {
                tyuusyoku = 0;
            }
            if (buf == 'y')
            {
                bekon = 1;
            }
            else if (buf == 'h')
            {
                bekon = 0;
            }

            // キーに対する処理がわったらCAN通信を行う
            // msg.id = 3;
            // msg.len = 8;

            // ThisThread::sleep_for(1000ms);
        }
        if (kukki == 1)
        {
            pwm0[0] = 0;
            pwm0[1] = 0;
            pwm0[2] = 0;
            pwm0[3] = 0;
        }
        if (tamago == 1)
        {
            pwm2[0] = 0;
            pwm2[1] = 0;
        }
        if (button1.read() == 0)
        {
            koppepan = 0;
        }
        if (button2.read() == 0)
        {
            koppepan = 1;
        }

        if (dorayaki == 0) // 安全装置
        {
            if (counter <= -3100)
            {
                pwm1[0] = 0;
            }
        }

        if (gohan == 1)
        {
            counter = 0;
            gohan = 0;
        }
        if (tyuusyoku == 1)
        {
            if (counter > 0) // 安全装置
            {
                pwm1[0] = 0;
            }
        }
        // もしbekonがaならラックアンドピニオンをとめる。bなら動かす。
        if (bekon == 1)
        {
            if (koppepan == 0)
            {
                pwm1[1] = 8000;
                pwm1[2] = 6000;
            }
            if (koppepan == 1)
            {
                pwm1[1] = -8000;
                pwm1[2] = 6000;
            }
        }
        else if (bekon == 0)
        {
            pwm1[1] = 0;
            pwm1[2] = 0;
        }

        CANMessage msg0(1, (const uint8_t *)pwm0, 8);
        CANMessage msg1(2, (const uint8_t *)pwm1, 8);
        CANMessage msg2(5, (const uint8_t *)pwm2, 8);

        can.write(msg0);
        can.write(msg1);
        can.write(msg2);
        printf("counter: %d\n", counter);
    }
}
