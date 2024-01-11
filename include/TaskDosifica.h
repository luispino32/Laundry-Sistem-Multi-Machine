#define OnProbetaSens_1 HIGH
#define OFFProbetaSens_1 LOW

#define OnProbetaSens_2 HIGH
#define OFFProbetaSens_2 LOW

void TaskSerial();
void TaskMessageDisplay();

void TaskDosifica(){
  if(!Dosificando && NLavsPendientes > 0 && systemOk){
    tubeSensor1_aux = true;
    Dosificando_2 = false;
    auxFlowSensor = false;
    Dosificando = true; 
    
    PunteroLavados[0] = LavsPendientes[0][0];
    PunteroLavados[1] = LavsPendientes[0][1];
    PunteroLavados[2] = LavsPendientes[0][2];
    Serial.println("LavsPendientes[NLavsPendientes-1][0] : " + String(LavsPendientes[NLavsPendientes-1][0]));
    Serial.println("LavsPendientes[NLavsPendientes-1][1] : " + String(LavsPendientes[NLavsPendientes-1][1]));
    Serial.println("LavsPendientes[NLavsPendientes-1][2] : " + String(LavsPendientes[NLavsPendientes-1][2]));
    Serial.println("PunteroLavados[0] : " + String(PunteroLavados[0]));
    Serial.println("PunteroLavados[1] : " + String(PunteroLavados[1]));
    Serial.println("PunteroLavados[2] : " + String(PunteroLavados[2]));

    NLavsPendientes--;

    if(NLavsPendientes > 0){
      for(int i=0; i<NLavsPendientes; i++){
        LavsPendientes[i][0] = LavsPendientes[i+1][0];
        LavsPendientes[i][1] = LavsPendientes[i+1][1];
        LavsPendientes[i][2] = LavsPendientes[i+1][2];
      }
    }

    kgLav = (reciveInfoEsp("Lavs/Kg_" + String(PunteroLavados[0]))).toInt();
    
    bool stateLav = (reciveInfoEsp("Lavs/Lv_" + String(PunteroLavados[0]))).toInt();
    bool stateFormLav = (reciveInfoEsp("Lavs/F" + String(PunteroLavados[1]) + "Lv" + String(PunteroLavados[0]))).toInt();
    
    NumFlush = (reciveInfoEsp("Config/NumFLush")).toInt();
    displayLDC = (reciveInfoEsp("Config/display")).toInt();
    tubeSensor1 = (reciveInfoEsp("Config/tubeSensor1")).toInt();
    tubeSensor2 = (reciveInfoEsp("Config/tubeSensor2")).toInt();
    multiplo_time_max[0] = (reciveInfoEsp("Config/mulTimeFlow")).toInt();
    multiplo_time_max[1] = (reciveInfoEsp("Config/mulTimeProb")).toInt();
    
    bool stateForm = (reciveInfoEsp("Form/F" + String(PunteroLavados[1]))).toInt();
    
    numProduct = (reciveInfoEsp("Product/N_Product")).toInt();
    timeFlushProb = (reciveInfoEsp("Flush/timeFlushProb")).toInt();
    timeExtraFlush_1 = (reciveInfoEsp("Flush/extraFlush_1")).toInt();
    timeExtraFlush_2 = (reciveInfoEsp("Flush/extraFlush_2")).toInt();
    portPump = (reciveInfoEsp("Ports/Pump")).toInt();
    portsSensor[2] = (reciveInfoEsp("Ports/LvProb2")).toInt();
    portsSensor[1] = (reciveInfoEsp("Ports/LvProb1")).toInt();
    portsSensor[0] = (reciveInfoEsp("Ports/FlowSens")).toInt(); // 32
    
    if(stateLav && stateFormLav && stateForm){
      CountLav = Registro_De_Lavado(PunteroLavados[0], PunteroLavados[1], PunteroLavados[2], displayLDC);
      Serial.println("valor Registro_De_Lavado: " + String(CountLav));

      if(CountLav <= 0){ //error -> activa alarma
        Dosificando = false;
        return;
      }
      auxNumFlush = 0;
    }else{ //Envio info a base de datos
      if(displayLDC) Serial3.println("Mqtt:Out/MasterControl*Error/Dosificacion cancelada -> lv:" + String(PunteroLavados[0]) + "/" +  
                                    String(stateLav) + ",F:" + String(PunteroLavados[1]) + "/" + String(stateForm) 
                                    + "-" + String(stateFormLav) + ",P:" + String(PunteroLavados[2]));
      Dosificando = false;
    }
    varDosifica = 0;
    DatoMQTT = "";
    if(displayLDC) Serial3.println("Mqtt:Out/MasterControl*Dosificacion/inicio -> lv:" + String(PunteroLavados[0]) + 
                                   ",F:" + String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2]));
  }else if(Dosificando){
    do{
      if(varDosifica <= numProduct && !Dosificando_2){
        varDosifica++;

        if(tubeSensor1 && (digitalRead(portsSensor[1]) == TUBE_SENSOR_1_ON)){
          Serial.println("valciando probeta para dosificacion producto: " + String(varDosifica));

          if(displayLDC) Serial3.println("Mqtt:Out/MasterControl*Dosificacion/vaciando probeta -> lv:" + String(PunteroLavados[0]) + 
                                        ",F:" + String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2]));
                                        
          if(!flushOnMachine(PunteroLavados[0], tubeSensor1, portsSensor[1], timeExtraFlush_1, TUBE_SENSOR_1_ON)){
            AlamrOn = true;
            Dosificando = false;
            varDosifica = 10;
              
            if(displayLDC) //envia error por mqtt
              Serial3.println("Mqtt:Out/MasterControl*Error/Sin presion en probeta 1(start) -> lv:" + 
                            String(PunteroLavados[0]) + ",F:" + String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2]));
          }    
        }
          
        if(varDosifica <= numProduct){
          tubeSensor1_aux = true;
          ml_X_kg = atof((reciveInfoEsp("Form/F" + String(PunteroLavados[1]) + "S" + String(PunteroLavados[2]) + "P" + String(varDosifica))).c_str());

          if(ml_X_kg > 0){
            Calibrate[0] = (reciveInfoEsp("Product/CalTime_" + String(varDosifica))).toInt();
            Calibrate[1] = (reciveInfoEsp("Product/CalMl_" + String(varDosifica))).toInt();
            Calibrate[2] = (reciveInfoEsp("Product/CalRv_" + String(varDosifica))).toInt();
                
            bool stateProduct = (reciveInfoEsp("Product/state_" + String(varDosifica))).toInt();

            if(stateProduct){
              Cantidad_dosis = round(ml_X_kg * kgLav);
              Serial.println("Cantidad_dosis: " + String(Cantidad_dosis));

              rv_dosis = Calibrate[1] > 0 ? (Cantidad_dosis*Calibrate[2])/Calibrate[1] : 0;
              Tiempo_dosis = Calibrate[1] > 0 ? (Cantidad_dosis*Calibrate[0])/Calibrate[1] : 0;
              portProduct = (reciveInfoEsp("Ports/P" + String(varDosifica))).toInt();

              Tiempo_dosis = Tiempo_dosis*1000;

              Serial.println(String(Cantidad_dosis) + "*" + String(Calibrate[0]) + "/" + String(Calibrate[1]));

              flowSensor = (reciveInfoEsp("Config/flowSensor_" + String(varDosifica))).toInt();
              topeProbeta[0] = (reciveInfoEsp("Config/topeProb1_" + String(varDosifica))).toInt();
              topeProbeta[1] = (reciveInfoEsp("Config/topeProb2_" + String(varDosifica))).toInt();
    
              Dosificando_2 = true;
              Rv_Sensor = 0;

              Serial.println("Pump:" + String(varDosifica) + ", PortProduct:" + String(portProduct) + ", PortPump:" + String(portPump)
                            + ", time:" + String(Tiempo_dosis));
              
              Serial.println("Activa producto " + String(varDosifica) + " y mono-Bomba");

              digitalWrite(portProduct, Rele_ON);
              digitalWrite(portPump, Rele_ON);
              TiempoDosificacion = millis();
              if(displayLDC) Serial3.println("Mqtt:Out/MasterControl*Dosificacion/progreso -> lv:" + String(PunteroLavados[0]) + 
                                            ",F:" + String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2])) + "/Quim:" +
                                            String(varDosifica) + "/" + String(Cantidad_dosis) + "ml/" + String(Tiempo_dosis) + 
                                            "ms/" + String(rv_dosis) + "rv";
            }
          }
        }
      }else if(Dosificando_2){
        if(tubeSensor2 && (digitalRead(portsSensor[2]) == TUBE_SENSOR_2_ON)){
          Serial.println("entro en tubo sensor: " + String(portsSensor[2]) + " - " + String(digitalRead(portsSensor[2])));
          Serial.println("DesActiva producto " + String(varDosifica) + " y mono-Bomba");

          digitalWrite(portPump, Rele_OFF);
          digitalWrite(portProduct, Rele_OFF);
          DatoMQTT = DatoMQTT + "/" + String(varDosifica) + "-" + String(millis()-TiempoDosificacion) + "-" + String(Rv_Sensor) + "-" + String(topeProbeta[1]) + "-ErrorTope";
          
          Serial.println("incia flush para mandar producto");
          if(!flushOnMachine(PunteroLavados[0], tubeSensor2, portsSensor[2], 0, TUBE_SENSOR_2_ON)){
            AlamrOn = true;
            Dosificando = false;
            varDosifica = 10;
              
            if(displayLDC) //envia error por mqtt
              Serial3.println("Mqtt:Out/MasterControl*Error/Sin presion en probeta 1(Clean) -> lv:" + 
                              String(PunteroLavados[0]) + ",F:" + String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2]));
          }
          
          Dosificando_2 = false;
        }else{
          if(digitalRead(portsSensor[0]) == HIGH && auxFlowSensor){
            Rv_Sensor++;
            auxFlowSensor = false;
          }else if(digitalRead(portsSensor[0]) == LOW){
            auxFlowSensor = true;
          }
          
          if((tubeSensor1 && (digitalRead(portsSensor[1]) == TUBE_SENSOR_1_ON)) || !tubeSensor1 || !tubeSensor1_aux){
            /*if(tubeSensor1_aux && tubeSensor1){
              if(Cantidad_dosis > topeProbeta[0]){
                Tiempo_dosis = Calibrate[1] > 0 ? ((Cantidad_dosis-topeProbeta[0])*Calibrate[0])/Calibrate[1] : 0;
                TiempoDosificacion = millis();

                Rv_Sensor = rv_dosis - round((topeProbeta[0]*rv_dosis)/Cantidad_dosis);
              }

              tubeSensor1_aux = false;
            }*/
        
            if(flowSensor){
              if(Rv_Sensor >= rv_dosis || (millis() - TiempoDosificacion > (Tiempo_dosis*multiplo_time_max[0]))){ 
                Serial.println("Termina flow sensor");
                Serial.println("DesActiva producto " + String(varDosifica) + " y mono-Bomba");

                digitalWrite(portPump, Rele_OFF);   
                digitalWrite(portProduct, Rele_OFF);  
                DatoMQTT = DatoMQTT + "/" + String(varDosifica) + "-" + String(millis() - TiempoDosificacion) + "-" + String(Rv_Sensor) + "-" + String(Cantidad_dosis);

                if(Rv_Sensor < rv_dosis){
                  DatoMQTT = DatoMQTT + "-ErrorTimeOver_1";
                  AlamrOn = true;
                  if(displayLDC) //envia error por mqtt
                    Serial3.println("Mqtt:Out/MasterControl*Error/Sin producto -> lv:" + String(PunteroLavados[0]) + ",F:" + 
                                    String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2]) + "/Quim:" + String(varDosifica) + 
                                    "/" + String(Cantidad_dosis) + "ml/" + String(millis() - TiempoDosificacion) + "ms/" + String(Rv_Sensor) + "rv");
                }else{
                  Serial3.println("Mqtt:Out/MasterControl*Dosificacion/completa -> Lv:" + String(PunteroLavados[0]) + ",F:" + String(PunteroLavados[1]) + 
                                ",P:" + String(PunteroLavados[2]) + "/Quim:" + String(varDosifica) + "/" + String(Cantidad_dosis) + "ml/" + 
                                String(millis() - TiempoDosificacion) + "ms/" + String(Rv_Sensor) + "rv");
                }

                Serial.println("incia flush para mandar producto");
                Dosificando_2 = false;
                if(!flushOnMachine(PunteroLavados[0], tubeSensor1, portsSensor[1], 0, TUBE_SENSOR_1_ON)){
                  AlamrOn = true;
                  Dosificando = false;
                  varDosifica = 10;
                  if(displayLDC) //envia error por mqtt
                    Serial3.println("Mqtt:Out/MasterControl*Error/Sin presion en probeta 1(Clean) -> lv:" + 
                                  String(PunteroLavados[0]) + ",F:" + String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2]));
                } 
              }
            }else if(millis() - TiempoDosificacion > Tiempo_dosis){
              Serial.println("Termina tiempo dosis: " + String(Tiempo_dosis));
              Serial.println("DesActiva producto " + String(varDosifica) + " y mono-Bomba");

              digitalWrite(portPump, Rele_OFF);   
              digitalWrite(portProduct, Rele_OFF);  
              DatoMQTT = DatoMQTT + String(varDosifica) + "-" + String(millis()-TiempoDosificacion) + "-" + String(Rv_Sensor) + "-" + String(Cantidad_dosis);
              Dosificando_2 = false;

              Serial.println("incia flush para mandar producto");
              flushOnMachine(PunteroLavados[0], false, portsSensor[1], 0, TUBE_SENSOR_1_ON);
              Serial3.println("Mqtt:Out/MasterControl*Dosificacion/completa -> Lv:" + String(PunteroLavados[0]) + ",F:" + String(PunteroLavados[1]) + 
                                ",P:" + String(PunteroLavados[2]) + "/Quim:" + String(varDosifica) + "/" + String(Cantidad_dosis) + "ml/" + 
                                String(millis() - TiempoDosificacion) + "ms/" + String(Rv_Sensor) + "rv");
            }  
          }else if(tubeSensor1 && flowSensor){
            if(Rv_Sensor >= rv_dosis){      
              Serial.println("Termina tiempo dosis sin sensor probeta 1 / sensor de pulso");
              Serial.println("DesActiva producto " + String(varDosifica) + " y mono-Bomba");

              digitalWrite(portPump, Rele_OFF);   
              digitalWrite(portProduct, Rele_OFF);
              DatoMQTT = DatoMQTT + "/" + String(varDosifica) + "-" + String(millis() - TiempoDosificacion) + "-" + String(Rv_Sensor) + "-" + String(Cantidad_dosis) + "ErrorLevel_1";
              Dosificando_2 = false;

              Serial.println("incia flush para mandar producto");
              flushOnMachine(PunteroLavados[0], false, portsSensor[1], 0, TUBE_SENSOR_1_ON);
              Serial3.println("Mqtt:Out/MasterControl*Error/Sensor probeta 1 -> lv:" + String(PunteroLavados[0]) + ",F:" + String(PunteroLavados[1]) + 
                            ",P:" + String(PunteroLavados[2]) + "/Quim:" + String(varDosifica) + "/" + String(Cantidad_dosis) + "ml/" + 
                            String(millis() - TiempoDosificacion) + "ms/" + String(Rv_Sensor) + "rv");
            }
          }else if(millis() - TiempoDosificacion > (Tiempo_dosis*multiplo_time_max[1])){
            Serial.println("Termina tiempo dosis sin sensor probeta 1 / por tiempo");
            Serial.println(String(millis() - TiempoDosificacion) + " > " + String(Tiempo_dosis*multiplo_time_max[1]));
            Serial.println("multiplo_time_max[1]: " + String(multiplo_time_max[1]));
            Serial.println("DesActiva producto " + String(varDosifica) + " y mono-Bomba");

            digitalWrite(portPump, Rele_OFF);  
            digitalWrite(portProduct, Rele_OFF);   
            DatoMQTT = DatoMQTT + "/" + String(varDosifica) + "-" + String(millis()-TiempoDosificacion) + "-" + String(Rv_Sensor) + "-0-ErrorTimeOver_2";
            Dosificando_2 = false;

            AlamrOn = true;
            Serial3.println("Mqtt:Out/MasterControl*Error/Sin producto -> lv:" + String(PunteroLavados[0]) + ",F:" + String(PunteroLavados[1]) + 
                          ",P:" + String(PunteroLavados[2]) + "Quim:" + String(varDosifica) + "/" + String(Cantidad_dosis) + "ml/" + 
                          String(millis() - TiempoDosificacion) + "ms/" + String(Rv_Sensor) + "rv");
          }
        }
      }else if(varDosifica == (numProduct + 1)){
        if(displayLDC) Serial3.println("Mqtt:Out/MasterControl*Dosificacion/limpiando probeta -> lv:" + String(PunteroLavados[0]) + 
                                      ",F:" + String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2]));

        if(!flushOnMachine(PunteroLavados[0], tubeSensor1, portsSensor[1], 0, TUBE_SENSOR_1_ON, true, timeFlushProb)){
          Serial.println("ERROR no vacio probeta");

          Dosificando = false;
          varDosifica = 10;
          AlamrOn = true;
          if(displayLDC) //envia error por mqtt
            Serial3.println("Mqtt:Out/MasterControl*Error/Sin presion en probeta 1(Flush) -> lv:" + 
                          String(PunteroLavados[0]) + ",F:" + String(PunteroLavados[1]) + ",P:" + String(PunteroLavados[2]));
        }else{
          auxNumFlush++;
          Serial.println("numFlush: " + String(NumFlush) + ", valor actual: " + String(auxNumFlush));

          NumFlush = PunteroLavados[2] == 6 ? 2 : 1;
          
          if(auxNumFlush >= NumFlush){
            if(displayLDC) Serial3.println("Mqtt:Out/MasterControl*Dosificacion/finalizada -> " + String(CountLav) + "/" + String(PunteroLavados[0]) 
                                        + "/" + String(PunteroLavados[1]) + "/" + String(PunteroLavados[2]) + "/" + DatoMQTT);

            unsigned long timeFlushLast = millis();

            /*byte AirLavPort = (reciveInfoEsp("Ports/AirLav")).toInt();
            digitalWrite(AirLavPort, Rele_ON);
            digitalWrite(portPump, Rele_ON);

            do{
              TaskSerial();
              TaskMessageDisplay();
            }while(millis() - timeFlushLast < 14000);

            digitalWrite(AirLavPort, Rele_OFF);
            digitalWrite(portPump, Rele_OFF);*/

            Dosificando = false;

            Serial.println("limpiando probeta 2");
            //flushOnMachine(PunteroLavados[0], tubeSensor1, portsSensor[1], timeExtraFlush_1, TUBE_SENSOR_1_ON);
            flushOnMachine(PunteroLavados[0], tubeSensor1, portsSensor[1], 0, TUBE_SENSOR_1_ON, true, timeFlushProb);
          }
        }
      }
    }while(Dosificando);
  }
}