
 #include "OTA.h"
 #include "fontBitmap27.h"
 #include "fontBitmap22.h"
 #include "fontBitmap20.h"
 #include <SPI.h>
 #include <EthernetSPI2.h>
 #include <EEPROM.h>
 #include "MgsModbus.h"
 #define EEPROM_SIZE 255

 #include <Arduino.h>
 #include <ESP32-HUB75-VirtualMatrixPanel_T.hpp>
 #include <Fonts/big_noodle_titling15pt7b.h>
 #include <Fonts/SF_Willamette14pt7b.h>
 #include <Fonts/SF_Willamette16pt7b.h>
 #include <Fonts/SF_Willamette20pt7b.h>
 #include <Fonts/SF_Willamette27pt7b.h>
 #include <Fonts/TT_Interphases_Pro_Trial_Bold7pt7b.h>
 #include <Fonts/bBerpatroliSelamanya4027pt7b.h>
 
 #define PANEL_RES_X     64     // Number of pixels wide of each INDIVIDUAL panel module. 
 #define PANEL_RES_Y     32     // Number of pixels tall of each INDIVIDUAL panel module.
 
 #define VDISP_NUM_ROWS      3 // Number of rows of individual LED panels 
 #define VDISP_NUM_COLS      2 // Number of individual LED panels per row
 
 #define PANEL_CHAIN_LEN     (VDISP_NUM_ROWS*VDISP_NUM_COLS)  // Don't change
 
 #define PANEL_CHAIN_TYPE 	CHAIN_BOTTOM_RIGHT_UP_ZZ
 
 #define PANEL_SCAN_TYPE  FOUR_SCAN_32PX_HIGH
 
 MatrixPanel_I2S_DMA *dma_display = nullptr;
 VirtualMatrixPanel_T<PANEL_CHAIN_TYPE>* virtualDisp = nullptr;

//ethernet setting
  MgsModbus Mb;

  byte mac[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x21 };
  IPAddress ip1(192, 168, 99, 195);
  IPAddress gateway1(192, 168, 99, 1);
  IPAddress subnet1(255, 255, 255, 0);
//

//Untuk Millis
 unsigned long previousMillis = 0;  // Waktu sebelumnya
 const int interval = 500;  // Interval pergantian teks (1 detik)
 unsigned long previousMillis1 = 0;  // Menyimpan waktu terakhir
 unsigned long interval1 = 10000;  // Interval untuk berganti teks (10 detik)
 const unsigned long defaultInterval1 = 10000; // Simpan default juga
//

//definisi
 bool isOTAActive = false;  // flag masuk ke mode OTA dengan data dari MB.MbData [8]
 bool displayOutputFlag = false;
 bool forceRedraw = false;
 bool forceRedrawMode = false;
 bool run = false;

 String ipBaru; //Menyimpan data string ipBaru yang di baca dari readFile SPIFFS

 IPAddress ipReal;//Menyimpan data ip yang sudah diconvert dari ipBaru

 int yTipe = 35;
 int yRecipe = 35;
 int yPerform = 57;
 int yOutput = 89;

 int currentText = 0;  // Menyimpan teks yang sedang ditampilkan 
 int tipeLen; //Menyimpan panjang Teks Tipe
 int tipeLastLen; //Menyimpan panjang Teks Tipe Prev untuk perbandingan
 int recipeLen; //Menyimpan panjang Teks Recipe
 int recipeLastLen; //Menyimpan panjang Teks Recipe Prev untu perbandingan
 int mbPerform; //Menyimpan data masuk Perform untuk flag
 int mbPerformLast; //Menyimpan data masuk perform untuk flag perbandingan
 int performLen; //Menyimpan panjang Teks perform
 int performLastLen; //Menyimpan panjang Teks perform Prev untuk perbandingan
 int outputLen; //Menyimpan panjang Teks output
 int outputLastLen; //Menyimpan panjang Teks output prev untuk perbandingan
 int nilaiUtamaLen;
 int nilaiUtamaLastlen;
 int performSetupLen;
 int performSetupLenLast;
 int o = Mb.MbData[7]; //Menyimpan data output dari Mb.MbData[7]
 int oLast; //Menyimpan data output dari Mb.MbData[7] prev untuk perbandingan
 int ip10, ip20, ip30, ip40;
 int e = 0;
 int f = 0;
 int i ;
 int iLast;
 float nilaiPerformSetup;
 int nilaiPerformSetupLen;
 int nilaiPerformSetupLenLast;
 int ethernetStatus = 0;
 int l;
 int mode;
 int modeLast;
 int menit;
 int menitLast;
 int jam;
 int jamLast;
 int b;
 int bLast;
 int row1Len;
 int row1LastLen;
 int row2Len;
 int row2LastLen;
 int mbWarnaRow1;
 int mbWarnaRow2;


 char tipe[20]; //Menyimpan data char tipe yang sudah di olah
 char tipeLast[20]; //Menyimpan data char tipe yang sudah di olah untuk perbandingan
 char recipe[20]; //Menyimpan data char recipe yang sudah di olah
 char recipeLast[20]; //Menyimpan data char recipe yang sudah di olah
 char row1[20]; //Menyimpan data char recipe yang sudah di olah
 char row2[20]; //Menyimpan data char recipe yang sudah di olah
 char row1Last[20]; //Menyimpan data char recipe yang sudah di olah
 char row2Last[20]; //Menyimpan data char recipe yang sudah di olah untuk perbandingan
 char perform[20]; //Menyimpan data char perform yang sudah di olah
 char performLast[20]; //Menyimpan data char perform yang sudah di olah untuk perbandingan
 char output[20]; //Menyimpan data char output yang sudah di olah
 char outputLast[20]; //Menyimpan data char output yang sudah di olah untuk perbandingan
 char ipModbus[15];
 char performSetup[20];
 char nilaiPerformSetupStr[8];
 char nilaiPerformSetupStrLast[8];
 char nilaiUtamaStr[8];
 char nilaiUtamaStrLast[8];
 char perfTulisan[8] = "PERF";
 char perfPersen[2] = "%";
 char outTulisan[12] = "OUT";
 char outputFull[12];
 char outputFullLast[12];
 char status[20];
 char statusLast[20];
 char waktuUtama[20];
 char waktuUtamaLast[20];
