//#pragma once
#include <project.h>
#ifndef AD985X_H_
#define AD985X_H_

    #define AD985X_LIB_VERSION    (F("0.3.3"))
    #define AD9851_MAX_FREQ       (70UL * 1000UL * 1000UL)
    #define HIGH 1u
    #define LOW 0u
    
extern volatile float    _freq;
extern volatile uint32_t _factor;
extern volatile uint8_t  _config;
extern volatile int32_t  _offset;
extern volatile int     _autoUpdate;
extern volatile int     _autoRefClock;
extern volatile uint32_t _ARCCutOffFreq;

          void     begin(void);
          void     reset();
          void     powerDown();
          void     powerUp();

          float    getFrequency(void);

          // 0 .. 31  steps of 11.25 degrees
          void     setPhase(uint8_t phase);
          uint8_t  getPhase(void) ;

          // offset to calibrate the frequency (internal counter)
          // offset must be stored by the user.
          void     setCalibration(int32_t offset);
          int32_t  getCalibration(void);
          // internal chip factor used for frequency. (debugging only)
          uint32_t getFactor(void);

          // autoUpdate is default true;
          void     setAutoUpdate(int update);  
          int     getAutoUpdate();
          void     FQ_update();
          void  sw_transfer_data(uint8_t val,uint8_t target);

          void     setFrequency(uint32_t freq,uint8_t target);    // 0..AD9851_MAX_FREQ
          void     setFrequencyF(float freq,uint8_t target);
          uint32_t getMaxFrequency(void);

          void     setRefClockHigh();   // 180 MHz
          void     setRefClockLow();    //  30 MHz
          uint8_t  getRefClock();

          void     setAutoRefClock(int arc);
          int     getAutoRefClock(void);

          // 10 MHz is default, set in Hz.
          // will be kept <= 30 MHz as that is the freq of LOW mode.
          void     setARCCutOffFreq(uint32_t Hz);
          uint32_t getARCCutOffFreq(void);

          void writeData(uint8_t target);
#endif 