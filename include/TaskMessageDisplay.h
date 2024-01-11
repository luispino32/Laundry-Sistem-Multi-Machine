
void TaskMessageDisplay(){
  index = 0;

  if(N_SendInfoControl > 0){
    if(millis() - timeTaskMessageDisplay > 5000){
      if(index < N_SendInfoControl){
        Serial3.println("Mqtt:Out/MasterControl*infoLav/" + 
                                String(infoControl[index][0]) + "/" + 
                                String(infoControl[index][1]) + "/" + 
                                String(infoControl[index][2]) + "/" +
                                String(infoControl[index][3]));
        if(infoControl[index][4] == 3){
          for(byte i=index; i<N_SendInfoControl; i++){
            if((i+1) < N_SendInfoControl){
              infoControl[i][0] = infoControl[i+1][0];
              infoControl[i][1] = infoControl[i+1][1];
              infoControl[i][2] = infoControl[i+1][2];
              infoControl[i][3] = infoControl[i+1][3];
              infoControl[i][4] = infoControl[i+1][4];
            }
          }
          infoControl[N_SendInfoControl-1][4] = 0;
          N_SendInfoControl--;
        }else{
          index = index >= (N_SendInfoControl-1) ? 0 : index+1;
        }
        infoControl[index][4]++;
      }
    }

    timeTaskMessageDisplay = millis();
  }
}