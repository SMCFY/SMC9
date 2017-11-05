/*
  ==============================================================================

    Gesture.cpp
    Created: 23 Oct 2017 1:11:42pm
    Author:  geri

  ==============================================================================
*/

#include "Gesture.h"

OwnedArray<Gesture::Position> Gesture::fingers;
//Gesture::Position Gesture::fing[3];

float Gesture::xNew;
float Gesture::xTemp;
float Gesture::xDelta;

float Gesture::yNew;
float Gesture::yTemp;
float Gesture::yDelta;

float Gesture::dist;

float Gesture::directionDeltaX;
float Gesture::directionDeltaY;
float Gesture::absDirectionDeltaX;
float Gesture::absDirectionDeltaY;
int Gesture::directionBuffSize = 5;
String Gesture::direction;

bool Gesture::resetPos = false;

void Gesture::addFinger(const MouseEvent& e)
{
    Gesture::Position* f = new Gesture::Position();
    f->xPos = e.position.x;
    f->yPos = e.position.y;
    fingers.add(f);
}

void Gesture::rmFinger(const MouseEvent& e)
{
    fingers.removeObject(getFinger(e));
}

Gesture::Position* Gesture::getFinger(const MouseEvent& e)
{
    for (int i = 0; i < fingers.size(); i++)
    {
        //Gesture::Position *f = fingers[i];
        if (fingers[i]->xPos == e.position.getX() && fingers[i]->yPos == e.position.getY())
            return fingers[i];
    }
    return nullptr;
}

Gesture::Position* Gesture::getFingerPosition(int index)
{
    return fingers[index];
}

void Gesture::updateFingers(const MouseEvent& e)
{
    for (int i = 0; i < fingers.size(); i++)
    {
        fingers[i]->xPos = e.position.x;
        fingers[i]->yPos = e.position.y;
    }
}

void Gesture::setVelocity(float x, float y)
{
    xNew  = x;
    yNew = y;
    
    if(resetPos)
    {
        xTemp = xNew;
        yTemp = yNew;
        resetPos = false;
    }
    
    
    xDelta = std::sqrt(std::pow(xNew-xTemp,2));
    yDelta = std::sqrt(std::pow(yNew-yTemp,2));
    dist = std::sqrt(std::pow(xNew-xTemp,2)+std::pow(yNew-yTemp,2));
        
    xTemp = xNew;
    yTemp = yNew;
    
    //std::cout << xDelta << "   ";
}

void Gesture::setDirection(float p [directionBuffSize][2])
{
    absDirectionDeltaX = std::sqrt(std::pow(p[0][0]-p[directionBuffSize-1][0],2));
    absDirectionDeltaY = std::sqrt(std::pow(p[0][1]-p[directionBuffSize-1][1],2));
    
    directionDeltaX = p[directionBuffSize-1][0]-p[0][0];
    directionDeltaY = p[directionBuffSize-1][1]-p[0][1];
    
    if(directionDeltaX > 0 && directionDeltaX != 0)
    {
        if(absDirectionDeltaY > absDirectionDeltaX)
        {
            if(directionDeltaY > 0)
            {
                direction = "UP";
            } else if (directionDeltaY < 0)
            {
                direction = "DOWN";
            }
        } else
        {
            direction = "RIGHT";
        }
    } else if (directionDeltaX < 0)
    {
        if(absDirectionDeltaY > absDirectionDeltaX)
        {
            if(directionDeltaY > 0)
            {
                direction = "UP";
            } else if (directionDeltaY < 0)
            {
                direction = "DOWN";
            }
        } else
        {
            direction = "LEFT";
        }
    } /*else if (directionDeltaX == 0 && directionDeltaY == 0)
        direction = "TAP";
    */
    //std::cout << p[directionBuffSize-1][0] << "  " << p[0][0] << "  " << p[1][0];
    std::cout << direction << "\n";
    //std::cout << "X:  " << directionDeltaX << "   Y:   " << directionDeltaY << "   ";
}

String Gesture::getDirection()
{
    return direction;
}

float Gesture::getVelocity()
{
    return std::pow(dist+1,4) ;
}

float Gesture::getVelocityX()
{
    return std::pow(xDelta+1,4);
}

float Gesture::getVelocityY()
{
    return std::pow(yDelta+1,4);
}

bool Gesture::getResetPos()
{
    return resetPos;
}

void Gesture::setResetPos(bool reset)
{
    Gesture::resetPos= reset;
}
