## TTask

Идеологически правильным способом создания подзадачи является создание класса
производного от TTask.

    class TTask {
        private:
            uint32_t startTime = 0;
            uint32_t stopTime = 0;

        public:
            TTask();
            TTask(const TTask& task);
            virtual ~TTask() = 0;

            inline void Tick(TLog& log);
            inline uint32_t GetStartTime();
            inline uint32_t GetStopTime();
            inline uint32_t GetDuration();

            inline virtual void Run(TLog& log);
    };

В производном классе необходимо переопределить метод **void Run(TLog& log)**.
В качестве примера пользовательского класса можно рассмотреть класс библиотеки **TCallback**.

    typedef void(*callbackPtr)(TLog& log);
    class TCallback: public TTask {
        private:
            callbackPtr cb;

        public:
            TCallback(callbackPtr cb): TTask(), cb(cb) {}
            TCallback(const TCallback& task): TTask(task), cb(task.cb) {}

            inline virtual void Run(TLog& log) {
                cb(log);
            };
    };

## Использование в скетче

    #define DIODE_PIN1 13
    #define DIODE_PIN2 14

    MTLoop mtLoop;

    class TBlinkDiode: public TTask {
        private:
            int diodePin
            bool state = false;

        public:
            TBlinkDiode (int diodePin): TTask(), diodePin(diodePin) {
                pinMode(diodePin, OUTPUT);
            }

            inline virtual void Run(TLog&) {
                if (state)
                    digitalWrite(diodePin, LOW);
                else
                    digitalWrite(diodePin, HIGH);
                state = !state;
            };
    };

    void setup() {
        TBlinkDiode d1(DIODE_PIN1);
        TBlinkDiode d2(DIODE_PIN2);

        mtLoop({
            {
                { d1, 1000000, 10 },
            },
            {
                { d2,  500000, 10 },
            }
        });
    }

    void loop() {
        mtLoop.Tick();
    }

