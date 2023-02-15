#include <led.c>
//boolean variable that indicates if the pump is working
bool pumpOn = false;

//animate the display and then start the pump
void startPump(const Graphics_Image* image){

    Graphics_clearDisplay(&g_sContext);
    //Graphics_drawStringCentered(&g_sContext, (int8_t *) "Starting pump", AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);

    //check if pump is already on -- check display animation --
    if(pumpOn){
        Graphics_drawStringCentered(&g_sContext, (int8_t *) "Pump is already on", AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
    }else{
        drawImage(image,16,16);
        //sleep(2);
        pumpOn = true;
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7); //close relay and turn on pump
        redOn();
    }
}

//animate the display and stop the pump
void stopPump(const Graphics_Image* image){

    Graphics_clearDisplay(&g_sContext);
    //Graphics_drawStringCentered(&g_sContext, (int8_t *) "Stopping pump", AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);

    //check if pump is already off -- check dipslay animation--
    if(!pumpOn){
        Graphics_drawStringCentered(&g_sContext, (int8_t *) "Pump is already off", AUTO_STRING_LENGTH, 64, 75, OPAQUE_TEXT);
    }else{
        drawImage(image,16,16);
        pumpOn = false;
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7); //open relay and turn off pump
        //GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0); //turn off led when pump isn't working
        clearLeds();
    }
}
