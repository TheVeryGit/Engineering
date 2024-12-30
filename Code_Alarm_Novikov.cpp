#include <LiquidCrystal.h> // Подключение библиотеки для работы с LCD

// Инициализация LCD (пины для подключения экрана)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

int starttime; // Время старта
int activetime; // Активное время (в секундах)
int prevoustime = 0; // Переменная для отслеживания времени

int hours = 0; // Часы
int mins = 0; // Минуты
int seconds = 0; //секунды

int ahours = 0; // Часы будильника
int amins = 0; // Минуты будильника

// Переменные для головоломки
bool alarmActive = false; // Активирован ли будильник
int correctButton = 0; // 1 - кнопка 10, 2 - кнопка 11, 3 - кнопка 12
bool puzzleSolved = false; //Флаг для отслеживания, решён ли пример

void setup() {
  lcd.begin(16, 2); // Инициализация LCD экрана (16 символов на 2 строки)
  lcd.clear(); // Очистка экрана

  Serial.begin(9600); // Инициализация серийного порта для отладки

  // Настройка пинов как входных и активация подтягивающих резисторов
  pinMode(11, INPUT);
  digitalWrite(11, HIGH);
  pinMode(10, INPUT);
  digitalWrite(10, HIGH);
  pinMode(12, INPUT);
  digitalWrite(12, HIGH);
  pinMode(8, INPUT);
  digitalWrite(8, HIGH);

  pinMode(A0, OUTPUT); // Настройка аналогового пина как выход
  digitalWrite(A0, HIGH); // Установка высокого уровня

  pinMode(9, OUTPUT); // Настройка пина 9 как выход для пьезо
  randomSeed(analogRead(A0)); // Инициализация генератора случайных чисел
}

void loop() {
  // Пока не решена головоломка продолжаем настройку времени будильника
  while (digitalRead(8) == LOW && !puzzleSolved) {
    lcd.setCursor(6, 1); // Устанавливаем курсор в нужное место
    lcd.print("Alarm"); 
    lcd.setCursor(6, 0); // Устанавливаем курсор на первую строку

    // Увеличиваем минуты или часы будильника в зависимости от нажатой кнопки
    if (digitalRead(11) == LOW) {
      amins++; // Увеличиваем минуты
    } else if (digitalRead(10) == LOW) {
      ahours++; // Увеличиваем часы
    }

    // Выводим текущее время будильника на экран
    lcd.setCursor(6, 0);
    if (ahours < 10) {
      lcd.print("0"); // Добавляем ноль перед числом, если меньше 10
      lcd.print(ahours); // Выводим часы
    } else {
      lcd.print(ahours); // Выводим часы
    }

    lcd.print(":"); // Печать двоеточия

    if (amins < 10) {
      lcd.print("0"); // Добавляем ноль перед числом, если меньше 10
      lcd.print(amins); // Выводим минуты
    } else {
      lcd.print(amins); // Выводим минуты
    }

    // Если минуты больше 59, увеличиваем часы и сбрасываем минуты
    if (amins > 59) {
      ahours++;
      amins = 0;
    }
    if (ahours > 23) { // Если часы больше 23, сбрасываем их
      ahours = 0;
    }

    delay(500); // Задержка перед обновлением экрана
    lcd.clear(); // Очистка экрана
  }

  // Обновление текущего времени
  activetime = (millis() / 1000) - starttime; // Получаем время с момента старта в секундах
  if (prevoustime < (activetime - 59)) { // Если прошло больше 59 секунд, увеличиваем минуты
    mins++;
    prevoustime = activetime; 
  }

  // Если минуты больше 59, увеличиваем часы и сбрасываем минуты
  if (mins > 59) {
    hours++;
    mins = 0;
  }
  if (hours > 23) { // Если часы больше 23, сбрасываем их
    hours = 0;
  }

  // Выводим текущее время
  lcd.setCursor(6, 0);
  if (hours < 10) {
    lcd.print("0");
    lcd.print(hours); // Выводим часы
  } else {
    lcd.print(hours); // Выводим часы
  }
  lcd.print(":");
  if (mins < 10) {
    lcd.print("0");
    lcd.print(mins); // Выводим минуты
  } else {
    lcd.print(mins); // Выводим минуты
  }

  // Если время будильника совпадает с текущим временем, активируем будильник
  if (ahours == hours && amins == mins && amins != 0 && !puzzleSolved) {
    if (!alarmActive) { // Генерация головоломки только если она не решена
      startPuzzle();
      alarmActive = true;
    }

    playAlarm(); // Воспроизведение сигнала будильника
    checkButtons(); // Проверка нажатых кнопок
  } else if (puzzleSolved) { // Если головоломка решена, останавливаем сигнал
    noTone(9); // Останавливаем пьезо
  } else {
    delay(300); // Задержка
    lcd.clear(); // Очистка экрана
  }
}

