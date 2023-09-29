void dataRead() {

  const unsigned long duration = 25000;  // Duração da leitura em milissegundos (10 segundos)
  unsigned long startTime3 = millis();   // Tempo inicial da leitura

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

  while (millis() - startTime3 < duration) {
    digitalWrite(LED_YELLOW, HIGH);
    Serial.println(".");
    bmeRead();
    pmsRead();
    adsRead();
    ccsRead();

    if (exTempStatus == true) {
      exTempRead();
    }

    //delay(100);
    digitalWrite(LED_YELLOW, LOW);
  }

  dataCalc();
  rtcRead();

  // Verificação final dos Status dos sensores e controle do led de erro
  if (bmeStatus == true && pmsStatus == true && adsStatus == true && rtcStatus == true && ccsStatus == true) {
    digitalWrite(LED_RED, LOW);
    erroLog = 0;
  } else {
    digitalWrite(LED_RED, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(150);
    digitalWrite(buzzerPin, LOW);
    erroLog = 2;
  }
}

void dataCalc() {
  //BME
  temp = (tempSum != 0) ? tempSum / bmeSampleCount : 0.0;
  hum = (humSum != 0) ? humSum / bmeSampleCount : 0.0;
  pres = (presSum != 0) ? presSum / bmeSampleCount : 0.0;
  alt = (altSum != 0) ? altSum / bmeSampleCount : 0.0;

  //ADS
  saveadc0 = (saveadc0Sum != 0) ? saveadc0Sum / adcSampleCount : 0.0;
  saveadc1 = (saveadc1Sum != 0) ? saveadc1Sum / adcSampleCount : 0.0;
  saveadc2 = (saveadc2Sum != 0) ? saveadc2Sum / adcSampleCount : 0.0;
  saveadc3 = (saveadc3Sum != 0) ? saveadc3Sum / adcSampleCount : 0.0;

  //CCS881
  co2 = (co2Sum != 0) ? co2Sum / ccsSampleCount : 0.0;
  tvoc = (tvocSum != 0) ? tvocSum / ccsSampleCount : 0.0;

  //PMS
  pm1 = (pm1Sum != 0) ? pm1Sum / pmsSampleCount : 0.0;
  pm25 = (pm25Sum != 0) ? pm25Sum / pmsSampleCount : 0.0;
  pm10 = (pm10Sum != 0) ? pm10Sum / pmsSampleCount : 0.0;

  if (exTempStatus == true) {
    exTemp = (exTempSum != 0) ? exTempSum / exTempSampleCount : 0.0;
  } else {
    exTemp = 0.0;
  }

  //Logs
  pmsLog = pmsLogSum;
  adsLog = adsLogSum;
  bmeLog = bmeLogSum;
  ccsLog = ccsLogSum;
}

void bmeRead() {
  if (!bme.begin(0x76)) {
    bmeLogSum += 1;
    bmeStatus = false;
    //Serial.print("DHT ERROR! ");
  } else {
    tempSum += bme.readTemperature();
    humSum += bme.readHumidity();
    presSum += bme.readPressure();
    altSum += bme.readAltitude(SEALEVELPRESSURE_HPA);
    bmeStatus = true;
    bmeSampleCount++;
    //Serial.print("SHT OK! ");
  }
}

void pmsRead() {
  pms.read();
  if (pms) {
    pm1Sum += (pms.pm01);
    pm25Sum += (pms.pm25);
    pm10Sum += (pms.pm10);
    pmsStatus = true;
    pmsSampleCount++;
    //Serial.print("PMS OK! ");
  } else {
    pmsLogSum += 1;
    pmsStatus = false;
    //Serial.print("PMS ERROR! ");
  }
}

void adsRead() {
  if (!ads.begin()) {
    //NO2 = 0;
    //NH3 = 0;
    //CO = 0;
    //O3 = 0;
    adsLogSum += 1;
    adsStatus = false;
    //Serial.print("ADS ERROR! ");
  } else {
    adsStatus = true;
    saveadc0Sum += (ads.readADC_SingleEnded(0));
    saveadc1Sum += (ads.readADC_SingleEnded(1));
    saveadc2Sum += (ads.readADC_SingleEnded(2));
    saveadc3Sum += (ads.readADC_SingleEnded(3));
    adcSampleCount++;
    //NO2 = (saveadc0 * multiplier)/1000;
    //NH3 = (saveadc1 * multiplier)/1000;
    //CO = (saveadc2 * multiplier)/1000;
    //O3 = (saveadc3 * multiplier)/1000;
  }
}

void ccsRead() {
  if (CCS811.checkDataReady() == true) {
    ccsStatus = true;
    co2Sum += (CCS811.getCO2PPM());
    tvocSum += (CCS811.getTVOCPPB());
    ccsSampleCount++;
    //Serial.print("CCS OK! ");
    CCS811.writeBaseLine(0x447B);
  } else {
    ccsLogSum += 1;
    ccsStatus = false;
    //Serial.print("CCS ERROR! ");
  }
}

void exTempRead() {
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  if (temperatureC > -10) {
    exTempSum += temperatureC;
    exTempSampleCount++;
    if (temperatureC > 70) {
      Serial.print("Temperatura maior que 70");
      Serial.println(temperatureC);
    }
  } else {
  }
}


void rtcRead() {
  DateTime now = rtc.now();
  if (!rtc.begin()) {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
      dia = 0;
      mes = 0;
      ano = 0;
      hora = 0;
      minuto = 0;
      segundo = 0;
      rtcLog = 2;
      rtcStatus = false;
      Serial.println("Failed to obtain time");
    } else {
      ano = timeinfo.tm_year + 1900;
      mes = timeinfo.tm_mon + 1;
      dia = timeinfo.tm_mday;
      hora = timeinfo.tm_hour;
      minuto = timeinfo.tm_min;
      segundo = timeinfo.tm_sec;
    }
    //Serial.print("RTC ERROR! ");
  } else {
    rtcLog = 0;
    rtcStatus = true;
    dia = now.day();
    mes = now.month();
    ano = now.year();
    hora = now.hour();
    minuto = now.minute();
    segundo = now.second();
    //Serial.print("RTC OK! ");
  }
}