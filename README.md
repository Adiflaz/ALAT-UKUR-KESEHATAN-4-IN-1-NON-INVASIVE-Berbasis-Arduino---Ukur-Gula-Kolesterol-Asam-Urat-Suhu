
ALAT UKUR KESEHATAN 4-IN-1 (NON-INVASIVE)
Berbasis Arduino - Ukur Gula, Kolesterol, Asam Urat, Suhu
===============================================================


DAFTAR ISI
===============================================================
1. FITUR LENGKAP ...................................... HAL 2
2. SPESIFIKASI TEKNIS ................................. HAL 3
3. DAFTAR KOMPONEN & BIAYA ............................ HAL 4
4. SKEMA RANGKAIAN .................................... HAL 5
5. CARA PERAKITAN ..................................... HAL 7
6. CARA PENGGUNAAN ..................................... HAL 9
7. CARA KALIBRASI ..................................... HAL 12
8. TROUBLESHOOTING .................................... HAL 14
9. SPESIFIKASI HASIL PENGUKURAN ....................... HAL 15
10. KESIMPULAN ........................................ HAL 16
===============================================================



1. FITUR LENGKAP
===============================================================

NON-INVASIVE (TANPA DARAH)
- Ukur Gula Darah (GDS & GDP) dengan LED Inframerah 940nm
- Ukur Kolesterol Total dengan LED Merah 660nm
- Ukur Asam Urat dengan LED Hijau 525nm
- Ukur Suhu Tubuh dengan sensor DS18B20

INTERPRETASI OTOMATIS
- GDS (Gula Darah Sewaktu): Normal <200 mg/dL
- GDP (Gula Darah Puasa): Normal <126 mg/dL
- Kolesterol: Normal <200 mg/dL
- Asam Urat Pria: Normal 3.4 - 7.0 mg/dL
- Asam Urat Wanita: Normal 2.4 - 6.0 mg/dL
- Suhu Tubuh: Normal 36.0 - 37.5°C

SISTEM KALIBRASI LENGKAP
- Kalibrasi untuk 4 parameter (Gula, Kolesterol, Asam Urat, Suhu)
- Input nilai dari alat konvensional (up/down per digit)
- Bisa 1-10 sampel (tanpa minimal)
- Opsi Tambah Sampel atau Selesai
- Koefisien tersimpan di EEPROM (tidak hilang saat mati)

ANTARMUKA PENGGUNA
- LCD 20x4 I2C (20 karakter, 4 baris)
- 4 Tombol Navigasi: UP, DOWN, OK, BACK
- Indikator panah (›) untuk posisi pilihan
- Petunjuk tombol selalu tampil di layar
- Layar konfirmasi sebelum pengukuran



2. SPESIFIKASI TEKNIS
===============================================================

MIKROKONTROLER
- Arduino Nano
- Clock Speed: 16 MHz
- Flash Memory: 32 KB
- SRAM: 2 KB
- EEPROM: 1 KB (untuk menyimpan kalibrasi)

SENSOR
- Fotodioda BPW34 (2 buah) - Detektor cahaya
  Rentang spektral: 350-1100 nm, Area aktif: 7.5 mm²
- LED IR 940nm - Sumber cahaya untuk Gula
- LED Merah 660nm - Sumber cahaya untuk Kolesterol
- LED Hijau 525nm - Sumber cahaya untuk Asam Urat
- DS18B20 - Sensor suhu digital waterproof
  Akurasi: ±0.5°C, Rentang: -55°C hingga +125°C

RANGKAIAN ANALOG
- Op-Amp LM358 (2 buah) - Penguat transimpedance
- Resistor feedback 1M ohm
- Kapasitor 0.1uF - Filter noise

DAYA
- Input: 5V DC dari USB atau baterai
- Modul TP4056 - Charger baterai lithium
- Modul MT3608 - Step-up converter (3.7V ke 5V)
- Baterai 18650 - 3.7V 2200mAh (tahan 8-10 jam)