// Функция для генерации головоломки
void startPuzzle() {
  int num1 = random(1, 10);   // Генерация случайных чисел от 1 до 9
  int num2 = random(1, 10);   // Генерация случайных чисел от 1 до 9
  int operation = random(0, 2); // 0 - сложение, 1 - умножение
  int correctAnswer = (operation == 0) ? num1 + num2 : num1 * num2; // Правильный ответ
  int wrongAnswer1 = correctAnswer + random(-3, 4); // Генерация первого неправильного ответа
  int wrongAnswer2 = correctAnswer + random(-3, 4); // Генерация второго неправильного ответа

  // Убедимся, что неправильные ответы не совпадают с правильным и друг с другом
  while (wrongAnswer1 == correctAnswer) {
    wrongAnswer1 = correctAnswer + random(-3, 4);
  }
  while (wrongAnswer2 == correctAnswer || wrongAnswer2 == wrongAnswer1) {
    wrongAnswer2 = correctAnswer + random(-3, 4);
  }

  lcd.clear(); // Очистка экрана
  lcd.setCursor(0, 0);
  lcd.print(num1); // Печать первого числа
  if (operation == 0) {
    lcd.print("+"); // Операция сложения
  } else {
    lcd.print("*"); // Операция умножения
  }
  lcd.print(num2); // Печать второго числа
  lcd.print("="); // Печать знака равно

  lcd.setCursor(0, 1);
  int order = random(0, 6); // Шесть возможных вариантов расположения ответов
  switch (order) {
    case 0:
      lcd.print("1)"); lcd.print(correctAnswer); lcd.print(" 2)"); lcd.print(wrongAnswer1); lcd.print(" 3)"); lcd.print(wrongAnswer2);
      correctButton = 1; // Правильный ответ - кнопка 1
      break;
    case 1:
      lcd.print("1)"); lcd.print(correctAnswer); lcd.print(" 2)"); lcd.print(wrongAnswer2); lcd.print(" 3)"); lcd.print(wrongAnswer1);
      correctButton = 1; // Правильный ответ - кнопка 1
      break;
    case 2:
      lcd.print("1)"); lcd.print(wrongAnswer1); lcd.print(" 2)"); lcd.print(correctAnswer); lcd.print(" 3)"); lcd.print(wrongAnswer2);
      correctButton = 2; // Правильный ответ - кнопка 2
      break;
    case 3:
      lcd.print("1)"); lcd.print(wrongAnswer2); lcd.print(" 2)"); lcd.print(correctAnswer); lcd.print(" 3)"); lcd.print(wrongAnswer1);
      correctButton = 2; // Правильный ответ - кнопка 2
      break;
    case 4:
      lcd.print("1)"); lcd.print(wrongAnswer1); lcd.print(" 2)"); lcd.print(wrongAnswer2); lcd.print(" 3)"); lcd.print(correctAnswer);
      correctButton = 3; // Правильный ответ - кнопка 3
      break;
    case 5:
      lcd.print("1)"); lcd.print(wrongAnswer2); lcd.print(" 2)"); lcd.print(wrongAnswer1); lcd.print(" 3)"); lcd.print(correctAnswer);
      correctButton = 3; // Правильный ответ - кнопка 3
      break;
  }
}

// Функция для воспроизведения сигнала будильника
void playAlarm() {
  tone(9, 1000, 200); // Включение сигнала на пине 9
  delay(200); // Задержка
  noTone(9); // Остановка сигнала
  delay(200); // Задержка
}

// Проверка нажатия кнопок
void checkButtons() {
  if (digitalRead(10) == LOW && correctButton == 1) { // Кнопка 10
    stopAlarm();
  } else if (digitalRead(11) == LOW && correctButton == 2) { // Кнопка 11
    stopAlarm();
  } else if (digitalRead(12) == LOW && correctButton == 3) { // Кнопка 12
    stopAlarm();
  }
}

// Остановка будильника
void stopAlarm() {
  noTone(9); // Остановить звук
  lcd.clear(); // Очистить экран
  lcd.print("Correct!"); // Правильно!
  puzzleSolved = true; // Устанавливаем флаг решённой головоломки
  delay(2000); // Задержка 2 секунды
  lcd.clear(); // Очистка экрана
  alarmActive = false; // Остановить будильник
}
