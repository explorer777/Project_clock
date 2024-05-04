//Підключення бібліотек та оголошення змінних
#include <Wire.h> 
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>  // бібліотеки
#include <EEPROM.h> // Підключаємо бібліотеку EEPROM

int currentLessonAddress = 0;
RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);  //дисплей

void ringBell(DateTime now);  //функція дзвоника

const int signalPin = 7;  // пін 7 для виводу сигналу
const int SIGNAL_DURATION = 5000;  // тривалість сигналу в мілісекундах
const unsigned long eventInterval = 5000;  // інтервал між подіями в мілісекундах (5 секунд)
unsigned long lastSignalMillis = 0;  //змінна для зберігання останнього сигналу
int signalCount = 0;  //змінна для лічильника сигналів
unsigned long lastEventMillis = 0; //змінна для зберігання часу останньої події

unsigned long lastDisplayUpdate = 0; // змінна для зберігання часу останнього оновлення дисплея
const unsigned long displayUpdateInterval = 1000; // інтервал оновлення дисплея в мілісекундах

bool waitingForCycleStart = true;
bool lessonTriggered = false;

String lessons[] = { "lesson_0", "lesson_1", "PAUSE", "lesson_2", "PAUSE",
 "lesson_3", "PAUSE", "lesson_4", "PAUSE", "lesson_5", "PAUSE",
"lesson_6", "PAUSE", "lesson_7", "PAUSE", "lesson_8", "PAUSE" };  //список з уроками

//Налаштування
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

//Основний цикл програми
void loop() {
    DateTime now = rtc.now();
    if (millis() - lastDisplayUpdate >= displayUpdateInterval) {
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
        // Вивести значення дня тижня на монітор Serial
        Serial.print("Day of the week: ");
        Serial.println(now.dayOfTheWeek());

        // Виведення часу у монітор порт
        Serial.print("Time: ");
        Serial.print(now.hour(), DEC);
        Serial.print(':');
        Serial.print(now.minute(), DEC);
        Serial.print(':');
        Serial.println(now.second(), DEC);

        lastDisplayUpdate = millis(); // оновлення часу останнього оновлення дисплея
    }

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
                lessonTriggered = true;  // Помітка, що подія вже викликана
                signalCount++;
            }
        }
        else {
            lessonTriggered = false;
            // Перевірка часу для вимкнення сигналу
            if (millis() - lastSignalMillis >= SIGNAL_DURATION) {
                digitalWrite(signalPin, LOW);  // Зниження піна, якщо не в час високого сигналу
            }
        }
    }
}
