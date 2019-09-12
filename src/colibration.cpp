#include <Arduino.h>
// Arduino sketch that returns calibration offsets for MPU6050 //   Version 1.1  (31th January 2014)
// Done by Luis Ródenas <luisrodenaslorda@gmail.com>
// Based on the I2Cdev library and previous work by Jeff Rowberg <jeff@rowberg.net>
// Updates (of the library) should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib

// These offsets were meant to calibrate MPU6050's internal DMP, but can be also useful for reading sensors.
// The effect of temperature has not been taken into account so I can't promise that it will work if you
// calibrate indoors and then use it outdoors. Best is to calibrate and use at the same room temperature.

// Эти смещения предназначались для калибровки внутреннего DMP MPU6050, 
// но также могут быть полезны для считывания датчиков. 
// Влияние температуры не было учтено, поэтому я не могу обещать, что она будет работать, если вы 
// откалибруете внутри помещения, а затем используете его на улице. 
// Лучше всего калибровать и использовать при той же комнатной температуре.

#include "I2Cdev.h"
#include "MPU6050.h"
#include "Wire.h"


// Измените эти 3 переменные, если вы хотите точно настроить scecth под ваши нужды.

int buffersize=1000;     // Количество показаний, используемых для усреднения, увеличьте его, 
                         //чтобы получить большую точность, но скетч будет медленнее (по умолчанию: 1000)
int acel_deadzone=8;     // Acelerolowed, сделать его ниже, чтобы получить большую точность, 
                         //но скетч может не сходиться (по умолчанию: 8)
int giro_deadzone=1;     // допускается ошибка Giro, уменьшите ее, чтобы получить большую точность, 
                         //но скетч может не сходиться (по умолчанию: 1)


// адрес I2C по умолчанию 0x68
// конкретные адреса I2C могут быть переданы в качестве параметра здесь
// AD0 low = 0x68 (по умолчанию для оценочной платы InvenSense)
// AD0 high = 0x69
// MPU6050 accelgyro;
MPU6050 accelgyro(0x68); // <- использовать для AD0 high

int16_t ax, ay, az, gx, gy, gz; //?  gx, gy, gz

int mean_ax,mean_ay,mean_az,mean_gx,mean_gy,mean_gz,state=0;
int ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset; // offset - смещение


void setup() {
  // подключаем шину I2C (библиотека I2Cdev не делает это автоматически)
  Wire.begin();
  // комментируйте СЛЕДУЮЩюю ЛИНию, ЕСЛИ ВЫ ИСПОЛЬЗУЕТЕ ARDUINO DUE
  TWBR = 24; // тактовая частота I2C 400 кГц (200 кГц, если процессор 8 МГц). Леонардо измерял 250 кГц.

  // инициализируем последовательную связь
  //Serial.begin(115200);
  Serial.begin(9600);

  // инициализируем устройство
  accelgyro.initialize();

  //   ждать готовности
  //   while (Serial.available() && Serial.read()); // пустой буфер
  //  while (!Serial.available()){
  //    Serial.println(F("Send any character to start sketch.\n")); //Отправите любой символ, чтобы начать скетч.
  //    delay(1500);
  //   }              
 //while (Serial.available() && Serial.read()); // снова опустошаем буфер

  // стартовое сообщение
  Serial.println("\nMPU6050 Calibration Sketch\nMPU6050 старт калибровки");
  delay(2000);

  Serial.println("\nYour MPU6050 should be placed in horizontal position, with package letters facing up. \nDon't touch it until you see a finish message.\n");
  Serial.println("следует размещать в горизонтальном положении, буквами пакета вверх");
  Serial.println("Не трогайте его, пока не увидите сообщение о завершении");
  delay(3000); 

  // проверяем соединение
  Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful\nMPU6050 связь установлена" : "MPU6050 connection failed\nMPU6050 связь потеряна" );
  delay(1000);

  // сбросить смещения
  accelgyro.setXAccelOffset(0);
  accelgyro.setYAccelOffset(0);
  accelgyro.setZAccelOffset(0);
  accelgyro.setXGyroOffset(0);
  accelgyro.setYGyroOffset(0);
  accelgyro.setZGyroOffset(0);
}

