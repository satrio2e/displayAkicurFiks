#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include <EthernetSPI2.h>

#include <FS.h>
#include <SPIFFS.h>

#ifndef OTA_h
#define OTA_h

#define namaLine "ESPLine_14"




String ipStr; //Menyimpan data string dari webServer
const char* ipChar; //menyimpan data char untuk dikirim ke SPIFFS yang sudah di convert dari string ipStr
extern String ipBaru;  // Deklarasi eksternal untuk variabel ipBaru
extern IPAddress ipReal;

String namaIp ;
String loginIndex;


IPAddress satrio; //Menyimpan data ip dari form web server
IPAddress IP; //menyimpan data ip Wifi
  
bool otaStarted = false;   // Menandakan apakah OTA sudah dimulai
bool lastOTAState = true; // Untuk deteksi perubahan dari LOW ke HIGH
bool currentState;

// Ganti SSID dan password sesuai kebutuhan
const char* ssid = namaIp.c_str();
const char* password = "12345678";


WebServer server(80);

 //Tampilan Web

  

//

String getNamaIP(IPAddress ipReal) {
  int lastOctet = ipReal[3];  // ambil oktet terakhir

  if (lastOctet >= 230 && lastOctet <= 245) {
    int lineNumber = lastOctet - 221;  // 230 -> 9, 244 -> 23
    char buffer[20];
    sprintf(buffer, "ESP_Line%02d", lineNumber); // format dua digit
    return String(buffer);
  }

  return "Unknown";
}

//membaca ram yang tersisa
 void cekHeap() {
  Serial.println("====== HEAP STATUS ======");
  Serial.printf("Free Heap           : %u bytes\n", ESP.getFreeHeap());
  Serial.printf("Minimum Ever Free   : %u bytes\n", ESP.getMinFreeHeap());
  Serial.printf("Max Allocatable Block: %u bytes\n", ESP.getMaxAllocHeap());
  Serial.println("=========================\n");
 }

//memulai SPIFFS
 void startSPIFFS(){
  if (!SPIFFS.begin(true)) {
    Serial.println("Failed to mount SPIFFS");
    return;
  }
 }
//

//fungsi menulis ke SPIFFS
void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}
//

//Fungsi untuk membaca dari SPIFFS
 void readFile(fs::FS &fs, const char *path, String &outputVar) {
  File file = fs.open(path);
  if (!file || file.isDirectory()) {
    Serial.println("- failed to open file for reading");
    return;
  }

  outputVar = file.readStringUntil('\n');
  outputVar.trim();  // hilangkan spasi atau newline

  file.close();
 }
//

//Fungsi Untuk convert string ip ke IP yang baru
 void convertIP(IPAddress &outputIP, const String &inputStr) {
  if (outputIP.fromString(inputStr)) {
    Serial.println("IP parsed: " + outputIP.toString());
  } else {
    Serial.println("Failed to parse IP from: " + inputStr);
  }
 }
//

