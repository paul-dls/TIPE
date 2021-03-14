  #include "I2Cdev.h"
  #include "MPU6050_6Axis_MotionApps20.h"
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  #include "Wire.h"
  #endif
  
  MPU6050 mpu;
  #define OUTPUT_TEAPOT
  #define LED_PIN 13
  bool blinkState = false;
  bool dmpReady = false;
  uint8_t mpuIntStatus;
  uint8_t devStatus;
  uint16_t packetSize;
  uint16_t fifoCount;
  uint8_t fifoBuffer[64];
  Quaternion q;
  VectorInt16 aa;
  VectorInt16 aaReal;
  VectorInt16 aaWorld;
  VectorFloat gravity;
  float euler[3];
  float ypr[3];
  uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };
  volatile bool mpuInterrupt = false;
  void dmpDataReady() {
  mpuInterrupt = true;
  }
  
  void setup() {
  
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  TWBR = 24;
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
  #endif
  
  Serial.begin(38400);
  while (!Serial);
  mpu.initialize();
  devStatus = mpu.dmpInitialize();
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);
  
  if (devStatus == 0) {
  mpu.setDMPEnabled(true);
  attachInterrupt(0, dmpDataReady, RISING);
  mpuIntStatus = mpu.getIntStatus();
  dmpReady = true;
  packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
  Serial.print(F("DMP Initialization failed (code "));
  Serial.print(devStatus);
  Serial.println(F(")"));
  }
  pinMode(LED_PIN, OUTPUT);
  }
  
  void loop() {
  
  if (!dmpReady) return;
  
  while (!mpuInterrupt && fifoCount < packetSize) {
  // other program behavior stuff here
  }
  
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();
  fifoCount = mpu.getFIFOCount();
  
  if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
  mpu.resetFIFO();
  Serial.println(F("FIFO overflow!"));
  } else if (mpuIntStatus & 0x02) {
  
  while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
  mpu.getFIFOBytes(fifoBuffer, packetSize);
  fifoCount -= packetSize;
  
  #ifdef OUTPUT_TEAPOT
  teapotPacket[2] = fifoBuffer[0];
  teapotPacket[3] = fifoBuffer[1];
  teapotPacket[4] = fifoBuffer[4];
  teapotPacket[5] = fifoBuffer[5];
  teapotPacket[6] = fifoBuffer[8];
  teapotPacket[7] = fifoBuffer[9];
  teapotPacket[8] = fifoBuffer[12];
  teapotPacket[9] = fifoBuffer[13];
  Serial.write(teapotPacket, 14);
  teapotPacket[11]++;
  #endif
  
  blinkState = !blinkState;
  digitalWrite(LED_PIN, blinkState);
  }
  }
