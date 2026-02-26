/*
 * ALAT UKUR KESEHATAN 4-IN-1
 * Versi Final - Siap Copy
 * 
 * Fitur:
 * - Ukur Gula Darah (GDS/GDP) dengan 1 kalibrasi
 * - Ukur Kolesterol
 * - Ukur Asam Urat (Pria/Wanita) dengan 1 kalibrasi
 * - Ukur Suhu Tubuh
 * - Kalibrasi semua parameter (input nilai konvensional)
 * - Tombol: UP, DOWN, OK, BACK
 * - LCD 20x4 I2C
 * 
 * Koneksi Hardware:
 * - Tombol UP     → D2
 * - Tombol DOWN   → D3
 * - Tombol OK     → D4
 * - Tombol BACK   → D5
 * - LED IR        → D6
 * - LED Merah     → D7
 * - LED Hijau     → D8
 * - Sensor Suhu   → D9
 * - Fotodioda     → A0
 * - LCD I2C       → A4(SDA), A5(SCL)
 */

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>

// ============================================
// INISIALISASI HARDWARE
// ============================================

// LCD 20x4 I2C (alamat biasanya 0x27 atau 0x3F)
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Sensor suhu DS18B20
OneWire oneWire(9);
DallasTemperature sensors(&oneWire);

// Pin tombol
const int btnUP = 2;
const int btnDOWN = 3;
const int btnOK = 4;
const int btnBACK = 5;

// Pin LED
const int ledIR = 6;      // IR 940nm untuk Gula
const int ledRed = 7;     // Merah 660nm untuk Kolesterol
const int ledGreen = 8;   // Hijau 525nm untuk Asam Urat

// Pin sensor
const int sensorPin = A0;  // Fotodioda BPW34

// ============================================
// STATE MACHINE
// ============================================

enum Screen {
  SCREEN_MAIN_MENU = 0,
  SCREEN_SUB_MENU_GULA,
  SCREEN_SUB_MENU_ASAM_URAT,
  SCREEN_CONFIRM,
  SCREEN_MEASUREMENT,
  SCREEN_SETTING,
  SCREEN_KALIBRASI_MENU,
  SCREEN_KALIBRASI_INPUT,
  SCREEN_KALIBRASI_UKUR,
  SCREEN_KALIBRASI_HASIL,
  SCREEN_KALIBRASI_SELESAI,
  SCREEN_HISTORY
};

int currentScreen = SCREEN_MAIN_MENU;

// Menu indices
int mainMenuIndex = 0;
int subMenuGulaIndex = 0;
int subMenuAsamUratIndex = 0;
int settingIndex = 0;
int kalibrasiMenuIndex = 0;
int selectedOption = 0;  // 0 untuk opsi kiri, 1 untuk opsi kanan

// Data pengukuran
int selectedParam = 0;     // 0=gula, 1=kolesterol, 2=asam urat, 3=suhu
int gulaType = 0;          // 0=GDS, 1=GDP
int genderAsamUrat = 0;    // 0=Pria, 1=Wanita

// Hasil pengukuran
float glucose = 0;
float cholesterol = 0;
float uricAcid = 0;
float bodyTemp = 0;

// ============================================
// VARIABEL KALIBRASI
// ============================================

int kalibrasiParam = 0;        // 0=Gula, 1=Kolesterol, 2=Asam Urat, 3=Suhu
int kalibrasiSampleCount = 0;
const int kalibrasiMaxSamples = 10;
float refValues[10];           // Nilai dari alat konvensional
float alatValues[10];          // Nilai dari sensor alat
float refInputTemp = 0;        // Nilai input sementara
float lastAlatValue = 0;       // Nilai alat terakhir
int inputDigitPos = 2;         // Posisi digit untuk input angka
bool isInputMode = true;       // Mode input angka

// Koefisien kalibrasi (masing-masing parameter punya sendiri)
float kalA_gula = 0;
float kalB_gula = 1;
float kalA_kol = 0;
float kalB_kol = 1;
float kalA_asam = 0;
float kalB_asam = 1;
float kalA_suhu = 0;
float kalB_suhu = 1;

// ============================================
// MENU ITEMS
// ============================================

const char* mainMenu[] = {"Gula Darah", "Kolesterol", "Asam Urat", "Suhu Tubuh", "Pengaturan"};
const int mainMenuCount = 5;

const char* subMenuGula[] = {"GDS (Sewaktu)", "GDP (Puasa)", "Kembali"};
const char* subMenuAsamUrat[] = {"Pria", "Wanita", "Kembali"};
const char* settingMenu[] = {"Kalibrasi", "Riwayat", "Kembali"};
const char* kalibrasiMenu[] = {"Gula Darah", "Kolesterol", "Asam Urat", "Suhu Tubuh", "Kembali"};
const int kalibrasiMenuCount = 5;

