#include <Arduino.h>
#include <Encoder.h>

/* highspeed pwm test */


/* duty VARs for set duty later example by uart or wifi or other */
uint32_t dutyCycleR=0, dutyCycleG=0, dutyCycleB=0;


		 
// highspeed 
void pwm_highspeed_setup() 
{
  /* Enable LED PWM peripheral */
  SET_PERI_REG_MASK(DPORT_PERIP_CLK_EN_REG, DPORT_LEDC_CLK_EN);
  CLEAR_PERI_REG_MASK(DPORT_PERIP_RST_EN_REG, DPORT_LEDC_RST);

  /* set high timer*/ 
  WRITE_PERI_REG (LEDC_HSTIMER0_CONF_REG, (1<<25)|(0b000000001000000000<<5)|10);
 
  /* set high speed channel 2 */ 
  WRITE_PERI_REG (LEDC_HSCH2_CONF0_REG, (1<<3)|(1<<2));
  WRITE_PERI_REG (LEDC_HSCH2_HPOINT_REG, 0 );
  WRITE_PERI_REG (LEDC_HSCH2_DUTY_REG, 0);
  WRITE_PERI_REG (LEDC_HSCH2_CONF1_REG, (1<<31));
  
  /* set high speed channel 3 */
  WRITE_PERI_REG (LEDC_HSCH3_CONF0_REG, (1<<3)|(1<<2));
  WRITE_PERI_REG (LEDC_HSCH3_HPOINT_REG, 0 );
  WRITE_PERI_REG (LEDC_HSCH3_DUTY_REG, 0);
  WRITE_PERI_REG (LEDC_HSCH3_CONF1_REG, (1<<31));

  /* set high speed channel 4 */
  WRITE_PERI_REG (LEDC_HSCH4_CONF0_REG, (1<<3)|(1<<2));
  WRITE_PERI_REG (LEDC_HSCH4_HPOINT_REG, 0 );
  WRITE_PERI_REG (LEDC_HSCH4_DUTY_REG, 0);
  WRITE_PERI_REG (LEDC_HSCH4_CONF1_REG, (1<<31));
  
  // R= 0  G= 2  B= 4
  /* set gpio out enable */
  WRITE_PERI_REG (GPIO_ENABLE_REG, (1<<0)|(1<<2)|(1<<4));
  
  
  /* set LEDC_HS_SIG_OUT to the Green, Blue, Red gpio */ 
  // info: LEDC_HS_SIG_OUT0_IDX is 71
  // https://github.com/espressif/esp-idf/blob/master/components/esp32/include/soc/gpio_sig_map.h#L154
  // setting is SIG_OUT + HighSpeedChannel here in the test 2, 3, and 4
  WRITE_PERI_REG (GPIO_FUNC2_OUT_SEL_CFG_REG, (1<<10)|73);  // G 71 + cn 2    
  WRITE_PERI_REG (GPIO_FUNC4_OUT_SEL_CFG_REG, (1<<10)|74);  // B 71 + cn 3     
  WRITE_PERI_REG (GPIO_FUNC0_OUT_SEL_CFG_REG, (1<<10)|75);  // R 71 + cn 4    

  
  /* GPIO MUX */ 
  WRITE_PERI_REG (PERIPHS_IO_MUX_GPIO2_U, (3<<10)|(1<<8));
  WRITE_PERI_REG (PERIPHS_IO_MUX_GPIO4_U, (3<<10)|(1<<8));
  WRITE_PERI_REG (PERIPHS_IO_MUX_GPIO0_U, (3<<10)|(1<<8));
  
}


// highspeed
void pwm_highspeed_update()
{
  
  /*chn-2 update duty green led */
  WRITE_PERI_REG (LEDC_HSCH2_DUTY_REG, (dutyCycleG * DUTY_CYCLE_MAX / 100) << 4);
  WRITE_PERI_REG (LEDC_HSCH2_CONF1_REG, (1<<31));
  
  /*chn-3 update blue led */
  WRITE_PERI_REG (LEDC_HSCH3_DUTY_REG, (dutyCycleB * DUTY_CYCLE_MAX / 100) << 4);
  WRITE_PERI_REG (LEDC_HSCH3_CONF1_REG, (1<<31));

  /*chn-4 update red led */
  WRITE_PERI_REG (LEDC_HSCH4_DUTY_REG, (dutyCycleR * DUTY_CYCLE_MAX / 100) << 4);
  WRITE_PERI_REG (LEDC_HSCH4_CONF1_REG, (1<<31));
  
}


#define COUNT_LOW 0
#define COUNT_HIGH 65535
#define ENC_MAX 2000
#define SAMPLE_TIME 0.0000005
#define PULLY_R 1.0 //仮の数字

float currentX = 0.0f; //x軸の座標
float prevX = 0.0f;

int motor1a = 21; //モータ1正転
int motor1b = 22; //モータ1逆転
int enc1a = 26;   //モータ1エンコーダa相
int enc1b = 25;   //モータ1エンコーダb相

int motor2a = 32; //モータ1正転
int motor2b = 33; //モータ1逆転
int enc2a = 18;   //モータ1エンコーダa相
int enc2b = 19;   //モータ1エンコーダb相

int PWM_HZ = 100;

Encoder myEnc1(enc1a,enc1b);
Encoder myEnc2(enc2a,enc2b);

void motorOut(int, float);
int SelectOutChannel(int, float);

void setup() {
  pwm_highspeed_setup();
  pinMode(motor1a,OUTPUT);
  pinMode(motor1b,OUTPUT);
  pinMode(motor2a,OUTPUT);
  pinMode(motor2b,OUTPUT);
  ledcSetup(1 ,PWM_HZ,16);
  ledcSetup(2,PWM_HZ,16);
  ledcSetup(3,PWM_HZ,16);
  ledcSetup(4,PWM_HZ,16);
  ledcAttachPin(motor1a,1);
  ledcAttachPin(motor1b,2);
  ledcAttachPin(motor2a,3);
  ledcAttachPin(motor2b,4);
  pwm_highspeed_update();
  Serial.begin(9600);
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