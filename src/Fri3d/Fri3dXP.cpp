#include <BinF/Engine/Platform.hpp>

#if BINF_PLATFORM == FRI3D2026

#include <Fri3d/Fri3dXP.hpp>
#include <esp32-hal-gpio.h>
#include <Wire.h>
#include <cstdint>

#define EXPANDER_SCL 42
#define EXPANDER_SDA 39
#define EXPANDER_INT 3
#define EXPANDER_FREQ 400000
#define EXPANDER_ADDRESS 0x50

#define REG_VERSION 0x00
#define REG_BUTTON  0x04
#define REG_AIN0    0x08
#define REG_BATT    0x0A
#define REG_USB     0x0C
#define REG_JOYY    0x0E
#define REG_JOYX    0x10
#define REG_LCD     0x12
#define REG_DEBUG   0x14
#define REG_DOUT    0x16

#define DOUT_AUX    0x01
#define DOUT_LCD    0x02
#define DOUT_BOOT   0x04
#define DOUT_SWD    0x08
#define DOUT_LORA   0x0A

// Initialize static member variable
Fri3dXP* Fri3dXP::instance = nullptr;
volatile bool dirty = true;

void IRAM_ATTR Fri3dXP::expander_callback() {
    dirty = true;
}

Fri3dXP::Fri3dXP()
{

}

void Fri3dXP::begin()
{
    // Set the static instance pointer to this object
    Fri3dXP::instance = this;
    
    //init peripheral expander:
    Wire.begin(EXPANDER_SDA, EXPANDER_SCL, EXPANDER_FREQ);
    pinMode(EXPANDER_INT, INPUT);
    attachInterrupt(EXPANDER_INT, expander_callback, RISING);

    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_VERSION);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 3);
    Wire.readBytes(this->version, 3);

    //the "S" button or BOOT button is wired to pin0 and allows you to force esp32 in BOOT mode
    pinMode(0, INPUT_PULLUP);
}

void Fri3dXP::loop()
{
    if (!dirty) return;

    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_BUTTON);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    Wire.readBytes((char*)&instance->buttonState, 2);
    dirty = false;
}

uint8_t Fri3dXP::getMajor() {return this->version[0];}
uint8_t Fri3dXP::getMinor() {return this->version[1];}
uint8_t Fri3dXP::getPatch() {return this->version[2];}

bool Fri3dXP::isCharging()       {return buttonState & 1;}
bool Fri3dXP::isChargerStandby() {return buttonState & 2;}
bool Fri3dXP::getButtonX()       {return buttonState & 4;}
bool Fri3dXP::getButtonY()       {return buttonState & 8;}
bool Fri3dXP::getButtonA()       {return buttonState & 16;}
bool Fri3dXP::getButtonB()       {return buttonState & 32;}
bool Fri3dXP::getButtonMenu()    {return buttonState & 64;}
bool Fri3dXP::getButtonStart()   {return digitalRead(0) == LOW;} //special case
bool Fri3dXP::isJoyUP()          {return buttonState & 128;}
bool Fri3dXP::isJoyDOWN()        {return buttonState & 256;}
bool Fri3dXP::isJoyLEFT()        {return buttonState & 512;}
bool Fri3dXP::isJoyRIGHT()       {return buttonState & 1024;}
bool Fri3dXP::isUSBPlugged()     {return buttonState & 2048;}

void Fri3dXP::setLCDBrightness(int b)
{
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_LCD);
    Wire.write((uint8_t*) &b, 2);
    Wire.endTransmission();
}

int  Fri3dXP::getLCDBrightness()
{
    int result = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_LCD);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    Wire.readBytes((char*)&result, 2);
    return result;
}

void Fri3dXP::setDebugBrightness(int b)
{
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DEBUG);
    Wire.write((uint8_t*) &b, 2);
    Wire.endTransmission();
}

int  Fri3dXP::getDebugBrightness()
{
    int result = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DEBUG);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    Wire.readBytes((char*)&result, 2);
    return result;
}

int Fri3dXP::getBattery()
{
    int result = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_BATT);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    Wire.readBytes((char*)&result, 2);
    return map(result, 0, 2400, 0, 100); //max battery seems to return 23xx, then i'm mapping it to a percentage
}

int Fri3dXP::getJoystickY()
{
    int result = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_JOYY);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    Wire.readBytes((char*)&result, 2);
    return result;
}
int Fri3dXP::getJoystickX()
{
    int result = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_JOYX);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    Wire.readBytes((char*)&result, 2);
    return result;
}

bool getAUX()
{
    uint8_t result = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    result = Wire.read();
    return result & DOUT_AUX;
}

void setAUX(bool power)
{
    uint8_t dout_bits = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    dout_bits = Wire.read();

    if (power) dout_bits |= DOUT_AUX;
    else dout_bits &= ~DOUT_AUX;

    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.write(&dout_bits, 2);
    Wire.endTransmission();
}

void Fri3dXP::resetLCD()
{
    //first read all the current bits:
    uint8_t dout_bits = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    dout_bits = Wire.read();

    //pull LCD pin low to force reset:
    dout_bits &= ~DOUT_LCD;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.write(&dout_bits, 2);
    Wire.endTransmission();
    delay(120); //screen documentation recommends holding pin low for 120ms

    //now set the LCD pin high again to enable screen:
    dout_bits |= DOUT_LCD;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.write(&dout_bits, 2);
    Wire.endTransmission();
}

void Fri3dXP::resetLORA()
{
    //first read all the current bits:
    uint8_t dout_bits = 0;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.endTransmission(false);
    Wire.requestFrom(EXPANDER_ADDRESS, 2);
    dout_bits = Wire.read();

    //pull LCD pin low to force reset:
    dout_bits &= ~DOUT_LORA;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.write(&dout_bits, 2);
    Wire.endTransmission();
    delay(120); //screen documentation recommends holding pin low for 120ms

    //now set the LCD pin high again to enable screen:
    dout_bits |= DOUT_LORA;
    Wire.beginTransmission(EXPANDER_ADDRESS);
    Wire.write(REG_DOUT);
    Wire.write(&dout_bits, 2);
    Wire.endTransmission();
}

#endif