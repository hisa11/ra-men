#include "mbed.h"

BufferedSerial pc(USBTX, USBRX, 9600); // パソコンとのシリアル通信
int16_t A_1 = 0;                       // 左前タイヤ
int16_t B_1 = 0;                       // 左後ろタイヤ
int16_t C_1 = 0;                       // 右前タイヤ
int16_t D_1 = 0;                       // 右後ろタイヤ
CAN can{PB_12, PB_13, (int)1e6};       // 12と13に送る
int16_t pwm[4] = {A_1, B_1, C_1, D_1}; // pwm制御
CANMessage msg;

int main()
{
  while (1)
  {
    if (pc.readable())
    {
      char buf;
      pc.read(&buf, sizeof(buf));
      if (buf == 'W' || buf == 'w') // 前進
      {
        A_1 = 8000;
        B_1 = 8000;
        C_1 = 8000;
        D_1 = 8000;
        osDelay(50);
      }

      if (buf == 'S' || buf == 's') // 後進
      {
        A_1 = -8000;
        B_1 = -8000;
        C_1 = -8000;
        D_1 = -8000;
        osDelay(50);
      }

      if (buf == 'A' || buf == 'a') // 左回転
      {
        A_1 = -2000;
        B_1 = -2000;
        C_1 = 2000;
        D_1 = 2000;
        osDelay(50);

      }
      if (buf == 'A' || buf == 'a') // 右回転
      {
        A_1 = 2000;
        B_1 = 2000;
        C_1 = -2000;
        D_1 = -2000;
        osDelay(50);

      }
      if (buf == 'Q' || buf == 'q') // 微弱前進
      {
        A_1 = 2000;
        B_1 = 2000;
        C_1 = 2000;
        D_1 = 2000;
        osDelay(50);

      }
      if (buf == 'Q' || buf == 'q') // 微弱後進
      {
        A_1 = -2000;
        B_1 = -2000;
        C_1 = -2000;
        D_1 = -2000;
        osDelay(50);

      }
      if (buf == 'V' || buf == 'v') // Vだっしゅ！！
      {
        A_1 = 12000;
        B_1 = 12000;
        C_1 = 12000;
        D_1 = 12000;
        osDelay(50);

      }
      
      else{ // 無操作時は止める
        A_1 = 0;
        B_1 = 0;
        C_1 = 0;
        D_1 = 0;
        
      }
      // 他のキーに対する処理も追加可能

      // キーに対する処理が終わったらCAN通信を行う

      CANMessage msg(5, reinterpret_cast<uint8_t *>(pwm), 8);
      can.write(msg);
    }
  }
}