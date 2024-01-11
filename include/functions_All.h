void TaskSerial();
void TaskS_Alarma();
void TaskMessageDisplay();
void callback_wifi(String topic, String payload);

void StringToEEPROM(int offset, const String &str) {
  byte len = str.length();
  len = len > 10 ? 10 : len;
  EEPROM.write(offset, len);
  for (int i=0; i<len; i++)
    EEPROM.write(offset+i, str[i]);
}

String readStringFromEEPROM(int offset) {
  int len = EEPROM.read(offset);
  int i;
  char data[len+1];
  for (i=0; i<len; i++)
    data[i]=EEPROM.read(offset+i+1);
  data[i]=0;
  return String(data);
}

void setValueEEPROM(byte type, int dirr, String value){
  Serial.println("Escribe en Direccion: " + String(dirr) + " => " + value);
  switch(type){
    case 1: 
    case 2: EEPROM.update(dirr, value.toInt()); break;
    case 3:{
      int auxInt = value.toInt();
      EEPROM.put(dirr, auxInt); 
      }break;
    case 4:{
      float auxFloat = atof(value.c_str());
      EEPROM.put(dirr, auxFloat); 
      }break;
    case 5: StringToEEPROM(dirr, value); break;
    case 6:{
      unsigned long auxLong = value.toInt();
      EEPROM.put(dirr, auxLong); 
      }break;
  }
}

String getValueEEPROM(byte type, int dirr){
  switch(type){
    case 1: 
    case 2: return String(EEPROM.read(dirr)); break;
    case 3:{
      int auxInt;
      EEPROM.get(dirr, auxInt);
      return String(auxInt); 
    }break;
    case 4:{
      float auxFloat;
      EEPROM.get(dirr, auxFloat);
      return String(auxFloat); 
    }break;
    case 5: return readStringFromEEPROM(dirr); break;
    case 6:{
      unsigned long auxLong;
      EEPROM.get(dirr, auxLong);
      return String(auxLong); 
    }break;
  }

  return "";
}

int getDirr(byte type, int dirr, byte offset){
  int auxDirr;
  
  switch(type){
    case 1:
    case 2: auxDirr =  dirr + offset; break;
    case 3: auxDirr =  dirr + (offset*2); break;
    case 4: auxDirr =  dirr + (offset*4); break;
    case 5: auxDirr =  dirr + (offset*10); break;
    case 6: auxDirr =  dirr + (offset*4); break;
  }

  return auxDirr;
}

