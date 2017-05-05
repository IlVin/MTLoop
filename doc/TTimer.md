## TTimer

    class {
        inline static uint32_t GetTime();
    }

Класс со статическим методом, возвращающим время.

С помощью макросов можно сгенерировать разные таймеры для разных целей:
  * Arduino micros()
  * Arduino millis()
  * Arduino tick таймер (считает время в тиках)
  * Mock для unit тестов