// ============================================
// DEBOUNCING
// ============================================

unsigned long lastButtonPress = 0;
const int debounceDelay = 200;

// ============================================
// SETUP
// ============================================

void setup() {
  Serial.begin(9600);
  
  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();
  
  // Setup pin tombol dengan internal pull-up
  pinMode(btnUP, INPUT_PULLUP);
  pinMode(btnDOWN, INPUT_PULLUP);
  pinMode(btnOK, INPUT_PULLUP);
  pinMode(btnBACK, INPUT_PULLUP);
  
  // Setup pin LED
  pinMode(ledIR, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  
  // Matikan semua LED
  digitalWrite(ledIR, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, LOW);
  
  // Inisialisasi sensor suhu
  sensors.begin();
  
  // Tampilkan splash screen
  showSplash();
  delay(2000);
  
  // Load data kalibrasi dari EEPROM
  loadCalibrationData();
  
  // Tampilkan menu utama
  currentScreen = SCREEN_MAIN_MENU;
  displayMainMenu();
}

// ============================================
// MAIN LOOP
// ============================================

void loop() {
  readButtons();
  
  // Update tampilan berdasarkan screen
  switch(currentScreen) {
    case SCREEN_MEASUREMENT:
      // Update setiap 2 detik untuk simulasi
      static unsigned long lastMeasure = 0;
      if(millis() - lastMeasure > 2000) {
        readAllSensors();
        displayMeasurement();
        lastMeasure = millis();
      }
      break;
      
    case SCREEN_KALIBRASI_INPUT:
      // Update kursor berkedip
      if(isInputMode) {
        displayKalibrasiInput();
        delay(100);
      }
      break;
  }
  
  delay(50);
}

// ============================================
// FUNGSI TAMPILAN
// ============================================

void showSplash() {
  lcd.clear();
  lcd.setCursor(4, 1);
  lcd.print("ALAT UKUR");
  lcd.setCursor(2, 2);
  lcd.print("KESEHATAN 4-IN-1");
  lcd.setCursor(3, 3);
  lcd.print("Gula|Kol|Urat|Suhu");
}

void displayMainMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" ALAT UKUR 4-IN-1 ");
  
  for(int i=0; i<4; i++) {
    lcd.setCursor(0, i+1);
    if(mainMenuIndex == i) {
      lcd.print("›");
    } else {
      lcd.print(" ");
    }
    lcd.print(mainMenu[i]);
  }
  
  // Tampilkan item ke-5 di baris 4
  lcd.setCursor(0, 4);
  if(mainMenuIndex == 4) {
    lcd.print("›");
  } else {
    lcd.print(" ");
  }
  lcd.print(mainMenu[4]);
}

void displaySubMenuGula() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(" PILIH JENIS GULA ");
  
  for(int i=0; i<3; i++) {
    lcd.setCursor(0, i+2);
    lcd.print(subMenuGulaIndex == i ? "›" : " ");
    lcd.print(subMenuGula[i]);
  }
}

void displaySubMenuAsamUrat() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   PILIH GENDER   ");
  
  for(int i=0; i<3; i++) {
    lcd.setCursor(0, i+2);
    lcd.print(subMenuAsamUratIndex == i ? "›" : " ");
    lcd.print(subMenuAsamUrat[i]);
  }
}

void displayConfirmScreen() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print(" Letakan jari pada ");
  lcd.setCursor(0, 2);
  lcd.print(" alat detektor.   ");
  lcd.setCursor(0, 3);
  lcd.print(" OK(lanjut) BACK(menu) ");
}

void displaySetting() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    PENGATURAN    ");
  
  for(int i=0; i<3; i++) {
    lcd.setCursor(0, i+2);
    lcd.print(settingIndex == i ? "›" : " ");
    lcd.print(settingMenu[i]);
  }
}

void displayKalibrasiMenu() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("    KALIBRASI     ");
  
  for(int i=0; i<4; i++) {
    lcd.setCursor(0, i+1);
    if(kalibrasiMenuIndex == i) {
      lcd.print("›");
    } else {
      lcd.print(" ");
    }
    lcd.print(kalibrasiMenu[i]);
  }
  
  // Tampilkan item ke-5 di baris 4
  lcd.setCursor(0, 4);
  if(kalibrasiMenuIndex == 4) {
    lcd.print("›");
  } else {
    lcd.print(" ");
  }
  lcd.print(kalibrasiMenu[4]);
}

