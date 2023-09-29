// Gerenciamento de wifi e conexões
#include <WiFi.h>

// Gerenciamento de IOs
#include <SoftwareSerial.h>
#include <Wire.h>
#include <OneWire.h>  // Biblioteca para sensores de comunicação por um fio

// Gerenciemento do relogio
#include "time.h"
#include <RTClib.h>

// Gerenciamento do cartão de memoria
#include "FS.h"
#include "SD.h"
#include <SPI.h>
// Gerenciamento de sensores

#include "DFRobot_CCS811.h"  // Em uso para permitir a verificação de desconexão do sensor
#include <Adafruit_ADS1X15.h>
#include <PMserial.h>
#include <DallasTemperature.h>  // para o DS18B20


#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Dependencias firebase
#include <Firebase_ESP_Client.h>
// Provide the token generation process info.
#include <addons/TokenHelper.h>
#include <esp_task_wdt.h>

//#define WIFI_SSID "Borg_Wifi-Backup"
//#define WIFI_PASSWORD "hqeF46K8H41TAu"

#define WIFI_SSID "wifi-ssid"
#define WIFI_PASSWORD "wifi-password"

/* 2. Define the API Key */
#define API_KEY "api-key-firebase"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "project-id-firebase"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "email"
#define USER_PASSWORD "password"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

//Pino dos sensores OneWire
const int oneWireBus = 4;

//Pinos dos switchs
const int switchPinOne = 32;    // Switch de wifi
const int switchPinTwo = 33;    // Switch do Oled
const int switchPinThree = 27;  // Sem configuração
const int switchPinFour = 13;   // Switch de configuração da rede wifi

// Definição dos pinos de Led
const int LED_BLUE = 15;    // Led de Wifi
const int LED_RED = 12;     // Led de erros
const int LED_YELLOW = 14;  // Led de gravação de dados
const int LED_GREEN = 2;    // Led de envio de dados

//Pino buzzer
const int buzzerPin = 25;

//Definição dos pinos de TX2 e RX2 para conexão do PMS
#if !defined(PMS_RX) && !defined(PMS_TX)
constexpr auto PMS_RX = 16;
constexpr auto PMS_TX = 17;
#endif

#define SEALEVELPRESSURE_HPA (1013.25)

// Configura o OneWire para comunicar com todos os dispositivos OneWire
OneWire oneWire(oneWireBus);
// Passa a referencia do onewire para o dallas temperature sensor
DallasTemperature sensors(&oneWire);
RTC_DS1307 rtc;  // Definição do relógio de tempo real
//Adafruit_CCS811 ccs;
DFRobot_CCS811 CCS811;  // Definição do sensor de CO2 e TVOCs
//DHT dht(DHTPIN, DHTTYPE); //Definição do DHT
//SHT31 sht; // Definição SHT30
Adafruit_ADS1115 ads;                   // Definição do conversor analogico digital ADS1115 que lê os sensores analogicos
SerialPM pms(PMS3003, PMS_RX, PMS_TX);  // Definição do sensor pms(PMSx003, Serial), onde x é o modelo do sensor
Adafruit_BME280 bme;

// NTP server to request time
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;
const int daylightOffset_sec = -3600 * 3;

uint8_t mac[6];
char macString[7];

String BOARD_ID = "3.2";
String CODE_ID = "394";
String SYSTEM_ID;

String hostname;  // Define o nome da placa na rede Wifi para facilitar a identificação

// Definição de variaveis de status de funcionamento dos sensores, aqui são guardados os status de erros do sistema
// True é funcionando e false é com problemas
bool pmsStatus = true;
bool adsStatus = true;
bool bmeStatus = true;
bool rtcStatus = true;
bool ccsStatus = true;
bool msdStatus = true;

bool onlineRTC = false;
bool offlineRTC = false;

bool exTempStatus = true;

//Variaveis dos switchs
int switchOneState = true;
int switchTwoState = true;
int switchThreeState = false;
int switchFourState = false;

//Erros ate reset
int errorToReset = 0;

// Definição de codigo de erro dos sensores, aqui são guardados os status de erro durante o funcionamento do sistema
// Codigo 0 = Sem erros
// Codigo 1 =  Erro de inicialização
// Codigo 2 = Erro de Leitura
int pmsLog = 0;
int adsLog = 0;
int bmeLog = 0;
int rtcLog = 0;
int ccsLog = 0;
int msdLog = 0;
int erroLog = 0;

const float multiplier = 0.1875F;

//Firebase count
unsigned long dataMillis = 0;

// Variaveis de leitura dos sensores
//BME
float temp = 0;
float hum = 0;
float pres = 0;
float alt = 0;

//ADS1115
float NO2 = 0;
int16_t saveadc0 = 0;

float NH3 = 0;
int16_t saveadc1 = 0;

