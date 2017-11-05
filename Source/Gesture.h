/*
  ==============================================================================

    Gesture.h
    Created: 23 Oct 2017 1:11:42pm
    Author:  geri

    Description:  Receives mouse coordinates from the PlayComponent and calculates
    			  gesture parameters. 

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class Gesture
{
	public:
        typedef struct Position { float xPos, yPos; } Position;
    
		static void setVelocity(float x, float y);
		static void setDirection(float p [][2]);
        static String getDirection();
        static float getVelocityX();
        static float getVelocityY();
        static float getVelocity();
    
        static void fillDirBuff(float x, float y);

        static void addFinger(const MouseEvent& e);
        static void rmFinger(const MouseEvent& e);
        static Position* getFinger(const MouseEvent& e);
        static Position* getFingerPosition(int index);
        static void updateFingers(const MouseEvent& e);

        static void setResetPos(bool reset);
        static bool getResetPos();
    
        static bool resetPos;
        static int directionBuffSize;
    
	private:
        static OwnedArray<Position> fingers; // stores the mouse locations
        //static Position fing[3];

		static float xNew;
		static float xTemp;
        static float xDelta;
    
        static float yNew;
        static float yTemp;
        static float yDelta;
    
        static float dist;
    
        static float directionDeltaX;
        static float directionDeltaY;
        static float absDirectionDeltaX;
        static float absDirectionDeltaY;
        static String direction;
};
