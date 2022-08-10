#include <project.h>
#include <AD985X.h>

#define AD985X_POWERDOWN      0x04

// SIGNAL
volatile float    _freq    = 0;
volatile uint32_t _factor  = 0;
volatile uint8_t  _config  = 1;
volatile int32_t  _offset  = 0;
volatile int     _autoUpdate = LOW;
volatile int     _autoRefClock = LOW;
volatile uint32_t _ARCCutOffFreq = 10000000UL;

int     getAutoRefClock()  { return _autoRefClock; };
uint32_t getARCCutOffFreq() { return _ARCCutOffFreq; };
uint32_t getMaxFrequency() { return AD9851_MAX_FREQ; };


void begin()
{
    Pin_DDS_Reset_Write(LOW);
    Pin_DDS_Data_Write(LOW);
    Pin_DDS1_Word_Clock_Write(LOW);
    Pin_DDS2_Word_Clock_Write(LOW);
    //Pin_DDS_Freq_Update_Write(LOW);
    FQ_UD_Write(LOW);
    CyDelayUs(2);
    
  reset();
}


void reset()
{
    //App note p5 : 2. Send a common RESET with a minimum high time of five REF CLK periods
    Pin_DDS_Reset_Write(HIGH);
    CyDelayUs(5);
    Pin_DDS_Reset_Write(LOW);
    CyDelayUs(2);
    
    Pin_DDS1_Word_Clock_Write(HIGH);
    CyDelayUs(2);
    Pin_DDS1_Word_Clock_Write(LOW);
    CyDelayUs(2);
    
    Pin_DDS2_Word_Clock_Write(HIGH);
    CyDelayUs(2);
    Pin_DDS2_Word_Clock_Write(LOW);
    CyDelayUs(2);
    
/*app note p5 : 3. In parallel or serial mode, write the following instructions to DDS #1
without issuing an FQ_UD:
W0 = 01 hex  W1 = 00 hex  W2 = 00 hex
W3 = 00 hex  W4 = 00 hex
    
datasheet p13 : W0 (parallel load) and W32 (serial load)
now contains a 6x REFCLK multiplier enable bit that needs
to be set high to enable or low to disable the internal reference
clock multiplier.*/
    
  _config = 1;    // 0 phase   no power down  6xREFCLK on
  _freq   = 0;
  _factor = 0;              
  _offset = 0;
  //_autoUpdate = HIGH;
    
  //setRefClockHigh();
  //powerUp(); //inutile si config = 0 ou 1

  writeData(1);
  writeData(2);
CyDelayUs(100);
  FQ_update(); 
//app note p 5 : 5. Assert a common FQ_UD and wait at least 100 us.
CyDelayUs(200);

    Pin_DDS_Reset_Write(HIGH);
    CyDelayUs(5);
    Pin_DDS_Reset_Write(LOW);
    CyDelayUs(2);
CyDelayUs(1);    
    Pin_DDS1_Word_Clock_Write(HIGH);
    CyDelayUs(2);
    Pin_DDS1_Word_Clock_Write(LOW);
    CyDelayUs(2);
    
    Pin_DDS2_Word_Clock_Write(HIGH);
    CyDelayUs(2);
    Pin_DDS2_Word_Clock_Write(LOW);
    CyDelayUs(2);
CyDelayUs(200);
}


void powerDown()
{
  _config |= AD985X_POWERDOWN;      // keep phase and REFCLK as is.
  //writeData();
}


void powerUp()
{
  _config &= ~AD985X_POWERDOWN;
  //writeData();
}


void setPhase(uint8_t phase)
{
  if (phase > 31) return;
  _config &= 0x07;
  _config |= (phase << 3);
  //writeData();
}


void writeData(uint8_t target)
{

 volatile uint32_t data = _factor;
                                    
    sw_transfer_data(data & 0xFF,target);
    data >>= 8;
    sw_transfer_data(data & 0xFF,target);
    data >>= 8;
    sw_transfer_data(data & 0xFF,target);
    sw_transfer_data(data >> 8,target);
    sw_transfer_data(_config,target);  // mask factory test bit
    
    CyDelayUs(1);
  // update frequency + phase + control bits.
  // should at least be 4 ns delay - P14 datasheet
  if (_autoUpdate) FQ_update();
}