//

//List Tipe PIPA
 const char* tipeList[] = {
  "AW 1/2\" 4M",
  "AW 1/2\" 5.8M",
  "AW 1\" 4M",
  "AW 1\" 5.8M",
  "AW 1 1/4\" 4M",
  "AW 1 1/4\" 5.8M",
  "AW 1 1/2\" 4M",
  "AW 1 1/2\" 5.8M",
  "AW 2\" 4M",
  "AW 2\" 5.8M",
  "AW 2 1/2\" 4M",
  "AW 2 1/2\" 5.8M",
  "AW 3\" 4M",
  "AW 3\" 5.8M",
  "AW 3/4\" 4M",
  "AW 3/4\" 5.8M",
  "AW 4\" 4M",
  "AW 4\" 5.8M",
  "AW 5\" 4M",
  "AW 5\" 5.8M",
  "AW 6\" 4M",
  "AW 6\" 5.8M",
  "AW 8\" 4M",
  "AW 8\" 5.8M",
  "D 1 1/4\" 4M",
  "D 1 1/4\" 5.8M",
  "D 1 1/2\" 4M",
  "D 1 1/2\" 5.8M",
  "D 2\" 4M",
  "D 2\" 5.8M",
  "D 2 1/2\" 4M",
  "D 2 1/2\" 5.8M",
  "D 3\" 4M",
  "D 3\" 5.8M",
  "D 4\" 4M",
  "D 4\" 5.8M",
  "D 5\" 4M",
  "D 5\" 5.8M",
  "D 6\" 4M",
  "D 6\" 5.8M",
  "D 8\" 4M",
  "D 8\" 5.8M",
  "D 5 1/2\" 4M",
  "D 5 1/2\" 5.8M"
 };
//

//
const char* statusList[] = {
  "NO USE",
  "TRIAL",
  "NO ORDER",
  "NO MAT",
  "PM 1500",
  "PM 3000",
  "PM 6000",
  "PM 12000",
  "PM ADD",
  "TBM DIES",
  "BD MT",
  "BD EXT",
  "BD VCU-ECU",
  "BD PRI",
  "BD HO",
  "BD CCM",
  "BD BE",
  "BD BUND",
  "BD STR",
  "BD NO OPR",
  "BD DIES",
  "BD QUALITY",
  "CHANGE",
  "SAWBLADE",
  "WAIT PART",
  "WAIT LORRY",
  "NO UTILITY",\
  "MATERIAL"
};
//

//fungsi memulai ethernet
 void startEthernet(){
  SPI.begin(14, 12, 13, 27);
  Ethernet.init(27);
  Ethernet.begin(mac, ipReal, gateway1, subnet1); //ubah ipReal jika tidak ingin mendapatkan ip dari OTA
 }
//

//
 void modeRun(){
  mode = Mb.MbData[15];

  if(mode == 0 && !run){
    run = true;
  } 
  if (mode == 1 && run) {
    run = false;
  }

  if (mode != modeLast){
    virtualDisp->clearScreen();

    modeLast = mode;
  }
  

 }
