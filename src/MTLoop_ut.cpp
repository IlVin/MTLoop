#define BOOST_TEST_MODULE testMTLoop

#define DEBUG
#define MTLOOP_MOCK_TIMER
//#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "MTLoop.h"
#include <string>
#include <memory>


using namespace MT;

class TMockTask: public TTask {
    public:
    bool taskIsRun = false;
    TMockTask(std::string name): TTask(name) {}
    void Run(TLog& log) {
#ifdef DEBUG
        std::cout << "RUN!!!" << std::endl;
#endif
        taskIsRun = true;
        TTimer::time += 50;
    }
};

BOOST_AUTO_TEST_SUITE(testSuiteMTLoop)

    struct TTaskFixture {

        std::shared_ptr<TLog> log;
        std::shared_ptr<TTimer> timer;
        TMockTask* myTask;

        TTaskFixture() {
            log = std::shared_ptr<TLog>(new TLog);
            timer = std::shared_ptr<TTimer>(new TTimer);
            myTask = new TMockTask("TestTask");
        }
       ~TTaskFixture() {
           delete myTask;
       }
    };


    BOOST_FIXTURE_TEST_CASE( testTTimeSlotInit, TTaskFixture ) {
        {
            TTimeSlot slot = { *myTask, 100, 0 };
            slot.SetStartTime(5);

            BOOST_CHECK_EQUAL(myTask->GetName(), "TestTask");

            BOOST_CHECK_EQUAL(slot.taskStat.GetStartTime(), 0);
            BOOST_CHECK_EQUAL(slot.taskStat.GetStopTime(), 0);
            BOOST_CHECK_EQUAL(slot.taskStat.GetDuration(), 0); // Таск не запускался, поэтому продолжительность = 0

            BOOST_CHECK_EQUAL(slot.GetLTime(), 5);
            BOOST_CHECK_EQUAL(slot.GetRTime(), 5 + 100 - 1);
        }
    }



    BOOST_FIXTURE_TEST_CASE( testTTimeSlotLife, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100);
            slot.SetStartTime(5);

            TTimer::time = 4;
            BOOST_CHECK_EQUAL(slot.Tick(*log), false);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, false);

            TTimer::time = 5;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
            myTask->taskIsRun = false;

            TTimer::time = 5;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, false);

            TTimer::time = 104;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, false);

            TTimer::time = 105;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, false);
        }
    }



    // [ { } ] *
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTime01, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100);
            slot.SetStartTime(5);

            TTimer::time = 10;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 115;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 104);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
        }
    }


    // [     ] *
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTime02, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100);
            slot.SetStartTime(5);

            TTimer::time = 115;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 115);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, false);
        }
    }


    // [ { } * ]
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTime03, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100);
            slot.SetStartTime(5);

            TTimer::time = 10;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 100;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 104);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
        }
    }


    // [  * ]
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTime04, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100);
            slot.SetStartTime(5);

            TTimer::time = 10;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 104);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, false);
        }
    }


    // [ { ] } *
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTime05, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100);
            slot.SetStartTime(5);

            TTimer::time = 90;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 200;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 140);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
        }
    }



    // [ { } ] *
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTimeWithPadding01, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 50, 10);
            slot.SetStartTime(5);

            TTimer::time = 5;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 105;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 65);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
        }
    }


    // [     ] *
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTimeWithPadding02, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100, 10);
            slot.SetStartTime(5);

            TTimer::time = 116;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 126);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, false);
        }
    }


    // [ { } * ]
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTimeWithPadding03, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100, 10);
            slot.SetStartTime(5);

            TTimer::time = 10;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 100;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 104);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
        }
    }


    // [ { } * ]
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTimeWithPadding04, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100, 20);
            slot.SetStartTime(5);

            TTimer::time = 45;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 100;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 115);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
        }
    }


    // [  * ]
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTimeWithPadding05, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100, 10);
            slot.SetStartTime(5);

            TTimer::time = 10;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 104);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, false);
        }
    }


    // [ { ] } *
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTimeWithPadding06, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100, 10);
            slot.SetStartTime(5);

            TTimer::time = 90;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 200;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 150);
            BOOST_CHECK_EQUAL(myTask->taskIsRun, true);
        }
    }




BOOST_AUTO_TEST_SUITE_END()
