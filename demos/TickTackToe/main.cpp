#include <appdef.hpp>
#include <sdk/os/debug.hpp>
#include <sdk/os/lcd.hpp>
#include <sdk/os/mem.hpp>
#include <sdk/os/input.hpp>

APP_NAME("TickTackToe")
APP_DESCRIPTION("A simple implementation of the multiplayer game XXO(or TickTackToe)")
APP_AUTHOR("Codecrafter_404")
APP_VERSION("0.0.1")


// Implementing Mid-Point Circle Drawing Algorithm
//Thanks to https://www.geeksforgeeks.org/mid-point-circle-drawing-algorithm/
//(Need to modify some things)
void midPointCircleDraw(int x_centre, int y_centre, int r, int color)
{
    int height, width;
    LCD_GetSize(&width, &height);
    int x = r, y = 0;
     
    // initial point on the axes
    // after translation
    LCD_SetPixelFromPalette( x + x_centre, height - (y + y_centre), color);
     
    // When radius is zero only a single
    // point
    if (r > 0)
    {
        //Added some '-', there ware some wired errors(2x the same coords)
        LCD_SetPixelFromPalette(-x + x_centre,height - ( -y + y_centre), color);
        LCD_SetPixelFromPalette(y + x_centre,height - ( x + y_centre), color);
        LCD_SetPixelFromPalette(-y + x_centre,height - ( -x + y_centre), color);
    }
     
    // Initialising the value of P
    int P = 1 - r;
    while (x > y)
    {
        y++;
         
        // Mid-point is inside or on the perimeter
        if (P <= 0)
            P = P + 2*y + 1;
        // Mid-point is outside the perimeter
        else
        {
            x--;
            P = P + 2*y - 2*x + 1;
        }
         
        // All the perimeter points have already been printed
        if (x < y)
            break;
         
        // Printing the generated point and its reflection
        // in the other octants after translation
        LCD_SetPixelFromPalette(x + x_centre,height - ( y + y_centre), color);
        LCD_SetPixelFromPalette(-x + x_centre,height - ( y + y_centre), color);
        LCD_SetPixelFromPalette( x + x_centre,height - ( -y + y_centre), color);
        LCD_SetPixelFromPalette(-x + x_centre,height - ( -y + y_centre), color);
         
        // If the generated point is on the line x = y then
        // the perimeter points have already been printed
        if (x != y)
        {
            LCD_SetPixelFromPalette(y + x_centre,height - ( x + y_centre), color);
            LCD_SetPixelFromPalette(-y + x_centre,height - ( x + y_centre), color);
            LCD_SetPixelFromPalette(y + x_centre,height - ( -x + y_centre), color);
            LCD_SetPixelFromPalette(-y + x_centre,height - ( -x + y_centre), color);
        }
    }
}

//Draw a box with the coordinates in the middle(Or left button)
void drawBox(int x, int y, int size, uint8_t color){
    int screenWidth;
    int screenHeight;
    LCD_GetSize(&screenWidth, &screenHeight);
    if(size <= 1){
        LCD_SetPixelFromPalette(x, screenHeight - y, color);
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
            LCD_SetPixelFromPalette(cX, screenHeight - cY, color);
            cY++;
        }
        cY = minY;
        cX++;
    }

}