String saveEEPROM(String zone, String key, String value = ""){
  int indice, indice2, indice3, indice4, indice5;
  bool getOrSet = value == "" ? false : true;
  bool indica = false;
  String dato, dato2;
  zone.trim();
  key.trim();

  for(byte i=0; i<7; i++){
    for(byte j=0; j<11; j++){
      if(itemZones[i][j] != NULL){
        dato = String(itemZones[i][j]);
        indice = dato.indexOf("-");
          
        if(indice >= 0){
          if(dato.indexOf("/") >= 0){
            if(i == 2 && j == 2){
              indice = key.indexOf("F"); indice2 = key.indexOf("S"); indice3 = key.indexOf("P");
              if(indice >= 0 && indice2 >= 0 && indice3 >= 0){
                dato = key.substring(indice + 1); 
                indice = dato.indexOf("S"); 
                indice2 = indice; 
                indice = dato.substring(0, indice).toInt();
                  
                dato = dato.substring(indice2 + 1); 
                indice2 = dato.indexOf("P"); 
                indice3 = indice2; 
                indice2 = dato.substring(0, indice2).toInt();
                  
                dato = dato.substring(indice3 + 1); 
                indice3 = dato.toInt();

                if(indice > 0 && indice <= 20 && indice2 > 0 && indice2 <= 6 && indice3 > 0 && indice3 <= 4){
                  if(getOrSet){
                    //Serial.println("indice:" + String(indice) + ", " + "indice2:" + String(indice2) + ", " + "indice3:" + String(indice3));
                    setValueEEPROM(typeData[i][j], getDirr(typeData[i][j], dirrData[i][j], ((indice-1)*96 + (indice2-1)*16 + indice3 - 1)), value);
                  }else{
                    value = getValueEEPROM(typeData[i][j], getDirr(typeData[i][j], dirrData[i][j], ((indice-1)*96 + (indice2-1)*16 + indice3 - 1)));
                  }
                    
                  indica = true;
                }
              }
            }else{//  F-20/Lv-6
              dato2 = dato;
              indice = dato2.indexOf("-"); 
              dato = dato2.substring(0, indice); // F
              indice2 = dato2.indexOf("/"); 
              indice = dato2.substring(indice+1, indice2).toInt();// 20
                
              dato2 = dato2.substring(indice2 + 1); 
              indice2 = dato2.indexOf("-");
              indice5 = dato2.substring(indice2 + 1).toInt(); // 6
              dato2 = dato2.substring(0, indice2); // Lv
              indice2 = indice5;

              indice3 = key.indexOf(dato);
              indice4 = key.indexOf(dato2);

              if(indice3 >= 0 && indice4 >= 0){   
                indice3 = key.substring((indice3 + dato.length()), indice4).toInt();
                indice4 = key.substring(indice4 + dato2.length()).toInt();
                  
                if(indice3 > 0 && indice3 <= indice && indice4 > 0 && indice4 <= indice2){
                  if(getOrSet){
                    //Serial.println("indice:" + String(indice) + ", indice2:" + String(indice2) + ", indice3:" + String(indice3) + ", indice4:" + String(indice4));
                    //Serial.println("getDirr(" + String(typeData[i][j]) + ", " + String(dirrData[i][j]) + ", " + String((indice3-1)*indice + indice4 - 1) + ");");
                    setValueEEPROM(typeData[i][j], getDirr(typeData[i][j], dirrData[i][j], ((indice3-1)*indice + indice4 - 1)), value);
                  }else{
                    value = getValueEEPROM(typeData[i][j], getDirr(typeData[i][j], dirrData[i][j], ((indice3-1)*indice + indice4 - 1)));
                  }
                    
                  indica = true;
                }
              }
            }
          }else{
            indice2 = dato.substring(indice + 1).toInt();
            if(indice2 > 0){
              for(byte k=0; k<indice2; k++){
                if(key == (dato.substring(0, indice) + String(k+1))){
                  if(getOrSet){
                    setValueEEPROM(typeData[i][j], getDirr(typeData[i][j], dirrData[i][j], k), value);
                  }else{
                    value = getValueEEPROM(typeData[i][j], getDirr(typeData[i][j], dirrData[i][j], k));
                  }
                    
                  indica = true;
                  break;
                }
              }
            }
          }
        }else{
          if(dato == key){
            if(getOrSet){
              setValueEEPROM(typeData[i][j], dirrData[i][j] , value);
            }else{
              value = getValueEEPROM(typeData[i][j], dirrData[i][j]);
            }

            indica = true;
          }
        }  
      }

      if(indica) break;
    }

    if(indica) break;
  }

  if(!getOrSet) return value;
  return "";
}


String reciveInfoEsp(String dataSend){
  int indice;
  
  indice = dataSend.indexOf("/");
  dataSend = saveEEPROM(dataSend.substring(0,indice), dataSend.substring(indice + 1));
  return dataSend;
}

