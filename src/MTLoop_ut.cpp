#define BOOST_TEST_MODULE testMTLoop

#define DEBUG
#define MTLOOP_MOCK_TIMER
//#include <boost/test/unit_test.hpp>
#include <boost/test/included/unit_test.hpp>
#include "MTLoop.h"
#include <string>
#include <memory>


using namespace MT;

BOOST_AUTO_TEST_SUITE(testSuiteMTLoop)

    struct TTimeSlotFixture {

        std::shared_ptr<TLog> log;
        TTask* myTask1;
        TTask* myTask2;
        TTask* myTask3;

        TTimeSlot* slot1;
        TTimeSlot* slot2;
        TTimeSlot* slot3;

        TTimeSlotFixture() {
            log = std::shared_ptr<TLog>(new TLog);
            myTask1 = new TTask([](TLog& log){}, "MyTask 1");
            myTask2 = new TTask([](TLog& log){}, "MyTask 2");
            myTask3 = new TTask([](TLog& log){}, "MyTask 3");
            slot1 = new TTimeSlot(*myTask1, 100, 10);
            slot2 = new TTimeSlot(*myTask2, 100, 10);
            slot3 = new TTimeSlot(*myTask3, 100, 10);
        }
       ~TTimeSlotFixture() {
            delete slot3;
            delete slot2;
            delete slot1;
            delete myTask1;
            delete myTask2;
            delete myTask3;
       }
    };

    struct TTaskFixture {

        std::shared_ptr<TLog> log;
        TTask* myTask;

        TTaskFixture() {
            log = std::shared_ptr<TLog>(new TLog);
            myTask = new TTask([](TLog& log){
                #ifdef DEBUG
                    std::cout << "RUN!!!" << std::endl;
                #endif
                TTimer::time += 50;
            }, "TestTask");
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

            TTimer::time = 5;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 5;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 104;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);

            TTimer::time = 105;
            BOOST_CHECK_EQUAL(slot.Tick(*log), true);
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
        }
    }


    // [     ] *
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTime02, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100);
            slot.SetStartTime(5);

            TTimer::time = 115;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 115);
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
        }
    }


    // [  * ]
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTime04, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100);
            slot.SetStartTime(5);

            TTimer::time = 10;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 104);
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
        }
    }


    // [     ] *
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTimeWithPadding02, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100, 10);
            slot.SetStartTime(5);

            TTimer::time = 116;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 126);
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
        }
    }


    // [  * ]
    BOOST_FIXTURE_TEST_CASE( testTTimeSlotGetRTimeWithPadding05, TTaskFixture ) {
        {
            TTimeSlot slot(*myTask, 100, 10);
            slot.SetStartTime(5);

            TTimer::time = 10;
            BOOST_CHECK_EQUAL(slot.GetRTime(), 104);
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
        }
    }


    BOOST_FIXTURE_TEST_CASE( testTTimeSlotChain01, TTimeSlotFixture ) {
        {
            TTimeSlotChain tsChain {
                {*myTask1, 100, 10},
                {*myTask2, 100, 20},
                {*myTask3, 50, 10}
            };
            TTimer::time = 90;
            BOOST_CHECK_EQUAL(tsChain.Tick(), true);

        }
    }


    BOOST_FIXTURE_TEST_CASE( testTTimeSlotChain02, TTimeSlotFixture ) {
        {
            TTimeSlotChain tsChain {
                { { [](TLog& log){ log.Log("Cb 1"); } }, 100, 10 },
                { { [](TLog& log){ log.Log("Cb 2"); } }, 100, 20 },
                { { [](TLog& log){ log.Log("Cb 3"); } }, 50, 10  }
            };
            TTimer::time = 90;
            BOOST_CHECK_EQUAL(tsChain.Tick(), true);

        }
    }





BOOST_AUTO_TEST_SUITE_END()
