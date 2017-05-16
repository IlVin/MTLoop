#include <MTLoop.h>

class TBlinkTask: public MT::IRunnable {
  private:
    bool state;
  public:
    TBlinkTask(bool state): state(state) {}

  virtual bool Run(MT::TLog& log) {
    log.Log("TBlink is run");
    return true;
  }
};

TBlinkTask* bt;
MT::TLoop mtLoop {};

void setup(){
  bt = new TBlinkTask(true);
  
  mtLoop.Attach (
    {
      { { [](MT::TLog& log){ log.Log((char*)"TASK1 IS RUN"); return true; } }, 100, 10 },
      { { [](){ } }, 100, 20 },
      { *bt, 100, 40  },
      { new TBlinkTask(true), 100, 30 }
    }
  );
}

void loop(){
  mtLoop.Run();
}
