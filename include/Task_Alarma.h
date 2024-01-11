void TaskSerial();
void TaskDosifica();
void TaskMessageDisplay();

void TaskS_Alarma(){
  if(millis() - auxAlarmaInter > (unsigned long)(delayAlarmaInterval*1000)){
    if(stateAlarmaAux){
      stateAlarmaAux = false;
      digitalWrite(portAlarma, Alarm_OFF);
    }else{
      stateAlarmaAux = true;
      digitalWrite(portAlarma, Alarm_ON);
    }
    auxAlarmaInter = millis();
  }
}