/* UART Configuration:
 * http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
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
        TIMER_A_CLOCKSOURCE_DIVIDER_2,         //32 KHz / 10 = 3.2 KHz / 32 000 = 0.1 Hz = 10 seconds | 32 KHz / 2 = 16 KHz / 32 000 = 0.5 Hz = 2 seconds
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
uint_fast8_t rangeTo7bits(int value, int minI, int maxI) {  //I for initial, F for final

    uint8_t returnValue;

    if(value > maxI){
        return 127;
    }

    returnValue = 0.0 + (127.0 / (maxI - minI)) * (value - minI);
    returnValue = 100 - returnValue;
    
    return returnValue;
}

//Map the input in percentage
uint_fast8_t mapToPercentage(int value, int minI, int maxI){
    if(value < minI)
        return 0;
    return (value - minI) * 100 / (maxI - minI);

    //return 0.0 + (100.0 / (maxI - minI)) * (value - minI);
}

//Set the control bit at 1 of the first byte
uint_fast8_t setControlBit(uint8_t firstData){
    return 128 + firstData;
}

//Map and send data to the ESP32
void mapAndSendData(float temp, int lux, float moistureAdcValue){

    if (lux > 2000) lux = 2000; //max value of light
    if (temp > 63) temp = 63; //max value of temperature

    //Mapping on 7 bits
    uint_fast8_t TXMoisture = mapToPercentage(moistureAdcValue, 5000 , 14200); //16384 max value of ADC-> 16000 water value
    uint_fast8_t TXLight = rangeTo7bits(lux, 0, 2000);
    uint_fast8_t TXTemp = rangeTo7bits(temp, 0, 63);

    //Set second bit of temperature byte to 1 if pump is on
    if(pumpOn) TXTemp+=64;
    //Setting control bit at 1 of first byte
    TXMoisture = setControlBit(TXMoisture);

    printf("Moisture: %f ",moistureAdcValue);
    printf("Light: %d ",lux);
    printf("Temp: %f ",temp);
    printf("***\n");
    printf("TXMoisture: %d\n",TXMoisture-128);
    printf("TXLight: %d\n",TXLight);
    printf("TXTemp: %d\n",TXTemp);
    printf("***\n");
    
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
