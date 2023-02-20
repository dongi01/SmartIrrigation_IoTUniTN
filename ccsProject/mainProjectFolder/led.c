//Turn off boosterpack's leds
void clearLeds(){

    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);

}

//Set as output boosterpack's leds
void ledPortsInit(){

       GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN4);
       GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN6);
       GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN6);

       clearLeds();
}

//Turn on boosterpack's leds
void redOn(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN6);
}
void blueOn(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P5, GPIO_PIN6);

}
void greenOn(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN4);
}
