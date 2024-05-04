//Ï³äêëþ÷åííÿ á³áë³îòåê òà îãîëîøåííÿ çì³ííèõ
#include <Wire.h> // new comment
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>  // á³áë³îòåêè
#include <EEPROM.h> // Ï³äêëþ÷àºìî á³áë³îòåêó EEPROM



RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);  //äèñïëåé

void ringBell(DateTime now);  //ôóíêö³ÿ äçâîíèêà

const int signalPin = 7;  // ï³í 7 äëÿ âèâîäó ñèãíàëó
const int SIGNAL_DURATION = 5000;  // òðèâàë³ñòü ñèãíàëó â ì³ë³ñåêóíäàõ
const unsigned long eventInterval = 5000;  // ³íòåðâàë ì³æ ïîä³ÿìè â ì³ë³ñåêóíäàõ (5 ñåêóíä)
unsigned long lastSignalMillis = 0;  //çì³ííà äëÿ çáåð³ãàííÿ îñòàííüîãî ñèãíàëó
int signalCount = 0;  //çì³ííà äëÿ ë³÷èëüíèêà ñèãíàë³â
unsigned long lastEventMillis = 0; //çì³ííà äëÿ çáåð³ãàííÿ ÷àñó îñòàííüî¿ ïîä³¿

unsigned long lastDisplayUpdate = 0; // çì³ííà äëÿ çáåð³ãàííÿ ÷àñó îñòàííüîãî îíîâëåííÿ äèñïëåÿ
const unsigned long displayUpdateInterval = 1000; // ³íòåðâàë îíîâëåííÿ äèñïëåÿ â ì³ë³ñåêóíäàõ

unsigned long lastMillis = 0;

bool waitingForCycleStart = true;
bool lessonTriggered = false;

String lessons[] = { "lesson_0", "lesson_1", "PAUSE", "lesson_2", "PAUSE",
 "lesson_3", "PAUSE", "lesson_4", "PAUSE", "lesson_5", "PAUSE",
"lesson_6", "PAUSE", "lesson_7", "PAUSE", "lesson_8", "PAUSE" };  //ñïèñîê ç óðîêàìè

void updateDisplay(DateTime now) {
    // Îòðèìàòè ïîòî÷íèé ÷àñ millis()
    unsigned long currentMillis = millis();

    // Ïåðåâ³ðêà íà ïåðåïîâíåííÿ millis()
    if (currentMillis < lastMillis) {
        // ßêùî ïåðåïîâíåííÿ, îíîâèòè çíà÷åííÿ lastMillis
        lastMillis = currentMillis;
    }

    // Ïåðåâ³ðêà, ÷è ïðîéøëî äîñèòü ÷àñó äëÿ îíîâëåííÿ äèñïëåþ
    if (currentMillis - lastDisplayUpdate >= displayUpdateInterval) {
        // Â³äîáðàæåííÿ ³íôîðìàö³¿ íà äèñïëå¿
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

        // Îíîâèòè çíà÷åííÿ lastDisplayUpdate
        lastDisplayUpdate = currentMillis;
    }
}


//Íàëàøòóâàííÿ
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

//Îñíîâíèé öèêë ïðîãðàìè
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
                lessonTriggered = true;  // Ïîì³òêà, ùî ïîä³ÿ âæå âèêëèêàíà
                signalCount++;
            }
        }
        else {
            lessonTriggered = false;
            // Ïåðåâ³ðêà ÷àñó äëÿ âèìêíåííÿ ñèãíàëó
            if (millis() - lastSignalMillis >= SIGNAL_DURATION) {
                digitalWrite(signalPin, LOW);  // Çíèæåííÿ ï³íà, ÿêùî íå â ÷àñ âèñîêîãî ñèãíàëó
            }
        }
    }
}
