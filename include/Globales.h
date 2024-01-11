#define TUBE_SENSOR_1_ON LOW 
#define TUBE_SENSOR_2_ON HIGH 

#define Rele_ON HIGH
#define Rele_OFF LOW

#define ReleModule_ON LOW
#define ReleModule_OFF HIGH

#define Alarm_ON HIGH
#define Alarm_OFF LOW

#define OPEN true
#define CLOSE false

#define TXD2 22
#define RXD2 21

byte ZoneStore[20];
bool valueStore[20];
int countStore = -1;

//EspMQTTClient client("sistemadeterquin.ml", 1883, "esp_client", "M4r14M4t3rD3y", "MasterCard");
//----------------------------------------------------
//----------------------------------------------------

byte auxInfoLav_pruga[4];

byte NLavsPendientes = 0;
byte LavsPendientes[20][3];

bool semaphore_memorySpace = true;

bool Dosificando = false;
bool systemOk = true; // variable que indica si el sistema esta listo para dosificar,(falso) si esta ocupado con otra tarea

bool AlamrOn = false;

byte NumDosificaReceive[6][3];
unsigned long TimeDosificaReceive[6];

byte N_SendInfoControl = 0;
byte infoControl[40][5];

//---------------- Task TaskMessageDisplay ---------------
byte index;
unsigned long timeTaskMessageDisplay;

//---------------------- Task Serial ---------------------
String Dato;
int indice;

//---------------------- Task Alarma ---------------------
byte portAlarma;
byte timeAlarma;
byte delayAlarmaInterval;
bool alarmaOn_2 = false;
unsigned long auxAlarmaInter;
bool stateAlarmaAux;
unsigned long delayTimeAlarma;

//---------------------- Task Purga ---------------------
byte purgaValues[4];

unsigned long Rv_Sensor;
bool auxFlowSensorPurga;

byte portPump;
byte portProbeta2;
byte portProbeta1;
byte portFlowSensor;
byte portProduct;
unsigned long timePurgaAux;
unsigned long timeSensorAux;

//--------------------- Task Dosifica ---------------------
byte kgLav;
byte NumFlush;
byte numProduct;
byte auxNumFlush;
byte varDosifica;
byte timeFlushProb;
byte portsSensor[3];
byte timeExtraFlush_1;
byte timeExtraFlush_2;
byte PunteroLavados[3];
byte multiplo_time_max[2];

bool tubeSensor1_aux;
bool Dosificando_2;
bool auxFlowSensor;
bool tubeSensor1;
bool tubeSensor2;
bool flowSensor;
bool displayLDC;
unsigned int Cantidad_dosis;
int topeProbeta[2];
float ml_X_kg;
unsigned long CountLav;
unsigned long rv_dosis;
unsigned long Tiempo_dosis;
unsigned long TiempoDosificacion;
unsigned int Calibrate[3];
String DatoMQTT;

const byte typeData[7][11] = {{2,2,2,2,2,2,2,2,2,2,2},
                              {2,3,3,3,1,0,0,0,0,0,0},
                              {5,1,4,0,0,0,0,0,0,0,0},
                              {2,1,1,0,0,0,0,0,0,0,0},
                              {2,2,2,2,2,6,0,0,0,0,0},
                              {2,2,2,2,2,2,2,2,0,0,0},
                              {1,1,1,1,2,2,3,3,3,1,2}};

const int dirrData[7][11] = {{0, 4, 10, 11, 12, 13, 14, 15, 16, 17, 18},
                             {19, 20, 28, 36, 44, 0, 0, 0, 0, 0, 0},
                             {48, 268, 288, 0, 0, 0, 0, 0, 0, 0, 0},
                             {2208, 2218, 2228, 0, 0, 0, 0, 0, 0, 0, 0},
                             {2428, 2438, 2448, 2458, 2658, 2858, 0, 0, 0, 0, 0},
                             {3658, 3659, 3660, 3661, 3662, 3663, 3664, 3665, 0, 0, 0},
                             {3666, 3670, 3671, 3672, 3673, 3674, 3675, 3683, 3691, 3693, 3694}};

const char *memoryZone[] = {"Ports", "Product", "Form", "Lavs", "RegLvs", "Flush", "Config"};

const char *itemZones[][11] = {{"P-4", "Lv-6", "Alarm", "AirLav", "FlushProb", "AirProb", "Pump", "Solenoide", "FlowSens", "LvProb1", "LvProb2"},
                       {"N_Product", "CalTime_-4", "CalMl_-4", "CalRv_-4", "state_-4", NULL, NULL, NULL, NULL, NULL, NULL},
                       {"Name_-20", "F-20", "F-20/S-6/P-4", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
                       {"Kg_-10", "Lv_-10", "F-20/Lv-10", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
                       {"posForm_-10", "posPaso_-10", "posCount_-10", "F_L-10/_N-20", "P_L-10/_N-20", "Cont_L-10/_N-20", NULL, NULL, NULL, NULL, NULL},
                       {"timeOver", "waitOnAir", "delayAir", "timeFlushProb", "extraFlush_1", "extraFlush_2", "timeAlarm", "delayAlarm", NULL, NULL, NULL},
                       {"flowSensor_-4", "display", "tubeSensor1", "tubeSensor2", "mulTimeFlow", "mulTimeProb", "topeProb1_-4", "topeProb2_-4", "t_DosiRepeat", "repeatStep", "NumFlush"}};
//--------------------------------------------------------------------------------------------------------------------------
//DateTime fechaSistema;





//------------------------------------------------------------------------
//------------------------ Espacio de Nombres ----------------------------
//------------------------------------------------------------------------

//-- Ports ->>    Numero de Puertos de Entrada y Salida
//-- Product ->>  Numero total de productos, Estado de Productos, Nombres de productos, Datos calibracion Productos
//-- Form ->>     Nombres de formulas, Estado de Formulas, Datos de Dosificacion
//-- Lavs ->>     Numero de lavadoras, Estado lavadoras, Estado formulas por lavadoras, Kg lavadoras 
//-- RegLvs ->>   Almacena registros de lavados por lavadora: ultimaFormula, ultimoPaso, conteoDeLavado 
//-- Flush ->>    Datos de flush, duracion
//-- Config ->>   Repite paso, Sensor probeta 1, sensor probeta 2, sensor flujo, tope minimo probeta, tope maximo probeta
//                multiplo tiempo maximo sensor flow, multiplo tiempo maximo dosis, tiempo entre señal dosificacion repetida,
//                Activa envio datos Display