void sw_transfer_data(uint8_t val,uint8_t target)
{

    if(target==1){   
      for (uint8_t mask = 0x01; mask; mask <<= 1)   // LSBFIRST
      {
        uint8 data = val&mask;          //fix

        if(data>0)data=1;else data=0;   //fix (god why D:)
        Pin_DDS_Data_Write(data);
        
        CyDelayUs(5);                   //fix timing
        Pin_DDS1_Word_Clock_Write(HIGH);
        CyDelayUs(2);
        Pin_DDS1_Word_Clock_Write(LOW);
        CyDelayUs(2);
      }
    }else if(target==2){
      for (uint8_t mask = 0x01; mask; mask <<= 1)   // LSBFIRST
      {
        uint8 data = val&mask;          //fix

        if(data>0)data=1;else data=0;   //fix (god why D:)
        Pin_DDS_Data_Write(data);
        
        CyDelayUs(5);                   //fix timing
        Pin_DDS2_Word_Clock_Write(HIGH);
        CyDelayUs(2);
        Pin_DDS2_Word_Clock_Write(LOW);
        CyDelayUs(2);
      }        
    }
}
void FQ_update()
{
    FQ_UD_Write(HIGH);
    CyDelayUs(2);
    FQ_UD_Write(LOW);
    CyDelayUs(2);
}


////////////////////////////////////////////////////////
//
// AD9851
//

#define AD9851_REFCLK        0x01    // bit is a 6x multiplier bit P.14 datasheet

void setFrequency(uint32_t freq,uint8_t target)
{
  // PREVENT OVERFLOW
  if (freq > AD9851_MAX_FREQ) freq = AD9851_MAX_FREQ;

  // AUTO SWITCH REFERENCE FREQUENCY
  if (_autoRefClock)
  {
    if (freq > _ARCCutOffFreq)
    {
      _config |= AD9851_REFCLK;
    }
    else
    {
      _config &= ~AD9851_REFCLK;
    }
  }

  if (_config & AD9851_REFCLK)  // 6x 30 = 180 MHz
  {
    _factor = (102481911520ULL * freq) >> 32;  //  (1 << 64) / 180000000
  }
  else                          // 1x 30 = 30 MHz
  {
    _factor = (614891469123ULL * freq) >> 32;  //  (1 << 64) / 30000000
  }
  _freq = freq;
  _factor += _offset;

  writeData(target);
}


// especially for lower frequencies (with decimals)
void setFrequencyF(float freq,uint8_t target)
{
  // PREVENT OVERFLOW
  if (freq > AD9851_MAX_FREQ) freq = AD9851_MAX_FREQ;

  // AUTO SWITCH REFERENCE FREQUENCY
  if (_autoRefClock)
  {
    if (freq > _ARCCutOffFreq)
    {
      _config |= AD9851_REFCLK;
    }
    else
    {
      _config &= ~AD9851_REFCLK;
    }
  }

  if (_config & AD9851_REFCLK)  // 6x 30 = 180 MHz
  {
    _factor = (uint64_t)(102481911520ULL * freq) >> 32;  //  (1 << 64) / 180000000
  }
  else                          // 1x 30 = 30 MHz
  {
    _factor = (6148914691ULL * (uint64_t) (100 * freq)) >> 32;
  }

  _freq = freq;
  _factor += _offset;
  writeData(target);
}


////////////////////////////////////////////////////////
//
// AD9851 - AUTO REFERENCE CLOCK
//
void setAutoRefClock(int arc)
{
  if(arc>0)arc=1;else arc=0;
  _autoRefClock = arc;
  setFrequency(_freq,1);
    setFrequency(_freq,2);
};


void setRefClockHigh()
{
  _config |= AD9851_REFCLK;
  //setFrequency(_freq);
}


void setRefClockLow()
{
  _config &= ~AD9851_REFCLK;
  //setFrequency(_freq);
}


uint8_t getRefClock()
{
  return (_config & AD9851_REFCLK) ? 180 : 30;
}

void setARCCutOffFreq(uint32_t Hz)
{
  if (Hz > 30000000UL) Hz = 30000000;
  _ARCCutOffFreq = Hz;
};


// -- END OF FILE --
