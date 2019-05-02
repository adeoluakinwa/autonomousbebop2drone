/*
 * RoboPeak RPLIDAR Arduino Example
 * This example shows the easy and common way to fetch data from an RPLIDAR
 * 
 * You may freely add your application code based on this template
 *
 * USAGE:
 * ---------------------------------
 * 1. Download this sketch code to your Arduino board
 * 2. Connect the RPLIDAR's serial port (RX/TX/GND) to your Arduino board (Pin 0 and Pin1)
 * 3. Connect the RPLIDAR's motor ctrl pin to the Arduino board pin 3 
 */
 
/* 
 * Copyright (c) 2014, RoboPeak 
 * All rights reserved.
 * RoboPeak.com
 *
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR 
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
// This sketch code is based on the RPLIDAR driver library provided by RoboPeak
#include <RPLidar.h>
#include <ArduinoJson.h>

// You need to create an driver instance 
RPLidar lidar;

#define RPLIDAR_MOTOR 3 // The PWM pin for control the speed of RPLIDAR's motor.
                        // This pin should connected with the RPLIDAR's MOTOCTRL signal 
                       
                        
void setup() {
  Serial.begin(9600);
  Serial1.begin(57600);
  Serial2.begin(9600);
  while(!Serial1);
  // bind the RPLIDAR driver to the arduino hardware serial
  lidar.begin(Serial1);
  // set pin modes
  pinMode(RPLIDAR_MOTOR, OUTPUT);
}

const size_t capacity = JSON_OBJECT_SIZE(5);
DynamicJsonDocument sample(capacity);
//StaticJsonDocument<300> sample;
//set 4000 as the max distance ahead
float min_left = 100000;
float min_right = 100000;
float min_ahead = 100000;
float max_left = 0;
float max_right = 0;
int counter = 0;

void loop() 
{
  if (IS_OK(lidar.waitPoint())) 
  {
    float distance = lidar.getCurrentPoint().distance; //distance value in mm unit
    float angle    = lidar.getCurrentPoint().angle; //anglue value in degree
    bool  startBit = lidar.getCurrentPoint().startBit; //whether this point is belong to a new scan
    byte  quality  = lidar.getCurrentPoint().quality; //quality of the current measurement
    
    //perform data processing here...
    if(angle >= 90 && angle < 140)
    {
      if(distance > 200 && distance < min_left)
       {
          min_left = distance;
       }
       if(distance > max_left)
       {
          max_left = distance;
       }
    }
    else if(angle >= 140 && angle < 230)
    {
      if(distance > 200 && distance < min_ahead)
       {
        min_ahead = distance;
       }
    }
    else if(angle >= 230 && angle <= 270)
    {
      if(distance > 200 && distance < min_right)
       {
        min_right = distance;
       }
       if(distance > max_right)
       {
        max_right = distance;
       }
    }

    
    if(startBit == true) //starting a new scan
    {
      counter++;
      sample["min_ahead"] = min_ahead;
      min_ahead = 100000;
      sample["min_left"] = min_left;
      min_left = 100000;
      sample["min_right"] = min_right;
      min_right = 100000;
      sample["max_left"] = max_left;
      max_left = 0;
      sample["max_right"] = max_right;
      max_right = 0;
      if(counter == 10)
      {
        serializeJson(sample, Serial2);
        serializeJson(sample, Serial);
        Serial.println();
        counter=0;
      }
      //serializeJson(sample, Serial);
      //Serial.println();
     }
  }
  else 
  {
    analogWrite(RPLIDAR_MOTOR, 0); //stop the rplidar motor
    // try to detect RPLIDAR... 
    rplidar_response_device_info_t info;
    if (IS_OK(lidar.getDeviceInfo(info, 100))) {
      // detected...
      lidar.startScan();
      // start motor rotating at max allowed speed
      analogWrite(RPLIDAR_MOTOR, 128);
      delay(1000);
    }
  }
}