//
//fungsi mengubah tipe
 void tipeMb(){
  uint8_t code = Mb.MbData[0]-1; // misal 13

  if (code < sizeof(tipeList) / sizeof(tipeList[0])) {
   strncpy(tipe, tipeList[code], sizeof(tipe));
   tipe[sizeof(tipe)-1] = '\0'; 
  } else {
   strcpy(tipe, "Unknown");
  }
 }
//

//fungsi mengubah status
 void statusMb(){
  uint8_t code = Mb.MbData[16]-1; // misal 13
  b = Mb.MbData[16]-1;
  b = bLast;

  if (code < sizeof(statusList) / sizeof(statusList[0])) {
   strncpy(status, statusList[code], sizeof(status));
   tipe[sizeof(status)-1] = '\0'; 
  } else {
   strcpy(status, "Unknown");
  }
 }
//

//Fungsi convert modbus ke ascii untuk membuat recipe
 void convertASCII(char* buffer, int startIndex, int endIndex) {
  int idx = 0;

  for (int i = startIndex; i <= endIndex; i++) {
   uint16_t val = Mb.MbData[i];
   buffer[idx++] = val & 0xFF;
   buffer[idx++] = (val >> 8) & 0xFF; 
  }
  buffer[idx] = '\0';
 }
//

//fungsi waktu status
 void waktuMode(){
  jam = Mb.MbData[17];
  menit = Mb.MbData[18];

  menitLast = menit;
  jamLast = jam;

  sprintf(waktuUtama, "%02d : %02d", jam, menit);
 }
//

//fungsi mengambil data performance dari MbData
 void performMb(){
  mbPerform = Mb.MbData[6];

  float nilaiUtama = mbPerform/10.0;
  String nilaiUtamaString = String(nilaiUtama, 1);

  sprintf(nilaiUtamaStr, "%s", nilaiUtamaString);
  sprintf(perform, "PR %s%%", nilaiUtamaString);
 }
//

//Fungsi mengambil data output dari mbData
 void outputMb(){
  o = Mb.MbData[7];

  sprintf(output, "%d", o);
  sprintf(outputFull, "%d KG/H", o);
 }
//

//////////////////////////////////////////////////////////////////////////////////////////////////
//fungsi warna tipe
 uint16_t warnaTipe(){
  if (f <= 24){
   return virtualDisp->color565(93, 226, 231);
  }
  if (f >= 25){
   return virtualDisp->color565(255, 0, 0); //74
  }
 }
//

//Fungsi mengubah Warna Perform
 uint16_t warnaPerform(){
  if (mbPerform <= 899){
   return virtualDisp->color565(255, 0, 0);
  }
  if (mbPerform >= 900){
   return virtualDisp->color565(74, 252, 2); //74
  }
 }
//

//Fungsi mengubah Warna output
 uint16_t warnaOutput(){
  if (mbPerform <= 200){
   return virtualDisp->color565(255, 0, 0);
  }
  if (mbPerform >= 201){
   return virtualDisp->color565(74, 252, 2); //74
  }
 }
//

//fungsi menampilkan ke display
 void displayText(char* input, uint16_t inputLen, uint16_t x, uint16_t y, uint16_t color, const GFXfont* font){
  virtualDisp->setFont(font);
  virtualDisp->setTextColor(color);  // warna background
  virtualDisp->setCursor(x - (inputLen/2), y);
  virtualDisp->print(input);
 }
//

