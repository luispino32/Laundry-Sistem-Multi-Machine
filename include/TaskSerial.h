void callback_wifi(String topic, String payload);

void TaskSerial(){
  if(Serial3.available()){
    Dato = Serial3.readStringUntil('\n'); 
    if(Dato.indexOf("MQTT >>") >= 0 || Dato.indexOf("MQTT <<") >= 0){
      Serial.println("Other_1 => " + Dato);
    }else if(Dato.indexOf("Dosifica/") >= 0){
      callback_wifi("In/MasterControl", Dato);
    }else if(Dato.indexOf("Mqtt:") >= 0){
      indice = Dato.indexOf(":"); Dato = Dato.substring(indice + 1);
      indice = Dato.indexOf("*");
      
      callback_wifi(Dato.substring(0, indice), Dato.substring(indice + 1));
    }else{
      Serial.println("Other_2 => " + Dato);
    }
  }
}

void callback_wifi(String topic, String payload){
  int indice, indice2;
  String dato, dato2;
  byte auxInfoLav[4];

  topic.trim();
  payload.trim();
  dato = payload;
  
  if(topic == "In/MasterControl"){
    if(dato.indexOf("Dosifica/") >= 0){
      Serial.println("Dosifica => " + dato);
      
      indice = dato.indexOf("/"); dato = dato.substring(indice+1); indice = dato.indexOf("/");
      auxInfoLav[0] = dato.substring(0, indice).toInt(); dato = dato.substring(indice+1); indice = dato.indexOf("/");
      auxInfoLav[1] = dato.substring(0, indice).toInt(); dato = dato.substring(indice+1); indice = dato.indexOf("/");
      auxInfoLav[2] = dato.substring(0, indice).toInt();
      indice = dato.substring(indice + 1).toInt();
  
      if((auxInfoLav[0] + auxInfoLav[1] + auxInfoLav[2]) == indice){
        dato = "Dosifica/" + String(auxInfoLav[0]) + "/" + String(auxInfoLav[1]) + "/" + String(auxInfoLav[2]) + "/" + String(auxInfoLav[0]+auxInfoLav[1]+auxInfoLav[2]);
        Serial3.println(dato);
  
        if(N_SendInfoControl < 40){
          infoControl[N_SendInfoControl-1][0] = auxInfoLav[0];
          infoControl[N_SendInfoControl-1][1] = auxInfoLav[1];
          infoControl[N_SendInfoControl-1][2] = auxInfoLav[2];
  
          if((reciveInfoEsp("Alarm/display")).toInt()) N_SendInfoControl++;
        }
          
        if(NumDosificaReceive[auxInfoLav[0]-1][0] > 0){
          if(NumDosificaReceive[auxInfoLav[0]-1][0] == auxInfoLav[0] && 
             NumDosificaReceive[auxInfoLav[0]-1][1] == auxInfoLav[1] &&
             NumDosificaReceive[auxInfoLav[0]-1][2] == auxInfoLav[2]){
  
            unsigned long indiceAux = (reciveInfoEsp("Config/t_DosiRepeat")).toInt();
            indiceAux = indiceAux*1000;

            //Serial.println("indice: " + String(indiceAux));
            //Serial.println("TimeDosificaReceive[auxInfoLav[0]-1] : " + String(TimeDosificaReceive[auxInfoLav[0]-1] ));
            //Serial.println("auxInfoLav[0]-1 : " + String(auxInfoLav[0]-1));

            if(millis() - TimeDosificaReceive[auxInfoLav[0]-1] < indiceAux){
              Serial.println(String(millis() - TimeDosificaReceive[auxInfoLav[0]-1]) + " < " + String((indice*1000)));
              Serial.println("Descartada -> Lav:" + String(auxInfoLav[0]) + ", Form:" + String(auxInfoLav[1]) + ", Step:" + String(auxInfoLav[2]));
              infoControl[N_SendInfoControl-1][3] = 11; // Error 1: Señal descartada por tiempo
              return;
            }
          } 
        }
          
        NLavsPendientes++;
        LavsPendientes[NLavsPendientes-1][0] = auxInfoLav[0];
        LavsPendientes[NLavsPendientes-1][1] = auxInfoLav[1];
        LavsPendientes[NLavsPendientes-1][2] = auxInfoLav[2];
  
        NumDosificaReceive[auxInfoLav[0]-1][0] = auxInfoLav[0];
        NumDosificaReceive[auxInfoLav[0]-1][1] = auxInfoLav[1];
        NumDosificaReceive[auxInfoLav[0]-1][2] = auxInfoLav[2];
        TimeDosificaReceive[auxInfoLav[0]-1] = millis();
  
        infoControl[N_SendInfoControl-1][3] = 1;     

        /*Serial.println("LavsPendientes[NLavsPendientes-1][0] : " + String(LavsPendientes[NLavsPendientes-1][0]));
        Serial.println("LavsPendientes[NLavsPendientes-1][1] : " + String(LavsPendientes[NLavsPendientes-1][1]));
        Serial.println("LavsPendientes[NLavsPendientes-1][2] : " + String(LavsPendientes[NLavsPendientes-1][2]));

        Serial.println("auxInfoLav[0] : " + String(auxInfoLav[0]));
        Serial.println("auxInfoLav[1] : " + String(auxInfoLav[1]));
        Serial.println("auxInfoLav[2] : " + String(auxInfoLav[2]));*/
        
      }
    }else if(payload.indexOf("SignalActive/") >= 0){
      if(N_SendInfoControl < 40){
        infoControl[N_SendInfoControl-1][0] = auxInfoLav[0];
        infoControl[N_SendInfoControl-1][1] = auxInfoLav[1];
        infoControl[N_SendInfoControl-1][2] = 0;
        infoControl[N_SendInfoControl-1][3] = 2;
  
        if((reciveInfoEsp("Alarm/display")).toInt()) N_SendInfoControl++;
      }
    }
  }else if(topic == "Out/ControlPanel"){
    if(dato.indexOf("infoLav/") >= 0){
      indice = dato.indexOf("/"); dato = dato.substring(indice+1); indice = dato.indexOf("/");
      auxInfoLav[0] = dato.substring(0,indice).toInt(); dato = dato.substring(indice+1); indice = dato.indexOf("/");
      auxInfoLav[1] = dato.substring(0,indice).toInt(); dato = dato.substring(indice+1); indice = dato.indexOf("/");
      auxInfoLav[2] = dato.substring(0,indice).toInt(); 
      auxInfoLav[3] = dato.substring(indice+1).toInt();

      indice = 0;
      for(byte i=0; i<N_SendInfoControl; i++){
        if(infoControl[i][0] == auxInfoLav[0] && infoControl[i][1] == auxInfoLav[1] && 
           infoControl[i][2] == auxInfoLav[2] && infoControl[i][3] == auxInfoLav[3]) indice = 1;
      
        if(indice == 1 && (i+1) < N_SendInfoControl){
            infoControl[i][0] = infoControl[i+1][0];
            infoControl[i][1] = infoControl[i+1][1];
            infoControl[i][2] = infoControl[i+1][2];
            infoControl[i][3] = infoControl[i+1][3];
            infoControl[i][4] = infoControl[i+1][4];
        }
      }

      if(indice == 1){
        infoControl[N_SendInfoControl-1][4] = 0;
        N_SendInfoControl--;
      }
    }else if(dato.indexOf("get/") >= 0 || dato.indexOf("set/") >= 0){
      indice = dato.indexOf("/"); auxInfoLav[2] = indice; dato = dato.substring(indice+1);
      indice = dato.indexOf("/"); auxInfoLav[2] = auxInfoLav[2] + indice + 1;

      dato2 = dato.substring(0, indice);
      dato2.trim(); //Nombre de la zona de memoria

      dato = dato.substring(indice+1);
      indice = dato.indexOf("/");
        
      dato = indice > 0 ? dato.substring(0, indice) : dato.substring(indice+1);
      dato.trim(); //Nombre de la varible a modificar

      auxInfoLav[3] = indice > 0 ? auxInfoLav[2] + indice + 1 : auxInfoLav[2] + dato.length() + 1;
      
      auxInfoLav[0] = 0;
      for(byte i=0; i<7; i++){
        if(dato2.indexOf(memoryZone[i]) >= 0){ 
          auxInfoLav[0] = i+1; 
          break;
        }
      }
      
      if(auxInfoLav[0] == 0){
        Serial3.println("Mqtt:In/ControlPanel*" + dato.indexOf("set/") >= 0 ? "set/ErrorInfo_1/Zone" : "get/ErrorInfo_1/Zone");
      }else{
        auxInfoLav[0]--;

        for(byte i=0; i<12; i++){
          if(itemZones[auxInfoLav[0]][i] != NULL){
            dato = String(itemZones[auxInfoLav[0]][i]); 
            indice = dato.indexOf("-"); indice2 = dato.indexOf("/");
            if(indice > 0){
              if(indice2 > 0){ // tiene '/'
                do{
                  if(dato.substring(0, indice) == payload.substring(auxInfoLav[2]+1, auxInfoLav[2]+indice+1)){
                    auxInfoLav[1] = payload.substring(auxInfoLav[2]+indice+1, 
                                                      isdigit(payload.charAt(auxInfoLav[2]+indice+2)) ? 
                                                              auxInfoLav[2]+indice+3 : auxInfoLav[2]+indice+3).toInt();
                    if(auxInfoLav[1] < 1 || auxInfoLav[1] > dato.substring(indice+1, indice2).toInt()) indice = 0;
                  }else indice = 0;

                  if(indice > 0){
                    if(dato.indexOf("/") < 0){
                      auxInfoLav[3] = 233;
                      indice = 0;
                    }else{
                      dato = dato.substring(indice2 + 1);
                      indice2 = dato.indexOf("/");
                      auxInfoLav[2] = isdigit(payload.charAt(auxInfoLav[2]+indice+2)) ? auxInfoLav[2]+indice+2 : auxInfoLav[2]+indice+1;
                      indice = dato.indexOf("-");
                      indice2 = indice2 > 0 ? indice2 : dato.length() + 1;  
                    }
                  }
                }while(indice > 0);
              }else{ // tiene '-'
                //Serial.println("tiene -");
                if(dato.substring(0, indice) == payload.substring(auxInfoLav[2]+1, auxInfoLav[2]+indice+1)){
                  indice2 = payload.substring(auxInfoLav[2]+indice+1, auxInfoLav[3]).toInt();
                  if(indice2 >= 1 && indice2 <= dato.substring(indice+1).toInt()) if((auxInfoLav[2]+indice+3) >= (auxInfoLav[3])) auxInfoLav[3] = 233;
                }
              }
            }else{// NO tiene '-' ni '/'
              dato2 = auxInfoLav[3] > 0 ? payload.substring(auxInfoLav[2]+1, auxInfoLav[3]) : payload.substring(auxInfoLav[2]+1);
              dato2.trim();
              if(dato == dato2) auxInfoLav[3] = 233;
            } 
          }
          
          if(auxInfoLav[3] == 233){
            auxInfoLav[1] = i;
            break;
          }
        }

        if(auxInfoLav[3] == 233){
          indice = payload.indexOf("/"); dato = payload.substring(indice+1);
          indice = dato.indexOf("/");
    
          dato = dato.substring(indice+1);
          indice = dato.indexOf("/");
          
          dato2 = indice > 0 ? dato.substring(indice+1): "";
          dato = indice > 0 ? dato.substring(0, indice) : dato.substring(indice+1);
          dato.trim(); //Nombre de la varible a modificar
          dato2.trim(); // valor de set para modificar

          if(payload.indexOf("set/") >= 0){
            switch(typeData[auxInfoLav[0]][auxInfoLav[1]]){
              case 1: saveEEPROM(String(memoryZone[auxInfoLav[0]]), dato, String(dato2)); break;
              case 2: saveEEPROM(String(memoryZone[auxInfoLav[0]]), dato, String(dato2)); break;
              case 3: saveEEPROM(String(memoryZone[auxInfoLav[0]]), dato, String(dato2)); break;
              case 4: saveEEPROM(String(memoryZone[auxInfoLav[0]]), dato, String(dato2)); break;
              case 5: saveEEPROM(String(memoryZone[auxInfoLav[0]]), dato, String(dato2)); break;
              case 6: saveEEPROM(String(memoryZone[auxInfoLav[0]]), dato, String(dato2)); break;
            }
          }else{
            //Serial.println("typeData[auxInfoLav[0]][auxInfoLav[1]]: " + String(typeData[auxInfoLav[0]][auxInfoLav[1]]));
            if(typeData[auxInfoLav[0]][auxInfoLav[1]] == 0){
              dato = "get/ErrorInfo_2";
            }else{
              //Serial.println("dato1: " + dato);
              dato = reciveInfoEsp(String(memoryZone[auxInfoLav[0]]) + "/" + dato);
              //Serial.println("dato2: " + dato);
            }
          }

          //indice = payload.indexOf("/");
          Serial3.println("Mqtt:In/ControlPanel*" + payload + " => " + dato);
          //Serial.println("Mqtt:In/ControlPanel*" + payload + " => " + dato);
        }else{
          if(dato.indexOf("set/") >= 0) Serial3.println("Mqtt:In/ControlPanel*set/ErrorInfo_3");
          else Serial3.println("Mqtt:In/ControlPanel*get/ErrorInfo_3");
        }
      }
    }else if(dato.indexOf("StoPurga/") >= 0){
      if(!systemOk){
        digitalWrite((reciveInfoEsp("Ports/Pump")).toInt(), Rele_OFF);
        for(byte i=1; i<5; i++) digitalWrite((reciveInfoEsp("Ports/P" + String(i))).toInt(), Rele_OFF);
        for(byte j=1; j<7; j++) digitalWrite((reciveInfoEsp("Ports/Lv" + String(j))).toInt(), Rele_OFF);
        digitalWrite((reciveInfoEsp("Ports/AirLav")).toInt(), Rele_OFF);
        digitalWrite((reciveInfoEsp("Ports/FlushProb")).toInt(), Rele_OFF);
        digitalWrite((reciveInfoEsp("Ports/AirProb")).toInt(), Rele_OFF);
        digitalWrite((reciveInfoEsp("Ports/Solenoide")).toInt(), Rele_OFF);

        systemOk = true;
        for(byte i=0; i<4; i++) auxInfoLav_pruga[i] = 0;
      }else{
        Serial3.println("Mqtt:In/ControlPanel*System free");
      }
    }else if(dato.indexOf("purga/") >= 0 || dato.indexOf("flushLav/") >= 0){// purga/Pump/timePump || purga/probeta_1/Pump - probeta_2/Pump/timePump
      if(Dosificando || !systemOk){                                         // flushLav/lavadora/timeFlushMax || 
        Serial3.println("Mqtt:In/ControlPanel*System busy");
      }else{
        indice = dato.indexOf("/"); dato = dato.substring(indice+1); indice = dato.indexOf("/");
        dato2 = dato.substring(0, indice); // bomba a purgar
        dato = dato.substring(indice+1); indice = dato.indexOf("/");
  
        auxInfoLav[2] = 0;
        auxInfoLav[3] = payload.indexOf("purga/") >= 0 ? 1 :  2; //purga bomba
        
        if(dato2 == "probeta_1" || dato2 == "probeta_2"){
          auxInfoLav[0] = indice > 0 ? dato.substring(0,indice).toInt() : dato.toInt();
          auxInfoLav[1] = indice > 0 ? dato.substring(indice+1).toInt() : 10;
          auxInfoLav[2] = dato2 == "probeta_1" ? 1 : 2;
        }else{
          auxInfoLav[0] = dato2.toInt();
          auxInfoLav[1] = dato.toInt();
        }

        //Serial.println("auxInfoLav[0]: " + String(auxInfoLav[0]) + " - auxInfoLav[1]: " + String(auxInfoLav[1]));
        if(auxInfoLav[0] > 0 && auxInfoLav[0] <= 6 && auxInfoLav[1] > 0 && auxInfoLav[1] <= 254){
          systemOk = false;
          for(byte k=0; k<4; k++) auxInfoLav_pruga[k] = auxInfoLav[k];
          TaskPurga();
        }else{
          Serial3.println("Mqtt:In/ControlPanel*Valores fuera de rango");
        }
      }
    }else if(dato.indexOf("AirProb/") >= 0){
      indice = dato.indexOf("/");
      dato = dato.substring(indice+1);
      indice = dato.toInt();

      if(Dosificando || !systemOk){
        Serial3.println("Mqtt:In/ControlPanel*System busy");
      }else{
        if(indice > 0 && indice < 5){
          digitalWrite((reciveInfoEsp("Ports/AirProb")).toInt(), Rele_ON);
          delay(indice*100);
          digitalWrite((reciveInfoEsp("Ports/AirProb")).toInt(), Rele_OFF);
        } 
      }
    }
  }
}