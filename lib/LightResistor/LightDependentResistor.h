/*
 * \brief Get light intensity value (Lux & FootCandles) from Light dependent Resistor (LDR) a.k.a. photocell or photoresistor
 *
 * This library is easily usable with most GL55xx photoresistors (at ~25°C).
 *
 * It is also possible to use it with any other photocell (with the right parameters).
 * If you use this library with other photocells, please send me the parameters in
 * order to add them in the list.
 *
 * Schematics:
 *                           ^
 *            _____      ___/___
 *    5V |---|_____|----|__/____|--| GND
 *    or      Other       /
 *   3.3V    Resistor   Photocell
 *
 * Note: By default, the photocell must be on the ground.
 *       It is possible to exchange the position of the photocell and the other resistor
 *       but you will have to call \p setPhotocellPositionOnGround(false).
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 30 January 2023
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2023 Quentin Comte-Gaz
 * \version 1.4
 */

#ifndef LightDependentResistor_h
#define LightDependentResistor_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

class LightDependentResistor
{
  public:

    /*!
     * \enum ePhotoCellKind Photocell component
     */
    enum ePhotoCellKind {
      GL5516,
      GL5528,
      GL5537_1,
      GL5537_2,
      GL5539,
      GL5549
    };

    /*!
     * \brief LightDependentResistor Initialize the light intensity getter class
     *
     * \param pin (int) Analog pin connected to the voltage divider
     * \param other_resistor (unsigned long) Resistor used for the voltage divider
     * \param kind (ePhotoCellKind) Used photocell
     * \param adc_resolution_bits (unsigned int, optional, default: 10) Number of resolution bits for the ADC pin (more information here: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/)
     * \param smoothing_history_size (unsigned int, optional, default: 10) Max number of raw values used for \f getSmoothedLux or \f getSmoothedFootCandles
     */
    LightDependentResistor(int pin, unsigned long other_resistor, ePhotoCellKind kind = GL5528, unsigned int adc_resolution_bits = 10, unsigned int smoothing_history_size = 10);

    /*!
     * \brief LightDependentResistor Initialize the light intensity getter class
     *
     * Even thought some photocells are already defined, it is possible to
     * define your own photocell.
     * The relation between the photocell resistor and the lux intensity can be
     * approximated to I[lux]=mult_value/(R[Ω]^pow_value).
     *
     * Example for GL5528 photocell:
     *   1) Find curve Resistor->Lux intensity: http://cdn.sparkfun.com/datasheets/Sensors/LightImaging/SEN-09088.pdf
     *   2) Get 2 points from the datasheet log curve: log(55000[Ω])->log(1[lux]) and log(3000[Ω])->log(100[lux])
     *   3) Convert those 2 point into a "log linear curve" (with Excel for example): log(R[Ω]) = -0.6316 * log(I[lux]) + 4.7404 (linear)
     *   4) Solve the equation to get I[lux]=mult_value/(R[Ω]^pow_value) approximation (with wolframalpha for example): I[lux] ~= 32017200/R[Ω]^1.5832
     *      https://www.wolframalpha.com/input/?i=log10(x)%3D-0.6316*log10(y)%2B4.7404
     *   5) You just found the 2 parameters: mult_value=32017200 and pow_value=1.5832
     *
     * \param pin (int) Analog pin connected to the voltage divider
     * \param other_resistor (unsigned long) Resistor used for the voltage divider
     * \param mult_value (float) Multiplication parameter in "I[lux]=mult_value/(R[Ω]^pow_value)" expression
     * \param pow_value (float) Power parameter in "I[lux]=mult_value/(R[Ω]^pow_value)" expression
     * \param adc_resolution_bits (unsigned int, optional, default: 10) Number of resolution bits for the ADC pin (more information here: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/)
     * \param smoothing_history_size (unsigned int, optional, default: 10) Max number of raw values used for \f getSmoothedLux or \f getSmoothedFootCandles
     */
    LightDependentResistor(int pin, unsigned long other_resistor, float mult_value, float pow_value, unsigned int adc_resolution_bits = 10, unsigned int smoothing_history_size = 10);

    ~LightDependentResistor();

    /*!
     * \brief getCurrentLux Get light intensity (in lux) from the photocell
     *
     * \return (float) Light intensity (in lux)
     */
    float getCurrentLux() const;

