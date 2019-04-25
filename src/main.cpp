#include <Arduino.h>
#include <Encoder.h>

#define COUNT_LOW 0
#define COUNT_HIGH 1023
#define ENC_MAX 2000
#define SAMPLE_TIME 0.0000005
#define PULLY_R 1.0 //仮の数字

float currentX = 0.0f; //x軸の座標
float prevX = 0.0f;

int motor1a = 14; //モータ1正転
int motor1b = 22; //モータ1逆転
int enc1a = 26;   //モータ1エンコーダa相
int enc1b = 25;   //モータ1エンコーダb相

int motor2a = 32; //モータ1正転
int motor2b = 33; //モータ1逆転
int enc2a = 18;   //モータ1エンコーダa相
int enc2b = 19;   //モータ1エンコーダb相

int PWM_HZ = 40000;

Encoder myEnc1(enc1a,enc1b);
Encoder myEnc2(enc2a,enc2b);

void motorOut(int, float);
int SelectOutChannel(int, float);

void setup() {
  pinMode(motor1a,OUTPUT);
  pinMode(motor1b,OUTPUT);
  pinMode(motor2a,OUTPUT);
  pinMode(motor2b,OUTPUT);
  ledcSetup(1 ,PWM_HZ,10);
  ledcSetup(2,PWM_HZ,10);
  ledcSetup(3,PWM_HZ,10);
  ledcSetup(4,PWM_HZ,10);
  ledcAttachPin(motor1a,1);
  ledcAttachPin(motor1b,2);
  ledcAttachPin(motor2a,3);
  ledcAttachPin(motor2b,4);

 // Serial.begin(9600);
}

void loop() {
  motorOut(1,20000);
  motorOut(2,-20000);
}

/*
モータを動かす関数 
motor:動かすモータ番号，value:値
*/
void motorOut(int motor, float value){
  int ch1 = SelectOutChannel(motor, value);
  int ch2 = SelectOutChannel(motor, -value); //正負が逆転したvalueを与えることで対のチャンネルを得られる
  ledcWrite(ch1,constrain(fabs(value),COUNT_LOW,COUNT_HIGH));
  ledcWrite(ch2,0);
}

/*
出力に使うpwmの出力チャンネルを選ぶ関数
 */
int SelectOutChannel(int motor,float value){
  //正転のとき
  if(value >= 0){
    switch (motor)
    {
      case 1:
        return 1; //pwmの出力チャンネル
        break;
      case 2:
        return 3;
        break;
    
      default:
        return 0;
        break;
    }
  }

  //逆転のとき
  else{
    switch (motor)
    {
      case 1:
        return 2;
        break;
      case 2:
        return 4;
        break;
    
      default:
        return 0;
        break;
    }
  }
}