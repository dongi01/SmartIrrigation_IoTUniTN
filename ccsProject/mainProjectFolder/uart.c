/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 115200 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 */

#define TIMER_PERIOD 0x8000 // = 32768, one sec timer
const eUSCI_UART_ConfigV1 uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        26,//13,                                      // BRDIV = 13
        0,                                       // UCxBRF = 0
        111,//37,                                      // UCxBRS = 37
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
        TIMER_A_CLOCKSOURCE_DIVIDER_2,         // 32 KHz / 10 = 3.2 KHz / 32 000 = 10 sec
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
uint_fast8_t rangeTo7bits(int value, int minI, int maxI) {  // I for initial, F for final

    uint8_t returnValue;

    if(value > maxI){
        return 127;
    }

    returnValue = 0.0 + (127.0 / (maxI - minI)) * (value - minI);
    returnValue = 100 - returnValue;
    
    return returnValue;
}

uint_fast8_t mapToPercentage(int value, int minI, int maxI){
    if(value < minI)
        return 0;
    return (value - minI) * 100 / (maxI - minI);

    //return 0.0 + (100.0 / (maxI - minI)) * (value - minI);
}

uint_fast8_t setControlBit(uint8_t firstData){
    return 128 + firstData;
}

void mapAndSendData(float temp, int lux, float moistureAdcValue){

    //Mapping on 7 bits
    uint_fast8_t TXMoisture = mapToPercentage(moistureAdcValue, 5000 , 14200); //16384 max value of ADC-> 16000 water value
    uint_fast8_t TXLight = rangeTo7bits(lux, 0, 2000); //need to set max value of light
    uint_fast8_t TXTemp = rangeTo7bits(temp, 0, 127); //max temp is set to 127

    if(pumpOn)TXTemp+=64;
    //Setting control bit at 1 of first byte
    TXMoisture = setControlBit(TXMoisture);

    printf("Moisture: %f ",moistureAdcValue);
    printf("TXMoisture: %d\n",TXMoisture-128);
    printf("Light: %d ",lux);
    printf("TXLight: %d\n",TXLight);
    printf("Temp: %f ",temp);
    printf("TXTemp: %d\n",TXTemp);
    printf("***\n");

    //Transmit all data
    UART_transmitData(EUSCI_A2_BASE, TXMoisture);
    UART_transmitData(EUSCI_A2_BASE, TXLight);
    UART_transmitData(EUSCI_A2_BASE, TXTemp);
}

void uartInit(){

     /* Selecting P1.2 and P1.3 in UART mode*/
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
             GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Configuring UART Module */
    UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    Interrupt_enableInterrupt(INT_EUSCIA2);
    Interrupt_enableSleepOnIsrExit();

    configureTimer();
}

