#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include "HAL_I2C.h"
#include "HAL_TMP006.h"
#include <HAL_OPT3001.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define TIMER_PERIOD 0x8000 // = 32768

//Graphic library context
Graphics_Context g_sContext;

//Variable for storing temperature value returned from TMP006
float temp;
//Variable for storing light value returned from OPT3001
int lux;

//Timer_A UpMode Configuration Parameter
const Timer_A_UpModeConfig upConfig ={
    TIMER_A_CLOCKSOURCE_ACLK,               //ACLK Clock Source
    TIMER_A_CLOCKSOURCE_DIVIDER_1,          //32 KHz / 1 = 32 KHz / 32 000 = 1
    TIMER_PERIOD,                           //Every second
    TIMER_A_TAIE_INTERRUPT_DISABLE,         //Disable Timer interrupt
    TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,     //Enable CCR0 interrupt
    TIMER_A_DO_CLEAR                        //Clear value
};

void configureTimer(){

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
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

void sensorsInit(){

    //Initialize I2C communication
    Init_I2C_GPIO();
    I2C_init();

    //Initialize TMP006 temperature sensor
    TMP006_init();
    //Initialize OPT3001 digital ambient light sensor
    OPT3001_init();

    __delay_cycles(100000);
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
    sensorsInit();
}

//Main
int main(void){

    WDT_A_holdTimer(); //Stop watchdog timer

    _hwInit();
    configureTimer();

    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Temperature:", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    while (1){
        PCM_gotoLPM0(); //Sleep mode
    }
}

//Will be called when TA0CCR0 CCIFG is set
void TA1_0_IRQHandler(){

    //Obtain temperature value from TMP006
    temp = TMP006_getTemp();

    /* Obtain lux value from OPT3001 */
    lux = OPT3001_getLux();

    //Temp in celsius
    temp = (temp - 32.0) * ( 5.0 / 9.0 );
    
    //Setting a max value for comunication with 7 bits
    if(temp > 127){
        temp = 127;
    }

    //printf("%f\n",temp);
    //printf("%i\n",lux);

    //Display temperature
    char outString[10];
    sprintf(outString, "%.2f", temp);
    strcat(outString, " C");
    Graphics_drawStringCentered(&g_sContext, (int8_t *) outString, AUTO_STRING_LENGTH, 55, 70, OPAQUE_TEXT);

    sprintf(outString, "%d", lux);
    strcat(outString, " Lux");
    Graphics_drawStringCentered(&g_sContext, (int8_t *) outString, AUTO_STRING_LENGTH, 55, 80, OPAQUE_TEXT);

    //Clear interrupt flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
}
