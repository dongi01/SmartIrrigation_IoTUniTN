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
#include "screen.c"
#include "pump.c"
#include "button.c"

//#define NUM_OPT 4 //number of options for menu
#define TIMER_PERIOD 0x8000 // = 32768, one sec timer
#define upLimitController 12200 //controller movement up-limit
#define lowLimitController 4200 //controller movement low-limit

//Initializing variable for temperature ADC
static volatile uint16_t curADCResult;

//Variable for storing temperature value returned from TMP006
float temp;

//Timer_A UpMode Configuration Parameter
const Timer_A_UpModeConfig upConfig ={
    TIMER_A_CLOCKSOURCE_ACLK,               //ACLK Clock Source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,          //32 KHz / 1 = 32 KHz / 32 768 = 1
    TIMER_PERIOD,                           //Every second
    TIMER_A_TAIE_INTERRUPT_DISABLE,         //Disable Timer interrupt
    TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,     //Enable CCR0 interrupt
    TIMER_A_DO_CLEAR                        //Clear value
};

void configureTimerOneSec(){

    //Configure Timer_A1 for up mode
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    //Enable sleep mode after ISR
    Interrupt_enableSleepOnIsrExit();
    //Enable interrupts
    Interrupt_enableInterrupt(INT_TA1_0);
    //Start timer in up mode
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    //Enable MASTER interrupts
    Interrupt_enableMaster();
}

void adcInit(){

    /* Configuring GPIOs (5.5 A0) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configures Pin 6.0 and 4.4 as ADC input */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Enable ADC block*/
    ADC14_enableModule();

    //![Single Sample Mode Configure]
    /* Initializing ADC (MCLK/1/4) */
   // ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_1, 0);

    //controller file clock
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

    /* Configuring ADC Memory */
    //ADC14_configureSingleSampleMode(ADC_MEM0, true);

    //nuovo incollato
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);


    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);
    ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

    /* Configuring Sample Timer */
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    /* Enabling/Toggling Conversion */
    ADC14_enableConversion();
    //ADC14_toggleConversionTrigger();

    /* Enabling interrupts */
    ADC14_enableInterrupt(ADC_INT0);
    ADC14_enableInterrupt(ADC_INT1);

    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    ADC14_toggleConversionTrigger();
}

//Mapping moisture value
float map(uint16_t AdcValue){

    const float airValue = 14200.0;
    const float waterValue = 16000.0;
    float percentage = 0.0;

    //valid AdcValue between 14200 and 16000
    if(AdcValue > airValue){
        percentage = ((100.0) / (waterValue - airValue)) * ((float)AdcValue - airValue);
        printf("Wet\n");
    }

    return percentage;
}




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

    //Set the core voltage level to VCORE1
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    //Set 2 flash wait states for Flash bank 0 and 1
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    //Initializes Clock System
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    _graphicsInit();
    portInit();
    ledPortsInit();
    buttonsInit();
    adcInit();
}

/*---Main---*/
int main(void){

    _hwInit();
    generateMenu();
    //configureTimerOneSec();

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
       Graphics_clearDisplay(&g_sContext); //clear display to avoid graphic bugs
       generateMenu();
    }
}

//Button one boosterpack interrupt
void PORT5_IRQHandler(){

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status); //clear interrupt flag

    /* check if we received P5.1 interrupt*/
    if(status & GPIO_PIN1){

       switch(currentOpt){

           case 0: //moisture graphic
               break;

           case 1: //moisture percentage
               break;

           case 2: //start pump
               startPump();
               break;

           case 3: //stop pump
               stopPump();
               break;

           default:
               printf("problems problems problems\n");
       }
    }
}

//Timer one sec, will be called when TA0CCR0 CCIFG is set
void TA1_0_IRQHandler(){

    //Clear interrupt flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    //Activate conversion
    //ADC14_toggleConversionTrigger();
}

//ADC Interrupt Handler. This handler is called whenever there is a conversion that is finished for ADC_MEM0.
void ADC14_IRQHandler(void){

    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status); //clear interrupt flag

    //moisture conversion
    if (ADC_INT0 & status){

        /* should be between 0 and 16384*/
        curADCResult = ADC14_getResult(ADC_MEM0);

        float perc = map(curADCResult);

        //printf("%d\n",curADCResult);

        //Display temperature
        /*char string[10];
        sprintf(string, "%.2f %%", perc);
        Graphics_drawStringCentered(&g_sContext, (int8_t *) string, 10, 64, 70, OPAQUE_TEXT);*/

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

            if(currentOpt == 3){
                currentOpt = 0;
            }else{
                currentOpt++;
            }
            //printf("currentOpt->%d\n",currentOpt);
            refreshMenu();
            int i = 0;
            for(i=0; i<600000; i++);
        }

        //joystick up
        if(yVal > upLimitController){

            if(currentOpt == 0){
                currentOpt = 3;
            }else{
                currentOpt--;
            }
            //printf("currentOpt->%d\n",currentOpt);
            refreshMenu();
            int i = 0;
            for(i=0; i<600000; i++);
        }

        /*-----DA TOGLIERE-------*/

        /*char string[10];
        sprintf(string, "X: %5d", resultsBuffer[0]);
        Graphics_drawStringCentered(&g_sContext, (int8_t *)string, 8, 64, 50, OPAQUE_TEXT);

        sprintf(string, "Y: %5d", resultsBuffer[1]);
        //Graphics_drawStringCentered(&g_sContext, (int8_t *)string, 8, 64, 70, OPAQUE_TEXT);

        //Determine if JoyStick button is pressed
        int buttonPressed = 0;
        if (!(P4IN & GPIO_PIN1))
            buttonPressed = 1;

        sprintf(string, "Button: %d", buttonPressed);
        //Graphics_drawStringCentered(&g_sContext, (int8_t *)string, AUTO_STRING_LENGTH, 64, 90, OPAQUE_TEXT);*/

        /*-------------------*/
    }

    ADC14_toggleConversionTrigger(); //toggle another conversion
}
