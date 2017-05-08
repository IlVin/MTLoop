/*
 * MTLoop.h
 */

#ifndef MTLOOP_H
#define MTLOOP_H

#include <inttypes.h>
#include <memory>
#include <vector>
#include <string>

#include <iostream>

namespace MT {

#ifdef MTLOOP_MOCK_TIMER
    class TTimer {
        public:
            static uint32_t time;
            static uint32_t increment;

            static inline uint32_t GetTime() {
                uint32_t t = time;
                time += increment;
                return t;
            }
    };

    uint32_t TTimer::time;
    uint32_t TTimer::increment;

#elif MTLOOP_DUMMY_TIMER
#else
    class TTimer {
    private:
    public:
        static inline uint32_t GetTime() {
            return 0;
        }
    };
#endif

    class TLog {

        private:

        public:
            TLog() {}
            void Log(std::string logLine = "") {
            }
    };

    TLog* defaultLog = new TLog();

    class TStat {

        private:
            uint32_t startTime = 0;
            uint32_t stopTime = 0;

        public:
            inline void Start() {
                startTime = TTimer::GetTime();
            }

            inline void Stop() {
                stopTime = TTimer::GetTime();
            }

            inline uint32_t GetDuration() {
                return stopTime - startTime;
            }

            inline uint32_t GetStartTime() {
                return startTime;
            }

            inline uint32_t GetStopTime() {
                return stopTime;
            }
    };

    class TTask {

        private:
            std::string name;

        public:
            TTask(std::string name): name(name) {}
            virtual ~TTask() {}
            inline const std::string& GetName() {
                return name;
            }
            virtual void Run(TLog& log) = 0;
    };

    class TTimeSlot {
    public:
        TStat taskStat;

    private:
        TTask& task;
        uint32_t startTime;
        uint32_t minDuration;
        uint32_t padding;

    public:
        TTimeSlot(TTask& task, uint32_t minDuration = 100, uint32_t padding = 0)
            : task(task)
            , startTime(1)
            , minDuration(minDuration)
            , padding(padding) {}

        ~TTimeSlot() {
        }

        inline void SetStartTime(uint32_t time) {
            startTime = time;
        }

        inline void SetMinDuration(uint32_t time) {
            minDuration = time;
        }

        inline void SetPadding(uint32_t time) {
            padding = time;
        }

        inline bool Tick(TLog& log) {
            uint32_t tm = TTimer::GetTime();
#ifdef DEBUG
            std::cout
                << "TTimeSlot(" << task.GetName() << ").Tick(): TTimeSlot=[" << GetLTime()
                << ", " << GetRTime()
                << "]; startTime=" << startTime
                << "; minDuration=" << minDuration
                << "; padding=" << padding
                << std::endl;
            std::cout
                << "                  tm=" << tm
                << "; taskStat.GetStartTime=" << taskStat.GetStartTime()
                << "; taskStat.GetStopTime=" << taskStat.GetStopTime()
                << std::endl;
#endif

            if (tm < startTime)
                return false;

            if (taskStat.GetStartTime() >= startTime)
                return true;

            taskStat.Start();
            task.Run(log);
            taskStat.Stop();

            return true;
        }

        inline uint32_t GetLTime() {
            return startTime;
        }

        inline uint32_t GetRTime() {
            uint32_t tm = TTimer::GetTime();
            uint32_t rTime = startTime + minDuration - 1;

            if (taskStat.GetStartTime() >= startTime) {
                uint32_t taskStopTimeWithPadding = taskStat.GetStopTime() + padding;
                if (rTime < taskStopTimeWithPadding)
                    rTime = taskStopTimeWithPadding;
            } else if (tm > rTime) {
                    rTime = tm + padding;
            }

            return rTime;
        }

    };


    class TTimeSlotChain {
        private:
            std::vector<TTimeSlot> timeSlots;
        public:
            TTimeSlotChain(std::vector<TTimeSlot> timeSlots)
            : timeSlots(timeSlots) { }

            bool Tick() { }
    };


    class TLoop {
        private:
            TLog* log;
            std::vector<TTimeSlotChain> timeSlotChains;
        public:
            TLoop(TLog* log = defaultLog)
                : log(log)
            {}

            void AddTaskChain(std::vector<TTimeSlot> timeSlots) {
                timeSlotChains.push_back(TTimeSlotChain(timeSlots));
            }
            void Tick() {
            }
    };
}

#endif