void displayKalibrasiInput() {
  lcd.clear();
  
  // Baris 1: Judul sesuai parameter
  lcd.setCursor(0, 0);
  lcd.print(" KALIBRASI ");
  if(kalibrasiParam == 0) lcd.print("GULA");
  else if(kalibrasiParam == 1) lcd.print("KOLESTEROL");
  else if(kalibrasiParam == 2) lcd.print("ASAM URAT");
  else if(kalibrasiParam == 3) lcd.print("SUHU");
  
  // Baris 2: Sampel ke
  lcd.setCursor(0, 1);
  lcd.print(" Sampel ");
  lcd.print(kalibrasiSampleCount + 1);
  lcd.print("/10");
  
  // Baris 3: Nilai referensi
  lcd.setCursor(0, 2);
  lcd.print(" Nilai: ");
  
  if(isInputMode) {
    // Mode input angka
    char buf[6];
    if(kalibrasiParam == 2 || kalibrasiParam == 3) {
      // Asam Urat & Suhu (1 desimal)
      dtostrf(refInputTemp, 4, 1, buf);
    } else {
      // Gula & Kolesterol (tanpa desimal)
      dtostrf(refInputTemp, 3, 0, buf);
    }
    lcd.print(buf);
    
    // Tampilkan kursor berkedip
    lcd.setCursor(7 + inputDigitPos, 2);
    if((millis() / 500) % 2 == 0) {
      lcd.print("_");
    } else {
      lcd.print(" ");
    }
  } else {
    // Tampilkan nilai tanpa kursor
    if(kalibrasiParam == 2 || kalibrasiParam == 3) {
      lcd.print(refInputTemp, 1);
    } else {
      lcd.print(refInputTemp, 0);
    }
    
    // Baris 4: Pilihan UKUR dan KEMBALI
    lcd.setCursor(0, 3);
    if(selectedOption == 0) {
      lcd.print("   [›UKUR]  [KEMBALI]   ");
    } else {
      lcd.print("   [UKUR]  [›KEMBALI]   ");
    }
  }
}

void displayKalibrasiUkur() {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print(" KALIBRASI ");
  if(kalibrasiParam == 0) lcd.print("GULA");
  else if(kalibrasiParam == 1) lcd.print("KOLESTEROL");
  else if(kalibrasiParam == 2) lcd.print("ASAM URAT");
  else if(kalibrasiParam == 3) lcd.print("SUHU");
  
  lcd.setCursor(0, 1);
  lcd.print(" Sampel ");
  lcd.print(kalibrasiSampleCount + 1);
  lcd.print("/10");
  
  lcd.setCursor(0, 2);
  lcd.print(" Nilai: ");
  if(kalibrasiParam == 2 || kalibrasiParam == 3) {
    lcd.print(refInputTemp, 1);
  } else {
    lcd.print(refInputTemp, 0);
  }
  
  lcd.setCursor(0, 3);
  lcd.print(" Letakan jari lalu ");
  lcd.setCursor(0, 4);
  lcd.print("   TEKAN OK UKUR   ");
}

void displayKalibrasiHasil() {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print(" KALIBRASI ");
  if(kalibrasiParam == 0) lcd.print("GULA");
  else if(kalibrasiParam == 1) lcd.print("KOLESTEROL");
  else if(kalibrasiParam == 2) lcd.print("ASAM URAT");
  else if(kalibrasiParam == 3) lcd.print("SUHU");
  
  lcd.setCursor(0, 1);
  lcd.print(" Sampel ");
  lcd.print(kalibrasiSampleCount + 1);
  lcd.print("/10");
  
  lcd.setCursor(0, 2);
  lcd.print(" Alat: ");
  if(kalibrasiParam == 2 || kalibrasiParam == 3) {
    lcd.print(lastAlatValue, 1);
  } else {
    lcd.print(lastAlatValue, 0);
  }
  
  // Baris 4: Pilihan TAMBAH dan SELESAI
  lcd.setCursor(0, 4);
  if(selectedOption == 0) {
    lcd.print("   [›TAMBAH]  [SELESAI]  ");
  } else {
    lcd.print("   [TAMBAH]  [›SELESAI]  ");
  }
}

void displayKalibrasiSelesai() {
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("   KALIBRASI SELESAI ");
  lcd.setCursor(0, 2);
  lcd.print("   Total: ");
  lcd.print(kalibrasiSampleCount);
  lcd.print(" sampel    ");
  lcd.setCursor(0, 3);
  lcd.print("   Data tersimpan   ");
  lcd.setCursor(0, 4);
  lcd.print("   TEKAN OK KEMBALI  ");
}