//Fungsi menampilkan Tipe ke Display
 void displayTipe(){
  f = Mb.MbData[0];

  if(f == 12){
   displayText(tipe, tipeLen, 63, yTipe, warnaTipe(), &SF_Willamette14pt7b);
   if (strcmp(tipe, tipeLast) != 0 || tipeLen != tipeLastLen){
    displayText(tipeLast, tipeLastLen, 63, yTipe, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
    displayText(tipe, tipeLen, 63, yTipe, warnaTipe(), &SF_Willamette14pt7b);

    strcpy(tipeLast, tipe);
    tipeLastLen = tipeLen;
   }
  } else {
    displayText(tipe, tipeLen, 63, yTipe, warnaTipe(), &SF_Willamette16pt7b);
    if (strcmp(tipe, tipeLast) != 0 || tipeLen != tipeLastLen){
     displayText(tipeLast, tipeLastLen, 63, yTipe, virtualDisp->color565(0, 0, 0), &SF_Willamette14pt7b);
     displayText(tipeLast, tipeLastLen, 63, yTipe, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
     displayText(tipe, tipeLen, 63, yTipe, warnaTipe(), &SF_Willamette16pt7b);

     strcpy(tipeLast, tipe);
     tipeLastLen = tipeLen;
    }
  }
 }
//

//Fungsi menampilkan recipe ke Display
 void displayRecipe(){
  displayText(recipe, recipeLen, 63, yRecipe, virtualDisp->color565(255, 255, 255), &SF_Willamette16pt7b);

  if(strcmp(recipe, recipeLast) != 0 || recipeLen != recipeLastLen){
   displayText(recipeLast, recipeLastLen, 63, yRecipe, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
   displayText(recipe, recipeLen, 63, yRecipe, virtualDisp->color565(255, 255, 255), &SF_Willamette16pt7b);

   strcpy(recipeLast, recipe);
   recipeLastLen = recipeLen;
  }
 }
//

//Fungsi menampilkan recipe ke Display
 void displayRow1(){
  displayText(row1, row1Len, 63, yRecipe, warnaRow1(), &SF_Willamette16pt7b);

  if(strcmp(row1, row1Last) != 0 || row1Len != row1LastLen){
   displayText(row1Last, row1LastLen, 63, yRecipe, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
   displayText(row1, row1Len, 63, yRecipe, warnaRow1(), &SF_Willamette16pt7b);

   strcpy(row1Last, row1);
   row1LastLen = row1Len;
  }
 }
//

//Fungsi menampilkan perform ke display
 void displayPerform() {
   bool isLenChanged = (performLastLen != performLen);
   bool isMbChanged = (mbPerformLast != mbPerform);

   if (isLenChanged) {
     // Hapus semua teks lama (perfTulisan, nilai, persen)
     displayText(perfTulisan, performLastLen, 66, yPerform, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
     displayText(nilaiUtamaStrLast, performLastLen, 66 + 45, yPerform, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
     displayText(perfPersen, performLastLen, 66 + 48 + nilaiUtamaLastlen, yPerform, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);

     // Tampilkan teks baru
     displayText(perfTulisan, performLen, 66, yPerform, warnaPerform(), &SF_Willamette16pt7b);
     displayText(nilaiUtamaStr, performLen, 66 + 45, yPerform, warnaPerform(), &SF_Willamette16pt7b);
     displayText(perfPersen, performLen, 66 + 48 + nilaiUtamaLen, yPerform, warnaPerform(), &SF_Willamette16pt7b);
   } else if (isMbChanged) {
     // Hanya hapus dan tampilkan nilai (angka) saja
     displayText(nilaiUtamaStrLast, performLen, 66 + 45, yPerform, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
     displayText(nilaiUtamaStr, performLen, 66 + 45, yPerform, warnaPerform(), &SF_Willamette16pt7b);
   }

    // Update variabel penyimpanan terakhir
   if (isLenChanged || isMbChanged) {
     mbPerformLast = mbPerform;
     strcpy(performLast, perform);
     strcpy(nilaiUtamaStrLast, nilaiUtamaStr);
     performLastLen = performLen;
     nilaiUtamaLastlen = nilaiUtamaLen;
   }
 }
//

//Fungsi menampilkan Output ke Display
 void displayOutput(){
  bool isLenChanged = (outputLen != outputLastLen);
  bool isMbChanged = (o != oLast);

  //fungsi menampilkan output pertama kali
  if (displayOutputFlag) {
   displayText(outTulisan, outputLen + 23, 62, yOutput, virtualDisp->color565(255, 255, 255), &SF_Willamette16pt7b);
   displayText(output, outputLen + 23, 97, yOutput, virtualDisp->color565(255, 255, 255), &SF_Willamette16pt7b);

   virtualDisp->setFont(&TT_Interphases_Pro_Trial_Bold7pt7b);
   virtualDisp->setCursor(61 - ((outputLen+23)/2) + outputLen + 2 , 79 - 1);
   virtualDisp->print("KG/");
   virtualDisp->setCursor(61 - ((outputLen+23)/2) + outputLen + 2 , 89 + 1);
   virtualDisp->print("H");
   displayOutputFlag = false;
  }

  if (isLenChanged) {
    // Hapus semuanya
    displayText(outTulisan, outputLastLen + 23, 62, yOutput, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
    displayText(outputLast, outputLastLen + 23, 97, yOutput, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);

    virtualDisp->setFont(&TT_Interphases_Pro_Trial_Bold7pt7b);
    virtualDisp->setCursor(61 - ((outputLastLen + 23) / 2) + outputLastLen + 2, 79 - 1);
    virtualDisp->print("KG/");
    virtualDisp->setCursor(61 - ((outputLastLen + 23) / 2) + outputLastLen + 2, 89 + 1);
    virtualDisp->print("H");

    // Tampilkan semua teks baru
    displayText(outTulisan, outputLen + 23, 62, yOutput, virtualDisp->color565(255, 255, 255), &SF_Willamette16pt7b);
    displayText(output, outputLen + 23 , 97, yOutput, virtualDisp->color565(255, 255, 255), &SF_Willamette16pt7b);

    virtualDisp->setFont(&TT_Interphases_Pro_Trial_Bold7pt7b);
    virtualDisp->setCursor(61 - ((outputLen + 23) / 2) + outputLen + 2, 79 - 1);
    virtualDisp->print("KG/");
    virtualDisp->setCursor(61 - ((outputLen + 23) / 2) + outputLen + 2, 89 + 1);
    virtualDisp->print("H");
  } else if (isMbChanged) {
    // Hanya hapus & tampilkan nilai output saja
    displayText(outputLast, outputLen + 23, 97, yOutput, virtualDisp->color565(0, 0, 0), &SF_Willamette16pt7b);
    displayText(output, outputLen + 23, 97, yOutput, virtualDisp->color565(255, 255, 255), &SF_Willamette16pt7b);
  }

  if (isLenChanged || isMbChanged) {
    strcpy(outputLast, output);
    oLast = o;
    outputLastLen = outputLen;
  }
 }
//

//Fungsi menghitung jumlah width tiap teks
 void hitungWidth(){
  int16_t x1, y1, x2, y2, x3, y3, x4, y4,x5, y5, x6, y6, x7, y7, x8, y8, x9, y9 ;
  uint16_t w1, h1, w2, h2, w3, h3, w4, h4, w5, h5, w6, h6, w7, h7, w8, h8, w9, h9 ;

  virtualDisp->setFont(&SF_Willamette16pt7b);
  virtualDisp->getTextBounds(String(tipe), 0, 0, &x1, &y1, &w1, &h1);
  virtualDisp->setFont(&SF_Willamette16pt7b);
  virtualDisp->getTextBounds(String(recipe), 0, 0, &x2, &y2, &w2, &h2);
  virtualDisp->getTextBounds(String(perform), 0, 0, &x3, &y3, &w3, &h3);
  virtualDisp->getTextBounds(String(outputFull), 0, 0, &x4, &y4, &w4, &h4);
  virtualDisp->getTextBounds(String(nilaiUtamaStr), 0, 0, &x5, &y5, &w5, &h5);
  virtualDisp->getTextBounds(String(nilaiPerformSetupStr), 0, 0, &x6, &y6, &w6, &h6);
  virtualDisp->getTextBounds(String(performSetup), 0, 0, &x7, &y7, &w7, &h7);
  virtualDisp->getTextBounds(String(row1), 0, 0, &x8, &y8, &w8, &h8);
  virtualDisp->getTextBounds(String(row2), 0, 0, &x9, &y9, &w9, &h9);
      
  tipeLen = w1;
  recipeLen = w2;
  performLen = w3;
  outputLen = w4;
  nilaiUtamaLen = w5;
  nilaiPerformSetupLen = w6;
  performSetupLen = w7;    
  row1Len = w8;    
 }
//


////////////////////////////////////////////////////////////////////////////////////////////////////

//Fungsi memulai DMA Display
 void startDMA(){
  // Standard panel type natively supported by this library (Example 1)
   HUB75_I2S_CFG mxconfig(
    PANEL_RES_X,   
    PANEL_RES_Y,  
    PANEL_CHAIN_LEN    
   );
 
  //fungsi agar display tidak flicker
   mxconfig.i2sspeed = HUB75_I2S_CFG::HZ_10M;
   mxconfig.clkphase = false;
   
   dma_display = new MatrixPanel_I2S_DMA(mxconfig);
   dma_display->begin();
   dma_display->setBrightness8(128); //0-255
   dma_display->clearScreen();
  // dma_display->setRotation(2);
   
   virtualDisp = new VirtualMatrixPanel_T<PANEL_CHAIN_TYPE>(VDISP_NUM_ROWS, VDISP_NUM_COLS, PANEL_RES_X, PANEL_RES_Y);
 
  // Pass a reference to the DMA display to the VirtualMatrixPanel_T class
   virtualDisp->setDisplay(*dma_display);
 }
//

//Fungsi Masuk ke mode OTA
 void masukModeOTA(){
  int masukOTA = Mb.MbData[8];
  if (masukOTA == 2) {
   modeOTA();
   if (!isOTAActive) {  // hanya saat pertama kali masuk OTA mode
      dma_display->stopDMAoutput();
      isOTAActive = true;
   }
  } 

 //Fungsi untuk merestart esp jika telah memasukan angka 2 ke mbdata[8]
  if (masukOTA == 1) { 
   if (isOTAActive) { 
    ESP.restart();   
    isOTAActive = false;
   }
  }
 }
//

//Fungsi memasukan ip yang sudah di baca dari SPIFFS ke mbData 10-13
 void ipFromMb(const String &ipInput, int &ipToModbus1, int &ipToModbus2, int &ipToModbus3, int &ipToModbus4){
  int firstDot = ipInput.indexOf('.');
  int secondDot = ipInput.indexOf('.', firstDot + 1);
  int thirdDot = ipInput.indexOf('.', secondDot + 1);

  ipToModbus1 = ipInput.substring(0, firstDot).toInt();
  ipToModbus2 = ipInput.substring(firstDot + 1, secondDot).toInt();
  ipToModbus3 = ipInput.substring(secondDot + 1, thirdDot).toInt();
  ipToModbus4 = ipInput.substring(thirdDot + 1).toInt();
 }
//

//fungsi mengganti IP
 void gantiIpFromModbus (int a, int b, int c, int d){
  if(Mb.MbData[14] == 1){ //data tidak akan di eksekusi sebelum nilai mb[14] = 1
   if ( d != e ) {

    sprintf (ipModbus, "%d.%d.%d.%d", a, b, c, d); //menyimpan nilai dari a, b, c, d ke char ipModbus
    writeFile(SPIFFS, "/ip.txt", ipModbus); //menyimpan data dari ipModbus ke SPIFFS

    e = d;
   }
  }
 }
//

//fungsi status ethernet
 void statusEthernet(){
    //status ethernet
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      // Serial.println("Ethernet shield tidak ditemukan.");
      ethernetStatus = 1; // Mengirim angka 1 jika tidak terhubung
      virtualDisp->drawPixel(2, 2, virtualDisp->color565(255, 0, 0));
      virtualDisp->drawPixel(1, 2, virtualDisp->color565(255, 0, 0));
      virtualDisp->drawPixel(3, 2, virtualDisp->color565(255, 0, 0));
      virtualDisp->drawPixel(2, 1, virtualDisp->color565(255, 0, 0));
      virtualDisp->drawPixel(2, 3, virtualDisp->color565(255, 0, 0));
    } 
    else if (Ethernet.linkStatus() == LinkOFF) {
      // Serial.println("Ethernet tidak terhubung.");
      ethernetStatus = 1; // Mengirim angka 1 jika tidak ada koneksi
      virtualDisp->drawPixel(2, 2, virtualDisp->color565(255, 255, 0));
      virtualDisp->drawPixel(1, 2, virtualDisp->color565(255, 255, 0));
      virtualDisp->drawPixel(3, 2, virtualDisp->color565(255, 255, 0));
      virtualDisp->drawPixel(2, 1, virtualDisp->color565(255, 255, 0));
      virtualDisp->drawPixel(2, 3, virtualDisp->color565(255, 255, 0));
    } 
    else {
      // Serial.println("Ethernet terhubung.");
      ethernetStatus = 0; // Ethernet terhubung, kirim angka 0
      virtualDisp->drawPixel(2, 2, virtualDisp->color565(0, 0, 0));
      virtualDisp->drawPixel(1, 2, virtualDisp->color565(0, 0, 0));
      virtualDisp->drawPixel(3, 2, virtualDisp->color565(0, 0, 0));
      virtualDisp->drawPixel(2, 1, virtualDisp->color565(0, 0, 0));
      virtualDisp->drawPixel(2, 3, virtualDisp->color565(0, 0, 0));
    }
 }
//

////////////////////////////////////////////////////////////////////////////////////////////////

//fungsi menggambar Tipe pakai char
 void drawTipe(){
  drawCharCentered(tipe, warnaTipe(), 8);
  if (strcmp(tipe, tipeLast) != 0){
    drawCharCentered(tipeLast, virtualDisp->color444(0, 0, 0), 8);
    drawCharCentered(tipe, warnaTipe(), 8);

    strcpy(tipeLast, tipe);
  }
 }
//

//fungsi menggambar recipe pakai char
 void drawRecipe(){
  drawCharCentered(recipe, virtualDisp->color444(15, 15, 15), 8);
  if (strcmp(recipe, recipeLast) != 0){
    drawCharCentered(recipeLast, virtualDisp->color444(0, 0, 0), 8);
    drawCharCentered(recipe, virtualDisp->color444(15, 15, 15), 8);

    strcpy(recipeLast, recipe);
  }
 }
//

//fungsi menggambar perform pakai char
 void drawPerform(){
  drawCharCentered(perform, warnaPerform(), 46);
  bool isLenChanged = (performLastLen != performLen);
  bool isMbChanged = (mbPerformLast != mbPerform);
  virtualDisp->setFont(&SF_Willamette27pt7b);

  if (isLenChanged) {
     // Hapus semua teks lama (perfTulisan, nilai, persen)
      drawCharCentered(performLast, virtualDisp->color444(0, 0, 0), 46);
     // Tampilkan teks baru
      drawCharCentered(perform, warnaPerform(), 46);
   } else if (isMbChanged) {
     // Hapus semua teks lama (perfTulisan, nilai, persen)
      drawCharCentered(performLast, virtualDisp->color444(0, 0, 0), 46);
     // Tampilkan teks baru
      drawCharCentered(perform, warnaPerform(), 46);
     // Hanya hapus dan tampilkan nilai (angka) saja

   }

    // Update variabel penyimpanan terakhir
   if (isLenChanged || isMbChanged) {
     mbPerformLast = mbPerform;
     strcpy(performLast, perform);
     strcpy(nilaiUtamaStrLast, nilaiUtamaStr);
     performLastLen = performLen;
     nilaiUtamaLastlen = nilaiUtamaLen;
   }
 }
//

//fungsi menggambar output pakai char
 void drawOutput(){
  bool isMbChanged = (o != oLast);
  virtualDisp->setFont(&SF_Willamette27pt7b);
  drawCharCentered(outputFull, warnaOutput(), 46);

  if(isMbChanged){
  drawCharCentered(outputFullLast, virtualDisp->color444(0, 0, 0), 46);
  drawCharCentered(outputFull, warnaOutput(), 46);

  strcpy(outputFullLast, outputFull);
  oLast = o;
  }
 }
//

//fungsi menggambar output pakai char
 void drawRow2(){
  bool isMbChanged = (strcmp(row2, row2Last) != 0);
  virtualDisp->setFont(&SF_Willamette27pt7b);
  drawCharCentered(row2, warnaRow2(), 46);

  if(isMbChanged){
  drawCharCentered(row2Last, virtualDisp->color444(0, 0, 0), 46);
  drawCharCentered(row2, warnaRow2(), 46);

  strcpy(row2Last, row2);
  }
 }
//

//fungsi menggambar status pakai char
 void drawStatus(){

  if(!run && forceRedrawMode){
  drawCharCentered20(status, virtualDisp->color444(15, 0, 0), 2, 12);
  }

  if(strcmp(status, statusLast) != 0){
  drawCharCentered20(statusLast, virtualDisp->color444(0, 0, 0), 2, 12);
  drawCharCentered20(status, virtualDisp->color444(15, 0, 0), 2, 12);

  strcpy(statusLast, status);
  bLast = b;
  }

 }
//

//fungsi menggambar waktu pakai char
 void drawWaktu(){

  virtualDisp->setFont(&SF_Willamette27pt7b);

  if(!run && forceRedrawMode){
  drawCharCentered(waktuUtama, virtualDisp->color444(15, 15, 15), 49); //53
  forceRedrawMode = false;
  }

  if(strcmp(waktuUtama, waktuUtamaLast) != 0){
  drawCharCentered(waktuUtamaLast, virtualDisp->color444(0, 0, 0), 49);
  drawCharCentered(waktuUtama, virtualDisp->color444(15, 15, 15), 49);

  strcpy(waktuUtamaLast, waktuUtama);
  menitLast = menit;
  jamLast = jam;
  }
 }
//

//Fungsi mengubah Warna output
 uint16_t warnaRow1(){
  if (mbWarnaRow1 == 0){
   return virtualDisp->color565(255, 255, 255); //merah
  }
  if (mbWarnaRow1 == 1){
   return virtualDisp->color565(74, 252, 2); //hijau
  }
  if (mbWarnaRow1 == 2){
   return virtualDisp->color565(255, 0, 0);
  }
  if (mbWarnaRow1 == 3){
   return virtualDisp->color565(93, 226, 231);;
  }
 }
//

//Fungsi mengubah Warna output
 uint16_t warnaRow2(){
  if (mbWarnaRow2 == 0){
   return virtualDisp->color565(255, 255, 255); //putih
  }
  if (mbWarnaRow2 == 1){
   return virtualDisp->color565(74, 252, 2); //hijau
  }
  if (mbWarnaRow2 == 2){
   return virtualDisp->color565(255, 0, 0); //merah
  }
  if (mbWarnaRow2 == 3){
   return virtualDisp->color565(93, 226, 231); //biru
  }
 }
//

//
 void warnaTulisan(){
  mbWarnaRow1 = Mb.MbData[34];
  mbWarnaRow2 = Mb.MbData[35];
 }
//


 void setup() {
   Serial.begin(115200);

   startSPIFFS(); //memulai SPIFFS untuk mengambil data IP dari SPIFFS
   readFile(SPIFFS, "/ip.txt", ipBaru); // membaca SPIFFS untuk mengambil nilai IP dan disimpan ke variabel IPBaru
   ipFromMb(ipBaru, ip10, ip20, ip30, ip40);
   convertIP(ipReal , ipBaru); //mengvonvert nilai IPBaru dari String ke varibel baru untuk menjadi IP yang dipakai
   startEthernet(); //memulai settingan ethernet
   
    //data awal modbus tipe
      Mb.MbData[0] = 0x0012; //Tipe product
      Mb.MbData[1] = 0x4f50; //Recipe
      Mb.MbData[2] = 0x344c;
      Mb.MbData[3] = 0x4631;
      Mb.MbData[4] = 0x3800;
      Mb.MbData[5] = 0x0000;
      Mb.MbData[6] = 0x039d; //Perf Rate(%)
      Mb.MbData[7] = 0x012c; //Output Kwh/kg

      Mb.MbData[9] = 0x0008; //Output Kwh/kg
      Mb.MbData[10] = ip10; //Output Kwh/kg
      Mb.MbData[11] = ip20; //Output Kwh/kg
      Mb.MbData[12] = ip30; //Output Kwh/kg
      Mb.MbData[13] = ip40; //Output Kwh/kg

      Mb.MbData[15] = 0x0000; //mode run atau stop
      Mb.MbData[16] = 0x0001; //status run atau pm
      Mb.MbData[17] = 0x000f; //menit breakdown
      Mb.MbData[18] = 0x000c; //jam breakdown

      Mb.MbData[20] = 0x4157; //row1
      Mb.MbData[21] = 0x2034;
      Mb.MbData[22] = 0x2220;
      Mb.MbData[23] = 0x344d;
      Mb.MbData[24] = 0x0000;
      Mb.MbData[25] = 0x0000;
      Mb.MbData[26] = 0x0000;

      Mb.MbData[27] = 0x5052; //Row2
      Mb.MbData[28] = 0x2039;
      Mb.MbData[29] = 0x302e;
      Mb.MbData[30] = 0x3525;
      Mb.MbData[31] = 0x0000;
      Mb.MbData[32] = 0x0000;
      Mb.MbData[33] = 0x0000;
      
      Mb.MbData[34] = 0x0000; //warna
      Mb.MbData[35] = 0x0001;


  startDMA(); //memulai DMA Display
  // displayPerformSetup();

  virtualDisp->clearScreen(); // Opsional: Bersihkan layar setelahnya

 }

 void loop() {
  unsigned long currentMillis = millis();
  unsigned long currentMillis1 = millis();
  Mb.MbsRun(); //menjalankan modbus
  masukModeOTA(); //fungsi masuk ke mode OTA untuk upload program via WIFI
  statusEthernet(); //menampilkan indikator ethernet terhunumg atau tidak
  gantiIpFromModbus(Mb.MbData[10], Mb.MbData[11], Mb.MbData[12], Mb.MbData[13]); //ganti IP melalui modbus
  tipeMb(); //ubah tipe sesuai mb0
  statusMb();
  modeRun();
  waktuMode();
  convertASCII(recipe, 1, 5); //convert MB ke char TIPE
  convertASCII(row1, 20, 26); //convert MB ke char TIPE
  convertASCII(row2, 27, 32); //convert MB ke char TIPE
  performMb(); //data masuk untuk perform
  outputMb(); //data masuk untuk output
  hitungWidth(); //mengsi menghitung jumlah pixel pada kalimat
  warnaTulisan();
  // cekHeap();

  virtualDisp->setFont(&SF_Willamette27pt7b);

  if (run){
// Ganti Tampilan Setelah 10 detik
 unsigned long newInterval = Mb.MbData[9] * 1000;
    // Cek apakah newInterval valid (minimal 1 detik = 1000 ms)
    if (newInterval >= 1000) {
        interval1 = newInterval;
    } else {
        interval1 = defaultInterval1; // Kalau nggak valid, pakai default
    }

  if (currentMillis1 - previousMillis1 >= interval1) {
    previousMillis1 = currentMillis1;  // Update waktu terakhir
    displayRow1();
    drawRow2();

/*
    virtualDisp->fillRect(0, 0, 128, 96, virtualDisp->color565(0, 0, 0));
    // Ganti teks yang ditampilkan
    currentText++;
    if (currentText > 1) {  
      currentText = 0;
    }
    */
    forceRedraw = true;
  }

    switch (currentText) {
      case 0:
      // displayTipe();
      displayRow1();
      // drawOutput();
      drawRow2();
      break;
      case 1:   
      displayRecipe(); 
      drawOutput();
      break;
    } 
  } 

  if (!run) {
    forceRedrawMode = true;
    drawStatus();
    drawWaktu();
   }
  
  delay(10);
 }