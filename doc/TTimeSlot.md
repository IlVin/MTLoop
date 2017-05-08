## TTimeSlot

    class TTimeSlot {
    public:
        TStat taskStat;

    private:
        TTask& task;
        uint32_t startTime;
        uint32_t minDuration;
        uint32_t padding;

    public:
        TTimeSlot(TTask& task, uint32_t minDuration = 100, uint32_t padding = 0);
        inline void SetStartTime(uint32_t time);
        inline void SetMinDuration(uint32_t);
        inline void SetPadding(uint32_t);
        inline bool Tick(TLog& log);
        inline uint32_t GetLTime();
        inline uint32_t GetRTime();
    };

**TTimeSlot** ответственен за запуск таска. Таск должен быть обязательно запущен, причем только один раз, на заданном
временном интервале. Это требование вытекает из логики функционирования класса **TTimeSlotChain**, который собирает
**TTimeSlot** в цепочки, обуславливающие поток/нить/цепочку команд.

Начало временного интервала устанавливает функция **SetStartTime**. Начиная с этого момента таск обязан быть выполнен при
первой же передаче кванта времени в функцию **Tick()**.

Конец временного интервала плавающий, но такой, чтобы обеспечить минимальную продолжительность тайм-слота, обусловленную
**minDuration**. Плавающая продолжительность временного интервала обусловлена логикой функционирования **TTimeSlotChaun**.

### Временной интервал:
    [GetLTime(), GetRTime()]

## bool Tick(TTimer& timer, TLog& log)

Метод, через который планировщик передает квант времени выполнения команд таску, привязанному к **TTimeSlot**.

**Output:**
  **true**  - Таск выполнен на заданном функцией **SetStartTime** временном интервале
  **false** - Таск не выполнен.

