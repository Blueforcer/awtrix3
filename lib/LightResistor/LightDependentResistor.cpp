/*
 * \brief Get light intensity value (Lux & FootCandles) from Light dependent Resistor (implementation)
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 30 January 2023
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2023 Quentin Comte-Gaz
 * \version 1.4
 */

#include "LightDependentResistor.h"

LightDependentResistor::LightDependentResistor(int pin, unsigned long other_resistor, ePhotoCellKind kind, unsigned int adc_resolution_bits, unsigned int smoothing_history_size) :
  _pin (pin),
  _other_resistor (other_resistor),
  _mult_value(32017200),
  _pow_value(1.5832),
  _photocell_on_ground (true),
  _adc_resolution_bits(adc_resolution_bits),
  _smoothing_sum(0.0f),
  _smoothing_history_size(smoothing_history_size),
  _smoothing_history_next(0),
  _smoothing_history_values(new float[smoothing_history_size])
{
  switch (kind)
  {
    case GL5516:
      _mult_value = 29634400;
      _pow_value = 1.6689;
    break;
    case GL5537_1:
      _mult_value = 32435800;
      _pow_value = 1.4899;
    break;
    case GL5537_2:
      _mult_value = 2801820;
      _pow_value = 1.1772;
    break;
    case GL5539:
      _mult_value = 208510000;
      _pow_value = 1.4850;
    break;
    case GL5549:
      _mult_value = 44682100;
      _pow_value = 1.2750;
    break;
    case GL5528:
    default:
      _mult_value = 32017200;
      _pow_value = 1.5832;
  }

  for (unsigned int i = 0 ; i < _smoothing_history_size ; i++)
  {
    // We initialize the values as impossible value (lux can't be negative)
    _smoothing_history_values[i] = -1.0f;
  }
}

LightDependentResistor::LightDependentResistor(int pin, unsigned long other_resistor, float mult_value, float pow_value, unsigned int adc_resolution_bits, unsigned int smoothing_history_size) :
  _pin (pin),
  _other_resistor (other_resistor),
  _mult_value (mult_value),
  _pow_value (pow_value),
  _photocell_on_ground (true),
  _adc_resolution_bits(adc_resolution_bits),
  _smoothing_sum(0.0f),
  _smoothing_history_size(smoothing_history_size),
  _smoothing_history_next(0),
  _smoothing_history_values(new float[smoothing_history_size])

{
  for (unsigned int i = 0 ; i < _smoothing_history_size ; i++)
  {
    // We initialize the values as impossible value (lux can't be negative)
    _smoothing_history_values[i] = -1.0f;
  }
}

LightDependentResistor::~LightDependentResistor()
{
  // De-allocate the table at the end of the use of the class
  delete[] _smoothing_history_values;
}

void LightDependentResistor::updatePhotocellParameters(float mult_value, float pow_value)
{
  _mult_value = mult_value;
  _pow_value = pow_value;
}

float LightDependentResistor::luxToFootCandles(float intensity_in_lux)
{
  return intensity_in_lux/10.764;
}

float LightDependentResistor::footCandlesToLux(float intensity_in_footcandles)
{
  return 10.764*intensity_in_footcandles;
}

void LightDependentResistor::setPhotocellPositionOnGround(bool on_ground)
{
  _photocell_on_ground = on_ground;
}

int LightDependentResistor::getCurrentRawAnalogValue() const
{
  // Analog resolution setter is not handled on all boards (not compatible boards: MEGA, ESP8266, Uno)
  #if !defined(__AVR_ATmega1280__) && !defined(__AVR_ATmega2560__) && !defined(ESP8266) && !defined(__AVR_ATmega328P__) && !defined(__AVR_ATmega168__)
    analogReadResolution(_adc_resolution_bits);
  #endif

  return analogRead(_pin);
}

float LightDependentResistor::rawAnalogValueToLux(int raw_analog_value) const
{
  unsigned long photocell_resistor;

  if (pow(2, _adc_resolution_bits) == raw_analog_value)
  {
    raw_analog_value--;
  }

  float ratio = ((float)pow(2, _adc_resolution_bits) / (float)raw_analog_value) - 1;
  if (_photocell_on_ground)
  {
    photocell_resistor = _other_resistor / ratio;
  }
  else
  {
    photocell_resistor = _other_resistor * ratio;
  }

  return _mult_value / (float)pow(photocell_resistor, _pow_value);
}

float LightDependentResistor::getCurrentLux() const
{
  return rawAnalogValueToLux(getCurrentRawAnalogValue());
}

float LightDependentResistor::getCurrentFootCandles() const
{
  return luxToFootCandles(getCurrentLux());
}

float LightDependentResistor::getSmoothedLux()
{
  float sumResult = 0;

  if (_smoothing_history_size == 0)
  {
    // Smoothing disabled, return current value.
    sumResult = getCurrentLux();
  }
  else
  {
    if (_smoothing_history_values[_smoothing_history_next] < -0.1f)
    {
      // Smoothing enabled but not all values are filled yet
      // (Let's fill one more)
      _smoothing_history_values[_smoothing_history_next] = getCurrentLux();
      _smoothing_sum += _smoothing_history_values[_smoothing_history_next];

      if (_smoothing_history_next < _smoothing_history_size - 1)
      {
        // Still not all buffers filled
        _smoothing_history_next++;
        sumResult = _smoothing_sum / _smoothing_history_next;
      }
      else
      {
        // All buffers filled now, start regular operation
        _smoothing_history_next = 0;
        sumResult =  _smoothing_sum / _smoothing_history_size;
      }
    }
    else
    {
      // Smoothing enabled and buffer filled previously.
      // => Regular operation from now on:

      // Replace previous value by the new one (from buffer and sum)
      _smoothing_sum -= _smoothing_history_values[_smoothing_history_next];
      _smoothing_history_values[_smoothing_history_next] = getCurrentLux();
      _smoothing_sum += _smoothing_history_values[_smoothing_history_next];

      // Update next value tu acquire
      _smoothing_history_next = (_smoothing_history_next < _smoothing_history_size - 1) ? _smoothing_history_next + 1 : 0;

      sumResult = _smoothing_sum / _smoothing_history_size;
    }
  }

  return sumResult;
}

float LightDependentResistor::getSmoothedFootCandles()
{
  return luxToFootCandles(getSmoothedLux());
}
