#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Custom_PID.h>
#include <Servo.h>


/*
* Elevon Configuration
* Roll + Pitch Axis Test with Adafruit BNO055 
* Requires Adafruit BNO055 Library and Adafruit Unified Sensor Library
*/

/* Set the delay between fresh samples */
#define BNO055_SAMPLERATE_DELAY_MS (100)

// Check I2C device address and correct line below (by default address is 0x29 or 0x28)
//                                   id, address
Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28);

Servo servoRight;
Servo servoLeft;

PID pidRoll(1, 0, 0);
PID pidPitch(1, 0, 0); 


void displaySensorDetails(void)
{
  sensor_t sensor;
  bno.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" xxx");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" xxx");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" xxx");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

/**************************************************************************/
/*
    Arduino setup function (automatically called at startup)
*/
/**************************************************************************/
void setup(void)
{
  Serial.begin(9600);
  Serial.println("Roll + Pitch Axis Test"); Serial.println("");

  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
   
  delay(1000);

  /* Use external crystal for better accuracy */
  bno.setExtCrystalUse(false);
   
  /* Display some basic information on this sensor */
  displaySensorDetails();

  servoRight.attach(9);
  servoLeft.attach(10);

  servoRight.write(90);
  servoLeft.write(90);

  pidRoll.Initialize();
  pidPitch.Initialize();
}

/**************************************************************************/
/*
    Arduino loop function, called once 'setup' is complete (your own code
    should go here)
*/
/**************************************************************************/
void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  bno.getEvent(&event);

  /* Board layout:
         +----------+
         |*         |   PITCH  ROLL  HEADING
     ADR |*         |
     INT |*         |     ^            /->
     PS1 |*         |     |            |
     PS0 |*         |     Y    X-->    \-Z
         |*         | 
         +----------+
  */

  /* heading, pitch, roll by Eulers Angle*/
  float yaw = 360 - (float)event.orientation.x, pitch = (float)event.orientation.y, roll = (float)event.orientation.z;
  Serial.print(F("Orientation: "));
  Serial.print(yaw);
  Serial.print(F(", "));
  Serial.print(pitch);
  Serial.print(F(", "));
  Serial.print(roll);
  Serial.println(F(""));

  /* roll, pitch, heading by Quaternion*/
//  imu::Quaternion quat = bno.getQuat();
//  
//  Serial.print(F("Quaternion: "));
//  Serial.print((float)quat.w());
//  Serial.print(F(", "));
//  Serial.print((float)quat.x());
//  Serial.print(F(", "));
//  Serial.print((float)quat.y());
//  Serial.print(F(", "));
//  Serial.print((float)quat.z());
//  Serial.println(F(""));

  /* PID */
  float updateAngleRoll = pidRoll.Update(roll, 10);
  float updateAnglePitch = pidPitch.Update(pitch, 10);
  servoRight.write(int(90 - updateAngleRoll - updateAnglePitch));
  servoLeft.write(int(90 - updateAngleRoll + updateAnglePitch));

  /* Also send calibration data for each sensor. */
  uint8_t sys, gyro, accel, mag = 0;
  bno.getCalibration(&sys, &gyro, &accel, &mag);
  Serial.print(F("Calibration: "));
  Serial.print(sys, DEC);
  Serial.print(F(", "));
  Serial.print(gyro, DEC);
  Serial.print(F(", "));
  Serial.print(accel, DEC);
  Serial.print(F(", "));
  Serial.print(mag, DEC);
  Serial.println(F(""));

  delay(BNO055_SAMPLERATE_DELAY_MS);
}