DIMENSI & BERAT
- Ukuran PCB: 10cm x 8cm
- Enclosure: 12cm x 10cm x 5cm
- Berat total: ~300 gram



3. DAFTAR KOMPONEN & BIAYA
===============================================================

KOMPONEN UTAMA
-----------------------------------------------------------------
No. Komponen                 | Spesifikasi        | Harga
-----------------------------------------------------------------
1. Arduino Nano              | Clone CH340        | Rp 45.000
2. LCD 20x4 I2C              | 20x4 + I2C module  | Rp 90.000
3. Fotodioda BPW34           | 2 buah             | Rp 20.000
4. LED IR 940nm              | 5mm                | Rp 3.000
5. LED Merah Super Bright    | 660nm              | Rp 2.000
6. LED Hijau Super Bright    | 525nm              | Rp 2.000
7. IC LM358                  | 2 buah             | Rp 10.000
8. Sensor DS18B20            | Waterproof         | Rp 15.000
9. Modul TP4056              | Charger 18650      | Rp 8.000
10. Modul MT3608             | Step-up converter  | Rp 10.000
11. Baterai 18650            | Bekas laptop       | Rp 0
12. Holder baterai 18650     | 1 buah             | Rp 5.000
13. Tombol push button       | 4 buah             | Rp 12.000
14. Resistor                 | 330, 10k, 1M       | Rp 5.000
15. Kapasitor                | 0.1uF, 10uF        | Rp 3.000
16. Kabel jumper             | Set                | Rp 15.000
17. PCB lubang/breadboard    | 1 buah             | Rp 10.000
18. Project box              | 12x10x5 cm         | Rp 25.000
-----------------------------------------------------------------
TOTAL BIAYA                                     | Rp 280.000
-----------------------------------------------------------------



4. SKEMA RANGKAIAN
===============================================================

KONEKSI TOMBOL (Internal Pull-up)
-----------------------------------------------------------------
Tombol UP    → Pin D2 → ke GND
Tombol DOWN  → Pin D3 → ke GND
Tombol OK    → Pin D4 → ke GND
Tombol BACK  → Pin D5 → ke GND

KONEKSI LED
-----------------------------------------------------------------
LED IR     → Pin D6 → Resistor 330Ω → GND
LED Merah  → Pin D7 → Resistor 330Ω → GND
LED Hijau  → Pin D8 → Resistor 330Ω → GND

RANGKAIAN FOTODIODA (Transimpedance Amplifier)
-----------------------------------------------------------------
                         +5V
                          |
                         [1M] Rf
                          |
 BPW34 Katoda ───┬───────┤ LM358 Pin 2 (in-)
 BPW34 Anoda ─────┴───────┤ LM358 Pin 3 (in+) ──── GND
                           │
                          LM358 Pin 6 (out) ───── Arduino A0
                           │
                          [0.1uF] ke GND

KONEKSI SENSOR SUHU
-----------------------------------------------------------------
DS18B20:
- Merah  → 5V
- Hitam  → GND
- Kuning → Pin D9 + Resistor 4.7k ke 5V

KONEKSI LCD I2C
-----------------------------------------------------------------
LCD I2C Module:
- VCC    → 5V
- GND    → GND
- SDA    → A4
- SCL    → A5

SISTEM DAYA (Portabel)
-----------------------------------------------------------------
Baterai 18650 (3.7V)
    │
    ▼
Modul TP4056 (Charger)
    │
    ▼
Modul MT3608 (Step-up ke 5V)
    │
    ├───► 5V ke Arduino VIN
    ├───► 5V ke LCD VCC
    └───► 5V ke Op-Amp VCC



5. CARA PERAKITAN
===============================================================

TAHAP 1: PERSIAPAN (30 MENIT)
-----------------------------------------------------------------
1. Siapkan semua komponen dan alat:
   - Soldir, timah, flux
   - Multimeter
   - Tang potong, tang lancip
   - Kabel jumper
   - Bor (untuk enclosure)

2. Pastikan semua komponen dalam kondisi baik:
   - Tes LED dengan multimeter
   - Tes tombol (kontinuitas)
   - Tes sensor suhu (baca nilai)