void meansensors(){
  long i=0,buff_ax=0,buff_ay=0,buff_az=0,buff_gx=0,buff_gy=0,buff_gz=0;

  while (i<(buffersize+101)){
    // считывание необработанных измерений ускорения / гироскопа с устройства
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
 
    if (i>100 && i<=(buffersize+100)){ // Первые 100 мер отбрасываются
      buff_ax=buff_ax+ax;
      buff_ay=buff_ay+ay;
      buff_az=buff_az+az;
      buff_gx=buff_gx+gx;
      buff_gy=buff_gy+gy;
      buff_gz=buff_gz+gz;
    }
    if (i==(buffersize+100)){
      mean_ax=buff_ax/buffersize;
      mean_ay=buff_ay/buffersize;
      mean_az=buff_az/buffersize;
      mean_gx=buff_gx/buffersize;
      mean_gy=buff_gy/buffersize;
      mean_gz=buff_gz/buffersize;
    }
    i++;
    delay(2); // Нужно, чтобы мы не получали повторные измерения
  }
}

void calibration(){
  ax_offset=-mean_ax/8;
  ay_offset=-mean_ay/8;
  az_offset=(16384-mean_az)/8;

  gx_offset=-mean_gx/4;
  gy_offset=-mean_gy/4;
  gz_offset=-mean_gz/4;
  while (1){
    int ready=0;
    accelgyro.setXAccelOffset(ax_offset);
    accelgyro.setYAccelOffset(ay_offset);
    accelgyro.setZAccelOffset(az_offset);

    accelgyro.setXGyroOffset(gx_offset);
    accelgyro.setYGyroOffset(gy_offset);
    accelgyro.setZGyroOffset(gz_offset);

    meansensors();
    Serial.println("...");

    if (abs(mean_ax)<=acel_deadzone) ready++;
    else ax_offset=ax_offset-mean_ax/acel_deadzone;

    if (abs(mean_ay)<=acel_deadzone) ready++;
    else ay_offset=ay_offset-mean_ay/acel_deadzone;

    if (abs(16384-mean_az)<=acel_deadzone) ready++;
    else az_offset=az_offset+(16384-mean_az)/acel_deadzone;

    if (abs(mean_gx)<=giro_deadzone) ready++;
    else gx_offset=gx_offset-mean_gx/(giro_deadzone+1);

    if (abs(mean_gy)<=giro_deadzone) ready++;
    else gy_offset=gy_offset-mean_gy/(giro_deadzone+1);

    if (abs(mean_gz)<=giro_deadzone) ready++;
    else gz_offset=gz_offset-mean_gz/(giro_deadzone+1);

    if (ready==6) break;
  }
}

void loop(){
    if (state==0){
    Serial.println("\nReading sensors for first time... \nСчитываем значение сенсоров в первый раз"); // считываем значение сенсоров в ресвый раз
    meansensors();
    state++;
    delay(1000);
  }

  if (state==1) {
    Serial.println("\nCalculating offsets... \nКалибровка"); //
    calibration();
    state++;
    delay(1000);
  }

  if (state==2) {
    meansensors();
    Serial.println("\nFINISHED! ФИНИШ!");
    Serial.print("\nSensor readings with offsets:\t <- Показания датчика со смещением");
    Serial.print(mean_ax);
    Serial.print("\t");
    Serial.print(mean_ay);
    Serial.print("\t");
    Serial.print(mean_az);
    Serial.print("\t");
    Serial.print(mean_gx);
    Serial.print("\t");
    Serial.print(mean_gy);
    Serial.print("\t");
    Serial.println(mean_gz);
    Serial.print(" Ваши показания. Your offsets:\t");
    Serial.print(ax_offset);
    Serial.print("\t");
    Serial.print(ay_offset);
    Serial.print("\t");
    Serial.print(az_offset);
    Serial.print("\t");
    Serial.print(gx_offset);
    Serial.print("\t");
    Serial.print(gy_offset);
    Serial.print("\t");
    Serial.println(gz_offset);

    Serial.println("\nData is printed as: acelX acelY acelZ giroX giroY giroZ");
    Serial.println("Данные печатаются как: acelX acelY acelZ giroX giroY giroZ"); 

    Serial.println("\nCheck that your sensor readings are close to 0 0 16384 0 0 0"); 
    Serial.println("Убедитесь, что показания вашего датчика близки к 0 0 16384 0 0 0"); 

    Serial.println("\nIf calibration was succesful write down your offsets so you can set them in your projects using something similar to mpu.setXAccelOffset(youroffset)");
    Serial.println("Если калибровка прошла успешно, запишите свои смещения, чтобы вы могли установить "); 
    Serial.println("их в своих проектах, используя что-то похожее на mpu.setXAccelOffset (youroffset)"); 

    while (1);
  }
}
