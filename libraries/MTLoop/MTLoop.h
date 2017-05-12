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

#define tick_t uint32_t

#ifdef MTLOOP_MOCK_TIMER
    class TTimer {
        public:
            static tick_t time;
            static tick_t increment;

            static inline tick_t GetTime() {
                tick_t t = time;
                time += increment;
                return t;
            }
    };

    tick_t TTimer::time;
    tick_t TTimer::increment;

#elif MTLOOP_DUMMY_TIMER
#else
    class TTimer {
    private:
    public:
        static inline tick_t GetTime() {
            return 0;
        }
    };
#endif

    class TLog {

        private:

        public:
            TLog() {}
            virtual ~TLog(){}
            void virtual Log(char* logLine) {
            }
    };

    TLog* defaultLog = new TLog();

    class TTask {

        private:
            tick_t startTime = 0;
            tick_t stopTime = 0;

        public:
            TTask() {}
            TTask(const TTask& task)
                : startTime(task.startTime)
                , stopTime(task.stopTime) {}
            virtual ~TTask() {};

            inline void Tick(TLog& log) {
                startTime = TTimer::GetTime();
                Run(log);
                stopTime = TTimer::GetTime();
            }

            virtual void Run(TLog& log) = 0;

            inline tick_t GetStartTime() {
                return startTime;
            }

            inline tick_t GetStopTime() {
                return stopTime;
            }

            inline tick_t GetDuration() {
                return stopTime - startTime;
            }
    };

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


    class TTimeSlot {
    private:
        TTask& task;
        tick_t startTime;
        tick_t minDuration;
        tick_t padding;

    public:
        TTimeSlot(TCallback task, tick_t minDuration = 100, tick_t padding = 0)
            : task(task)
            , startTime(1)
            , minDuration(minDuration)
            , padding(padding) {}

        TTimeSlot(TTask& task, tick_t minDuration = 100, tick_t padding = 0)
            : task(task)
            , startTime(1)
            , minDuration(minDuration)
            , padding(padding) {}

        TTimeSlot(const TTimeSlot& ts)
            : task(ts.task)
            , startTime(ts.startTime)
            , minDuration(ts.minDuration)
            , padding(ts.padding) {}

        inline void SetStartTime(tick_t time) {
            startTime = time;
        }

        inline void SetMinDuration(tick_t time) {
            minDuration = time;
        }

        inline void SetPadding(tick_t time) {
            padding = time;
        }

        inline bool Tick(TLog& log) {
            tick_t tm = TTimer::GetTime();
            if (tm < startTime)
                return false;
            if (task.GetStartTime() >= startTime)
                return true;
            try {
                task.Tick(log);
            } catch (...) {
                log.Log((char*)"Exception detect");
            }
            return true;
        }

        inline tick_t GetLTime() {
            return startTime;
        }

        inline tick_t GetRTime() {
            tick_t tm = TTimer::GetTime();
            tick_t rTime = startTime + minDuration - 1;
            if (task.GetStartTime() >= startTime) {
                tick_t taskStopTimeWithPadding = task.GetStopTime() + padding;
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
            size_t curTimeSlot = 0;

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

            bool Tick(TLog& log) {
                TTimeSlot* ts = timeSlots[curTimeSlot];
                if (ts->Tick(log)) {
                     curTimeSlot = (curTimeSlot + 1) % size;
                     timeSlots[curTimeSlot]->SetStartTime(ts->GetRTime() + 1);
                     return true;
                };
                return false;
            }
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