void initProgramm(){
  if(EEPROM.read(0) != 9) EEPROM.update(0,9);
  if(EEPROM.read(1) != 3) EEPROM.update(1,3);
  if(EEPROM.read(2) != 23) EEPROM.update(2,23);
  if(EEPROM.read(3) != 25) EEPROM.update(3,25);

  if(EEPROM.read(4) != 33) EEPROM.update(4,33);
  if(EEPROM.read(5) != 35) EEPROM.update(5,35);
  if(EEPROM.read(6) != 37) EEPROM.update(6,37);
  if(EEPROM.read(7) != 7) EEPROM.update(7,7);

  if(EEPROM.read(10) != 5) EEPROM.update(10,5);
  if(EEPROM.read(11) != 6) EEPROM.update(11,6);

  if(EEPROM.read(12) != 27) EEPROM.update(12,27);
  if(EEPROM.read(13) != 46) EEPROM.update(13,46);

  if(EEPROM.read(14) != 2) EEPROM.update(14,2);
  if(EEPROM.read(15) != 31) EEPROM.update(15,31);

  if(EEPROM.read(16) != 10) EEPROM.update(16,10);
  if(EEPROM.read(17) != 11) EEPROM.update(17,11);
  if(EEPROM.read(18) != 13) EEPROM.update(18,13);
}
bool CompruebaPASO_LAVADO(byte N_Form, byte Step){
  bool ReturnDate = false;
  
  byte numProduct = (reciveInfoEsp("Product/N_Product")).toInt();

  for(int i=0; i<numProduct; i++){
    float varAux = atof((reciveInfoEsp("Form/F" + String(N_Form) + "S" + String(Step) + "P" + String(i+1))).c_str());
    Serial.println("F" + String(N_Form) + "S" + String(Step) + "P" + String(i+1) + " = " + varAux);
        
    if(varAux > 0){ //F1S2P3 -> formula 1 - Paso 2 - Producto 3
      ReturnDate = true;
      break;
    }
  }

  return ReturnDate;
}

bool CompruebaUltimoPaso(byte N_Paso, byte N_Formula){
  float varAux;
  
  if(N_Formula > 0){
    byte UltimoPaso = 0;

    byte numProduct = (reciveInfoEsp("Product/N_Product")).toInt();

    for(int i=0; i<6; i++){
      for(int j=0; j<numProduct; j++){
        varAux = atof((reciveInfoEsp("Form/F" + String(N_Formula) + "S" + String(i+1) + "P" + String(j+1))).c_str());
        if(varAux > 0) UltimoPaso = i+1;
      }
    }
    
    if(UltimoPaso == N_Paso) return true;
    return false;
  }
  return true;
}

bool CompruebaSecuenciaPaso(byte N_PasoAnterior, byte N_FormulaAnterior, byte N_PasoActual){
  float varAux;
  
  if(N_FormulaAnterior > 0){
    for(byte i=N_PasoAnterior; i<6; i++){
      for(byte j=0; j<6; j++){
        varAux = atof((reciveInfoEsp("Form/F" + String(N_FormulaAnterior) + "S" + String(i+1) + "P" + String(j+1))).c_str());
         
        if(varAux > 0){
          if((i+1) == N_PasoActual) return true;
          else return false;
        }
      }
    }
  }

  return false;
}

bool CompruebaPrimerPaso(byte N_Paso, byte N_Formula){
  float varAux;
  
  if(N_Formula > 0){
    for(int i=0; i<6; i++){
      for(int j=0; j<6; j++){
        varAux = atof((reciveInfoEsp("Form/F" + String(N_Formula) + "S" + String(i+1) + "P" + String(j+1))).c_str());
          
        if(varAux > 0){
          if((i+1) == N_Paso) return true;
          else return false;
        }
      }
    }
  }

  return false;
}

void VerificaIncrementoConteos(byte *posCount, byte *posForm, byte *posPaso, unsigned long CountLav, byte N_Lav = 0, byte N_Form = 0){ 
  byte UltimoPaso, UltimaFormula;
  
  if(CountLav > 70000*(*posCount)){
    (*posCount)++;
    saveEEPROM("RegLvs/posCount", String(*posCount));

    if(N_Lav > 0){
      UltimaFormula = (reciveInfoEsp("RegLvs/F_L" + String(N_Lav) + "_N" + String(*posForm))).toInt();
      UltimoPaso = (reciveInfoEsp("RegLvs/P_L" + String(N_Lav) + "_N" + String(*posPaso))).toInt();
    }

    (*posForm)++;
    saveEEPROM("RegLvs/posForm", String(*posForm));
    
    (*posPaso)++;
    saveEEPROM("RegLvs/posPaso", String(*posPaso));

    if(N_Lav > 0){
      saveEEPROM("RegLvs/F_L" + String(N_Lav) + "_N" + String(*posForm), String(UltimaFormula)); 
      saveEEPROM("RegLvs/P_L" + String(N_Lav) + "_N" + String(*posPaso), String(UltimoPaso)); 
    }
  }else{
    if(CountLav == (unsigned long)(70000*(*posCount-1)+17500) || 
       CountLav == (unsigned long)(70000*(*posCount-1)+35000) || 
       CountLav == (unsigned long)(70000*(*posCount-1)+52500)){

      if(N_Lav > 0) UltimoPaso = (reciveInfoEsp("RegLvs/P_L" + String(N_Lav) + "_N" + String(*posPaso))).toInt();
      (*posPaso)++;

      saveEEPROM("RegLvs/posPaso", String(*posPaso));
      if(N_Lav > 0) saveEEPROM("RegLvs/P_L" + String(N_Lav) + "_N" + String(*posPaso), String(UltimoPaso)); 
    }
  }
}

