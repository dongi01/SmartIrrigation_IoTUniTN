#include <led.c>
//boolean variable that indicates if the pump is working
bool pumpOn = false;

//animate the display and then start the pump
void startPump(){

    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Starting pump", AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);

    //check if pump is already on -- check display animation --
    if(pumpOn){
        Graphics_drawStringCentered(&g_sContext, (int8_t *) "Pump is already on", AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
    }else{
        pumpOn = true;
        GPIO_setOutputLowOnPin(GPIO_PORT_P4, GPIO_PIN1); //close relay and turn on pump
        //GPIO_setOutputHighOnPin(GPIO_PORT_P1,GPIO_PIN0); //turn on led when pump is working
        redOn();
    }
}

//animate the display and stop the pump
void stopPump(){

    Graphics_clearDisplay(&g_sContext);
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Stopping pump", AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);

    //check if pump is already off -- check dipslay animation--
    if(!pumpOn){
        Graphics_drawStringCentered(&g_sContext, (int8_t *) "Pump is already off", AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
    }else{
        pumpOn = false;
        GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN1); //open relay and turn off pump
        //GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0); //turn off led when pump isn't working
        clearLeds();
    }
}
