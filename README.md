# ❤️ Heart Rate Monitor Berbasis Mikrokontroler

## 📖 Deskripsi

**Heart Rate Monitor** adalah sistem monitoring detak jantung berbasis mikrokontroler yang mampu mengukur denyut jantung (**BPM - Beats Per Minute**) secara **real-time** menggunakan sensor detak jantung. Data hasil pengukuran dikirimkan ke platform **Internet of Things (IoT)** sehingga pengguna dapat memantau kondisi denyut jantung melalui dashboard berbasis web menggunakan smartphone maupun komputer.

Sistem ini dirancang sebagai solusi sederhana untuk pemantauan denyut jantung secara langsung dengan memanfaatkan konektivitas internet dan mikrokontroler ESP32.

---

## ✨ Fitur Utama

* ❤️ Pengukuran detak jantung (**BPM**) secara real-time.
* 📡 Pengiriman data sensor ke platform IoT melalui koneksi Wi-Fi.
* 📊 Menampilkan nilai BPM pada dashboard web secara langsung.
* 🚨 Memberikan peringatan apabila detak jantung berada di luar batas normal.
* 📱 Dapat diakses melalui smartphone maupun komputer.
* ⚡ Menggunakan ESP32 yang memiliki konektivitas Wi-Fi bawaan.

---

## 🛠️ Hardware yang Digunakan

| Komponen                | Fungsi                                        |
| ----------------------- | --------------------------------------------- |
| ESP32                   | Mikrokontroler utama dan pengirim data ke IoT |
| Pulse Heart Rate Sensor | Mengukur denyut jantung pengguna              |
| Breadboard              | Media perakitan rangkaian                     |
| Kabel Jumper            | Menghubungkan seluruh komponen                |

---

## ⚙️ Cara Kerja Sistem

1. Pengguna meletakkan jari pada **Pulse Heart Rate Sensor**.
2. Sensor membaca sinyal denyut jantung.
3. ESP32 mengolah sinyal menjadi nilai **Beats Per Minute (BPM)**.
4. Data BPM dikirim melalui koneksi Wi-Fi ke platform IoT.
5. Dashboard web menampilkan data secara real-time.
6. Apabila nilai BPM berada di luar batas normal, sistem akan memberikan peringatan.

---

## 📊 Output Sistem

* Nilai BPM secara real-time.
* Dashboard monitoring berbasis web.
* Status kondisi denyut jantung (Normal <100BPM / Tinggi >100BPM).
* Riwayat data pengukuran (dalam bentuk csv).

---

## 🎯 Tujuan Proyek

Proyek ini bertujuan untuk membangun sistem monitoring detak jantung yang sederhana, murah, dan mudah digunakan dengan memanfaatkan teknologi **Internet of Things (IoT)** sehingga pengguna dapat memantau kondisi denyut jantung kapan saja dan di mana saja.

---

## 📷 Dokumentasi
**Low-Rate BPM**<br>
<img hight="300" width="700" alt="GIF" align="center" src="https://github.com/Hernada/Mikrokontroller/blob/a5a338ef73dfe19224f82ef2a819522b39d4cefa/Low-rate.gif">

**High-Rate BPM**<br>
<img hight="300" width="700" alt="GIF" align="center" src="https://github.com/Hernada/Mikrokontroller/blob/a5a338ef73dfe19224f82ef2a819522b39d4cefa/High-rate.gif">

---

## 🚀 Pengembangan Selanjutnya

* Penyimpanan riwayat data ke database.
* Integrasi notifikasi melalui Telegram atau WhatsApp.
* Penambahan sensor kesehatan lainnya, seperti SpO₂ dan suhu tubuh.

---

## 👨‍💻 Teknologi yang Digunakan

* ESP32
* Pulse Heart Rate Sensor
* Arduino IDE
* RTOS
* HTTP Dashboard Web

---

## Contributor
Nama    : Rizki Fauzi<br>
NPM     : 23552011070<br>
Username: Ikki-uzi

Nama    : Adi Esa Putra<br>
NPM     : 23552011230<br>
Username: adiesaputra2011230

Nama    : Muhammad Nabil Hernada<br>
NPM     : 23552011064<br>
Username: Hernada