void displayMeasurement() {
  lcd.clear();
  
  switch(selectedParam) {
    case 0: // Gula
      if(gulaType == 0) {
        lcd.setCursor(0, 0);
        lcd.print(" GDS: ");
        lcd.print(glucose, 0);
        lcd.print(" mg/dL");
      } else {
        lcd.setCursor(0, 0);
        lcd.print(" GDP: ");
        lcd.print(glucose, 0);
        lcd.print(" mg/dL");
      }
      
      lcd.setCursor(0, 1);
      lcd.print(" STATUS: ");
      if(gulaType == 0) { // GDS
        if(glucose < 70) lcd.print("RENDAH ");
        else if(glucose < 200) lcd.print("NORMAL ");
        else if(glucose < 300) lcd.print("TINGGI ");
        else lcd.print("BAHAYA!");
      } else { // GDP
        if(glucose < 70) lcd.print("RENDAH ");
        else if(glucose < 126) lcd.print("NORMAL ");
        else if(glucose < 200) lcd.print("TINGGI ");
        else lcd.print("BAHAYA!");
      }
      
      lcd.setCursor(0, 2);
      lcd.print(" Suhu: ");
      lcd.print(bodyTemp, 1);
      lcd.print((char)223);
      lcd.print("C");
      break;
      
    case 1: // Kolesterol
      lcd.setCursor(0, 0);
      lcd.print(" KOLESTEROL: ");
      lcd.print(cholesterol, 0);
      lcd.print(" mg/dL");
      
      lcd.setCursor(0, 1);
      lcd.print(" STATUS: ");
      if(cholesterol < 200) lcd.print("NORMAL");
      else if(cholesterol < 240) lcd.print("BATAS ");
      else lcd.print("TINGGI");
      
      lcd.setCursor(0, 2);
      lcd.print(" (normal <200)");
      break;
      
    case 2: // Asam Urat
      lcd.setCursor(0, 0);
      lcd.print(" ASAM URAT: ");
      lcd.print(uricAcid, 1);
      lcd.print(" mg/dL");
      
      lcd.setCursor(0, 1);
      lcd.print(" STATUS: ");
      
      if(genderAsamUrat == 0) { // PRIA
        if(uricAcid < 3.4) lcd.print("RENDAH");
        else if(uricAcid <= 7.0) lcd.print("NORMAL");
        else if(uricAcid <= 9.0) lcd.print("TINGGI");
        else lcd.print("S.TINGGI");
      } else { // WANITA
        if(uricAcid < 2.4) lcd.print("RENDAH");
        else if(uricAcid <= 6.0) lcd.print("NORMAL");
        else if(uricAcid <= 8.0) lcd.print("TINGGI");
        else lcd.print("S.TINGGI");
      }
      
      lcd.setCursor(0, 2);
      if(genderAsamUrat == 0) {
        lcd.print(" (Pria: 3.4-7.0)");
      } else {
        lcd.print(" (Wanita: 2.4-6.0)");
      }
      break;
      
    case 3: // Suhu
      lcd.setCursor(0, 0);
      lcd.print(" SUHU TUBUH: ");
      lcd.print(bodyTemp, 1);
      lcd.print((char)223);
      lcd.print("C");
      
      lcd.setCursor(0, 1);
      lcd.print(" STATUS: ");
      if(bodyTemp >= 39.0) lcd.print("D.TINGGI");
      else if(bodyTemp >= 37.5) lcd.print("DEMAM  ");
      else if(bodyTemp >= 36.0) lcd.print("NORMAL ");
      else if(bodyTemp >= 35.0) lcd.print("DINGIN ");
      else lcd.print("HIPOTERMIA");
      
      lcd.setCursor(0, 2);
      lcd.print(" (normal 36-37.5)");
      break;
  }
  
  lcd.setCursor(0, 3);
  lcd.print("[OK]Ulang [BACK]Menu");
}

void displayHistory() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("     RIWAYAT      ");
  lcd.setCursor(0, 2);
  lcd.print("  Fitur dalam     ");
  lcd.setCursor(0, 3);
  lcd.print("  pengembangan    ");
  lcd.setCursor(0, 4);
  lcd.print("  Tekan BACK      ");
}

// ============================================
// FUNGSI HANDLE TOMBOL
// ============================================