// Thanks to SnailMath(https://github.com/SnailMath/hollyhock-2/blob/master/sdk/calc/calc.cpp)
void drawLine(int x1, int y1, int x2, int y2, int size, uint8_t color){
	int8_t ix, iy;

	int dx = (x2>x1 ? (ix=1, x2-x1) : (ix=-1, x1-x2) );
	int dy = (y2>y1 ? (iy=1, y2-y1) : (iy=-1, y1-y2) );

	drawBox(x1, y1, size, color);
	if(dx>=dy){ //the derivative is less than 1 (not so steep)
		//y1 is the whole number of the y value
		//error is the fractional part (times dx to make it a whole number)
		// y = y1 + (error/dx)
		//if error/dx is greater than 0.5 (error is greater than dx/2) we add 1 to y1 and subtract dx from error (so error/dx is now around -0.5) 
		int error = 0;
		while (x1!=x2) {
			x1 += ix; //go one step in x direction
			error += dy;//add dy/dx to the y value.
			if (error>=(dx>>1)){ //If error is greater than dx/2 (error/dx is >=0.5)
				y1+=iy;
				error-=dx;
			}
			drawBox(x1, y1, size, color);
		}
	}else{ //the derivative is greater than 1 (very steep)
		int error = 0;
		while (y1!=y2) { //The same thing, just go up y and look at x
			y1 += iy; //go one step in y direction
			error += dx;//add dx/dy to the x value.
			if (error>=(dy>>1)){ //If error is greater than dx/2 (error/dx is >=0.5)
				x1+=ix;
				error-=dy;
			}
			drawBox(x1, y1, size, color);
		}
	}
}

void drawGrid(int width, int height){
    int step = width / 5;
    int cX = step;
    int cY = height - (step * 4);
    const int fieldSize = step * 3;
    
    while(cX <= step + fieldSize){
        drawLine(
            cX,
            cY,
            cX,
            cY + fieldSize,
            9,
            0
        );
        cX += step;
    }

    cX = step;

    while(cY <= height - step){
        drawLine(
            cX,
            cY,
            cX + fieldSize,
            cY,
            9,
            0
        );
        cY += step;
    }
}

//Only supports Playernumbers from 0 - 9
void drawPLayer(int player){
    Debug_SetCursorPosition(9, 15);
    Debug_PrintString("Player: ", false);
    Debug_PrintNumberHex_Byte(player, 17, 15);
}


//Prints the winner
void printWinner(int player){
    Debug_SetCursorPosition(7,0);
    if(player == 1){
        Debug_PrintString("Winner: X(1)", false);
    }else{
        Debug_PrintString("Winner: O(2)", false);
    }
}

class Field
{
    public:
        int x1;
        int y1;
        int x2;
        int y2;

        //If field is:
        // NONE = 0
        // X = 1
        // O = 2
        int state = 0;


        Field(int xStart, int yStart, int xEnd, int yEnd)
        {
            x1 = xStart;
            y1 = yStart;
            x2 = xEnd;
            y2 = yEnd;
        }

        //Returns true, if specified coords are inside the square
        bool isTouched(int x, int y)
        {
            if(x >= x1 && x <= x2){
                if(y >= y1 && y <= y2){
                    return true;
                }
            }
            return false;
        }

        void drawX()
        {
            int width, height;
            LCD_GetSize(&width, &height);
            drawLine(
                x1,
                y1,
                x2,
                y2,
                5,
                4
            );

            drawLine(
                x1 + (width / 5),
                y1,
                x2 - (width / 5),
                y2,
                5,
                4
            );
        }

        void drawO()
        {
            int fieldWidth = x2 - x1;
            int fieldHeigh = y2 - y1;
            int x0 = x2 - (fieldWidth / 2);
            int y0 = y2 - (fieldHeigh / 2);
            midPointCircleDraw(x0, y0, fieldWidth / 2, 1);
        }

};

