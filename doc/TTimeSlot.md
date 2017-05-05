## TTimeSlot

    class TTimeSlot {
    public:
        TStat taskStat;

    private:
        std::shared_ptr<TTask> task;
        uint32_t startTime;
        uint32_t duration;

    public:
        TTimeSlot(std::shared_ptr<TTask> task, uint32_t duration = 100);
        inline void SetStartTime(uint32_t time);
        inline bool Tick(TLog& log);
        inline uint32_t GetLTime();
        inline uint32_t GetRTime();
        inline bool IsTaskStarted();
    };

Отслеживает временной интервал, задаваемый функцией **SetStartTime** и определяемый
параметром **duration**

### Временной интервал:
    [GetLTime, GetRTime]

## void SetStartTime(unt32_t time)

Переносит начало временного интервала в заданный момент времени.

## bool Tick(TTimer& timer, TLog& log)

Метод, через который планировщик пытается передать управление таску, привязанному к
**TTimeSlot**. Если время, выдаваемое **TTimer**, попадает в интервал **[GetLTime, GetRTime]**
и **TTask** ранее не запускался в данном слоте, происходит запуск TTask.

**Output:**
  **true** - Время, считанное с таймера попадает в интервал [GetLTime, GetRTime]
  **false** - Время, считанное с таймера **не** попадает в интервал [GetLTime, GetRTime]

## bool IsTaskStarted

**True** - если task уже стартовал в текущем временном интервале
