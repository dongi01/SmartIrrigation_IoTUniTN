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

#define upLimitController 12200 //controller movement up-limit
#define lowLimitController 4200 //controller movement low-limit

//Initializing variable for moisture ADC
static volatile uint16_t curADCResult;

//Variable for storing temperature value returned from TMP006
float temp;
//Variable for storing light value returned from OPT3001
unsigned int lux;
//Variable for storing moisturePercetage taken from adc convertion
float moisturePercentage;

//Boolean to keep refreshing data in sensors data mode
bool showMode = false;

//boolean to not refresh menu if we are in the submenu
bool inSubMenu = false;

//Number of timer interrupt before removing the app logo
int showLogoMode = 1;

//Variable to store the received uart value
uint8_t RXData = 0;

//Boolean to prioritize the manual control
bool manualControl = false;

//boolean to check if the controller was triggered once
bool triggered = false;

void _hwInit(){

    //Halting WDT and disabling master interrupts
    WDT_A_holdTimer();
    Interrupt_disableMaster();

    /* Set the core voltage level to VCORE1 */
    PCM_setCoreVoltageLevel(PCM_VCORE1);

    /* Set 2 flash wait states for Flash bank 0 and 1*/
    FlashCtl_setWaitState(FLASH_BANK0, 2);
    FlashCtl_setWaitState(FLASH_BANK1, 2);

    /* Initializes Clock System */
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_48);
    CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_HSMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
    CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    graphicsInit();
    ledPortsInit();
    buttonsInit();
    adcInit();
    sensorsInit();
    uartInit();
}

/*---Main---*/
int main(void){

    _hwInit();

    //Show Smart Irrigation logo
    showAppLogo();

    while (1){
        PCM_gotoLPM0(); //Sleep mode
    }
}

//Button two boosterpack interrupt
void PORT3_IRQHandler(){

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P3);
    GPIO_clearInterruptFlag(GPIO_PORT_P3, status); //Clear interrupt flag

    //Check if we received P3.5 interrupt
    if((status & GPIO_PIN5)){
        lightMode();
        refreshMenu();
        showMode=false;
        inSubMenu=false;
    }
}

//Button one boosterpack interrupt
void PORT5_IRQHandler(){

    uint_fast16_t status = GPIO_getEnabledInterruptStatus(GPIO_PORT_P5);
    GPIO_clearInterruptFlag(GPIO_PORT_P5, status); //Clear interrupt flag

    //Check if we received P5.1 interrupt
    if(status & GPIO_PIN1){

        inSubMenu = true;

        switch(currentOpt){

            case 0: //Sensors data
                Graphics_clearDisplay(&g_sContext);
                lightMode();
                Graphics_drawImage(&g_sContext, &showDataLayoutImage, 0, 0);
                showSensorData(lux,temp,moisturePercentage);
                showMode=1;
                break;

            case 1: //start pump
                manualControl = true;
                startPump(&dropImage); //Water drop image
                break;

            case 2: //stop pump
                manualControl = false;
                stopPump(&barDropImage); //Crossed water drop image
                break;

            default:
                printf("problems problems problems\n");
        }
    }
}

//Timer interrupt handler
void TA1_0_IRQHandler(){

    //Clear interrupt flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    //Show the app logo for N timer interrupts
    if(showLogoMode > 0){
        showAppLogo();
        showLogoMode--;
    }else if(showLogoMode == 0){
        Graphics_clearDisplay(&g_sContext);
        refreshMenu();
    }

    //Obtain temperature value from TMP006
    temp = TMP006_getTemp();
    //Temp in celsius
    temp = (temp - 32.0) * ( 5.0 / 9.0 );

    //Obtain lux value from OPT3001
    lux = OPT3001_getLux();

    //Map moisutre value from ADC
    moisturePercentage = mapToPercentage(curADCResult,MIN_ADC_MOISTURE_VALUE,MAX_ADC_MOISTURE_VALUE);

    //printf("Moisture percentage %f \n",moisturePercentage);

    if(moisturePercentage < 20 && !pumpOn && !manualControl){
        startPump(&dropImage);
    }
    if(moisturePercentage > 80 && pumpOn && !manualControl){
        stopPump(&barDropImage);
    }

    //If show mode print sensors data
    if(showMode){
        showSensorData(lux,temp,moisturePercentage);
    }

    //Send data to the ESP32
    mapAndSendData(temp,lux,moisturePercentage);
}

//ADC Interrupt Handler, this handler is called whenever there is a conversion that is finished for ADC_MEM0
void ADC14_IRQHandler(void){

    uint64_t status = ADC14_getEnabledInterruptStatus();
    ADC14_clearInterruptFlag(status); //Clear interrupt flag

    //Get moisture value from ADC
    if (ADC_INT0 & status){
        //Should be between 0 and 16384
        curADCResult = ADC14_getResult(ADC_MEM0);
    }

    //joystick conversion
    if(ADC_INT1 & status){

        int yVal = ADC14_getResult(ADC_MEM1);

        //Joystick down
        if (yVal < lowLimitController && !tilted){

            tilted = true;

            //Update selected menu option
            if(currentOpt == NUM_OPT-1){
                currentOpt = 0;
            }else{
                currentOpt++;
            }

            if(!inSubMenu){
                refreshMenu();
                int i = 0;
                for(i=0; i<300000; i++); //Delay for controller usability
            }
        }

        //Joystick up
        if(yVal > upLimitController && !tilted){
 
            tilted = true;

            //Update selected menu option
            if(currentOpt == 0){
                currentOpt = NUM_OPT-1;
            }else{
                currentOpt--;
            }

            if(!inSubMenu){
                refreshMenu();
                int i = 0;
                for(i=0; i<300000; i++); //Delay for controller usability
            }
        }
        
        //Joystick center to set back tilted to false
        if(yVal > 7000 && yVal < 9000){
            tilted = false;
        }
    }

    ADC14_toggleConversionTrigger(); //Toggle another conversion
}

//EUSCI A0 UART ISR - reciever handler
void EUSCIA2_IRQHandler(void){

    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG){

        //Save recieved data
        RXData = UART_receiveData(EUSCI_A2_BASE);
        printf("RXData: %d\n",RXData);

        if(RXData == 240){
            manualControl = true;
            redOn();
            startPump(&dropImage);
        }else if(RXData == 10){
            manualControl = false;
            greenOn();
            stopPump(&barDropImage);
        }else{
            blueOn();
        }

        Interrupt_disableSleepOnIsrExit();
    }
}