void readButtons() {
  if(millis() - lastButtonPress < debounceDelay) return;
  
  bool upPressed = (digitalRead(btnUP) == LOW);
  bool downPressed = (digitalRead(btnDOWN) == LOW);
  bool okPressed = (digitalRead(btnOK) == LOW);
  bool backPressed = (digitalRead(btnBACK) == LOW);
  
  if(upPressed) {
    lastButtonPress = millis();
    handleUp();
  }
  
  if(downPressed) {
    lastButtonPress = millis();
    handleDown();
  }
  
  if(okPressed) {
    lastButtonPress = millis();
    handleOK();
  }
  
  if(backPressed) {
    lastButtonPress = millis();
    handleBack();
  }
}

void handleUp() {
  switch(currentScreen) {
    case SCREEN_MAIN_MENU:
      mainMenuIndex--;
      if(mainMenuIndex < 0) mainMenuIndex = mainMenuCount - 1;
      displayMainMenu();
      break;
      
    case SCREEN_SUB_MENU_GULA:
      subMenuGulaIndex--;
      if(subMenuGulaIndex < 0) subMenuGulaIndex = 2;
      displaySubMenuGula();
      break;
      
    case SCREEN_SUB_MENU_ASAM_URAT:
      subMenuAsamUratIndex--;
      if(subMenuAsamUratIndex < 0) subMenuAsamUratIndex = 2;
      displaySubMenuAsamUrat();
      break;
      
    case SCREEN_SETTING:
      settingIndex--;
      if(settingIndex < 0) settingIndex = 2;
      displaySetting();
      break;
      
    case SCREEN_KALIBRASI_MENU:
      kalibrasiMenuIndex--;
      if(kalibrasiMenuIndex < 0) kalibrasiMenuIndex = kalibrasiMenuCount - 1;
      displayKalibrasiMenu();
      break;
      
    case SCREEN_KALIBRASI_INPUT:
      if(isInputMode) {
        // Atur digit (naik)
        float digitValue = pow(10, inputDigitPos);
        int digit = (int)(refInputTemp / digitValue) % 10;
        digit++;
        if(digit > 9) digit = 0;
        refInputTemp = refInputTemp - (digit-1)*digitValue + digit*digitValue;
        displayKalibrasiInput();
      } else {
        // Ganti pilihan UKUR/KEMBALI
        selectedOption = (selectedOption == 0) ? 1 : 0;
        displayKalibrasiInput();
      }
      break;
      
    case SCREEN_KALIBRASI_HASIL:
      // Ganti pilihan TAMBAH/SELESAI
      selectedOption = (selectedOption == 0) ? 1 : 0;
      displayKalibrasiHasil();
      break;
  }
}

void handleDown() {
  switch(currentScreen) {
    case SCREEN_MAIN_MENU:
      mainMenuIndex = (mainMenuIndex + 1) % mainMenuCount;
      displayMainMenu();
      break;
      
    case SCREEN_SUB_MENU_GULA:
      subMenuGulaIndex = (subMenuGulaIndex + 1) % 3;
      displaySubMenuGula();
      break;
      
    case SCREEN_SUB_MENU_ASAM_URAT:
      subMenuAsamUratIndex = (subMenuAsamUratIndex + 1) % 3;
      displaySubMenuAsamUrat();
      break;
      
    case SCREEN_SETTING:
      settingIndex = (settingIndex + 1) % 3;
      displaySetting();
      break;
      
    case SCREEN_KALIBRASI_MENU:
      kalibrasiMenuIndex = (kalibrasiMenuIndex + 1) % kalibrasiMenuCount;
      displayKalibrasiMenu();
      break;
      
    case SCREEN_KALIBRASI_INPUT:
      if(isInputMode) {
        // Atur digit (turun)
        float digitValue = pow(10, inputDigitPos);
        int digit = (int)(refInputTemp / digitValue) % 10;
        digit--;
        if(digit < 0) digit = 9;
        refInputTemp = refInputTemp - (digit+1)*digitValue + digit*digitValue;
        displayKalibrasiInput();
      } else {
        // Ganti pilihan UKUR/KEMBALI
        selectedOption = (selectedOption == 0) ? 1 : 0;
        displayKalibrasiInput();
      }
      break;
      
    case SCREEN_KALIBRASI_HASIL:
      // Ganti pilihan TAMBAH/SELESAI
      selectedOption = (selectedOption == 0) ? 1 : 0;
      displayKalibrasiHasil();
      break;
  }
}

