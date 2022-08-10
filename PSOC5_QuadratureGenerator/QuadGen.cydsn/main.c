#include <project.h>
#include <AD985X.h>

#if defined (__GNUC__)
    asm (".global _printf_float");
#endif

#define HIGH 1u
#define LOW 0u
#define DDS1 1u
#define DDS2 2u

uint32_t maxFreq;

int main()
{   //CyDelay(1000);
    LED_Write(HIGH); //setup start
    maxFreq = getMaxFrequency();
    CyDelay(100);
            begin();
            FQ_update(); 
        CyDelay(500);
            powerDown();
            writeData(DDS1);
            writeData(DDS2);
            FQ_update();
            powerUp();
            writeData(DDS1);
            writeData(DDS2);
            FQ_update();
    LED_Write(LOW);  //setup end
    
            setPhase(8);
            setFrequency(1000,DDS1);
            setPhase(0);
            setFrequency(1000,DDS2);
            FQ_update();    

    
    for(;;)
    {   
        CyDelay(2000);
    }
}
