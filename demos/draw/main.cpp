#include <appdef.hpp>
#include <sdk/os/lcd.hpp>
#include <sdk/os/input.hpp>
#include <sdk/os/mem.hpp>
#include <sdk/os/debug.hpp>
#include <sdk/os/file.hpp>

/*
 * Fill this section in with some information about your app.
 * All fields are optional - so if you don't need one, take it out.
 */
APP_NAME("Draw")
APP_DESCRIPTION("A draw program")
APP_AUTHOR("Codecrafter_404")
APP_VERSION("1.0.0")


void drawFilledMidPoitnCircle(int x, int y, int radius, int color){
    for(int cy=-radius; cy<=radius; cy++)
    for(int cx=-radius; cx<=radius; cx++)
        if(cx*cx+cy*cy <= radius*radius)
            LCD_SetPixelFromPalette(x + cx, y + cy, color);
}


//Draw a box with the coordinates in the middle(Or left button)
void drawBox(int x, int y, int size, uint8_t color){
    if(size <= 1){
        LCD_SetPixelFromPalette(x, y, color);
        return;
    }
    int minX = 0;
    int maxX = 0;
    int minY = 0;
    int maxY = 0;

    if(size % 2 != 0){
        //Coordinate is in the middle
        minX = x - ((size - 1) / 2);
        minY = y - ((size - 1) / 2);

        maxX = x + ((size - 1) / 2);
        maxY = y + ((size - 1) / 2);
        
    }else{
        //Coordinate is in the button left corner
        minX = x;
        minY = y;
        maxX = x + size;
        maxY = y + size;
    }
    int cX = minX;
    int cY = minY;

    while(cX <= maxX){
        while(cY <= maxY){
            LCD_SetPixelFromPalette(cX, cY, color);
            cY++;
        }
        cY = minY;
        cX++;
    }

}



extern "C"
void main() {
    bool running = true;

    struct InputEvent event;
    LCD_ClearScreen();
    LCD_Refresh();

    uint8_t pencil = 3;
    // Selected PENCIL
    /*
        1 -> 1px
        2 -> 3px
        3 -> 5px
        4 -> 7px
        5 -> 10px
    */

   uint8_t color = 0;
   // Selected Color
   /*
        0 -> Black
        1 -> Blue
        2 -> Green
        4 -> Red

        3 -> Cyan
        5 -> Magenta
        6 -> Yellow

        7 -> Rubber
   */
    uint8_t pencilType = 0;
    // Pencil Mode
    /*
        0 -> Circle
        1 -> Square
    */

    // Is Infodisplay showing?
    bool isInfoShowing = false;

    while (running)
    {
        memset(&event, 0, sizeof(event));
        GetInput(&event, 0x0, 0x12);

        // Check Exit
        if(event.type == EVENT_KEY && event.data.key.direction == KEY_PRESSED && event.data.key.keyCode == KEYCODE_POWER_CLEAR){
            running = false;
            continue;
        }

        if(event.type == EVENT_KEY && event.data.key.direction == KEY_PRESSED){
            switch (event.data.key.keyCode)
            {
            case KEYCODE_EQUALS: // Rubber
                color = 7;
                break;
            case KEYCODE_X: // 1px
                pencil = 1;
                break;
            case KEYCODE_Y: // 3px
                pencil = 3;
                break;
            case KEYCODE_Z: // 5px
                pencil = 5;
                break;
            case KEYCODE_POWER: // 7px
                pencil = 7;
                break;
            case KEYCODE_DIVIDE: // 10px
                pencil = 10;
                break;
            case KEYCODE_OPEN_PARENTHESIS: // Black
                color = 0;
                break;
            case KEYCODE_CLOSE_PARENTHESIS: // Blue
                color = 1;
                break;
            case KEYCODE_COMMA: // Green
                color = 2;
                break;
            case KEYCODE_NEGATIVE: // Red
                color = 4;
                break;
            case KEYCODE_TIMES: // Cyan
                color = 3;
                break;
            case KEYCODE_MINUS: // Magenta
                color = 5;
                break;
            case KEYCODE_PLUS: // Yellow
                color = 6;
                break;
            case KEYCODE_EXE: // Switch between pencil types: Circle, Square
            {
                LCD_Refresh();
                pencilType = !pencilType;
                break;
            }

            
            default: // Show Current Color
            {
                isInfoShowing = !isInfoShowing;
                if(isInfoShowing){
                    LCD_VRAMBackup();
                }else{
                    LCD_VRAMRestore();
                    LCD_Refresh();
                }
                break;
            }
            }
        }
        
        // Diplays infos, if it's triggerd
        if(isInfoShowing){
            char* colorName = "-- UNIMPLEMENTED --";
            switch (color)
            {
            case 0:
                colorName = "Black";
                break;
            case 1:
                colorName = "Blue";
                break;
            case 2:
                colorName = "Green";
                break;
            case 3:
                colorName = "Cyan";
                break;
            case 4:
                colorName = "Red";
                break;
            case 5:
                colorName = "Magenta";
                break;
            case 6:
                colorName = "Yellow";
                break;
            case 7:
                colorName = "Rubber";
                break;
            
            default: // Let default in
                break;
            }
            
            // Print Actual Values
            Debug_Printf(0, 0, false, 0, "Color: %8s", colorName);
            Debug_Printf(0, 1, false, 0, "Pencil Size: %02d", pencil);
            Debug_Printf(0, 2, false, 0, "Pencil Mode: %s", !pencilType? "Circle" : "Square");
            LCD_Refresh();
        }
        // Check Touch
        if(!isInfoShowing && event.type == EVENT_TOUCH && event.data.touch_single.direction == TOUCH_HOLD_DRAG){
            if(!pencilType){
                // Draw Circle
                drawFilledMidPoitnCircle(
                    event.data.touch_single.p1_x,
                    event.data.touch_single.p1_y,
                    pencil,
                    color
                );

            }else{
                // Draw box
                drawBox(
                    event.data.touch_single.p1_x,
                    event.data.touch_single.p1_y,
                    pencil / 2,
                    color
                );
            }
            
            LCD_Refresh();
        }
    }
    LCD_ClearScreen();
    Debug_SetCursorPosition(0, 0);
    Debug_PrintString("Tap On 'Menu' to exit. ", false);
    LCD_Refresh();
    return;
}
