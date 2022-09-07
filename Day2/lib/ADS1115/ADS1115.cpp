#include "ADS1115.h"

ADS1115::ADS1115(uint8_t addr, TwoWire *wire)
{
	_wire = wire;
	_addr = addr;

	//default value
	_gain = GAIN_TWOTHIRDS;
	_rate = RATE_128SPS;
}

float ADS1115::readDiff_0_1()
{
	adsMux_t mux = MUX_DIFF_0_1;
	startRead(mux,false);
	while(!conversionComplete());

	return toVolt(getLastConversion());
}

float ADS1115::readDiff_0_3()
{
	adsMux_t mux = MUX_DIFF_0_3;
	startRead(mux,false);
	while(!conversionComplete());

	return toVolt(getLastConversion());
}

float ADS1115::readDiff_1_3()
{
	adsMux_t mux = MUX_DIFF_1_3;
	startRead(mux,false);
	while(!conversionComplete());

	return toVolt(getLastConversion());
}

float ADS1115::readDiff_2_3()
{
	adsMux_t mux = MUX_DIFF_2_3;
	startRead(mux,false);
	while(!conversionComplete());

	return toVolt(getLastConversion());
}

float ADS1115::readChannel(uint8_t channel)
{
	adsMux_t mux;
	switch (channel)
	{
		case 0:
			mux = MUX_SINGLE_0;
			break;
		case 1:
			mux = MUX_SINGLE_1;
			break;
		case 2:
			mux = MUX_SINGLE_2;
			break;
		case 3:
			mux = MUX_SINGLE_3;
			break;
		
		default:
			return 0;
	}

	startRead(mux,false);
	while(!conversionComplete());

	return toVolt(getLastConversion());
}

/**************************************************************************/
/*!
    @brief  Set Programable Gain Amplifier

    @param gain setting to use
		GAIN_TWOTIRTH +/-6.144V range,
		GAIN_ONE +/-4.096V range,
		GAIN_TWO +/-2.048V range,
		GAIN_FOUR +/-1.024V range,
		GAIN_EIGHT +/-0.512V range,
		GAIN_SIXTEEN +/-0.256V range
*/
/**************************************************************************/
void ADS1115::setGain(adsGain_t gain)
{
	_gain = gain;
}

float ADS1115::getGain()
{
	float gain;
	switch (_gain){
	case GAIN_TWOTHIRDS:
		gain = 6.144f;
		break;
	case GAIN_ONE:
		gain = 4.096f;
		break;
	case GAIN_TWO:
		gain = 2.048f;
		break;
	case GAIN_FOUR:
		gain = 1.024f;
		break;
	case GAIN_EIGHT:
		gain = 0.512f;
		break;
	case GAIN_SIXTEEN:
		gain = 0.256f;
		break;
	default:
		gain = 0.0f;
	}
	return gain;
}

/**************************************************************************/
/*!
    @brief  Set Output Rate (samples per second)

    @param rate setting to use
		RATE_8SPS,
		RATE_16SPS,
		RATE_32SPS,
		RATE_64SPS,
		RATE_128SPS,
		RATE_250SPS,
		RATE_475SPS,
		RATE_860SPS,
*/
/**************************************************************************/
void ADS1115::setRate(adsRate_t rate)
{
	_rate = rate;
}

uint16_t ADS1115::getRate()
{
	uint16_t rate;
	switch(_rate){
	case RATE_8SPS:
		rate = 8;
		break;
	case RATE_16SPS:
		rate = 16;
		break;
	case RATE_32SPS:
		rate = 32;
		break;
	case RATE_64SPS:
		rate = 64;
		break;
	case RATE_128SPS:
		rate = 128;
		break;
	case RATE_250SPS:
		rate = 250;
		break;
	case RATE_475SPS:
		rate = 475;
		break;
	case RATE_860SPS:
		rate = 860;
		break;
	}
	return rate;
}

void ADS1115::startRead(adsMux_t mux, bool continuous)
{
	uint16_t config = REG_CONFIG_OS_SINGLE;
	config |= mux;
	config |= _gain;
	config |= _rate;

	if(continuous) 
		config |= MODE_CONTINUOUS;
	else
		config |= MODE_SINGLE;
	
	writeReg(REG_POINTER_CONFIG, config);
}

bool ADS1115::conversionComplete()
{
	return (readReg(REG_POINTER_CONFIG) & 0x8000) != 0;
}

int16_t ADS1115::getLastConversion()
{
	return (int16_t)readReg(REG_POINTER_CONVERT);
}

void ADS1115::writeReg(uint8_t reg, uint16_t value)
{
	buffer.asUInt = value;
	_wire->beginTransmission(_addr);
	_wire->write(reg);
	_wire->write(buffer.asByte[1]);
	_wire->write(buffer.asByte[0]);
	_wire->endTransmission();
}

uint16_t ADS1115::readReg(uint8_t reg)
{
	_wire->beginTransmission(_addr);
	_wire->write(reg);
	_wire->endTransmission();

	_wire->requestFrom(_addr,2);
	buffer.asByte[1] = _wire->read();
	buffer.asByte[0] = _wire->read();

	return buffer.asUInt;
}

float ADS1115::toVolt(int16_t value)
{
	float fsRange;
	switch (_gain){
	case GAIN_TWOTHIRDS:
		fsRange = 6.144f;
		break;
	case GAIN_ONE:
		fsRange = 4.096f;
		break;
	case GAIN_TWO:
		fsRange = 2.048f;
		break;
	case GAIN_FOUR:
		fsRange = 1.024f;
		break;
	case GAIN_EIGHT:
		fsRange = 0.512f;
		break;
	case GAIN_SIXTEEN:
		fsRange = 0.256f;
		break;
	default:
		fsRange = 0.0f;
	}

	return value*fsRange/32768.0f;
}