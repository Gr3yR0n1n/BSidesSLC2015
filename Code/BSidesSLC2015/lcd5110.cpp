/*
    Filename: lcd5110.cpp
    Purpose: Library for Nokia5110 LCD on the 2015 BSidesSLC Badge
    Created: March 16, 2013
    Original Author: d3c4f

    Original Credit: Nathan Seidle, Spark Fun Electronics 2011
    Original Credit URL: https://github.com/sparkfun/GraphicLCD_Nokia_5110
    (Thanks Nathan!)
*/

#include "Arduino.h"
#include "lcd5110.h"
#include "bitmaps.h"

/*
SCE      : PB4
RESET    : PB5
DC       : PB6
SDIN     : PB2
SCLK     : PB1
LED Back : Arduino Analog 11 (PWM)
*/


// Constructor
LCD5110::LCD5110()
{
    Init();
    
    SetBacklight(0x80); // Set initial backlight level
}


// Initialize the display.
// Sets the LCD to a known state, Sets up the command list, Contrast, etc.
void LCD5110::Init()
{
    // Set display pins as outputs
    DDRB |= B11110110; // Set B7, B6, B5, B4, B2, B1 as outputs

    // Reset the LCD by toggling the reset pin
    PORTB &= B11011111; // Pull B5 Low
    PORTB |= B00100000; // Pull B5 High

    Write(LCD_COMMAND, 0x21); //Tell LCD that extended commands follow
    Write(LCD_COMMAND, 0xB3); //Set LCD Vop (Contrast): Try 0xB1(good @ 3.3V) or 0xBF if your display is too dark
    Write(LCD_COMMAND, 0x04); //Set Temp coefficient
    Write(LCD_COMMAND, 0x13); //LCD bias mode 1:48: Try 0x13 or 0x14

    Write(LCD_COMMAND, 0x20); //We must send 0x20 before modifying the display control mode
    Write(LCD_COMMAND, 0x0C); //Set display control, normal mode. 0x0D for inverse

    Clear();
}


// Change the backlight level
void LCD5110::SetBacklight(uint8_t level)
{
    // Could maybe switch this to direct port manipulation later...
    analogWrite(11, level);
}


// Set a pixel of the (84 x 48) display buffer
void LCD5110::SetPixel(uint16_t x, uint16_t y)
{
    if (x >= 84 || y >= 48) return; // Bounds check
    displayBuffer[(84 * (y / 8)) + x] |= (1 << (y % 8));
}


// Clear a pixel of the (84 x 48) display buffer
void LCD5110::ClearPixel(uint16_t x, uint16_t y)
{
    if (x >= 84 || y >= 48) return; // Bounds check
    displayBuffer[(84 * (y / 8)) + x] &= ~(1 << (y % 8));
}


// Toggle a pixel of the (84 x 48) display buffer
void LCD5110::TogglePixel(uint16_t x, uint16_t y)
{
    if (x >= 84 || y >= 48) return; // Bounds check
    displayBuffer[(84 * (y / 8)) + x] ^= (1 << (y % 8));
}


// Write a byte array to the display buffer and show it
void LCD5110::DisplayBitmap(const uint8_t bitmap_array[])
{
    for (uint16_t curByte = 0 ; curByte < 504 ; curByte++)
    {
        //Write(LCD_DATA, bitmap_array[index]);
        displayBuffer[curByte] = bitmap_array[curByte];
    }
    
    Redraw();
}


// Write a byte array stored in program (flash) memory to the display buffer and show it
void LCD5110::DisplayBitmap_PROGMEM(const uint8_t bitmap_array[])
{
    for (uint16_t curByte = 0 ; curByte < 504 ; curByte++)
    {
        //Write(LCD_DATA, bitmap_array[index]);
        displayBuffer[curByte] = pgm_read_byte_near(bitmap_array + curByte);
    }
    
    Redraw();
}