TAHAP 2: RANGKAIAN DAYA (30 MENIT)
-----------------------------------------------------------------
1. Solder modul TP4056:
   - Input: konektor micro USB
   - Output: B+ dan B- untuk baterai
   - Out+ dan Out- untuk beban

2. Solder modul MT3608:
   - Atur potensiometer untuk output 5V
   - Ukur dengan multimeter
   - IN+ ke Out+ TP4056, IN- ke Out- TP4056

3. Pasang holder baterai:
   - Kabel merah ke B+ TP4056
   - Kabel hitam ke B- TP4056

TAHAP 3: RANGKAIAN OP-AMP (45 MENIT)
-----------------------------------------------------------------
1. Solder IC LM358 di PCB:
   - Pin 4 ke GND
   - Pin 8 ke 5V

2. Buat 2 rangkaian identik (untuk 2 fotodioda):
   - Resistor 1M antara pin 2 dan 6 (feedback)
   - Kapasitor 0.1uF antara pin 6 dan GND
   - Pin 3 ke GND

3. Pasang fotodioda BPW34:
   - Katoda ke pin 2 LM358
   - Anoda ke GND

4. Output pin 6 LM358 ke:
   - A0 untuk fotodioda sensor
   - A1 untuk fotodioda referensi

TAHAP 4: RANGKAIAN INPUT/OUTPUT (30 MENIT)
-----------------------------------------------------------------
1. Pasang LED:
   - LED IR: Anoda ke D6, Katoda via 330Ω ke GND
   - LED Merah: Anoda ke D7, Katoda via 330Ω ke GND
   - LED Hijau: Anoda ke D8, Katoda via 330Ω ke GND

2. Pasang tombol:
   - Setiap tombol: 1 kaki ke pin Arduino, 1 kaki ke GND
   - UP   → D2
   - DOWN → D3
   - OK   → D4
   - BACK → D5

3. Pasang sensor suhu DS18B20:
   - Merah ke 5V
   - Hitam ke GND
   - Kuning ke D9 + resistor 4.7k ke 5V

TAHAP 5: RANGKAIAN LCD (15 MENIT)
-----------------------------------------------------------------
1. Solder header ke modul LCD I2C
2. Pasang modul I2C ke LCD
3. Hubungkan:
   - VCC ke 5V
   - GND ke GND
   - SDA ke A4
   - SCL ke A5

TAHAP 6: PERAKITAN MEKANIK (30 MENIT)
-----------------------------------------------------------------
1. Siapkan enclosure (kotak project box):
   - Lubang untuk LCD (16x2 atau 20x4)
   - Lubang untuk 4 tombol
   - Lubang untuk sensor jari (diameter 1.5cm)
   - Lubang untuk kabel USB (charging)

2. Buat jig jari:
   - Buat lubang di sisi kiri untuk LED (3 buah)
   - Buat lubang di sisi kanan untuk fotodioda sensor
   - Pastikan sejajar dan kedap cahaya

3. Pasang semua komponen ke enclosure:
   - LCD dengan baut
   - Tombol dengan mur
   - PCB dengan spacer
   - Baterai dengan double tape

TAHAP 7: PENGUJIAN (30 MENIT)
-----------------------------------------------------------------
1. Upload kode ke Arduino
2. Cek tegangan dengan multimeter:
   - Output MT3608: 5.0V ±0.1V
   - Output regulator: 5V ke semua komponen

3. Cek tombol:
   - Tekan setiap tombol, lihat perubahan di LCD

4. Cek LED:
   - Nyalakan bergantian via kode test

5. Cek sensor:
   - Fotodioda: tutup dan buka, lihat perubahan nilai A0
   - Sensor suhu: pegang dan lihat perubahan



6. CARA PENGGUNAAN
===============================================================

6.1 MENU UTAMA
-----------------------------------------------------------------
Setelah dinyalakan, akan tampil:

 ALAT UKUR 4-IN-1
