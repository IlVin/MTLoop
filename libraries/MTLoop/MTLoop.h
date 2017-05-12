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


// ///////////////////////// //
//         TTimer            //
// ///////////////////////// //
#ifdef MTLOOP_MOCK_TIMER
    class TTimer {
        public:
            static tick_t time;
            static tick_t increment;

            static tick_t GetTime();
    };

    tick_t TTimer::time;
    tick_t TTimer::increment;
    inline tick_t TTimer::GetTime() {
        tick_t t = time;
        time += increment;
        return t;
    }

#elif MTLOOP_DUMMY_TIMER
#else
    class TTimer {
    private:
    public:
        static tick_t GetTime();
    };

    inline tick_t TTimer::GetTime() {
        return 0;
    }
#endif


// ///////////////////////// //
//         TLog              //
// ///////////////////////// //
    class TLog {
        private:
        public:
            TLog() {}
            virtual ~TLog(){}
            void virtual Log(char* logLine);
    };

    inline void TLog::Log(char* logLine) { }

    TLog* defaultLog = new TLog();


// ///////////////////////// //
//         TTask             //
// ///////////////////////// //
    class TTask {
        private:
            tick_t startTime = 0;
            tick_t stopTime = 0;
        public:
            TTask();
            TTask(const TTask& task);
            virtual ~TTask();

            void Tick(TLog& log);
            virtual void Run(TLog& log) = 0;
            tick_t GetStartTime();
            tick_t GetStopTime();
            tick_t GetDuration();
    };

    inline TTask::TTask() {}
    inline TTask::TTask(const TTask& task)
        : startTime(task.startTime)
        , stopTime(task.stopTime) {}
    inline TTask::~TTask() {};

    inline void TTask::Tick(TLog& log) {
        startTime = TTimer::GetTime();
        Run(log);
        stopTime = TTimer::GetTime();
    }

    inline tick_t TTask::GetStartTime() {
        return startTime;
    }

    inline tick_t TTask::GetStopTime() {
        return stopTime;
    }

    inline tick_t TTask::GetDuration() {
        return stopTime - startTime;
    }


// ///////////////////////// //
//         TCallback         //
// ///////////////////////// //
    typedef void(*callbackPtr)(TLog& log);
    class TCallback: public TTask {
        private:
            callbackPtr cb;
        public:
            TCallback(callbackPtr cb);
            TCallback(const TCallback& task);
            virtual void Run(TLog& log);
    };

    inline TCallback::TCallback(callbackPtr cb): TTask(), cb(cb) {}
    inline TCallback::TCallback(const TCallback& task): TTask(task), cb(task.cb) {}
    inline void TCallback::Run(TLog& log) {
        cb(log);
    };


// ///////////////////////// //
//         TTimeSlot         //
// ///////////////////////// //
    class TTimeSlot {
    private:
        TTask& task;
        tick_t startTime;
        tick_t minDuration;
        tick_t padding;
    public:
        TTimeSlot(TCallback task, tick_t minDuration = 100, tick_t padding = 0);
        TTimeSlot(TTask& task, tick_t minDuration = 100, tick_t padding = 0);
        TTimeSlot(const TTimeSlot& ts);
        void SetStartTime(tick_t time);
        void SetMinDuration(tick_t time);
        void SetPadding(tick_t time);
        bool Tick(TLog& log);
        tick_t GetLTime();
        tick_t GetRTime();
    };

    inline TTimeSlot::TTimeSlot(TCallback task, tick_t minDuration, tick_t padding)
        : task(task)
        , startTime(1)
        , minDuration(minDuration)
        , padding(padding) {};

    inline TTimeSlot::TTimeSlot(TTask& task, tick_t minDuration, tick_t padding)
        : task(task)
        , startTime(1)
        , minDuration(minDuration)
        , padding(padding) {};

    inline TTimeSlot::TTimeSlot(const TTimeSlot& ts)
        : task(ts.task)
        , startTime(ts.startTime)
        , minDuration(ts.minDuration)
        , padding(ts.padding) {}

    inline void TTimeSlot::SetStartTime(tick_t time) {
        startTime = time;
    }

    inline void TTimeSlot::SetMinDuration(tick_t time) {
        minDuration = time;
    }

    inline void TTimeSlot::SetPadding(tick_t time) {
        padding = time;
    }

    inline bool TTimeSlot::Tick(TLog& log) {
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

    inline tick_t TTimeSlot::GetLTime() {
        return startTime;
    }

    inline tick_t TTimeSlot::GetRTime() {
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

// ///////////////////////// //
//      TTimeSlotChain       //
// ///////////////////////// //
    typedef TTimeSlot * TTimeSlotPtr;
    class TTimeSlotChain {
        private:
            TTimeSlotPtr* timeSlots;
            size_t size = 0;
            size_t curTimeSlot = 0;
        public:
            TTimeSlotChain(std::initializer_list<TTimeSlot> ts);
            ~TTimeSlotChain();
            bool Tick(TLog& log);
    };

    inline TTimeSlotChain::TTimeSlotChain(std::initializer_list<TTimeSlot> ts) {
        size = ts.size();
        timeSlots = new TTimeSlotPtr[size];
        size_t i = 0;
        for (const auto& item : ts)
            timeSlots[i++] = new TTimeSlot(item);
    }

    inline TTimeSlotChain::~TTimeSlotChain() {
        for (size_t i = 0; i < size; ++i)
            delete timeSlots[i];
        delete[] timeSlots;
    }

    inline bool TTimeSlotChain::Tick(TLog& log) {
        TTimeSlot* ts = timeSlots[curTimeSlot];
        if (ts->Tick(log)) {
             curTimeSlot = (curTimeSlot + 1) % size;
             timeSlots[curTimeSlot]->SetStartTime(ts->GetRTime() + 1);
             return true;
        };
        return false;
    }


// ///////////////////////// //
//          TLoop            //
// ///////////////////////// //
    class TLoop {
        private:
            TLog* log;
            std::vector<TTimeSlotChain> timeSlotChains;
        public:
            TLoop(TLog* log = defaultLog);
//          void AddTaskChain(std::vector<TTimeSlot> timeSlots);
            void Tick();
    };
    inline TLoop::TLoop(TLog* log = defaultLog)
        : log(log)
    {}

//  inline  void TLoop::AddTaskChain(std::vector<TTimeSlot> timeSlots) {
//        timeSlotChains.push_back(TTimeSlotChain(timeSlots));
//    }
    inline void TLoop::Tick() {
    }
}

#endif