// Write a character to the display
// Pads each character with 1px on either side for readability
void LCD5110::WriteCharacter(const char character, uint16_t x, uint16_t y)
{
    for (uint8_t colByte = 0 ; colByte < 5 ; colByte++)
    {
        //0x20 is the ASCII character for Space (' '). The font table starts with this character
        if(ASCII[character - 0x20][colByte] & BIT0_SET) SetPixel(x + colByte, y);
        else ClearPixel(x + colByte, y);

        if(ASCII[character - 0x20][colByte] & BIT1_SET) SetPixel(x + colByte, y + 1);
        else ClearPixel(x + colByte, y + 1);

        if(ASCII[character - 0x20][colByte] & BIT2_SET) SetPixel(x + colByte, y + 2);
        else ClearPixel(x + colByte, y + 2);

        if(ASCII[character - 0x20][colByte] & BIT3_SET) SetPixel(x + colByte, y + 3);
        else ClearPixel(x + colByte, y + 3);

        if(ASCII[character - 0x20][colByte] & BIT4_SET) SetPixel(x + colByte, y + 4);
        else ClearPixel(x + colByte, y + 4);

        if(ASCII[character - 0x20][colByte] & BIT5_SET) SetPixel(x + colByte, y + 5);
        else ClearPixel(x + colByte, y + 5);

        if(ASCII[character - 0x20][colByte] & BIT6_SET) SetPixel(x + colByte, y + 6);
        else ClearPixel(x + colByte, y + 6);

        if(ASCII[character - 0x20][colByte] & BIT7_SET) SetPixel(x + colByte, y + 7);
        else ClearPixel(x + colByte, y + 7);
        
        //Write(LCD_DATA, ASCII[character - 0x20][colByte]);
    }
}


// Writes an array of characters to a single line.
void LCD5110::WriteString(const char *characters, uint16_t x, uint16_t y)
{
    uint8_t curChar = 0;
    while (*characters)
    {
        WriteCharacter(*characters++, x + (curChar * 6), y);
        curChar ++;
    }
}


// Zero out the entire display buffer and redraw
void LCD5110::Clear()
{
    for (uint16_t curByte = 0 ; curByte < 504 ; curByte++)
    {
        displayBuffer[curByte] = 0x00;
    }
    Redraw();
}


// Change the contrast: try 0xB1 to 0xB3
//void SetContrast(uint8_t cont);


// Change the display bias mode: try 0x13 or 0x14
//void SetBias(uint8_t bias);


// Move to X,Y position
void LCD5110::GotoXY(uint8_t x, uint8_t y)
{
        Write(LCD_COMMAND, 0x80 | x);  // Column.
        Write(LCD_COMMAND, 0x40 | y);  // Row.  ?
}


// There are two memory banks in the LCD, data/RAM and commands. This 
// function sets the DC pin high or low depending, and then sends
// the data byte
void LCD5110::Write(byte data_or_command, byte data)
{
    // Set if we are sending data or a command : 0 for command, 1 for data
    PORTB = data_or_command == 1 ? (PORTB | BIT6_SET) : (PORTB & BIT6_MASK);
    
    // Send the data
    PORTB &= BIT4_MASK; // Pull SCE Low (B5)
    
    // Clock in each bit
    PORTB = (data & BIT7_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    
    
    PORTB = (data & BIT6_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT5_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT4_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT3_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT2_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT1_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT0_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    
    
    PORTB |= BIT4_SET; // Pull SCE High (B4)
}


// Same as Write, but in LSBF-order for screen-buffer processing moistly ;)
void LCD5110::WriteLSBF(byte data)
{
    // We will only send data this way
    PORTB |= BIT6_SET;
    
    PORTB &= BIT4_MASK; // Pull SCE Low (B5)
    
    // Clock in each bit
    PORTB = (data & BIT0_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT1_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT2_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT3_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT4_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT5_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT6_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

    PORTB = (data & BIT7_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
    PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    
    
    PORTB |= BIT4_SET; // Pull SCE High (B4)
}


// Pushes the Display Buffer out to the Display
void LCD5110::Redraw()
{
    Write(LCD_COMMAND, 0x80); Write(LCD_COMMAND, 0x40); // Reset position to "0,0"
    PORTB |= BIT6_SET; // We're sending date (not commands)
    PORTB &= BIT4_MASK; // Pull SCE Low (B5) (enable)
    
    // Draw the screen buffer
    for(int curByte = 503; curByte > 0; curByte --)
    {
        // Clock in each bit
        PORTB = (displayBuffer[curByte] & BIT0_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
        PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

        PORTB = (displayBuffer[curByte] & BIT1_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
        PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

        PORTB = (displayBuffer[curByte] & BIT2_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
        PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

        PORTB = (displayBuffer[curByte] & BIT3_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
        PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

        PORTB = (displayBuffer[curByte] & BIT4_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
        PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

        PORTB = (displayBuffer[curByte] & BIT5_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
        PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

        PORTB = (displayBuffer[curByte] & BIT6_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
        PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    

        PORTB = (displayBuffer[curByte] & BIT7_SET) ? (PORTB | BIT2_SET) : (PORTB & BIT2_MASK);
        PORTB |= BIT1_SET; PORTB &= BIT1_MASK; // Pull SCLK (B1) High and then Low    
    }
    PORTB |= BIT4_SET; // Pull SCE High (B4) (done)
}