›Gula Darah          ← Posisi kursor
 Kolesterol
 Asam Urat
 Suhu Tubuh
 Pengaturan

Cara navigasi:
   ▲ UP    : Pindah ke menu atas
   ▼ DOWN  : Pindah ke menu bawah
   OK      : Pilih menu yang aktif (ditandai ›)
   BACK    : Tidak berfungsi (tetap di menu)


6.2 PENGUKURAN GULA DARAH
-----------------------------------------------------------------
Langkah-langkah:

1. Dari Menu Utama, pilih "Gula Darah" → OK

2. Pilih jenis gula:
   
   PILIH JENIS GULA
   ›GDS (Sewaktu)     ← GDS = gula sewaktu (kapan saja)
    GDP (Puasa)        ← GDP = gula puasa (≥8 jam puasa)
    Kembali

3. Layar konfirmasi muncul:
   
   Letakan jari pada
   alat detektor.
   
   OK(lanjut) BACK(menu)

4. Letakkan jari pada sensor, tekan OK

5. Hasil pengukuran muncul:
   
   GDS: 120 mg/dL
   STATUS: NORMAL
   Suhu: 36.7°C
   [OK]Ulang [BACK]Menu

6. Tekan OK untuk mengulang pengukuran
   Tekan BACK untuk kembali ke Menu Utama


6.3 PENGUKURAN KOLESTEROL
-----------------------------------------------------------------
Langkah-langkah:

1. Dari Menu Utama, pilih "Kolesterol" → OK
2. Langsung ke layar konfirmasi (tanpa sub-menu)
3. Letakkan jari, tekan OK
4. Hasil pengukuran:
   
   KOLESTEROL: 185
   STATUS: NORMAL
   (normal <200)
   [OK]Ulang [BACK]Menu


6.4 PENGUKURAN ASAM URAT
-----------------------------------------------------------------
Langkah-langkah:

1. Dari Menu Utama, pilih "Asam Urat" → OK

2. Pilih gender:
   
   PILIH GENDER
   ›Pria
    Wanita
    Kembali

3. Layar konfirmasi → letakkan jari → OK

4. Hasil pengukuran (Pria):
   
   ASAM URAT: 6.8
   STATUS: NORMAL
   (Pria: 3.4-7.0)
   [OK]Ulang [BACK]Menu

   Hasil pengukuran (Wanita):
   
   ASAM URAT: 6.8
   STATUS: TINGGI
   (Wanita: 2.4-6.0)
   [OK]Ulang [BACK]Menu


6.5 PENGUKURAN SUHU TUBUH
-----------------------------------------------------------------
Langkah-langkah:

1. Dari Menu Utama, pilih "Suhu Tubuh" → OK
2. Langsung ke layar konfirmasi
3. Tempelkan sensor suhu di ketiak atau mulut selama 5 detik
4. Hasil pengukuran:
   
   SUHU TUBUH: 36.7°C
   STATUS: NORMAL
   (normal 36-37.5)
   [OK]Ulang [BACK]Menu



7. CARA KALIBRASI
===============================================================

7.1 MEMASUKI MENU KALIBRASI
-----------------------------------------------------------------
1. Dari Menu Utama, pilih "Pengaturan" → OK
2. Pilih "Kalibrasi" → OK
3. Pilih parameter yang akan dikalibrasi:
   
   KALIBRASI
   ›Gula Darah
    Kolesterol
    Asam Urat
    Suhu Tubuh
    Kembali


7.2 CARA INPUT NILAI REFERENSI
-----------------------------------------------------------------
Setelah memilih parameter, tampil layar input:

   KALIBRASI GULA
   Sampel 1/10
   Nilai: _ _ _       ← Kursor berkedip

Cara mengisi nilai:
   1. Tekan UP/DOWN untuk mengubah digit yang berkedip
   2. Tekan OK untuk pindah ke digit berikutnya
   3. Ulangi sampai semua digit terisi
   4. Setelah digit terakhir, tekan OK untuk selesai