float CO = 0;
int16_t saveadc2 = 0;

float O3 = 0;
int16_t saveadc3 = 0;

//CCS881
float co2 = 0;
float readco2 = 0;
float readtvoc = 0;
float tvoc = 0;


//PMS
float pm1 = 0;
float pm25 = 0;
float pm10 = 0;

//Leitura do relogio
unsigned long dia = 0;
unsigned long mes = 0;
unsigned long ano = 0;
unsigned long hora = 0;
unsigned long minuto = 0;
unsigned long segundo = 0;

//Variaveis de npt
int yr = 0;
int mt = 0;
int dy = 0;
int hr = 0;
int mi = 0;
int se = 0;


//Sensores opcionais
float exTemp = 0;

//Devinição da mensagem a ser gravada no SDCard
String dataMessage;

float tempSum = 0;
float exTempSum = 0;
float humSum = 0;
float presSum = 0;
float altSum = 0;
float pm1Sum = 0;
float pm25Sum = 0;
float pm10Sum = 0;
float saveadc0Sum = 0;
float saveadc1Sum = 0;
float saveadc2Sum = 0;
float saveadc3Sum = 0;
float co2Sum = 0;
float tvocSum = 0;

float pmsLogSum = 0;
float adsLogSum = 0;
float ccsLogSum = 0;
float bmeLogSum = 0;

int bmeSampleCount = 0;
int pmsSampleCount = 0;
int adcSampleCount = 0;
int ccsSampleCount = 0;
int exTempSampleCount = 0;

hw_timer_t* timer = NULL;  //faz o controle do temporizador (interrupção por tempo)

#define WDT_TIMEOUT 35

//função que o temporizador irá chamar, para reiniciar o ESP32
void IRAM_ATTR resetModule() {
  //ets_printf("(watchdog) reiniciar\n"); //imprime no log
  ESP.restart();  //reinicia o chip
}

void setup() {

  Serial.begin(115200);

  //hw_timer_t * timerBegin(uint8_t num, uint16_t divider, bool countUp)
  /*
       num: é a ordem do temporizador. Podemos ter quatro temporizadores, então a ordem pode ser [0,1,2,3].
      divider: É um prescaler (reduz a frequencia por fator). Para fazer um agendador de um segundo, 
      usaremos o divider como 80 (clock principal do ESP32 é 80MHz). Cada instante será T = 1/(80) = 1us
      countUp: True o contador será progressivo
    */
  timer = timerBegin(0, 80, true);  //timerID 0, div 80
  //timer, callback, interrupção de borda
  timerAttachInterrupt(timer, &resetModule, true);
  //timer, tempo (us), repetição
  timerAlarmWrite(timer, 35000000, true);
  timerAlarmEnable(timer);  //habilita a interrupção

  esp_task_wdt_init(WDT_TIMEOUT, true);  //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL);                //add current thread to WDT watch

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
  multi.addAP(WIFI_SSID, WIFI_PASSWORD);
  multi.run();
#else
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
#endif

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
    if (millis() - ms > 10000)
      break;
#endif
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // The WiFi credentials are required for Pico W
  // due to it does not have reconnect feature.
#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
  config.wifi.clearAP();
  config.wifi.addAP(WIFI_SSID, WIFI_PASSWORD);
#endif

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

#if defined(ESP8266)
  // In ESP8266 required for BearSSL rx/tx buffer for large data handle, increase Rx size as needed.
  fbdo.setBSSLBufferSize(2048 /* Rx buffer size in bytes from 512 - 16384 */, 2048 /* Tx buffer size in bytes from 512 - 16384 */);
#endif

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

  // For sending payload callback
  // config.cfs.upload_callback = fcsUploadCallback;
  Serial.print("Iniciando sistema");
  systemIdMac();    // Gera o id das placas
  switchsDefine();  // Define os switchs
  ledDefine();      // Define os pinos dos leds
  //ledTest(); // Testa todos os leds
  switchCheck();  // Verifica a posição dos botões
  // Chama funções em systemGlobals
  sensorBegin();  // Inicia comunicação com os sensores

  sensorTest();
  initSDCard();  // Inicia o teste dos sensores e inicia o SDcard

  // Chama funções em sdCard
  checkSDFile();  // Checa o arquivo data.csv no cartão de memoria ou cria se ele não existir

  rtcVerify();

  // Chama funções em oledScreens
  //nowTimeDate(); // Mostra a data e a hora atual no modulo RTC
}

void loop() {

  timerWrite(timer, 0);
  esp_task_wdt_reset();
  dataRead();          // Lê e novos dados nos sensores
  saveData();          // Salva os dados
  fireBaseDataSend();  // Envia os dados

  if (errorToReset > 3) {
    errorVerify();
  }
}