//Check if the player wins
bool checkWin(int player, Field* fields){
    //vertical
    if(fields[6].state == player && fields[7].state == player && fields[8].state == player) return true;
    if(fields[3].state == player && fields[4].state == player && fields[5].state == player) return true;
    if(fields[0].state == player && fields[1].state == player && fields[2].state == player) return true;

    //horitontal
    if(fields[6].state == player && fields[3].state == player && fields[0].state == player) return true;
    if(fields[7].state == player && fields[4].state == player && fields[1].state == player) return true;
    if(fields[8].state == player && fields[5].state == player && fields[2].state == player) return true;

    //diagonal
    if(fields[6].state == player && fields[4].state == player && fields[2].state == player) return true;
    if(fields[8].state == player && fields[4].state == player && fields[0].state == player) return true;

    return false;
}
extern "C"
void main() 
{
    /*
        6 7 8 
        3 4 5
        0 1 2
    */
    Field* fields = (Field*)malloc(sizeof(Field) * 9);

    int height, width;
    LCD_GetSize(&width, &height);

    LCD_VRAMBackup();
    LCD_ClearScreen();
    /*
    midPointCircleDraw(100, 100, 25, 4);
    Pos p1;
    p1.x = 0;
    p1.y = 0;
    Pos p2;
    p2.x = 100;
    p2.y = 100;
    drawLine(
        p1,
        p2,
        10,
        4
    );*/
    drawGrid(width, height);
    //LCD_Refresh();
    //drawPLayer(1);
    LCD_Refresh();

    
    
    const int step = width / 5;
    const int gridX = step;
    const int gridY = height - (step * 4);

    int fieldCount = 0;

    //Init field array
    for(int y = 0; y < 3; y++){
        for(int x = 0; x < 3; x++){
            fields[fieldCount] = Field(
                gridX + (x * step),
                gridY + (y * step),
                gridX + (x * step) + step,
                gridY + (y * step) + step
            );
            fieldCount++;
        }
    }
    bool running = true;
    struct InputEvent event;
    

    int player = 1;
    while(running){
        memset(&event, 0, sizeof(event));
        GetInput(&event, 0x0, 0x12);

        //Exit Game when POWER_CLEAR
        if(event.type == EVENT_KEY){
            if(event.data.key.direction == KEY_PRESSED){
                if(event.data.key.keyCode == KEYCODE_POWER_CLEAR){
                    running = false;
                    continue;
                }
            }
        }

        drawPLayer(player);
        LCD_Refresh();

        //Handle Touch
        
        if(event.type == EVENT_TOUCH){
            if(event.data.touch_single.direction == TOUCH_DOWN){
                Field* touched;
                bool found = false;
                for(int i = 0; i < 9; i++){

                    if(fields[i].isTouched(
                        event.data.touch_single.p1_x,
                        height - event.data.touch_single.p1_y
                    ))
                    {
                        touched = &fields[i];
                        found = true;
                    }
                    if(found){
                        //Touched on Field
                        if(touched->state == 0){
                            int currentPlayer = player;
                            if(player == 1){
                                touched->state = 1;
                                touched->drawX();
                                player = 2;
                            }else{
                                touched->state = 2;
                                touched->drawO();
                                player = 1;
                            }
                            drawPLayer(player);
                            LCD_Refresh();
                            //Handle win
                            if(checkWin(currentPlayer, fields)){
                                printWinner(currentPlayer);
                                LCD_Refresh();
                                running = false;
                                continue;
                            }else{
                                // Handle and check for draw
                                bool draw = true;
                                for(int i = 0; i < 9; i++){
                                    if(fields[i].state == 0){
                                        draw = false;
                                        break;
                                    }
                                }
                                if(draw){
                                    // draw
                                    Debug_SetCursorPosition(7,0);
                                    Debug_PrintString("DRAW ;)", false);
                                    LCD_Refresh();
                                    running = false;
                                    continue;
                                }
                            }

                        }   
                    }
                }
            }
        }


    }
    Debug_SetCursorPosition(0, 21);
    Debug_PrintString("Press EXE to Exit...", false);
    LCD_Refresh();
    bool isEnd = true;
    // Wait for end
    while(isEnd){
        memset(&event, 0, sizeof(event));
        GetInput(&event, 0x0, 0x12);
        if(event.type == EVENT_KEY){
            if(event.data.key.direction == KEY_PRESSED){
                if(event.data.key.keyCode == KEYCODE_EXE){
                    isEnd = false;
                    continue;
                }
            }
        }
    }
    

    LCD_ClearScreen();
    LCD_Refresh();
    LCD_VRAMRestore();
    LCD_Refresh();
    //sh4-elf-g++ -S main.cpp -o main.s -fverbose-asm -ffreestanding -fno-exceptions -fno-rtti -fshort-wchar -Wall -Wextra -O2 -I /mnt/c/Users/Codecrafter_404/Development/Python/HollyHock2/sdk/include/
}