void setupOTA() {
  startSPIFFS();

  loginIndex = R"rawliteral(
  <!DOCTYPE html>
  <html lang="id">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>)rawliteral" + namaIp + R"rawliteral( OTA Update</title>
    <style>
      body {
        font-family: 'Helvetica Neue', Arial, sans-serif;
        background-color: #f5f7fa;
        color: #333;
        display: flex;
        flex-direction: column;
        align-items: center;
        padding: 30px 20px;
        margin: 0;
      }
      h2 {
        color: #2c3e50;
        margin-bottom: 10px;
        text-align: center;
      }
      form {
        background: #fff;
        padding: 20px;
        border-radius: 12px;
        box-shadow: 0 4px 8px rgba(0,0,0,0.05);
        margin-bottom: 30px;
        width: 100%;
        max-width: 400px;
        box-sizing: border-box;
      }
      input[type="file"], input[type="text"] {
        width: 100%;
        padding: 10px;
        margin-top: 10px;
        margin-bottom: 20px;
        border: 1px solid #ccc;
        border-radius: 8px;
        box-sizing: border-box;
      }
      input[type="submit"], button {
        background-color: #3498db;
        color: white;
        border: none;
        padding: 10px 15px;
        font-size: 16px;
        border-radius: 8px;
        cursor: pointer;
        width: 100%;
        box-sizing: border-box;
        transition: background-color 0.3s ease;
      }
      input[type="submit"]:hover, button:hover {
        background-color: #2980b9;
      }
      progress {
        width: 100%;
        margin-top: 10px;
        height: 20px;
      }
      p {
        margin-top: 10px;
        font-size: 14px;
        text-align: center;
      }
      hr {
        width: 100%;
        max-width: 400px;
        margin: 40px 0;
        border: none;
        border-top: 1px solid #ccc;
      }

      /* Responsiveness untuk handphone */
      @media (max-width: 480px) {
        body {
          padding: 20px 10px;
        }
        form {
          padding: 15px;
        }
        input[type="submit"], button {
          font-size: 14px;
          padding: 10px;
        }
        progress {
          height: 18px;
        }
      }
    </style>
  </head>
  <body>

    <h2>OTA Firmware Upload</h2>
    <form id="upload_form" method="POST" action="/update" enctype="multipart/form-data">
      <input type="file" name="update" required>
      <input type="submit" value="Upload Firmware">
      <progress id="progressBar" value="0" max="100"></progress>
      <p id="status"></p>
    </form>

    <hr>

    <h2>Set IP Ethernet</h2>
    <form id="ip_form">
      <input type="text" id="new_ip" placeholder="Masukkan IP baru" required>
      <button type="submit">Set IP</button>
      <p id="ip_status"></p>
    </form>

  <script>
    // Handle OTA upload
    const form = document.getElementById('upload_form');
    const progressBar = document.getElementById('progressBar');
    const status = document.getElementById('status');

    form.addEventListener('submit', function(e) {
      e.preventDefault();
      const fileInput = form.querySelector('input[name="update"]');
      const file = fileInput.files[0];
      const xhr = new XMLHttpRequest();

      xhr.upload.addEventListener("progress", function(e) {
        if (e.lengthComputable) {
          const percent = (e.loaded / e.total) * 100;
          progressBar.value = Math.round(percent);
        }
      }, false);

      xhr.upload.addEventListener("load", function(e) {
        status.innerHTML = "Upload complete.";
      }, false);

      xhr.upload.addEventListener("error", function(e) {
        status.innerHTML = "Upload failed.";
      }, false);

      xhr.open("POST", "/update", true);
      const formData = new FormData();
      formData.append("update", file);
      xhr.send(formData);
    });

    // Handle Set IP Form
    const ipForm = document.getElementById('ip_form');
    const ipStatus = document.getElementById('ip_status');

    ipForm.addEventListener('submit', function(e) {
      e.preventDefault();
      const newIP = document.getElementById('new_ip').value;

      fetch('/setip', {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json'
        },
        body: JSON.stringify({ ip: newIP })
      })
      .then(response => response.text())
      .then(data => {
        ipStatus.innerText = "Server Response: " + data;
      })
      .catch(error => {
        ipStatus.innerText = "Error: " + error;
      });
    });
  </script>

  </body>
  </html>
  )rawliteral";

  // mDNS hanya jika dibutuhkan, biasanya tidak terpakai di mode AP
  if (!MDNS.begin("esp32")) {
    Serial.println("Error setting up MDNS responder!");
  }

  // Halaman upload
  server.on("/", HTTP_GET, []() {
    server.send(200, "text/html", loginIndex);
  });

  // Endpoint upload firmware
  server.on("/update", HTTP_POST, []() {
      server.sendHeader("Connection", "close");
      server.send(200, "text/plain", Update.hasError() ? "FAIL" : "OK");
      // delay(1000);   --> INI HAPUS SAJA
      // ESP.restart(); --> INI HAPUS SAJA, supaya tidak restart dua kali
  }, []() {
      HTTPUpload& upload = server.upload();
      if (upload.status == UPLOAD_FILE_START) {
          Serial.printf("Update Start: %s\n", upload.filename.c_str());
          if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
              Update.printError(Serial);
          }
      } else if (upload.status == UPLOAD_FILE_WRITE) {
          if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
              Update.printError(Serial);
          }
      } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) {
              Serial.printf("Update Success: %u bytes\nRebooting...\n", upload.totalSize);
              server.send(200, "text/plain", "OK");
              delay(1500); // kasih waktu ke browser sebelum reboot
              ESP.restart();  // <-- RESTART di sini saja, setelah benar-benar selesai
          } else {
              Update.printError(Serial);
              server.send(500, "text/plain", "Update failed");
          }
      }
  });

server.on("/setip", HTTP_POST, []() {
  String body = server.arg("plain");
  DynamicJsonDocument doc(128);
  DeserializationError error = deserializeJson(doc, body);
  
  if (error) {
    server.send(400, "text/plain", "Invalid JSON");
    return;
  }

  // Ambil nilai dari JSON dan simpan ke variabel satrio
  String dataMasuk = doc["ip"];

  // Cek apakah IP valid
  if (satrio.fromString(dataMasuk)) {
    Serial.print("IP stored in variable satrio: ");
    Serial.println(satrio);

    ipStr = satrio.toString();
    ipChar = ipStr.c_str();

    writeFile(SPIFFS, "/ip.txt", ipChar);
    server.send(200, "text/plain", "IP received and saved to variable 'satrio'");
    
  } else {
    server.send(400, "text/plain", "Invalid IP format");
    }
  });

    // Endpoint untuk menampilkan data variabel makan
  server.on("/ip", HTTP_GET, []() {
    String response = "<html><body><h1>IP</h1><p>IP saat ini adalah: " + ipBaru + "</p></body></html>";
    server.send(200, "text/html", response);  // Kirimkan data ke browser dalam format HTML
  });

  server.begin();
  Serial.println("OTA server ready. Connect to ESP WiFi and upload firmware.");

  // checkFlashSpace();
  cekHeap();
}

void modeOTA(){    
  bool currentTrigger = false;
  // Saat PIN33 HIGH dan OTA belum aktif
  if (!currentTrigger && lastOTAState) {
    namaIp = getNamaIP(ipReal);
    Serial.println("OTA Triggered!");
    WiFi.softAP(namaIp.c_str(), password);
    delay(100);
    setupOTA();
    otaStarted = true;
  }

  // Saat PIN33 kembali LOW → matikan OTA
  if (currentTrigger && !lastOTAState && otaStarted) {
    Serial.println("OTA Deactivated - Turning off AP");
    WiFi.softAPdisconnect(true);        
    otaStarted = false;
  }

  lastOTAState = currentTrigger;
  server.handleClient();
 }

#endif