Contoh: input nilai 125
   - Digit 1 (ratusan): UP 1x → "1 _ _" → OK
   - Digit 2 (puluhan): UP 4x → "1 4 _" → OK
   - Digit 3 (satuan): UP 5x → "1 4 5" → OK

Setelah selesai input, tampil:

   KALIBRASI GULA
   Sampel 1/10
   Nilai: 125
   [›UKUR] [KEMBALI]


7.3 PROSES PENGUKURAN KALIBRASI
-----------------------------------------------------------------
1. Pilih [›UKUR] (pastikan kursor di UKUR) → OK

2. Layar siap ukur:
   
   KALIBRASI GULA
   Sampel 1/10
   Nilai: 125
   Letakan jari lalu
   TEKAN OK UKUR

3. Letakkan jari pada sensor, tekan OK

4. Hasil pengukuran muncul:
   
   KALIBRASI GULA
   Sampel 1/10
   Alat: 124.3
   [›TAMBAH] [SELESAI]


7.4 MENAMBAH SAMPEL ATAU SELESAI
-----------------------------------------------------------------
Setelah melihat hasil:

Jika memilih [›TAMBAH]:
   - Sampel tersimpan
   - Kembali ke layar input untuk sampel berikutnya
   - Ulangi langkah 7.2 - 7.3

Jika memilih [SELESAI]:
   - Semua data diproses
   - Koefisien kalibrasi dihitung
   - Tersimpan di EEPROM
   - Tampil layar selesai:
     
     KALIBRASI SELESAI
     Total: 5 sampel
     Data tersimpan
     TEKAN OK KEMBALI



8. TROUBLESHOOTING
===============================================================

MASALAH: LCD TIDAK MENYALA
-----------------------------------------------------------------
Kemungkinan Penyebab:
   - Koneksi I2C terbalik
   - Alamat I2C salah (harus 0x27 atau 0x3F)
   - Tegangan tidak sampai 5V

Solusi:
   1. Cek koneksi SDA ke A4, SCL ke A5
   2. Jalankan sketch I2C scanner untuk cek alamat
   3. Ukur tegangan di pin VCC LCD (harus 5V)


MASALAH: TOMBOL TIDAK RESPON
-----------------------------------------------------------------
Kemungkinan Penyebab:
   - Koneksi tombol terbalik
   - Tidak menggunakan pull-up internal
   - Debounce delay terlalu kecil

Solusi:
   1. Pastikan satu kaki tombol ke pin, satu ke GND
   2. Cek pinMode dengan INPUT_PULLUP
   3. Tambah debounceDelay jadi 300


MASALAH: NILAI SENSOR TIDAK STABIL
-----------------------------------------------------------------
Kemungkinan Penyebab:
   - Cahaya luar masuk ke sensor
   - Posisi jari tidak konsisten
   - Tegangan tidak stabil

Solusi:
   1. Perbaiki enclosure (kedap cahaya)
   2. Buat jig jari yang presisi
   3. Tambah kapasitor 100uF di output regulator
   4. Lakukan averaging (baca 10x, ambil rata-rata)


MASALAH: SENSOR SUHU ERROR (TAMPIL -127°C)
-----------------------------------------------------------------
Kemungkinan Penyebab:
   - Koneksi DS18B20 terbalik
   - Resistor pull-up 4.7k tidak terpasang
   - Sensor rusak

Solusi:
   1. Cek warna kabel (Merah=5V, Hitam=GND, Kuning=Data)
   2. Pasang resistor 4.7k antara Data dan 5V
   3. Ganti sensor jika perlu


MASALAH: BATERAI CEPAT HABIS
-----------------------------------------------------------------
Kemungkinan Penyebab:
   - LED menyala terus
   - Modul step-up tidak efisien
   - Baterai sudah tua

Solusi:
   1. Pastikan LED hanya menyala saat pengukuran
   2. Ganti MT3608 dengan yang lebih efisien
   3. Gunakan baterai 18650 baru



9. SPESIFIKASI HASIL PENGUKURAN
===============================================================

