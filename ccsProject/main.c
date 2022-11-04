#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include <stdio.h>
#include <stdbool.h>

#define NUM_OPT 4

#define TIMER_PERIOD 0x8000 // = 32768

/* Initializing Variables */
static volatile uint16_t curADCResult;
static uint16_t resultsBuffer[2];

//controller movement limit
const int upLimitController = 12200;
const int lowLimitController = 4200;

//current menu option selected
short currentOpt = 0;

//menu options
int8_t * menuOpt[NUM_OPT] = {"Moisture graphic","Moisture percentage","Start the pump","Stop the pump"};

//boolean variable that indicates if the pump is working
bool pumpOn = false;

//Graphic library context
Graphics_Context g_sContext;

//Variable for storing temperature value returned from TMP006
float temp;

//Timer_A UpMode Configuration Parameter
const Timer_A_UpModeConfig upConfig ={
    TIMER_A_CLOCKSOURCE_ACLK,               //ACLK Clock Source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,          //32 KHz / 1 = 32 KHz / 32 000 = 1
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

void _graphicsInit(){

    //Initializes display
    Crystalfontz128x128_Init();

    //Set default screen orientation
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    //Initializes graphics context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
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

//Main

void generateMenu(){

    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Menu:", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    int32_t verticalPos = 60;
    int i=0;
    char toWrite[22];
    for(i = 0 ; i < NUM_OPT ; i++){
        if(i == currentOpt){
            sprintf(toWrite,"->%s", menuOpt[i]);
        }else{
            sprintf(toWrite,"%s", menuOpt[i]);
        }

        printf("Tryna printin %s\n", toWrite);
        Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 64, verticalPos, OPAQUE_TEXT);
        verticalPos+=10;
    }
}

void refreshMenu(){

    //Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Menu:", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    int32_t verticalPos = 60;
        int i=0;
        char toWrite[30];
        for(i = 0 ; i < NUM_OPT ; i++){
            if(i == currentOpt){
                sprintf(toWrite,"->%s", menuOpt[i]);
            }else{
                sprintf(toWrite,"%s", menuOpt[i]);
            }

            //printf("Tryna printin %s\n", toWrite);
            Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 64, verticalPos, OPAQUE_TEXT);
            verticalPos+=10;
        }



}

void clearLeds(){

    //turn off booster pack leds
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);

}

void portInit(){

    //set as output leds boosterpack
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN6);

    //set P4.1 for relay control
    GPIO_setAsOutputPin(GPIO_PORT_P4,GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4,GPIO_PIN1);

    clearLeds();
}


void redOn(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
}

void blueOn(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);

}

void greenOn(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
}

void _buttonsInit(){

    //setting pin as input in pull up mode
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5,GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3,GPIO_PIN5);

    //enabling interrupt
    GPIO_enableInterrupt(GPIO_PORT_P5,GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P3,GPIO_PIN5);

    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableInterrupt(INT_PORT3);
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
    _buttonsInit();
    adcInit();
}

void startPump(){

    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Starting pump", AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);

    //check if pump is already on -- controllare stampa display --
    if(pumpOn){
        Graphics_drawStringCentered(&g_sContext, (int8_t *) "Pump is already on", AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);
    }else{
        pumpOn = true;
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1); //close relay and turn on pump
    }
}

void stopPump(){

    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Stopping pump", AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);

    //check if pump is already off -- controllare stampa display --
    if(!pumpOn){
        Graphics_drawStringCentered(&g_sContext, (int8_t *) "Pump is already off", AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);
    }else{
        pumpOn = false;
        GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1); //open relay and turn off pump
    }
}

int main(void){

    _hwInit();
    //configureTimerOneSec();
    generateMenu();

    while (1){
        PCM_gotoLPM0(); //Sleep mode
    }
}

void PORT3_IRQHandler(){

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    /* clear interrupt flag (to clear pending interrupt indicator */
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status);
    /* check if we received P3.5 interrupt*/
    if((status & GPIO_PIN5)){
       printf("second button pressed\n");
       generateMenu();
       //GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);
    }
}

void PORT5_IRQHandler(){

    printf("first button interrupt \n");

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status);
    /* check if we received P5.1 interrupt*/
    if((status & GPIO_PIN1)){
       printf("first button pressed\n");
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
       //GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
    }
}

//Will be called when TA0CCR0 CCIFG is set
void TA1_0_IRQHandler(){

    //Clear interrupt flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    //Activate conversion
    //ADC14_toggleConversionTrigger();
}

/* ADC Interrupt Handler. This handler is called whenever there is a conversion
 * that is finished for ADC_MEM0.
*/
void ADC14_IRQHandler(void){

    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status);

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

    if(status & ADC_INT1){

        /* Store ADC14 conversion results */
        //resultsBuffer[0] = ADC14_getResult(ADC_MEM0);
        resultsBuffer[1] = ADC14_getResult(ADC_MEM1);

        int yVal = resultsBuffer[1];

        //joystick up
        if (yVal < lowLimitController){
            if(currentOpt < 3)currentOpt++;
            printf("currentOpt->%d\n",currentOpt);
            refreshMenu();
            int i = 0;
            for(i = 0 ; i < 1000000 ; i++);
        }

        //joystick down
        if(yVal > upLimitController){
            if(currentOpt > 0)currentOpt--;
            printf("currentOpt->%d\n",currentOpt);
            refreshMenu();
            int i = 0;
            for(i = 0 ; i < 1000000 ; i++);
        }

        char string[10];
        /*sprintf(string, "X: %5d", resultsBuffer[0]);
        Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        8,
                                        64,
                                        50,
                                        OPAQUE_TEXT);*/

        sprintf(string, "Y: %5d", resultsBuffer[1]);
        /*Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        8,
                                        64,
                                        70,
                                        OPAQUE_TEXT);*/

        /* Determine if JoyStick button is pressed */
        int buttonPressed = 0;
        if (!(P4IN & GPIO_PIN1))
            buttonPressed = 1;

        sprintf(string, "Button: %d", buttonPressed);
       /* Graphics_drawStringCentered(&g_sContext,
                                        (int8_t *)string,
                                        AUTO_STRING_LENGTH,
                                        64,
                                        90,
                                        OPAQUE_TEXT);*/
    }
    ADC14_toggleConversionTrigger();
}
