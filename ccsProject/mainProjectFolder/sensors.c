float MAX_ADC_MOISTURE_VALUE = 14200.0;
float MIN_ADC_MOISTURE_VALUE = 5000.0;

void adcInit(){

    /* Configuring GPIOs (5.5 A0) */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Configures Pin 6.0 and 4.4 as ADC input */
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6, GPIO_PIN0, GPIO_TERTIARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P4, GPIO_PIN4, GPIO_TERTIARY_MODULE_FUNCTION);

    /* Enable ADC block*/
    ADC14_enableModule();

    //controller file clock
    ADC14_initModule(ADC_CLOCKSOURCE_ADCOSC, ADC_PREDIVIDER_64, ADC_DIVIDER_8, 0);

    //nuovo incollato
    ADC14_configureMultiSequenceMode(ADC_MEM0, ADC_MEM1, true);


    ADC14_configureConversionMemory(ADC_MEM0, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);
    ADC14_configureConversionMemory(ADC_MEM1, ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A9, ADC_NONDIFFERENTIAL_INPUTS);

    /* Configuring Sample Timer */
    ADC14_enableSampleTimer(ADC_MANUAL_ITERATION);

    /* Enabling/Toggling Conversion */
    ADC14_enableConversion();

    /* Enabling interrupts */
    ADC14_enableInterrupt(ADC_INT0);
    ADC14_enableInterrupt(ADC_INT1);

    Interrupt_enableInterrupt(INT_ADC14);
    Interrupt_enableMaster();

    ADC14_toggleConversionTrigger();
}

void sensorsInit(){

    //Initialize control pin for relay
    GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN7);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN7); //Relay activates when P2.7 = 0

    //Initialize I2C communication
    Init_I2C_GPIO();
    I2C_init();

    //Initialize TMP006 temperature sensor
    TMP006_init();
    //Initialize OPT3001 digital ambient light sensor
    OPT3001_init();

    __delay_cycles(100000);

    adcInit();
}
