/*
 * MTLoop.h
 */

#ifndef MTLOOP_H
#define MTLOOP_H

#include <stddef.h>
#include <inttypes.h>
#include <initializer_list> // Custom initializer_list for AVR

#include <memory>
#include <vector>
#include <string>

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

    typedef void(*callbackPtr)(TLog& log);
    class TTask {

        private:
            callbackPtr cb;
            std::string name;
            uint32_t startTime = 0;
            uint32_t stopTime = 0;

        public:
            TTask(callbackPtr cb, std::string name = "NoName callback"): cb(cb), name(name) {}
            TTask(const TTask& task): cb(task.cb), name(task.name) {}

            inline void Run(TLog& log) {
                startTime = TTimer::GetTime();
                cb(log);
                stopTime = TTimer::GetTime();
            };

            inline const std::string& GetName() {
                return name;
            }

            inline uint32_t GetStartTime() {
                return startTime;
            }

            inline uint32_t GetStopTime() {
                return stopTime;
            }

            inline uint32_t GetDuration() {
                return stopTime - startTime;
            }
    };

    class TTimeSlot {
    private:
        TTask task;
        uint32_t startTime;
        uint32_t minDuration;
        uint32_t padding;

    public:
        TTimeSlot(TTask task, uint32_t minDuration = 100, uint32_t padding = 0)
            : task(task)
            , startTime(1)
            , minDuration(minDuration)
            , padding(padding) {}

        TTimeSlot(const TTimeSlot& ts)
            : task(ts.task)
            , startTime(ts.startTime)
            , minDuration(ts.minDuration)
            , padding(ts.padding) {}

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
            if (tm < startTime)
                return false;
            if (task.GetStartTime() >= startTime)
                return true;
            task.Run(log);
            return true;
        }

        inline uint32_t GetLTime() {
            return startTime;
        }

        inline uint32_t GetRTime() {
            uint32_t tm = TTimer::GetTime();
            uint32_t rTime = startTime + minDuration - 1;
            if (task.GetStartTime() >= startTime) {
                uint32_t taskStopTimeWithPadding = task.GetStopTime() + padding;
                if (rTime < taskStopTimeWithPadding)
                    rTime = taskStopTimeWithPadding;
            } else if (tm > rTime) {
                rTime = tm + padding;
            }
            return rTime;
        }

    };

    typedef TTimeSlot * TTimeSlotPtr;

    class TTimeSlotChain {
        private:
            TTimeSlotPtr* timeSlots;
            size_t size = 0;
            size_t curSlot = 0;

        public:
            TTimeSlotChain(std::initializer_list<TTimeSlot> ts) {
                size = ts.size();
                timeSlots = new TTimeSlotPtr[size];
                size_t i = 0;
                for (const auto& item : ts)
                    timeSlots[i++] = new TTimeSlot(item);
            }
            ~TTimeSlotChain() {
                for (size_t i = 0; i < size; ++i)
                    delete timeSlots[i];
                delete[] timeSlots;
            }

            bool Tick() { return true; }
    };


    class TLoop {
        private:
            TLog* log;
            std::vector<TTimeSlotChain> timeSlotChains;
        public:
            TLoop(TLog* log = defaultLog)
                : log(log)
            {}

//            void AddTaskChain(std::vector<TTimeSlot> timeSlots) {
//                timeSlotChains.push_back(TTimeSlotChain(timeSlots));
//            }
            void Tick() {
            }
    };
}

#endif