void handleOK() {
  switch(currentScreen) {
    case SCREEN_MAIN_MENU:
      selectedParam = mainMenuIndex;
      
      if(selectedParam == 0) { // Gula
        currentScreen = SCREEN_SUB_MENU_GULA;
        subMenuGulaIndex = 0;
        displaySubMenuGula();
      }
      else if(selectedParam == 1) { // Kolesterol
        currentScreen = SCREEN_CONFIRM;
        displayConfirmScreen();
      }
      else if(selectedParam == 2) { // Asam Urat
        currentScreen = SCREEN_SUB_MENU_ASAM_URAT;
        subMenuAsamUratIndex = 0;
        displaySubMenuAsamUrat();
      }
      else if(selectedParam == 3) { // Suhu
        currentScreen = SCREEN_CONFIRM;
        displayConfirmScreen();
      }
      else if(selectedParam == 4) { // Pengaturan
        currentScreen = SCREEN_SETTING;
        settingIndex = 0;
        displaySetting();
      }
      break;
      
    case SCREEN_SUB_MENU_GULA:
      if(subMenuGulaIndex == 2) { // Kembali
        currentScreen = SCREEN_MAIN_MENU;
        displayMainMenu();
      } else {
        gulaType = subMenuGulaIndex;
        currentScreen = SCREEN_CONFIRM;
        displayConfirmScreen();
      }
      break;
      
    case SCREEN_SUB_MENU_ASAM_URAT:
      if(subMenuAsamUratIndex == 2) { // Kembali
        currentScreen = SCREEN_MAIN_MENU;
        displayMainMenu();
      } else {
        genderAsamUrat = subMenuAsamUratIndex;
        currentScreen = SCREEN_CONFIRM;
        displayConfirmScreen();
      }
      break;
      
    case SCREEN_CONFIRM:
      currentScreen = SCREEN_MEASUREMENT;
      readAllSensors();
      displayMeasurement();
      break;
      
    case SCREEN_MEASUREMENT:
      currentScreen = SCREEN_CONFIRM;
      displayConfirmScreen();
      break;
      
    case SCREEN_SETTING:
      if(settingIndex == 0) { // Kalibrasi
        currentScreen = SCREEN_KALIBRASI_MENU;
        kalibrasiMenuIndex = 0;
        displayKalibrasiMenu();
      } else if(settingIndex == 1) { // Riwayat
        currentScreen = SCREEN_HISTORY;
        displayHistory();
      } else if(settingIndex == 2) { // Kembali
        currentScreen = SCREEN_MAIN_MENU;
        displayMainMenu();
      }
      break;
      
    case SCREEN_KALIBRASI_MENU:
      if(kalibrasiMenuIndex == 4) { // Kembali
        currentScreen = SCREEN_SETTING;
        displaySetting();
      } else {
        kalibrasiParam = kalibrasiMenuIndex; // 0=Gula, 1=Kolesterol, 2=Asam Urat, 3=Suhu
        kalibrasiSampleCount = 0;
        refInputTemp = 0;
        inputDigitPos = 2;
        isInputMode = true;
        selectedOption = 0;
        currentScreen = SCREEN_KALIBRASI_INPUT;
        displayKalibrasiInput();
      }
      break;
      
    case SCREEN_KALIBRASI_INPUT:
      if(isInputMode) {
        // Selesai input angka
        isInputMode = false;
        selectedOption = 0;
        displayKalibrasiInput();
      } else {
        if(selectedOption == 0) {
          // UKUR dipilih
          currentScreen = SCREEN_KALIBRASI_UKUR;
          displayKalibrasiUkur();
        } else {
          // KEMBALI dipilih
          currentScreen = SCREEN_KALIBRASI_MENU;
          displayKalibrasiMenu();
        }
      }
      break;
      
    case SCREEN_KALIBRASI_UKUR:
      // Lakukan pengukuran
      lastAlatValue = readKalibrasiSensor(kalibrasiParam);
      alatValues[kalibrasiSampleCount] = lastAlatValue;
      refValues[kalibrasiSampleCount] = refInputTemp;
      
      currentScreen = SCREEN_KALIBRASI_HASIL;
      selectedOption = 0;
      displayKalibrasiHasil();
      break;
      
    case SCREEN_KALIBRASI_HASIL:
      if(selectedOption == 0) {
        // TAMBAH dipilih
        kalibrasiSampleCount++;
        
        if(kalibrasiSampleCount >= kalibrasiMaxSamples) {
          // Selesai semua sampel
          hitungKoefisienKalibrasi(kalibrasiParam);
          saveCalibrationData();
          currentScreen = SCREEN_KALIBRASI_SELESAI;
          displayKalibrasiSelesai();
        } else {
          // Lanjut ke sampel berikutnya
          refInputTemp = 0;
          inputDigitPos = 2;
          isInputMode = true;
          currentScreen = SCREEN_KALIBRASI_INPUT;
          displayKalibrasiInput();
        }
      } else {
        // SELESAI dipilih
        if(kalibrasiSampleCount > 0) {
          hitungKoefisienKalibrasi(kalibrasiParam);
          saveCalibrationData();
        }
        currentScreen = SCREEN_KALIBRASI_SELESAI;
        displayKalibrasiSelesai();
      }
      break;
      
    case SCREEN_KALIBRASI_SELESAI:
      currentScreen = SCREEN_KALIBRASI_MENU;
      displayKalibrasiMenu();
      break;
      
    case SCREEN_HISTORY:
      currentScreen = SCREEN_SETTING;
      displaySetting();
      break;
  }
}

