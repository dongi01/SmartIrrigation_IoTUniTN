#include <led.c>

//Boolean variable that indicates if the pump is working
bool pumpOn = false;

//Start the pump and animate the display
void startPump(const Graphics_Image* image){

    darkMode();
    Graphics_clearDisplay(&g_sContext);
    
    //Check if pump is already on -- check display animation --
    if(pumpOn){
        drawImage(image,16,5);
        Graphics_drawStringCentered(&g_sContext, (int8_t *) "Pump is already on", AUTO_STRING_LENGTH, 64, 120, OPAQUE_TEXT);
    }else{
        drawImage(image,16,16);
        pumpOn = true;
        GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN7); //Close relay and turn on pump
        redOn();
    }
}

//animate the display and stop the pump
void stopPump(const Graphics_Image* image){

    darkMode();
    Graphics_clearDisplay(&g_sContext);
    
    //check if pump is already off -- check dipslay animation--
    if(!pumpOn){
        drawImage(image,16,5);
        Graphics_drawStringCentered(&g_sContext, (int8_t *) "Pump is already off", AUTO_STRING_LENGTH, 64, 120, OPAQUE_TEXT);
    }else{
        drawImage(image,16,16);
        pumpOn = false;
        GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN7); //open relay and turn off pump
        //GPIO_setOutputLowOnPin(GPIO_PORT_P1,GPIO_PIN0); //turn off led when pump isn't working
        clearLeds();
    }
}
