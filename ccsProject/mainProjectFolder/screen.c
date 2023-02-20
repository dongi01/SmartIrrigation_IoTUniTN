#define NUM_OPT 3 //Number of menu options

Graphics_Context g_sContext; //Screen object
int currentOpt=0; //Currently selected menu option
const int8_t * menuOpt[NUM_OPT] = { "Sensors data", "Start the pump", "Stop the pump" }; //Menu option

//Function to initialize the graphic components of the boosterpack
void graphicsInit(){

    //Initializes display
    Crystalfontz128x128_Init();

    //Set default screen orientation
    Crystalfontz128x128_SetOrientation(LCD_ORIENTATION_UP);

    //Initializes graphics context
    Graphics_initContext(&g_sContext, &g_sCrystalfontz128x128, &g_sCrystalfontz128x128_funcs);
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

//Function to showing the app logo in the screen
void showAppLogo(){
    Graphics_drawImage(&g_sContext, &appNameImage, 17, 40);
}

//Function that changes the current selected menu option
void refreshMenu(){

    Graphics_drawImage(&g_sContext, &menuLayoutImage, 0, 0);
    //Graphics_drawStringCentered(&g_sContext, (int8_t *) "Menu:", AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);

    int32_t verticalPos = 60; //Vertical position of the first menu option
    int i = 0;
    char toWrite[22] = ""; //Storing the value of the string that has to be written

    for(i = 0 ; i < NUM_OPT ; i++){ //Looping through the menu to change its options

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

//White background and black text
void lightMode(){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
}

//Black background and white text
void darkMode(){
    Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
    Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_BLACK);
}

//Function that shows the sensor data in the screen
void showSensorData(int lux, float temp, int moisturePercentage){

    char toWrite[22] = ""; //Storing the value of the string that has to be written
    
    //Add a 0 before the first decimal digit of the lux if it is less than 100
    if(lux < 10){
        sprintf(toWrite,"000%d", lux);
    }else if(lux < 100){
        sprintf(toWrite,"00%d", lux);
    }else if(lux < 1000){
        sprintf(toWrite,"0%d", lux);
    }else{
        sprintf(toWrite,"%d", lux);
    }
    //Print lux value
    Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 30, 49, OPAQUE_TEXT);

    if(temp<0.0){
        temp=0.0;
    }
    //Write in toWrite only the first 2 decimal digits of the temperature
    sprintf(toWrite,"%.2f", temp);
    //Print temperature value
    Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 95, 49, OPAQUE_TEXT);

    sprintf(toWrite,"%d%%", moisturePercentage);
    //Print moisture value
    Graphics_drawStringCentered(&g_sContext,(int8_t *) toWrite, AUTO_STRING_LENGTH, 64, 117, OPAQUE_TEXT);
}
