//ϳ��������� ������� �� ���������� ������
#include <Wire.h> // �������� 222
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>  // ��������
#include <EEPROM.h> // ϳ�������� �������� EEPROM



RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);  //�������

void ringBell(DateTime now);  //������� ��������

const int signalPin = 7;  // �� 7 ��� ������ �������
const int SIGNAL_DURATION = 5000;  // ��������� ������� � ����������
const unsigned long eventInterval = 5000;  // �������� �� ������ � ���������� (5 ������)
unsigned long lastSignalMillis = 0;  //����� ��� ��������� ���������� �������
int signalCount = 0;  //����� ��� ��������� �������
unsigned long lastEventMillis = 0; //����� ��� ��������� ���� �������� ��䳿

unsigned long lastDisplayUpdate = 0; // ����� ��� ��������� ���� ���������� ��������� �������
const unsigned long displayUpdateInterval = 1000; // �������� ��������� ������� � ����������

unsigned long lastMillis = 0;

bool waitingForCycleStart = true;
bool lessonTriggered = false;

String lessons[] = { "lesson_0", "lesson_1", "PAUSE", "lesson_2", "PAUSE",
 "lesson_3", "PAUSE", "lesson_4", "PAUSE", "lesson_5", "PAUSE",
"lesson_6", "PAUSE", "lesson_7", "PAUSE", "lesson_8", "PAUSE" };  //������ � �������

void updateDisplay(DateTime now) {
    // �������� �������� ��� millis()
    unsigned long currentMillis = millis();

    // �������� �� ������������ millis()
    if (currentMillis < lastMillis) {
        // ���� ������������, ������� �������� lastMillis
        lastMillis = currentMillis;
    }

    // ��������, �� ������� ������ ���� ��� ��������� �������
    if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        // ³���������� ���������� �� ������
        lcd.clear();
        lcd.setCursor(4, 1);
        if ((now.hour() >= 7 && now.hour() <= 16) && (now.dayOfTheWeek() >= 1 && now.dayOfTheWeek() <= 5)) {
            lcd.print(lessons[signalCount]);
        }
        else {
            lcd.clear();
            lcd.setCursor(2, 1);
            lcd.print("coffee time");
        }

        lcd.setCursor(1, 0);
        lcd.print("*time:");
        lcd.print(now.hour(), DEC);
        lcd.print(':');
        lcd.print(now.minute(), DEC);
        lcd.print(':');
        lcd.print(now.second(), DEC);

        // ������� �������� lastDisplayUpdate
        lastDisplayUpdate = currentMillis;
    }
}


//������������
void setup() {
    Serial.begin(9600);
    pinMode(signalPin, OUTPUT);

    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1);
    }
    if (rtc.lostPower()) {
        Serial.println("RTC lost power, let's set the time!");
        //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    }

    lcd.init();
    lcd.backlight();

}

//�������� ���� ��������
void loop() {
    DateTime now = rtc.now();
    updateDisplay(now);

    if (now.dayOfTheWeek() >= 1 && now.dayOfTheWeek() <= 5) {
        ringBell(now);
    }


}

void ringBell(DateTime now) {
    if (waitingForCycleStart) {
        if (now.hour() >= 8 && !(now.hour() == 16)) {
            waitingForCycleStart = false;
            signalCount = 0;
            lcd.clear();
            lcd.setCursor(8, 0);
            lastSignalMillis = millis();
        }
    }
    else {
        if ((now.hour() == 8 && now.minute() == 0 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 8 && now.minute() == 45 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 8 && now.minute() == 55 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 9 && now.minute() == 40 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 9 && now.minute() == 50 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 10 && now.minute() == 35 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 10 && now.minute() == 45 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 11 && now.minute() == 30 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 11 && now.minute() == 40 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 12 && now.minute() == 25 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 13 && now.minute() == 10 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 13 && now.minute() == 55 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 14 && now.minute() == 5 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 14 && now.minute() == 50 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 15 && now.minute() == 0 && now.second() >= 0 && now.second() < 5) ||
            (now.hour() == 15 && now.minute() == 45 && now.second() >= 0 && now.second() < 5)) {

            if (!lessonTriggered) {
                digitalWrite(signalPin, HIGH);
                lessonTriggered = true;  // ������, �� ���� ��� ���������
                signalCount++;
            }
        }
        else {
            lessonTriggered = false;
            // �������� ���� ��� ��������� �������
            if (millis() - lastSignalMillis >= SIGNAL_DURATION) {
                digitalWrite(signalPin, LOW);  // �������� ���, ���� �� � ��� �������� �������
            }
        }
    }
}
