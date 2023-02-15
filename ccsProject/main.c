#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include "HAL_OPT3001.h"
#include "image.c"
#include "screen.c"
#include "pump.c"
#include "button.c"
#include "uart.c"
#include "sensors.c"


//#define NUM_OPT 4 //number of options for menu
#define TIMER_PERIOD 0x8000 // = 32768, one sec timer
#define upLimitController 12200 //controller movement up-limit
#define lowLimitController 4200 //controller movement low-limit

//Initializing variable for temperature ADC
static volatile uint16_t curADCResult;

//Variable for storing temperature value returned from TMP006
float temp;
//Variable for storing light value returned from OPT3001
int lux;
//Variable for storing moisturePercetage taken from adc convertion
int moisturePercentage;

//Boolean to keep refreshing data in sensors data mode
bool showMode = false;

//boolean to not refresh menu if we are in the submenu
bool inSubMenu = false;


//Variable to store the received uart value
uint8_t RXData = 0;

void portInit(){

    //set P4.1 for relay control
    GPIO_setAsOutputPin(GPIO_PORT_P4,GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN1); //relay activates when P4.1 = 0

    //set P1.0 for relay check
    GPIO_setAsOutputPin(GPIO_PORT_P1,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0);
}

void _hwInit(){

    //Halting WDT and disabling master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    /* Set the core voltage level to VCORE1 */
    // PCM_setCoreVoltageLevel(PCM_VCORE1);

    // /* Set 2 flash wait states for Flash bank 0 and 1*/
    // FlashCtl_setWaitState(FLASH_BANK0, 2);
    // FlashCtl_setWaitState(FLASH_BANK1, 2);

    // /* Initializes Clock System */
    // CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    // CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    // CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);

    graphicsInit();
    portInit();
    ledPortsInit();
    buttonsInit();
    adcInit();
    uartInit();
    sensorsInit();

}

/*---Main---*/
int main(void){

    _hwInit();
    generateMenu();

    while (1){
        PCM_gotoLPM0(); //Sleep mode
    }
}

//Button two boosterpack interrupt
void PORT3_IRQHandler(){

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status); //clear interrupt flag

    /* check if we received P3.5 interrupt*/
    if((status & GPIO_PIN5)){
        generateMenu();
        showMode=false;
        inSubMenu=false;
    }
}

//Button one boosterpack interrupt
void PORT5_IRQHandler(){

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status); //clear interrupt flag

    /* check if we received P5.1 interrupt*/
    if(status & GPIO_PIN1){

        inSubMenu = true;

       switch(currentOpt){

           case 0: //Sensors data
                Graphics_clearDisplay(&g_sContext);
                showSensorData(lux,temp,moisturePercentage);
                showMode=1;
                break;

           case 1: //start pump
               startPump(&dropImage);
               break;

           case 2: //stop pump
               stopPump(&barDropImage);
               break;

           default:
               printf("problems problems problems\n");
       }
    }
}

void TA1_0_IRQHandler(){

    //Clear interrupt flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    //Obtain temperature value from TMP006
    temp = TMP006_getTemp();

    //Temp in celsius
    temp = (temp - 32.0) * ( 5.0 / 9.0 );

    //Obtain lux value from OPT3001
    lux = OPT3001_getLux();

    //Obtain moisture value from ADC
    moisturePercentage = mapToPercentage(curADCResult,14200,16000);

    if(showMode){
        showSensorData(lux,temp,moisturePercentage);
    }

    //Send data to the server
    mapAndSendData(temp,lux,curADCResult);
}

//ADC Interrupt Handler. This handler is called whenever there is a conversion that is finished for ADC_MEM0.
void ADC14_IRQHandler(void){

    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status); //clear interrupt flag

    //moisture conversion
    if (ADC_INT0 & status){
        /* should be between 0 and 16384*/
        curADCResult = ADC14_getResult(ADC_MEM0);
    }

    //joystick conversion
    if(status & ADC_INT1){

        //store ADC14 conversion results
        //resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
        //resultsBuffer[1] = ADC14_getResult(ADC_MEM1);
        //int yVal = resultsBuffer[1];
        int yVal = ADC14_getResult(ADC_MEM1);

        //joystick down
        if (yVal < lowLimitController){

            if(currentOpt == NUM_OPT-1){
                currentOpt = 0;
            }else{
                currentOpt++;
            }
            //printf("currentOpt->%d\n",currentOpt);
            if(!inSubMenu){
                refreshMenu();
                int i = 0;
                for(i=0; i<600000; i++);
            }
        }

        //joystick up
        if(yVal > upLimitController){

            if(currentOpt == 0){
                currentOpt = NUM_OPT-1;
            }else{
                currentOpt--;
            }
            //printf("currentOpt->%d\n",currentOpt);
            if(!inSubMenu){
                refreshMenu();
                int i = 0;
                for(i=0; i<600000; i++);
            }
        }
    }

    ADC14_toggleConversionTrigger(); //toggle another conversion
}