GULA DARAH
-----------------------------------------------------------------
Parameter           : GDS (Sewaktu) dan GDP (Puasa)
Satuan              : mg/dL
Rentang pengukuran  : 50 - 400 mg/dL
Akurasi (setelah kalibrasi) : ±10-15%
Waktu pengukuran    : 5-10 detik

Interpretasi GDS:
   < 70   : RENDAH
   70-199 : NORMAL
   200-299: TINGGI
   ≥ 300  : BAHAYA!

Interpretasi GDP:
   < 70   : RENDAH
   70-125 : NORMAL
   126-199: TINGGI
   ≥ 200  : BAHAYA!


KOLESTEROL TOTAL
-----------------------------------------------------------------
Satuan              : mg/dL
Rentang pengukuran  : 100 - 400 mg/dL
Akurasi (setelah kalibrasi) : ±10-15%

Interpretasi:
   < 200  : NORMAL
   200-239: BATAS
   ≥ 240  : TINGGI


ASAM URAT
-----------------------------------------------------------------
Satuan              : mg/dL
Rentang pengukuran  : 1.0 - 15.0 mg/dL
Akurasi (setelah kalibrasi) : ±0.5-1.0 mg/dL

Interpretasi PRIA:
   < 3.4 : RENDAH
   3.4-7.0 : NORMAL
   7.1-9.0 : TINGGI
   > 9.0  : SANGAT TINGGI

Interpretasi WANITA:
   < 2.4 : RENDAH
   2.4-6.0 : NORMAL
   6.1-8.0 : TINGGI
   > 8.0  : SANGAT TINGGI


SUHU TUBUH
-----------------------------------------------------------------
Satuan              : °C
Rentang pengukuran  : 32 - 42°C
Akurasi             : ±0.5°C (sensor DS18B20)

Interpretasi:
   < 35.0 : HIPOTERMIA
   35.0-35.9 : DINGIN
   36.0-37.5 : NORMAL
   37.6-38.9 : DEMAM
   ≥ 39.0   : DEMAM TINGGI



10. KESIMPULAN
===============================================================

KEUNGGULAN ALAT
-----------------------------------------------------------------
✓ NON-INVASIVE - Tidak perlu ambil darah, aman dari infeksi
✓ 4-IN-1 - Ukur Gula, Kolesterol, Asam Urat, dan Suhu
✓ MURAH - Total biaya hanya Rp 280.000
✓ MUDAH DIGUNAKAN - Navigasi 4 tombol, panduan di LCD
✓ KALIBRASI LENGKAP - Bisa kalibrasi dengan alat konvensional
✓ PORTABEL - Menggunakan baterai 18650, tahan 8-10 jam
✓ DATA TERSIMPAN - Koefisien kalibrasi di EEPROM
✓ INTERPRETASI OTOMATIS - Langsung tahu normal/tidaknya

KETERBATASAN
-----------------------------------------------------------------
✗ BUKAN ALAT MEDIS - Hanya untuk screening/monitoring pribadi
✗ Akurasi bergantung pada kalibrasi
✗ Perlu kalibrasi berkala (1-2 bulan sekali)
✗ Hasil dapat dipengaruhi suhu jari dan tekanan
✗ Tidak dapat menggantikan tes laboratorium

SARAN PENGGUNAAN
-----------------------------------------------------------------
- Lakukan kalibrasi awal dengan 5-10 sampel dari alat medis
- Ukur pada waktu yang sama setiap hari untuk konsistensi
- Pastikan jari bersih dan kering sebelum pengukuran
- Jangan menggerakkan jari selama pengukuran
- Lakukan pengukuran 2-3 kali dan ambil rata-rata
- Jika hasil meragukan, cek dengan alat medis konvensional

KONTAK & DUKUNGAN
-----------------------------------------------------------------
Proyek ini bersifat open-source.
Untuk pertanyaan, saran, atau pengembangan lebih lanjut:

- GitHub: @Adiflaz
- Forum: Arduino Indonesia
- Email: adiflazuardiimani@gmail.com

Selamat mencoba dan semoga bermanfaat!

--- TERIMA KASIH ---
