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

    class TTimer {

        private:
            uint32_t curTime = 0;

        public:
            virtual uint32_t GetTime() {
                return ++curTime;
            }
    };

    TTimer* defaultTimer = new TTimer();

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
            inline void Start(TTimer& timer) {
                startTime = timer.GetTime();
            }

            inline void Stop(TTimer& timer) {
                stopTime = timer.GetTime();
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
        std::shared_ptr<TTask> task;
        uint32_t startTime;
        uint32_t duration;

    public:
        TTimeSlot(std::shared_ptr<TTask> task, uint32_t duration = 100)
            : task(task)
            , startTime(1)
            , duration(duration) {}

        ~TTimeSlot() {
//            delete task;
        }

        inline void SetStartTime(uint32_t time) {
            startTime = time;
        }

        void Tick(TTimer& timer, TLog& log) {
            std::cout
                << "TIME=" << timer.GetTime()
                << "; startTime=" << startTime
                << "; duration=" << duration
                << std::endl;
            if (timer.GetTime() <= startTime + duration) { // Мы находимся в слоте
            std::cout
                << "taskStartTime=" << taskStat.GetStartTime()
                << "; startTime=" << startTime
                << std::endl;
                if (taskStat.GetStartTime() < startTime) { // Задача еще не была запущена в этом слоте
                    taskStat.Start(timer);
                    task->Run(log);
                    taskStat.Stop(timer);
                }
            }
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
            TTimer* timer;
            TLog* log;
            std::vector<TTimeSlotChain> timeSlotChains;
        public:
            TLoop(TTimer* timer = defaultTimer, TLog* log = defaultLog)
                : timer(timer)
                , log(log)
            {}

            void AddTaskChain(std::vector<TTimeSlot> timeSlots) {
                timeSlotChains.push_back(TTimeSlotChain(timeSlots));
            }
            void Tick() {
            }
    };
}

#endif