void handleBack() {
  switch(currentScreen) {
    case SCREEN_MAIN_MENU:
      // Tetap di menu
      break;
      
    case SCREEN_SUB_MENU_GULA:
    case SCREEN_SUB_MENU_ASAM_URAT:
    case SCREEN_CONFIRM:
    case SCREEN_MEASUREMENT:
    case SCREEN_SETTING:
    case SCREEN_KALIBRASI_MENU:
    case SCREEN_KALIBRASI_INPUT:
    case SCREEN_KALIBRASI_UKUR:
    case SCREEN_KALIBRASI_HASIL:
    case SCREEN_KALIBRASI_SELESAI:
    case SCREEN_HISTORY:
      currentScreen = SCREEN_MAIN_MENU;
      displayMainMenu();
      break;
  }
}

// ============================================
// FUNGSI BACA SENSOR
// ============================================

void readAllSensors() {
  // Baca sensor suhu
  sensors.requestTemperatures();
  float rawTemp = sensors.getTempCByIndex(0);
  if(rawTemp < 20 || rawTemp > 45 || rawTemp == -127) {
    rawTemp = 36.5; // Default jika error
  }
  
  // ============================================
  // GANTI INI DENGAN KODE SENSOR ASLI ANDA
  // ============================================
  
  // SIMULASI - Hanya untuk demo
  // Di sini Anda harus membaca dari fotodioda dan LED
  float rawGlucose = random(80, 250);
  float rawCholesterol = random(150, 280);
  float rawUricAcid = random(20, 90) / 10.0;
  
  // Contoh pembacaan sensor yang sebenarnya:
  /*
  // Ukur Gula dengan LED IR
  digitalWrite(ledIR, HIGH);
  delay(100);
  float irValue = analogRead(sensorPin);
  digitalWrite(ledIR, LOW);
  
  // Ukur Kolesterol dengan LED Merah
  digitalWrite(ledRed, HIGH);
  delay(100);
  float redValue = analogRead(sensorPin);
  digitalWrite(ledRed, LOW);
  
  // Ukur Asam Urat dengan LED Hijau
  digitalWrite(ledGreen, HIGH);
  delay(100);
  float greenValue = analogRead(sensorPin);
  digitalWrite(ledGreen, LOW);
  
  // Hitung absorbansi
  float refIR = 800;  // Nilai referensi dari kalibrasi
  float refRed = 750;
  float refGreen = 700;
  
  float absorbIR = log(refIR / irValue);
  float absorbRed = log(refRed / redValue);
  float absorbGreen = log(refGreen / greenValue);
  
  // Konversi ke mg/dL (dengan koefisien kalibrasi)
  rawGlucose = 42.3 + (352.0 * absorbIR);
  rawCholesterol = 98.5 + (645.0 * absorbRed);
  rawUricAcid = 2.1 + (52.3 * absorbGreen);
  */
  
  // Terapkan koefisien kalibrasi masing-masing
  glucose = kalA_gula + kalB_gula * rawGlucose;
  cholesterol = kalA_kol + kalB_kol * rawCholesterol;
  uricAcid = kalA_asam + kalB_asam * rawUricAcid;
  bodyTemp = kalA_suhu + kalB_suhu * rawTemp;
  
  // Batasi dalam range wajar
  glucose = constrain(glucose, 50, 400);
  cholesterol = constrain(cholesterol, 100, 400);
  uricAcid = constrain(uricAcid, 1.0, 15.0);
  bodyTemp = constrain(bodyTemp, 32, 42);
}

float readKalibrasiSensor(int param) {
  // Baca sensor untuk keperluan kalibrasi
  // ============================================
  // GANTI DENGAN KODE SENSOR ASLI ANDA
  // ============================================
  
  sensors.requestTemperatures();
  float rawTemp = sensors.getTempCByIndex(0);
  if(rawTemp < 20 || rawTemp > 45 || rawTemp == -127) {
    rawTemp = 36.5;
  }
  
  // SIMULASI - Hanya untuk demo
  switch(param) {
    case 0: // Gula
      return random(80, 250);
    case 1: // Kolesterol
      return random(150, 280);
    case 2: // Asam Urat
      return random(20, 90) / 10.0;
    case 3: // Suhu
      return rawTemp;
    default:
      return 0;
  }
}

