//BoosterPack buttons
void buttonsInit(){

    //setting pin as input in pull up mode
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5,GPIO_PIN1);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P3,GPIO_PIN5);

    //enabling interrupt
    GPIO_enableInterrupt(GPIO_PORT_P5,GPIO_PIN1);
    GPIO_enableInterrupt(GPIO_PORT_P3,GPIO_PIN5);

    Interrupt_enableInterrupt(INT_PORT5);
    Interrupt_enableInterrupt(INT_PORT3);
}
