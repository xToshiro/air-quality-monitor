void errorVerify() {
  if (errorToReset == 4) {
    delay(3000);
    errorToReset++;
  } else if (errorToReset > 8) {
    ESP.restart();
  }
}

//Função de sincronia npt para rtc
void getTime() {

  struct tm timeinfo;

  getLocalTime(&timeinfo);

  yr = timeinfo.tm_year + 1900;
  mt = timeinfo.tm_mon + 1;
  dy = timeinfo.tm_mday;
  hr = timeinfo.tm_hour;
  mi = timeinfo.tm_min;
  se = timeinfo.tm_sec;
}

void systemIdMac() {
  Serial.print("\nDefault ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
  WiFi.macAddress(mac);
  sprintf(macString, "%02X%02X%02X", mac[3], mac[4], mac[5]);

  String macAdd = macString;

  SYSTEM_ID = macString;
  hostname = BOARD_ID + CODE_ID + macString + "_wifi";
  Serial.println(SYSTEM_ID);
}

void switchCheck() {
  if (digitalRead(switchPinOne) == HIGH) {
    switchOneState = true;
    Serial.println("1 on");
  }
  if (digitalRead(switchPinTwo) == HIGH) {
    Serial.println("2 on");
    switchTwoState = true;
  }
  if (digitalRead(switchPinThree) == HIGH) {
    Serial.println("3 on");
    switchThreeState = true;
    //oledOn = true;
  }
  if (digitalRead(switchPinFour) == HIGH) {
    Serial.println("4 on");
    switchFourState = true;
  }
}

void rtcVerify() {
  delay(2000);
  Serial.println("estou aqui");
  if (rtc.begin() && (WiFi.status() == WL_CONNECTED)) {
    rtcTimeRefresh();
    //Serial.println("estou aqui 1");
    if (ano < 2020) {
      while (yr < 2020) {
        getTime();
        //Serial.println("estou aqui 2");
        rtcTimeRefresh();
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        //getTime();
        Serial.println(yr);
        //Serial.println("estou aqui 3");

        //Serial.println("estou aqui 3");

        //Serial.println("estou aqui 4");
        //Serial.print("aqui1");
      }
    }
    rtcTimeRefresh();
    if (yr > 2020) {
      while ((yr != ano) && (mt != mes) && (dy != dia) && (hr != hora) && (mi != minuto)) {
        getTime();
        rtc.adjust(DateTime(yr, mt, dy, hr, mi, se));
        rtcTimeRefresh();
        Serial.print("aqui2");
      }
    }
    delay(3000);

  } else if (!rtc.begin() && (WiFi.status() == WL_CONNECTED)) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    getTime();
    delay(3000);
  } else if (rtc.begin() && (WiFi.status() != WL_CONNECTED)) {
    rtcTimeRefresh();
    if (ano < 2022) {
      delay(5000);
      ESP.restart();
    } else {
      delay(3000);
    }
  } else if (!rtc.begin() && (WiFi.status() != WL_CONNECTED)) {
    delay(5000);
    ESP.restart();
  }
}

void rtcTimeRefresh() {
  DateTime now = rtc.now();
  dia = now.day();
  mes = now.month();
  ano = now.year();
  hora = now.hour();
  minuto = now.minute();
}

// Essa função define os pinos dos LEDs que no esp devem ser touch
void switchsDefine() {
  pinMode(switchPinOne, INPUT);
  pinMode(switchPinTwo, INPUT);
  pinMode(switchPinThree, INPUT);
  pinMode(switchPinFour, INPUT);
}
void ledDefine() {
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(LED_YELLOW, LOW);
  digitalWrite(LED_RED, LOW);
  delay(150);
}

void blinkErrors(int longBlinks, int shortBlinks) {
  // Piscadas longas
  for (int i = 0; i < longBlinks; i++) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(500);  // LED fica aceso por 500 ms (meio segundo)
    digitalWrite(LED_RED, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(500);  // LED fica apagado por 500 ms (meio segundo)
  }

  delay(1000);  // Aguarda 1 segundo antes das piscadas curtas

  // Piscadas curtas
  for (int i = 0; i < shortBlinks; i++) {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(200);  // LED fica aceso por 200 ms (um quinto de segundo)
    digitalWrite(LED_RED, LOW);
    digitalWrite(buzzerPin, LOW);
    delay(200);  // LED fica apagado por 200 ms (um quinto de segundo)
  }
}

// Inicia os sensores
void sensorBegin() {
  sensors.begin();
  bme.begin(0x76);  //Sensor I2C Address
  Wire.setClock(100000);
  rtc.begin();
  //dht.begin();
  ads.begin();
  //ccs.begin();
  CCS811.begin();
  pms.init();
}


void sensorTest() {
  pms.read();

  // Testa o DHT
  if (!bme.begin(0x76)) {
    bmeLog = 1;
    digitalWrite(LED_RED, HIGH);
    delay(500);
    Serial.println("temp erro");
    blinkErrors(1, 4);  // 1 piscadas longas, 4 piscadas curtas

  } else {
    bmeLog = 0;
    Serial.println("temp ok");
  }
  delay(1500);

  // Testa o PMS
  if (pms) {
    pmsLog = 0;
    Serial.println("mp ok");
  } else {
    pmsLog = 1;
    digitalWrite(LED_RED, HIGH);
    delay(500);
    Serial.println("mp erro");
    blinkErrors(2, 3);
  }
  delay(1500);

  // Testa o ADS1115
  if (!ads.begin()) {
    adsLog = 1;
    digitalWrite(LED_RED, HIGH);
    delay(500);
    Serial.println("gas erro");
    blinkErrors(3, 2);
  } else {
    adsLog = 0;
    Serial.println("gas ok");
  }
  delay(1500);

  // Testa o modulo de relogio de tempo real ou RTC
  if (!rtc.begin()) {
    rtcLog = 1;
    digitalWrite(LED_RED, HIGH);
    delay(500);
    Serial.println("relogio erro");
    blinkErrors(4, 1);
  } else {
    rtcLog = 0;
    Serial.println("relogio ok");
  }
  delay(1500);

  // Testa o sensor de TVOCs
  if (CCS811.checkDataReady() == true) {
    ccsLog = 0;
    Serial.println("ccs ok");
  } else {
    ccsLog = 1;
    digitalWrite(LED_RED, HIGH);
    delay(500);
    Serial.println("ccs erro");
    blinkErrors(0, 5);
  }
  delay(1000);

  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  delay(500);
  if (temperatureC < 0) {
    exTempStatus = false;
  } else {
    exTempStatus = true;
    delay(1500);
  }




  // Refaz uma verificação rapida final
  if (bme.begin(0x76) && (pms) && ads.begin() && rtc.begin() && (!CCS811.begin() == 1)) {
    digitalWrite(LED_RED, LOW);
    erroLog = 0;
  } else {
    digitalWrite(LED_RED, HIGH);
    erroLog = 1;
  }
}