    /*!
     * \brief getCurrentFootCandles Get light intensity (in footcandles) from the photocell
     *
     * \return (float) Light intensity (in footcandles)
     */
    float getCurrentFootCandles() const;

    /*!
     * \brief getCurrentRawAnalogValue Read light intensity from the photocell, and provide the sensor raw analogic value (not a really readable value)
     *
     * \return (int) Light intensity (in sensor raw analogic value), not a really readable value, prefer \f getCurrentLux or \f getCurrentFootCandles
     */
    int getCurrentRawAnalogValue() const;

    /*!
     * \brief rawAnalogValueToLux Convert raw value from photocell sensor into lux
     *
     *  This function is only needed if the sensor MUST NOT be handled by this library...
     *  Else, it is better to directly use \f getCurrentLux that will read sensor value and convert it into lux.
     *
     * \param raw_value (int) Analog value of the photocell sensor (WARNING: This value must be with the same adc resolution as the one in the constructor)
     *
     * \return (float) Light intensity (in lux)
     */
    float rawAnalogValueToLux(int raw_analog_value) const;

    /*!
     * \brief luxToFootCandles Get footcandles from lux intensity
     *
     * \param intensity_in_lux (float) Intensity in lux
     *
     * \return Footcandles retrieved from \p intensity_in_lux
     */
    static float luxToFootCandles(float intensity_in_lux);

    /*!
     * \brief footCandlesToLux Get Lux intensity from footcandles
     *
     * \param intensity_in_footcandles (float) Footcandles
     *
     * \return Intensity in lux retrieved from \p intensity_in_footcandles
     */
    static float footCandlesToLux(float intensity_in_footcandles);

    /*!
     * \brief setPhotocellPositionOnGround Configure the photocell as connected to +5V/3.3V or GND
     *
     * \param on_ground (bool) True if the photocell is connected to GND, else false
     *
     *  True:                    ^
     *            _____      ___/___
     *    5V |---|_____|----|__/____|--| GND
     *    or      Other       /
     *   3.3V    Resistor   Photocell
     *
     *  False:                    ^
     *             _____      ___/___
     *    GND |---|_____|----|__/____|--| 5V
     *            Other        /          or
     *           Resistor   Photocell    3.3V
     */
    void setPhotocellPositionOnGround(bool on_ground);

    /*!
     * \brief updatePhotocellParameters Redefine the photocell parameters
     *
     * \param mult_value (float) Multiplication parameter in "I[lux]=mult_value/(R[Ω]^pow_value)" expression
     * \param pow_value (float) Power parameter in "I[lux]=mult_value/(R[Ω]^pow_value)" expression
     */
    void updatePhotocellParameters(float mult_value, float pow_value);

    /*!
     * \brief getSmoothedLux Read light intensity (in lux) from the photocell, apply linear smoothing using the number of historic values specified with the constructor.
     *
     * \return (float) Light intensity (in lux) after applying linear smoothing
     */
    float getSmoothedLux();

    /*!
     * \brief getCurrentFootCandles Read light intensity from the photocell, apply linear smoothing using the number of historic values specified with the constructor, convert to footcandles.
     *
     * \return (float) Light intensity (in footcandles) after applying linear smoothing
     */
    float getSmoothedFootCandles();

  private:
    int _pin; //!< Analog pin connected to the voltage divider
    unsigned long _other_resistor; //!< Resistor used for the voltage divider
    float _mult_value; //!< Multiplication parameter in "I[lux]=mult_value/(R[Ω]^pow_value)" expression
    float _pow_value; //!< Power parameter in "I[lux]=mult_value/(R[Ω]^pow_value)" expression
    bool _photocell_on_ground; //!< Photocell is connected to +5V/3.3V (false) or GND (true) ?
    unsigned int _adc_resolution_bits; //!< Number of resolution bits for the ADC pin
    float _smoothing_sum; //!< (smoothing only) Current sum of valid values of \v _smoothing_history_values
    unsigned int _smoothing_history_size; //!< (smoothing only) Size of the table of values
    unsigned int _smoothing_history_next; //!< (smoothing only) Next value to get/replace
    float* _smoothing_history_values; //!< (smoothing only) All valid values (in lux) in a table of \v _smoothing_history_size values maximum (oldest value will be replaced by a new one if table is full)
};

#endif //LightDependentResistor_h
