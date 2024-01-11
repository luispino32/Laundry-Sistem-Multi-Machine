void TaskSerial();
void TaskS_Alarma();
void TaskMessageDisplay();

void TaskPurga(){
  if(auxInfoLav_pruga[0] || auxInfoLav_pruga[1] || auxInfoLav_pruga[2] || auxInfoLav_pruga[3]){
    auxFlowSensorPurga = true;

    for(byte i=0; i<4; i++) purgaValues[i] = auxInfoLav_pruga[i];

    //Serial.println("purgaValues: " + String(purgaValues[0]) + ", "  + String(purgaValues[1]) + ", "  + String(purgaValues[2]) + ", "  + String(purgaValues[3]));
    
    portPump = (reciveInfoEsp("Ports/Pump")).toInt();
    portProbeta2 = (reciveInfoEsp("Ports/LvProb2")).toInt();
    portProbeta1 = (reciveInfoEsp("Ports/LvProb1")).toInt();
    portFlowSensor = (reciveInfoEsp("Ports/FlowSens")).toInt();
    portProduct = purgaValues[0] <= 4 ? (reciveInfoEsp("Ports/P" + String(purgaValues[0]))).toInt() : (reciveInfoEsp("Ports/FlushProb")).toInt();
    
    //Serial.println("Puerto sensor: " + String(portFlowSensor));
    //Serial.println("Puerto Pump: " + String(portPump));
    //Serial.println("Puerto Producto: " + String(portProduct));
    
    if(purgaValues[3] == 1){  
      //Serial.println("digitalWrite(" + String(portProduct) + ", Rele_ON)");
      digitalWrite(portProduct, Rele_ON);
      if(purgaValues[0] <= 4) digitalWrite(portPump, Rele_ON);
    }

    timePurgaAux = millis();
    timeSensorAux = millis();

    if(purgaValues[3] == 1){
      do{
        if(digitalRead(portFlowSensor) == HIGH && auxFlowSensorPurga){
          Rv_Sensor++;
          auxFlowSensorPurga = false;
          //Serial.println("aumenta Rv:" + String(Rv_Sensor));
        }else if(digitalRead(portFlowSensor) == LOW){
          auxFlowSensorPurga = true;
        }
    
        if(millis() - timeSensorAux > 4000){
          //Serial.println("Rv Sensor: " + String(Rv_Sensor) + ", time: " + String((millis()-timePurgaAux)/1000));
          Serial3.println("Mqtt:In/ControlPanel*rv:" + String(Rv_Sensor) + ", time:" + String((millis()-timePurgaAux)/1000));
          timeSensorAux = millis();
        }
    
        if((millis() - timePurgaAux > purgaValues[1]*1000) || 
            (purgaValues[2] == 1 && portProbeta1 == TUBE_SENSOR_1_ON) ||
            (purgaValues[2] == 2 && portProbeta2 == TUBE_SENSOR_2_ON)){
          digitalWrite(portProduct, Rele_OFF);
          digitalWrite(portPump, Rele_OFF);
          Serial3.println("Mqtt:In/ControlPanel*rv:" + String(Rv_Sensor) + ", time:" + String((millis()-timePurgaAux)/1000));
    
          systemOk = true;
          for(byte i=0; i<4; i++) auxInfoLav_pruga[i] = 0;
        }

        TaskSerial();
        TaskS_Alarma();
        TaskMessageDisplay();
      }while(auxInfoLav_pruga[0] || auxInfoLav_pruga[1] || auxInfoLav_pruga[2] || auxInfoLav_pruga[3]);
    }else{
      Serial.println("Inicia flush");
      Serial3.println("Mqtt:In/ControlPanel*Flush/" + 
                      String(purgaValues[0]) + "/" + 
                      String(flushOnMachine(purgaValues[0], 
                                            purgaValues[2] > 0 || purgaValues[2] <= 2, 
                                            purgaValues[2] == 1 ? portProbeta1 : portProbeta2, 
                                            purgaValues[1], 
                                            purgaValues[2] == 1 ? TUBE_SENSOR_1_ON : TUBE_SENSOR_2_ON,
                                            false,
                                            1)));
      Serial.println("Termina flush");
      systemOk = true;
      
      for(byte i=0; i<4; i++) auxInfoLav_pruga[i] = 0;
    }
  }

}