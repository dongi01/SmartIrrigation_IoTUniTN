void clearLeds(){

    //turn off boosterpack's leds
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN4);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN6);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN6);

}

void ledPortsInit(){
        //set as output boosterpack's leds
       GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN4);
       GPIO_setAsOutputPin(GPIO_PORT_P2,GPIO_PIN6);
       GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN6);

       clearLeds();
}

//Turn on boosterpack's leds
void redOn(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN6);
}

void blueOn(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN6);

}

void greenOn(){
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN4);
}
