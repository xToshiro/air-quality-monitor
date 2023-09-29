void fcsUploadCallback(CFS_UploadStatusInfo info) {
  if (info.status == fb_esp_cfs_upload_status_init) {
    Serial.printf("\nUploading data (%d)...\n", info.size);
  } else if (info.status == fb_esp_cfs_upload_status_upload) {
    Serial.printf("Uploaded %d%s\n", (int)info.progress, "%");
  } else if (info.status == fb_esp_cfs_upload_status_complete) {
    Serial.println("Upload completed ");
  } else if (info.status == fb_esp_cfs_upload_status_process_response) {
    Serial.print("Processing the response... ");
  } else if (info.status == fb_esp_cfs_upload_status_error) {
    Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
  }
}

void fireBaseDataSend() {
  if (Firebase.ready() && (millis() - dataMillis > 25000 || dataMillis == 0)) {
    dataMillis = millis();

    // For the usage of FirebaseJson, see examples/FirebaseJson/BasicUsage/Create.ino
    FirebaseJson content;

    // Note: If new document created under non-existent ancestor documents, that document will not appear in queries and snapshot
    // https://cloud.google.com/firestore/docs/using-console#non-existent_ancestor_documents.

    // We will create the document in the parent path "a0/b?
    // a0 is the collection id, b? is the document id in collection a0.

    String documentPath = "system-1/";
    //+ String(count);

    // If the document path contains space e.g. "a b c/d e f"
    // It should encode the space as %20 then the path will be "a%20b%20c/d%20e%20f"

    // double
    String timeStampFireBase = String(ano) + "-" + String(mes) + "-" + String(dia) + "T" + String(hora) + ":" + String(minuto) + ":" + String(segundo) + "Z";
    content.set("fields/intTemp/doubleValue", temp);
    content.set("fields/Pres/doubleValue", pres);
    content.set("fields/hum/doubleValue", hum);
    content.set("fields/alt/doubleValue", alt);
    content.set("fields/adc0/doubleValue", saveadc0);
    content.set("fields/adc1/doubleValue", saveadc1);
    content.set("fields/adc2/doubleValue", saveadc2);
    content.set("fields/adc3/doubleValue", saveadc3);
    content.set("fields/co2/doubleValue", co2);
    content.set("fields/tvocs/doubleValue", tvoc);
    content.set("fields/pm1/doubleValue", pm1);
    content.set("fields/pm25/doubleValue", pm25);
    content.set("fields/pm10/doubleValue", pm10);
    content.set("fields/extTemp/doubleValue", exTemp);
    content.set("fields/pmsLog/integerValue", pmsLog);
    content.set("fields/adsLog/integerValue", adsLog);
    content.set("fields/rtcLog/integerValue", rtcLog);
    content.set("fields/ccsLog/integerValue", ccsLog);
    content.set("fields/msdLog/integerValue", msdLog);
    content.set("fields/bmeLog/integerValue", bmeLog);
    content.set("fields/erroLog/integerValue", erroLog);
    content.set("fields/moqaID/stringValue", SYSTEM_ID);
    content.set("fields/boardID/stringValue", BOARD_ID);
    content.set("fields/codeID/stringValue", CODE_ID);
    //content.set("fields/redeStat/integerValue", rede);
    //content.set("fields/hora/stringValue", String(hora) + ":" + String(minuto) + ":" + String(segundo));
    //content.set("fields/data/stringValue", String(mes) + "/" + String(dia) + "/" + String(ano));
    content.set("fields/Timestamp/timestampValue", timeStampFireBase);  // RFC3339 UTC "Zulu" format
                                                                        //content.set("fields/Timestamp/timestampValue", String(ano) + "-" + String(mes) + "-" + String(dia) + "T" + String(hora) + ":" + String(minuto) + ":" + String(segundo) + "Z"); // RFC3339 UTC "Zulu" format

    Serial.print("Create a document... ");

    if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "" /* databaseId can be (default) or empty */, documentPath.c_str(), content.raw())) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      digitalWrite(LED_GREEN, HIGH);
    } else {
      Serial.println(fbdo.errorReason());
      digitalWrite(LED_RED, HIGH);
      delay(250);
      digitalWrite(LED_GREEN, LOW);
      errorToReset++;
    }
  }
}