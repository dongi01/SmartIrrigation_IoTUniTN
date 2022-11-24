#include <ti/grlib/grlib.h>
#include "LcdDriver/Crystalfontz128x128_ST7735.h"
#include "LcdDriver/HAL_MSP_EXP432P401R_Crystalfontz128x128_ST7735.h"
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/driverlib.h>
#include <string.h>
#include <stdio.h>

#define NUM_OPT 4 //number of menu option

 //number of menu option

Graphics_Context g_sContext; //screen object
int currentOpt=0; //currently selected menu option
const int8_t * menuOpt[NUM_OPT] = { "Moisture graphic", "Moisture percentage", "Start the pump", "Stop the pump" }; //menu option

//function to initialize the graphic components of the boosterpack
void _graphicsInit(){

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
