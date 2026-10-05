#include <Arduino.h>

volatile extern bool dirty;

class Fri3dXP {
    public:
        Fri3dXP();
        void begin(); //sets up the interrupt routine and initializes 2 pins
        void loop();  //call this method every loop before you request button states
        uint8_t getMajor();
        uint8_t getMinor();
        uint8_t getPatch();

        bool isCharging();
        bool isChargerStandby();
        bool getButtonX();    //true when pushed
        bool getButtonY();    //true when pushed
        bool getButtonA();    //true when pushed
        bool getButtonB();    //true when pushed
        bool getButtonMenu(); //true when pushed
        bool getButtonStart();//true when pushed
        bool isJoyUP();
        bool isJoyDOWN();
        bool isJoyLEFT();
        bool isJoyRIGHT();
        int getJoystickY();           //rather than a boolean, this returns the actual position. [0-4096], 2048 is roughly neutral position
        int getJoystickX();           //rather than a boolean, this returns the actual position. [0-4096], 2048 is roughly neutral position
        bool isUSBPlugged();

        void setLCDBrightness(int b);   //accepts a value from 0-100
        int  getLCDBrightness();        //returns a value from 0-100
        void setDebugBrightness(int b); //accepts a value from 0-100
        int  getDebugBrightness();      //returns a value from 0-100
        int getBattery();               //returns a percentage as an indication of the battery charge status
        bool getAUX();                  //true if 3v3 AUX power ON
        void setAUX(bool power);        //disable/enable 3v3 AUX power
        void resetLCD();                //warning! If you call this, you need to re-initialize TFT_eSPI
        void resetLORA();

    private:
        uint8_t version[3];
        uint16_t buttonState;
        static Fri3dXP* instance;  // Static pointer to class instance
        static void IRAM_ATTR expander_callback();  // Static member callback
};