bool Registro_De_Lavado(byte N_Lav, byte N_Form, byte Step, bool displayLDC){
  if(!CompruebaPASO_LAVADO(N_Form, Step)){
    if(displayLDC)
      Serial3.println("Mqtt:Out/MasterControl*Error/Paso de lavado no encontrado -> lv:" + String(N_Lav) + ",F:" + String(N_Form) + ",P:" + String(Step));

    //Envia error a base de datos
    return false;
  }

  Serial.println("Paso Compruea paso de lavado");

  byte UltimaFormula, UltimoPaso;
  byte posForm, posPaso, posCount;

  unsigned long CountLav;

  posForm = (reciveInfoEsp("RegLvs/posForm_" + String(N_Lav))).toInt();
  UltimaFormula = (reciveInfoEsp("RegLvs/F_L" + String(N_Lav) + "_N" + String(posForm))).toInt();

  posPaso = (reciveInfoEsp("RegLvs/posPaso_" + String(N_Lav))).toInt();
  UltimoPaso = (reciveInfoEsp("RegLvs/P_L" + String(N_Lav) + "_N" + String(posPaso))).toInt();

  posCount = (reciveInfoEsp("RegLvs/posCount_" + String(N_Lav))).toInt();
  CountLav = (reciveInfoEsp("RegLvs/Cont_L" + String(N_Lav) + "_N" + String(posCount))).toInt();
  
  //Serial.println("RegLvs/Cont_L" + String(N_Lav) + "_N" + String(posCount));
  //Serial.println("CountLav: " + String(CountLav));

  Serial.println("Compruea paso de lavado");
  
  //Serial.println("UltimaFormula: " + String(UltimaFormula) + " == N_Form: " + String(N_Form));
  if(UltimaFormula == N_Form){ // Que la formula ANTERIOR y la PRESENTE, son las mismas   
    if(Step <= UltimoPaso){ //Si el paso ACTUAL es INFERIOR o IGUAL al paso ANTERIOR      
      if(CompruebaUltimoPaso(UltimoPaso, UltimaFormula)){ // Si es INFERIOR o IGUAL, pero el PASO ANTERIOR finalizo
        CountLav++;
        VerificaIncrementoConteos(&posCount, &posForm, &posPaso, CountLav, N_Lav, N_Form);
        saveEEPROM("RegLvs/Cont_L" + String(N_Lav) + "_N" + String(posCount), String(CountLav)); 
        
        if(!CompruebaPrimerPaso(Step, N_Form)){ // Si el PASO, NO es el PRIMERO en la SECUENCIA
          //Posible error
        }
      }else{ // Si el PASO es INFERIOR al PASO ANTERIOR, y el PASO ANTERIOR no es el ULTIMO (Error se repiten PASOS)
        bool repeatBool; 
        repeatBool = (reciveInfoEsp("Config/repeatStep")).toInt();
          
        if(repeatBool){
          //Serial.println("retorno conteo 2: " + String(CountLav));
          return CountLav;
        }else{
          if(displayLDC) Serial3.println("Mqtt:Out/MasterControl*Error/Paso repetido -> lv:" + String(N_Lav) + ",F:" + String(N_Form) + ",P:" + String(Step));
          //Serial.println("retorno conteo 3: 0");
          return 0;
        }
      }
    }else{ // Si el paso ACTUAL es superior al paso ANTERIOR
      saveEEPROM("RegLvs/P_L" + String(N_Lav) + "_N" + String(posPaso), String(Step)); 

      if(CompruebaSecuenciaPaso(UltimoPaso, UltimaFormula, Step)){ // Si el Paso es el CORRECTO en la secuencia
        
      }else{ // Si el PASO SALTO en la secuencia.
        if(displayLDC)
          Serial3.println("Mqtt:Out/MasterControl*Error/Paso omitido: lv:" + String(N_Lav) + ", Formula:" + String(N_Form) + ", Paso:" + String(Step));
        //error se salto un paso
      }

      Serial.println("no retorna");
    }
  }else{  // Que la formula ANTERIOR y la PRESENTE, sean diferentes
    CountLav++;
    VerificaIncrementoConteos(&posCount, &posForm, &posPaso, CountLav);
    saveEEPROM("RegLvs/Cont_L" + String(N_Lav) + "_N" + String(posCount), String(CountLav));
    saveEEPROM("RegLvs/P_L" + String(N_Lav) + "_N" + String(posPaso), String(Step)); 
    saveEEPROM("RegLvs/F_L" + String(N_Lav) + "_N" + String(posForm), String(N_Form)); 

    if(!CompruebaUltimoPaso(UltimoPaso, UltimaFormula)) //Error: si el Lavado anterior NO termino
      if(displayLDC)
          Serial3.println("Mqtt:Out/MasterControl*Error/No finalizo lavado anteiror -> lv:" + String(N_Lav) + ", Formula:" + String(N_Form) + ", Paso:" + String(Step));
          
    if(CompruebaUltimoPaso(Step, N_Form)){ // Error: Comenzo en el Ultimo Paso
      if(displayLDC)
          Serial3.println("Mqtt:Out/MasterControl*Error/Inicio en ultimo paso -> lv:" + String(N_Lav) + ", Formula:" + String(N_Form) + ", Paso:" + String(Step));

    }else{
      if(!CompruebaPrimerPaso(Step, N_Form)) // Error: Salto de PASO (no es el PRIMER PASO de la FORMULA)    
        if(displayLDC)
          Serial3.println("Mqtt:Out/MasterControl*Error/Omitido primer paso -> lv:" + String(N_Lav) + ", Formula:" + String(N_Form) + ", Paso:" + String(Step));
    }
  }
  
  Serial.println("retorno conteo 1: " + String(CountLav));
  return CountLav;
}