// ============================================
// FUNGSI KALIBRASI
// ============================================

void hitungKoefisienKalibrasi(int param) {
  // Regresi linear sederhana: y = a + b*x
  // y = nilai konvensional, x = nilai alat
  
  if(kalibrasiSampleCount < 2) {
    // Jika sampel kurang dari 2, gunakan slope default
    Serial.println("Sampel kurang dari 2, gunakan koefisien default");
    return;
  }
  
  float sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
  
  for(int i=0; i<kalibrasiSampleCount; i++) {
    sumX += alatValues[i];
    sumY += refValues[i];
    sumXY += alatValues[i] * refValues[i];
    sumX2 += alatValues[i] * alatValues[i];
  }
  
  float rataX = sumX / kalibrasiSampleCount;
  float rataY = sumY / kalibrasiSampleCount;
  
  // Hitung slope (b)
  float b = (sumXY - kalibrasiSampleCount * rataX * rataY) / (sumX2 - kalibrasiSampleCount * rataX * rataX);
  
  // Hitung intercept (a)
  float a = rataY - b * rataX;
  
  // Simpan ke parameter yang sesuai
  switch(param) {
    case 0: // Gula
      kalA_gula = a;
      kalB_gula = b;
      Serial.println("Koefisien Gula tersimpan");
      break;
    case 1: // Kolesterol
      kalA_kol = a;
      kalB_kol = b;
      Serial.println("Koefisien Kolesterol tersimpan");
      break;
    case 2: // Asam Urat
      kalA_asam = a;
      kalB_asam = b;
      Serial.println("Koefisien Asam Urat tersimpan");
      break;
    case 3: // Suhu
      kalA_suhu = a;
      kalB_suhu = b;
      Serial.println("Koefisien Suhu tersimpan");
      break;
  }
  
  // Tampilkan hasil di Serial
  Serial.print("a = "); Serial.print(a);
  Serial.print(", b = "); Serial.println(b);
}

// ============================================
// FUNGSI EEPROM (MENYIMPAN KALIBRASI)
// ============================================

void saveCalibrationData() {
  // Simpan koefisien kalibrasi ke EEPROM
  // Alamat: 0-3 untuk kalA_gula (float = 4 byte)
  //        4-7 untuk kalB_gula
  //        8-11 untuk kalA_kol
  //        12-15 untuk kalB_kol
  //        16-19 untuk kalA_asam
  //        20-23 untuk kalB_asam
  //        24-27 untuk kalA_suhu
  //        28-31 untuk kalB_suhu
  //        32 untuk checksum
  
  EEPROM.put(0, kalA_gula);
  EEPROM.put(4, kalB_gula);
  EEPROM.put(8, kalA_kol);
  EEPROM.put(12, kalB_kol);
  EEPROM.put(16, kalA_asam);
  EEPROM.put(20, kalB_asam);
  EEPROM.put(24, kalA_suhu);
  EEPROM.put(28, kalB_suhu);
  
  // Simpan checksum sederhana
  byte checksum = 0;
  for(int i=0; i<32; i++) {
    checksum += EEPROM.read(i);
  }
  EEPROM.write(32, checksum);
  
  Serial.println("Data kalibrasi tersimpan di EEPROM");
}

void loadCalibrationData() {
  // Load koefisien kalibrasi dari EEPROM
  EEPROM.get(0, kalA_gula);
  EEPROM.get(4, kalB_gula);
  EEPROM.get(8, kalA_kol);
  EEPROM.get(12, kalB_kol);
  EEPROM.get(16, kalA_asam);
  EEPROM.get(20, kalB_asam);
  EEPROM.get(24, kalA_suhu);
  EEPROM.get(28, kalB_suhu);
  
  // Verifikasi checksum
  byte checksum = 0;
  for(int i=0; i<32; i++) {
    checksum += EEPROM.read(i);
  }
  
  if(EEPROM.read(32) != checksum) {
    // Data rusak, gunakan default
    kalA_gula = 0;
    kalB_gula = 1;
    kalA_kol = 0;
    kalB_kol = 1;
    kalA_asam = 0;
    kalB_asam = 1;
    kalA_suhu = 0;
    kalB_suhu = 1;
    Serial.println("Data kalibrasi rusak, gunakan default");
  } else {
    Serial.println("Data kalibrasi dimuat dari EEPROM");
  }
}