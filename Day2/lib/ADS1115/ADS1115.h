#ifndef ADS1115_H
#define ADS1115_H

#include <Arduino.h>
#include <Wire.h>

#define REG_POINTER_MASK (0x03)      ///< Point mask
#define REG_POINTER_CONVERT (0x00)   ///< Conversion
#define REG_POINTER_CONFIG (0x01)    ///< Configuration
#define REG_POINTER_LOWTHRESH (0x02) ///< Low threshold
#define REG_POINTER_HITHRESH (0x03)  ///< High threshold

#define REG_CONFIG_OS_MASK (0x8000) ///< OS Mask
#define REG_CONFIG_OS_SINGLE (0x8000) ///< Write: Set to start a single-conversion
#define REG_CONFIG_OS_BUSY (0x0000) ///< Read: Bit = 0 when conversion is in progress
#define REG_CONFIG_OS_NOTBUSY (0x8000) ///< Read: Bit = 1 when device is not performing a conversion

#define MODE_MASK (0x0100)
#define MODE_SINGLE (0x0100)
#define MODE_CONTINUOUS (0x0000)

#define MUX_MASK 0x7000
typedef enum {
    MUX_DIFF_0_1 = 0x0000,
    MUX_DIFF_0_3 = 0x1000,
    MUX_DIFF_1_3 = 0x2000,
    MUX_DIFF_2_3 = 0x3000,
    MUX_SINGLE_0 = 0x4000,
    MUX_SINGLE_1 = 0x5000,
    MUX_SINGLE_2 = 0x6000,
    MUX_SINGLE_3 = 0x7000
}adsMux_t;

#define RATE_MASK 0x00E0
typedef enum {
    RATE_8SPS = 0x0000,
    RATE_16SPS = 0x0020,
    RATE_32SPS = 0x0040,
    RATE_64SPS = 0x0060,
    RATE_128SPS = 0x0080,
    RATE_250SPS = 0x00A0,
    RATE_475SPS = 0x00C0,
    RATE_860SPS = 0x00E0
}adsRate_t;

#define GAIN_MASK 0x0E00
typedef enum {
    GAIN_TWOTHIRDS = 0x0000,    //+/-6.144V range
    GAIN_ONE = 0x0200,          // +/-4.096V range
    GAIN_TWO = 0x0400,          // +/-2.048V range
    GAIN_FOUR = 0x0600,         // +/-1.024V range
    GAIN_EIGHT = 0x800,         // +/-0.512V range
    GAIN_SIXTEEN = 0x0A00       // +/-0.256V range
}adsGain_t;

typedef union UInt16ToByte
{
    uint16_t asUInt;
    uint8_t asByte[2];
}adsBuffer_t;

class ADS1115
{
    protected:
        TwoWire *_wire;
        adsGain_t _gain; 
        adsRate_t _rate;
        uint8_t _addr;

    public: 
        ADS1115(uint8_t addr = 0x48, TwoWire *wire = &Wire);

        float readChannel(uint8_t channel);
        float readDiff_0_1();
        float readDiff_0_3();
        float readDiff_1_3();
        float readDiff_2_3();

        void setGain(adsGain_t gain);
        float getGain();
        void setRate(adsRate_t rate);
        uint16_t getRate();

        void startRead(adsMux_t mux, bool continuous);
        int16_t getLastConversion();
        bool conversionComplete();
        float toVolt(int16_t value);

    private:
        void writeReg(uint8_t reg,uint16_t value);
        uint16_t readReg(uint8_t reg);
        adsBuffer_t buffer;

};

#endif