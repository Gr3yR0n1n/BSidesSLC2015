/*
    theTransistor, DC801, and 801 Labs brings you another fun conference badge. :)
    
    Credits go out to:
    
    ++ Electronics Work & Processing
    + Compukidmike
    + Devino
    
    ++ QA, Assembly, & Loading
    + l3mur (aka aarobc)
    + Yukaia
    + Compukidmike
    
    ++ Original Design, Code, & Mgmt
    + D3c4f
    
    ++ Special Thanks
    + Nemus
    + T4rkus & BSidesSLC
    
    Questions and comments can be directed twards the following forms of communication:
    #dc801 on freenode
    d3c4f@thetransistor.com
*/

#include "Arduino.h"
#include "badge.h"
#include "lcd5110.h"
#include "bitmaps.h"
#include <avr/pgmspace.h>
#include <EEPROM.h>

Badge badge;    // Welcome to Arduino... :S
LCD5110 display;
bool ShowMenu = true; // Super basic menu support
bool Menu1 = true;


void setup(void) {
	display.DisplayBitmap(BSidesSLC);
	delay(1000);
    BSidesSLC_Animation();
}

// TODO: Update the menu "system"
void loop(void) {
    if (ShowMenu == true)
    {
        ShowMenu = false;
        if(Menu1)
        {
            display.DisplayBitmap_PROGMEM(Menu1_px);   
        }
        else
        {
            display.DisplayBitmap_PROGMEM(Menu2_px);
        }
        
        //display.Clear();
        //display.WriteString("a) BSidesLogo", 1, 30);
        //display.WriteString("b) NYAN", 1, 39);
        //display.Redraw();
    }
	
    if (badge.JoyUp(15, false) || badge.JoyDown(15, false))
    {
        if(Menu1) Menu1 = false;
        else Menu1 = true;
        ShowMenu = true;
    }
    
    if( badge.Button0(15, false) || badge.Button1(15, false))
    {
        if(Menu1) BSidesSLC_Animation();
        else NYAN_Animation();
        ShowMenu = true;
    }
    
}


// Animation routine for the BSidesSLC Logo
void NYAN_Animation(void)
{
    long delayTime = random(250,750);
    long lastUpdate = millis();
    uint8_t curFrame = 1;
    display.DisplayBitmap_PROGMEM(NYAN_frame1);
    
    while(!badge.Button0(50, false) && !badge.Button1(50, false) && !(badge.JoyUp(50, false)) && !(badge.JoyDown(50, false)))
    {
        if(millis() < lastUpdate) lastUpdate = millis(); // Edge case precaution
        
        if(millis() > lastUpdate + delayTime)
        {
            lastUpdate = millis();
            delayTime = random(250,750);
            
            if (curFrame <= 1)
            {
                display.DisplayBitmap_PROGMEM(NYAN_frame1);
                curFrame++;
            }
            else if (curFrame == 2)
            {
                display.DisplayBitmap_PROGMEM(NYAN_frame2);
                curFrame++;
            }
            else if (curFrame >= 3)
            {
                display.DisplayBitmap_PROGMEM(NYAN_frame3);
                curFrame = 1;
            }
        } 
    }
}


// Animation routine for NYAN Cat
void BSidesSLC_Animation(void)
{
    long delayTime = random(250,750);
    long lastUpdate = millis();
    display.DisplayBitmap(BSidesSLC);
	
    while(!badge.Button0(50, false) && !badge.Button1(50, false) && !(badge.JoyUp(50, false)) && !(badge.JoyDown(50, false)))
    {
        if(millis() < lastUpdate) lastUpdate = millis(); // Edge case precaution
        
        if(millis() > lastUpdate + delayTime)
        {
            lastUpdate = millis();
            delayTime = random(250,750);
            
            switch(random(12))
            {
                case 0:
                    display.TogglePixel(44,11);
                    break;
                case 1:
                    display.TogglePixel(43,13);
                    break;
                case 2:
                    display.TogglePixel(47,12);
                    break;
                case 3:
                    display.TogglePixel(49,11);
                    break;
                case 4:
                    display.TogglePixel(54,6);
                    break;
                case 5:
                    display.TogglePixel(54,8);
                    break;
                case 6:
                    display.TogglePixel(56,10);
                    break;
                case 7:
                    display.TogglePixel(57,8);
                    break;
                case 8:
                    display.TogglePixel(63,13);
                    break;
                case 9:
                    display.TogglePixel(67,12);
                    break;
                case 10:
                    display.TogglePixel(77,8);
                    break;
                case 11:
                    display.TogglePixel(77,9);
                    break;
                case 12:
                    display.TogglePixel(77,11);
                    break;
            }
            display.Redraw();
        } 
    }
}


