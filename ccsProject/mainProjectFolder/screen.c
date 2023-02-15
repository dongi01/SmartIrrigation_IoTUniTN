#define NUM_OPT 3

Graphics_Context g_sContext; //screen object
int currentOpt=0; //currently selected menu option
const int8_t * menuOpt[NUM_OPT] = { "Sensors data", "Start the pump", "Stop the pump" }; //menu option

//function to initialize the graphic components of the boosterpack
void graphicsInit(){

    //Initializes display
    Crystalfontz128x128_Init();

    //Set default screen orientation
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    //Initializes graphics context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

//function to generate the menu in the screen
void generateMenu(){

    Graphics_clearDisplay(&g_sContext); //clear display to avoid graphic bugs
    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Menu:", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    int32_t verticalPos = 60; //vertical position of the first menu option
    int i=0;
    char toWrite[22]; //storing the value of the string that has to be written

    for(i = 0 ; i < NUM_OPT ; i++){ //looping through the menu to change its options
        if(i == currentOpt){
            sprintf(toWrite,"->%s", menuOpt[i]);
        }else{
            sprintf(toWrite,"%s", menuOpt[i]);
        }

        Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 64, verticalPos, OPAQUE_TEXT); //change the voice option in the screen
        verticalPos+=10;
    }
}

//function to change the current selected menu option
void refreshMenu(){

    Graphics_drawStringCentered(&g_sContext, (int8_t *) "Menu:", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    int32_t verticalPos = 60; //vertical position of the first menu option
    int i = 0;
    char toWrite[22] = ""; //storing the value of the string that has to be written

    for(i = 0 ; i < NUM_OPT ; i++){ //looping through the menu to change its options

        if(i == currentOpt){
            sprintf(toWrite,"->%s", menuOpt[i]);
        }else{
            sprintf(toWrite," %s ", menuOpt[i]);
        }

        Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 64, verticalPos, OPAQUE_TEXT); //change the voice option in the screen
        verticalPos+=10;
    }
}

void drawImage(const Graphics_Image* image, int16_t xSpawn, int16_t ySpawn){
    Graphics_drawImage(&g_sContext, image, xSpawn, ySpawn);
}

//function to show the sensor data in the screen
void showSensorData(int lux, float temp, int moisturePercentage){

    char toWrite[22] = ""; //storing the value of the string that has to be written
    
    //add a 0 before the first decimal digit of the lux if it is less than 100
    if(lux < 10){
        sprintf(toWrite,"Lux: 00%d", lux);
    }else if(lux < 100){
        sprintf(toWrite,"Lux: 0%d", lux);
    }else{
        sprintf(toWrite,"Lux: %d", lux);
    }
    Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 64, 50, OPAQUE_TEXT);

    if(temp<0.0){
        temp=0.0;
    }
    //write in toWrite only the first 2 decimal digits of the temperature
    sprintf(toWrite,"Temp: %.2f C", temp);
    Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 64, 60, OPAQUE_TEXT);

    sprintf(toWrite,"Moisture: %d%%", moisturePercentage);
    Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 64, 70, OPAQUE_TEXT);

}
