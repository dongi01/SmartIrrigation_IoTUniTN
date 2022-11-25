/******************************************************************************
 * MSP432 UART - Loopback with 24MHz DCO BRCLK
 *
 * Description: This demo connects TX to RX of the MSP432 UART
 * The example code shows proper initialization of registers
 * and interrupts to receive and transmit data. If data is incorrect P1.0 LED
 * is turned ON.
 *
 *  MCLK = HSMCLK = SMCLK = DCO of 24MHz
 *
 *               MSP432P401
 *             -----------------
 *            |                 |
 *       RST -|     P3.3/UCA0TXD|----|
 *            |                 |    |
 *           -|                 |    |
 *            |     P3.2/UCA0RXD|----|
 *            |                 |
 *            |             P1.0|---> LED
 *            |                 |
 *
 *******************************************************************************/
/* DriverLib Includes */
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#define TIMER_PERIOD 0x8000 // = 32000

uint8_t RXData = 0;

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */
const eUSCI_UART_ConfigV1 uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        13,                                      // BRDIV = 13
        0,                                       // UCxBRF = 0
        37,                                      // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  // Oversampling
        EUSCI_A_UART_8_BIT_LEN                   // 8 bit data length
};

/* Timer_A UpMode Configuration Parameter */
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,              // SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_1,         // 32 KHz / 10 = 3.2 KHz / 32 000 = 10 sec
        TIMER_PERIOD,                           // every half second
        TIMER_A_TAIE_INTERRUPT_DISABLE,         // Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,     // Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                        // Clear value
};

void configureTimer(){

    /* Configuring Timer_A1 for Up Mode */
    Timer_A_configureUpMode(TIMER_A1_BASE, &upConfig);

    //enable sleep mode after ISR
    Interrupt_enableSleepOnIsrExit();
    //enable interrupts
    Interrupt_enableInterrupt(INT_TA1_0);
    //start timer
    Timer_A_startCounter(TIMER_A1_BASE, TIMER_A_UP_MODE);

    /* Enabling MASTER interrupts */
    Interrupt_enableMaster();
}

// maps the input value in a smaller range (sender side)
uint8_t rangeTo7bits(int value, int minI, int maxI) {  // I for initial, F for final

    uint8_t returnValue;

    if(value < 128){
        returnValue = value;
    }else{
        returnValue = 0.0 + (127.0 / (maxI - minI)) * (value - minI);
    }
    return returnValue;
}

uint8_t mapToPercentage(int value, int minI, int maxI){
    return (value - minI) * 100 / (maxI - minI);

    //return 0.0 + (100.0 / (maxI - minI)) * (value - minI);
}

uint8_t setControlBit(uint8_t firstData){
    return 128 + firstData;
}

int main(void)
{
    /* Halting WDT  */
    WDT_A_holdTimer();

    /* Selecting P1.2 and P1.3 in UART mode and P1.0 as output (LED) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
             GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN2);

    /* Setting DCO to 24MHz (upping Vcore) -> CPU operates at 24 MHz!*/
    FlashCtl_setWaitState(FLASH_BANK0, 1);
    FlashCtl_setWaitState(FLASH_BANK1, 1);
    PCM_setCoreVoltageLevel(PCM_VCORE1);
    CS_setDCOCenteredFrequency(CS_DCO_FREQUENCY_24);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);
    Interrupt_enableSleepOnIsrExit();

    configureTimer();

    while(1){
        Interrupt_enableSleepOnIsrExit();
        PCM_gotoLPM0InterruptSafe();
    }
}

/* EUSCI A0 UART ISR - Echos data back to PC host */
void EUSCIA2_IRQHandler(void)
{
    uint32_t status = UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        RXData = UART_receiveData(EUSCI_A2_BASE);

        printf("RXData: %d\n",RXData);

        if(RXData == 5){
            GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);//red
        }else if(RXData == 1){
            GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);//green
        }else{
            GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN2);//blue
        }

        int i=0;
        for(i=0; i<100000; i++);

        Interrupt_disableSleepOnIsrExit();
    }
}

/*DEBUGGING*/
// checks if the input number has the right dimention
//bool checkDimension(uint8_t bitDimension, uint8_t variable) {
//    variable = variable >> bitDimension;
//    return (variable == 0) ? true : false;
//}

// maps the input value in a larger range (reciver side)
int sevenBitsToRange(uint8_t value, int minF, int maxF) {
    int returnValue = minF + value * (maxF - minF) / 127; // 127 max value for 7 bits
    return returnValue;
}
/**********/

//Will be called when TA0CCR0 CCIFG is set
void TA1_0_IRQHandler(){

    //Clear interrupt flag
    Timer_A_clearCaptureCompareInterrupt(TIMER_A1_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);

    int dataMoisture = 15000;
    int dataLight = 13000;
    int dataTemp = 29;

    //Mapping on 7 bits
    uint_fast8_t TXMoisture = mapToPercentage(dataMoisture, 14000, 16384); //16384 max value of ADC
    uint_fast8_t TXLight = rangeTo7bits(dataLight, 0, 20000);
    uint_fast8_t TXTemp = rangeTo7bits(dataTemp, 0, 127);

    //Setting control bit at 1 of first byte
    TXMoisture = setControlBit(TXMoisture);

    printf("TXMoisture: %d\n",TXMoisture-128);
    printf("TXLight: %d\n",TXLight);
    printf("TXTemp: %d\n",TXTemp);
    printf("\n");

    /*DEBUGGING*/
//    dataMoisture = sevenBitsToRange(TXMoisture-128, 14000, 16000);
//    dataLight = sevenBitsToRange(TXLight, 0, 20000);
//    dataTemp = sevenBitsToRange(TXTemp, 0, 127);
//
//    printf("dataMoisture: %d\n",dataMoisture);
//    printf("dataLight: %d\n",dataLight);
//    printf("dataTemp: %d\n",dataTemp);
//    printf("\n");
    /**********/

    //Transmit all data
    UART_transmitData(EUSCI_A2_BASE, TXMoisture);
    UART_transmitData(EUSCI_A2_BASE, TXLight);
    UART_transmitData(EUSCI_A2_BASE, TXTemp);
}
