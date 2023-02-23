/* UART Configuration:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
*/
/*
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
 */

#define TIMER_PERIOD 0x8000 // = 32768

const eUSCI_UART_ConfigV1 uartConfig = //115200 baud rate
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          //SMCLK Clock Source
        26,//13,                                      // BRDIV = 13
        0,                                       //UCxBRF = 0
        111,//37,                                      // UCxBRS = 37
        EUSCI_A_UART_NO_PARITY,                  //No Parity
        EUSCI_A_UART_LSB_FIRST,                  //LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               //One stop bit
        EUSCI_A_UART_MODE,                       //UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION,  //Oversampling
        EUSCI_A_UART_8_BIT_LEN                   //8 bit data length
};

//Timer_A UpMode Configuration Parameter
const Timer_A_UpModeConfig upConfig =
{
        TIMER_A_CLOCKSOURCE_ACLK,              //SMCLK Clock Source
        TIMER_A_CLOCKSOURCE_DIVIDER_5,         //32 KHz / 5 = 6.4 KHz / 32 000 = 0.2 Hz = 5 seconds
        TIMER_PERIOD,                          //Number of ticks
        TIMER_A_TAIE_INTERRUPT_DISABLE,        //Disable Timer interrupt
        TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE,    //Enable CCR0 interrupt
        TIMER_A_DO_CLEAR                       //Clear value
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

//Map the input value in a 7 bit range (sender side)
uint_fast8_t rangeTo7bits(int value, int minI, int maxI) {

    uint8_t returnValue;

    if(value > maxI){
        return 127;
    }
    returnValue = 0.0 + (127.0 / (maxI - minI)) * (value - minI);
    
    return returnValue;
}

//Map the input in percentage
uint_fast8_t mapToPercentage(float value, float minI, float maxI){

    if(value < minI)
        return 100;
    else if (value > maxI)
        return 0;
    else{
        float percentage = (value - minI)/(maxI-minI);
        printf("Percentage %f\n",percentage);
        return 100 - percentage*100.0;
    }
}

//Set the control bit at 1 of the first byte
uint_fast8_t setControlBit(uint8_t firstData){
    return 128 + firstData;
}

//Set pump bit
uint_fast8_t setPumpBit(uint8_t firstData){
    if(pumpOn)
        return 64 + firstData;
    return firstData
}

//Map and send data to the ESP32
void mapAndSendData(float temp, int lux, float moistureAdcValue){

    //Mapping on 7 bits
    uint_fast8_t TXMoisture = moistureAdcValue;
    uint_fast8_t TXLight = rangeTo7bits(lux, 0, 2000);
    uint_fast8_t TXTemp = temp;

    if (TXTemp > 63)
        TXTemp = 63;

    //Set second bit of temperature byte to 1 if pump is on
    TXTemp = setPumpBit(TXTemp);
    //Setting control bit at 1 of first byte
    TXMoisture = setControlBit(TXMoisture);

    // printf("Moisture: %f ",moistureAdcValue);
    // printf("Light: %d ",lux);
    // printf("Temp: %f ",temp);
    // printf("***\n");
    // printf("TXMoisture: %d\n",TXMoisture-128);
    // printf("TXLight: %d\n",TXLight);
    // printf("TXTemp: %d\n",TXTemp);
    // printf("***\n");
    
    //Transmit all data
    UART_transmitData(EUSCI_A2_BASE, TXMoisture);
    UART_transmitData(EUSCI_A2_BASE, TXLight);
    UART_transmitData(EUSCI_A2_BASE, TXTemp);
}

void uartInit(){

     //Selecting P1.2 and P1.3 in UART mode
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
             GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    //Configuring UART Module
    UART_initModule(EUSCI_A2_BASE, &uartConfig);

    //Enable UART module
    UART_enableModule(EUSCI_A2_BASE);

    //Enabling interrupts
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);
    Interrupt_enableSleepOnIsrExit();

    configureTimer();
}
