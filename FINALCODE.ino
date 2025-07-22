

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <MFRC522.h>
#include <RTClib.h>
#include <WiFi.h>
#include <FirebaseESP32.h>
#include <ESP_Mail_Client.h>
SMTPData smtp;  // Ensure this is declared properly
#define SMTP_HOST "smtp.gmail.com"


// 🔹 WiFi Credentials
const char* ssid = "Sanjay-5G";
const char* password = "04274591";

// 🔹 Firebase Credentials
#define FIREBASE_HOST "https://attendance-41062-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "Q2o4Arpup5FeXAFxjnJorFqR5OUceO6j6Ar0VxIX"

// 🔹 Email Credentials
#define EMAIL_SENDER "shreya081105@gmail.com"   // Replace with your Gmail
#define EMAIL_PASSWORD "efyq dmkd ldav jgvu"       // Replace with App Password

// 🔹 RFID & Display
#define RST_PIN 9
#define SS_PIN 10

MFRC522 mfrc522(SS_PIN, RST_PIN);
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
SMTPData smtp;

// 🔹 Student Data (RFID UID → Name & Email)
struct CardInfo {
    byte uid[4];
    String name;
    String email;
};

CardInfo knownCards[] = {
    {{0xD3, 0xD0, 0x61, 0xFC}, "Abhinand_Ajith", "shreyasanjay811@gmail.com"},
    {{0x03, 0x60, 0xB2, 0x0E}, "Zidane_Mustafa", "sigmashreya33@gmail.com"},
    {{0x91, 0xFF, 0xC5, 0x1B}, "Mark_Judy", "nivishreya8@gmail.com"},
    {{0x02, 0xA1, 0x19, 0x1B}, "Shehzad_Iqbal", "cheyubeenand@gmail.com"}
};

// 🔹 Email Function
void sendLowAttendanceEmail(String email, String studentName, float attendancePercentage) {
    smtp.setLogin(SMTP_HOST, 465, EMAIL_SENDER, EMAIL_PASSWORD);
    smtp.setSender("Attendance System", EMAIL_SENDER);
    smtp.setPriority(esp_mail_smtp_priority::esp_mail_smtp_priority_normal);
    smtp.setSubject("⚠️ Low Attendance Alert!");

    String emailBody = "Dear " + studentName + ",\n\n"
                       "Your attendance is currently at " + String(attendancePercentage, 1) + "%. "
                       "Please ensure you attend more classes to avoid any penalties.\n\n"
                       "Best regards,\nAttendance System";

    smtp.setMessage(emailBody, false);
    smtp.addRecipient(email);

    if (!MailClient.sendMail(smtp)) {
        Serial.println("❌ Email failed to send: " + smtp.errorReason());
    } else {
        Serial.println("✅ Email sent successfully to " + email);
    }
    smtp.empty();
}


void setup() {
    Serial.begin(115200);
    Wire.begin();
    SPI.begin();
    mfrc522.PCD_Init();
    lcd.init();
    lcd.backlight();


    if (!rtc.begin()) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RTC not found!");
        while (1);
    }

    Serial.print("Connecting to WiFi...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\n✅ WiFi Connected!");

    config.database_url = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan Card...");
}

void loop() {
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial()) return;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Processing...");
    Serial.println("\n**Card Detected!**");

    String studentName = "Unknown";
    String studentEmail = "";
    bool studentFound = false;

    for (CardInfo card : knownCards) {
        if (memcmp(mfrc522.uid.uidByte, card.uid, 4) == 0) {
            studentName = card.name;
            studentEmail = card.email;
            studentFound = true;
            break;
        }
    }

    DateTime now = rtc.now();
    String timestamp = String(now.day()) + "-" + String(now.month()) + "-" + String(now.year()) + " " +
                        String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());

    String basePath = "/attendance/" + studentName;
    Serial.print("Uploading to Firebase: ");
    Serial.println(basePath);

    String totalClassesPath = basePath + "/total_classes";
    int totalClasses = Firebase.getInt(fbdo, totalClassesPath) ? fbdo.intData() : 100; 

    if (studentFound) {
        String attendedClassesPath = basePath + "/attended_classes";
        int attendedClasses = Firebase.getInt(fbdo, attendedClassesPath) ? fbdo.intData() : 0;
        Firebase.setInt(fbdo, attendedClassesPath, attendedClasses + 1);

        float attendancePercentage = ((float)(attendedClasses + 1) / totalClasses) * 100;

        Serial.print("Attendance for ");
        Serial.print(studentName);
        Serial.print(": ");
        Serial.print(attendancePercentage);
        Serial.println("%");

        if (attendancePercentage < 75) {
            Serial.println("⚠️ Low attendance! Sending email...");
            sendLowAttendanceEmail(studentEmail, studentName, attendancePercentage);
        }

    }

    Firebase.setString(fbdo, basePath + "/last_attendance", timestamp);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Attendance Logged");
    lcd.setCursor(0, 1);
    lcd.print(studentName);
    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Scan Card...");

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}