bool flushOnMachine(byte Nlav, bool tubeSensor, byte portTubeSensor, byte timeExtraFlush, bool stateOnTubeSensor, bool flushProb = false, byte delayFlushProb = 0){
  /*Serial.println(String(Nlav) + "/" + String(tubeSensor) + "/" + String(portTubeSensor) + "/" + 
                 String(timeExtraFlush) + "/" + String(stateOnTubeSensor) + "/" + String(flushProb) + "/" + String(delayFlushProb));*/
  
  unsigned long timeOverFlush = ((reciveInfoEsp("Flush/timeOver")).toInt())*1000;
  unsigned long timeWaitOnAir = ((reciveInfoEsp("Flush/waitOnAir")).toInt())*1000;

  unsigned long timeDelayAir = (reciveInfoEsp("Flush/delayAir")).toInt();
  
  unsigned long timeWaitOnAir_aux;
  unsigned long lavPort_Aux = 0;
  unsigned long timeFlush;

  timeDelayAir = timeDelayAir*1000;

  bool stateLavPort = true;
  bool stateAirManifold = false;
  bool stateAirManifold_aux = false;

  byte AirProbPort = (reciveInfoEsp("Ports/AirProb")).toInt();
  byte SolenoidePort = (reciveInfoEsp("Ports/Solenoide")).toInt();
  byte LavPort = (reciveInfoEsp("Ports/Lv" + String(Nlav))).toInt();

  Serial.println("puertos: " + String(AirProbPort) + "/" + String(SolenoidePort) + "/" + String(LavPort));
    
  if(flushProb){
    digitalWrite((reciveInfoEsp("Ports/FlushProb")).toInt(), Rele_ON);
    delay(int(delayFlushProb)*1000);
    digitalWrite((reciveInfoEsp("Ports/FlushProb")).toInt(), Rele_OFF);
  }

  Serial.println("Activa solenoide y lavadora: " + String(Nlav));
  digitalWrite(SolenoidePort, Rele_ON);
  digitalWrite(LavPort, Rele_ON);
  
  timeFlush = millis();
  timeWaitOnAir_aux = millis();

  do{
    if(timeDelayAir > 0){
      if(stateAirManifold){
        if(millis() - lavPort_Aux > 500){
          if(stateAirManifold_aux){
            if(millis() - lavPort_Aux > 1000){
              digitalWrite(LavPort, Rele_ON);
              stateAirManifold_aux = false;
              stateAirManifold = false;
            }
          }else{
            digitalWrite(AirProbPort, ReleModule_OFF);
            stateAirManifold_aux = true;
            lavPort_Aux = millis();
          }
        }
      }else{
        if(millis() - timeWaitOnAir_aux > timeWaitOnAir){
          if(millis() - lavPort_Aux > timeDelayAir){
            if(stateLavPort){
              digitalWrite(LavPort, Rele_OFF);
              lavPort_Aux = millis();
              stateLavPort = false;
            }else{
              digitalWrite(AirProbPort, ReleModule_ON);
              timeWaitOnAir_aux = millis();
              stateAirManifold = true;
              lavPort_Aux = millis();
              stateLavPort = true;
            }
          }
        }
      }
    }

    if(millis() - timeFlush > timeOverFlush){
      Serial.println("Apaga solenoide y lavadora: " + String(Nlav));
      digitalWrite(AirProbPort, ReleModule_OFF);
      digitalWrite(SolenoidePort, Rele_OFF);
      digitalWrite(LavPort, Rele_OFF);

      if(tubeSensor) return false;
      return true;
    }

    TaskSerial();
    TaskS_Alarma();
    TaskMessageDisplay();
  }while(tubeSensor && digitalRead(portTubeSensor) == stateOnTubeSensor);
  //Serial.println(String(tubeSensor) +  " && " + " digitalRead(" + String(portTubeSensor) + ") == " + String(stateOnTubeSensor) + ")");
  //Serial.println("Tiempo extra Flush: " + String(timeExtraFlush));
  
  digitalWrite(AirProbPort, ReleModule_OFF);

  if(timeExtraFlush > 0){
    timeFlush = millis();
    timeWaitOnAir_aux = millis();

    do{
      if(timeDelayAir > 0){
        if(stateAirManifold){
          if(millis() - lavPort_Aux > 500){
            if(stateAirManifold_aux){
              if(millis() - lavPort_Aux > 1000){
                digitalWrite(LavPort, Rele_ON);
                stateAirManifold_aux = false;
                stateAirManifold = false;
                lavPort_Aux = millis();
              }
            }else{
              digitalWrite(AirProbPort, ReleModule_OFF);
              stateAirManifold_aux = true;
              lavPort_Aux = millis();
            }
          }
        }else{
          if(millis() - timeWaitOnAir_aux > timeWaitOnAir){
            if(millis() - lavPort_Aux > timeDelayAir){
              if(stateLavPort){
                digitalWrite(LavPort, Rele_OFF);
                lavPort_Aux = millis();
                stateLavPort = false;
              }else{
                digitalWrite(AirProbPort, ReleModule_ON);
                timeWaitOnAir_aux = millis();
                stateAirManifold = true;
                lavPort_Aux = millis();
                stateLavPort = true;
              }
            }
          }
        }
      }

      TaskSerial();
      TaskS_Alarma();
      TaskMessageDisplay();
    }while((millis() - timeFlush) < (timeExtraFlush*1000));
  }
  
  digitalWrite(AirProbPort, ReleModule_OFF);
  digitalWrite(SolenoidePort, Rele_OFF);
  digitalWrite(LavPort, Rele_OFF);
  
  //Serial.println("Apaga solenoide y lavadora: " + String(Nlav));

  return true;
}