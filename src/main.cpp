#include <Arduino.h>
#include<EEPROM.h>

#include "Globales.h"
#include "functions_All.h"
#include "TaskPurga.h"
#include "TaskSerial.h"
#include "Task_Alarma.h"
#include "TaskDosifica.h"
#include "TaskMessageDisplay.h"

void setup(){
  Serial.begin(9600);
  Serial3.begin(9600);

  initProgramm();
  pinMode(EEPROM.read(0), OUTPUT); digitalWrite(EEPROM.read(0), Rele_OFF);
  pinMode(EEPROM.read(1), OUTPUT); digitalWrite(EEPROM.read(1), Rele_OFF);
  pinMode(EEPROM.read(2), OUTPUT); digitalWrite(EEPROM.read(2), Rele_OFF);
  pinMode(EEPROM.read(3), OUTPUT); digitalWrite(EEPROM.read(3), Rele_OFF);
  
  pinMode(EEPROM.read(4), OUTPUT); digitalWrite(EEPROM.read(4), Rele_OFF);
  pinMode(EEPROM.read(5), OUTPUT); digitalWrite(EEPROM.read(5), Rele_OFF);
  pinMode(EEPROM.read(6), OUTPUT); digitalWrite(EEPROM.read(6), Rele_OFF);
  pinMode(EEPROM.read(7), OUTPUT); digitalWrite(EEPROM.read(7), Rele_OFF);
  //pinMode(EEPROM_Datos.getUChar("Lv5", 2), OUTPUT); digitalWrite(EEPROM_Datos.getUChar("Lv5"), Rele_OFF);
  //pinMode(EEPROM_Datos.getUChar("Lv6", 4), OUTPUT); digitalWrite(EEPROM_Datos.getUChar("Lv6"), Rele_OFF);

  pinMode(EEPROM.read(10), OUTPUT); digitalWrite(EEPROM.read(10), Rele_OFF);
  pinMode(EEPROM.read(11), OUTPUT); digitalWrite(EEPROM.read(11), Rele_OFF);
    
  pinMode(EEPROM.read(12), OUTPUT); digitalWrite(EEPROM.read(12), Rele_OFF);
  pinMode(EEPROM.read(13), OUTPUT); digitalWrite(EEPROM.read(13), ReleModule_OFF);

  pinMode(EEPROM.read(14), OUTPUT); digitalWrite(EEPROM.read(14), Rele_OFF);
  pinMode(EEPROM.read(15), OUTPUT); digitalWrite(EEPROM.read(15), Rele_OFF); //i2c

  pinMode(EEPROM.read(16), INPUT);
  pinMode(EEPROM.read(17), INPUT_PULLUP);
  pinMode(EEPROM.read(18), INPUT_PULLUP);
  
  for(byte i=0; i<6; i++) 
    NumDosificaReceive[i][0] = 0;

  for(byte j=0; j<40; j++){
    infoControl[j][0] = 0;
    infoControl[j][1] = 0;
    infoControl[j][2] = 0;
    infoControl[j][3] = 0;
    infoControl[j][4] = 0;
  }

  timeTaskMessageDisplay = millis();
}

void loop() {
  TaskSerial();
  TaskDosifica();
  TaskMessageDisplay();

  if(AlamrOn){
    if(!alarmaOn_2){
      auxAlarmaInter = millis();
      stateAlarmaAux = false;

      portAlarma = (reciveInfoEsp("Ports/Alarm")).toInt(); 
      delayAlarmaInterval = (reciveInfoEsp("Flush/delayAlarm")).toInt(); 
      timeAlarma = (reciveInfoEsp("Flush/timeAlarm")).toInt(); 

      digitalWrite(portAlarma, Alarm_ON);
      alarmaOn_2 = true;

      delayTimeAlarma = millis();
    }else if(millis() - delayTimeAlarma > (timeAlarma*1000)){
      digitalWrite(portAlarma, Alarm_OFF);
      alarmaOn_2 = false;
      AlamrOn = false;
    }else{
      TaskS_Alarma();
    }
  }
}
