/*
  The most complete library for analog Keypad.
  Can accept up to 25 keys, but you can change the value in the library at line 12.
*/

#ifndef _ANALOGKEYPAD_H_
#define _ANALOGKEYPAD_H_

#include "Arduino.h"

/* Limit of 25 keys, equivalent to a 5x5 keyboard */
#define MAX_KEYS 25 

/* Save error if name is empty */
#define ERR_NAME_EMPTY 0b000
/* Save error if value is forbidden */
#define ERR_FORBIDDEN_VALUE 0b001
/* Save error if lists are not the same length */
#define ERR_NOT_SAME_LENGTH 0b010
/* Save error if list size exceeded */
#define ERR_SIZE_EXCEEDED 0b011
/* Registered successfully */
#define REGISTERED 0b100


class AnalogKeypad {
  public:
    /*
      @brief The constructor
      
      @param pin The keypad analog pin
      @param noPressed [optional] The default value if no key was pressed
    */
    AnalogKeypad(uint8_t pin) { AnalogKeypad(pin, 0); }
    AnalogKeypad(uint8_t pin, uint16_t noPressed) {
      _pin = pin;
      _noPressed = noPressed;
      _mapSize = 0;
    }

    /*
      @brief Add a key in the list
      
      @param name The name of the key (not necessarily a letter)
      @param value The value of the key

      @return ERR_NAME_EMPTY if the name of the key is empty, ERR_FORBIDDEN_VALUE if the value is same as noPressedValue, ERR_SIZE_EXCEEDED if the list size was exceeded or REGISTERED if there is no verification error
    */
    uint8_t registerKey(const String name, uint16_t value) {
      if (name == "") return ERR_NAME_EMPTY;
      if (value == 0 || value >= 1023) return ERR_FORBIDDEN_VALUE;
      if (_mapSize > MAX_KEYS) return ERR_SIZE_EXCEEDED;

      _names[_mapSize] = name;
      _values[_mapSize] = value;
      _mapSize++;
      
      return REGISTERED;
    }

    /*
      @brief Same as registerKey() but for register a list of keys
      
      @param names The names list of keys (not necessarily a letter)
      @param values The values list of keys

      @return Same errors at registerKey() but if names list and values list are not the same length, return ERR_NOT_SAME_LENGTH
    */
    uint8_t registerKeys(const String names[], const uint16_t values[]) {
      uint8_t listSize = sizeof(names)/sizeof(String);
      uint8_t error;
      if (listSize != sizeof(values)/sizeof(uint16_t)) return ERR_NOT_SAME_LENGTH;

      for (_i=0; _i<listSize; _i++) {
        error = registerKey(names[_i], values[_i]);
        if (error != REGISTERED) return error;
      }
      
      return REGISTERED;
    }

    /*
      @brief Test is the specified key name is registered

      @return If names list contains the key name
    */
    bool isRegistered(const String name) {
      for (_i=0; _i<_mapSize; _i++) {
        if (_names[_i] == name) return true;
      }

      return false;
    }

    /*
      @brief Test is the specified key value has a name

      @return If values list contains the key value
    */
    bool isRegistered(uint16_t value) {
      for (_i=0; _i<_mapSize; _i++) {
        if (_values[_i] == value) return true;
      }

      return false;
    }
    
    /*
      @brief Get the analog value of the pin and return his name

      @return an empty string if no key was pressed or value not registered in the list
    */
    String getPressed() {
      _key = analogRead(_pin);

      if (_key == 0 || _key >= 1023) return "";
      for (_i=0; _i<_mapSize; _i++) {
        if (_values[_i] == _key) return _names[_i];
      }

      return "";
    }    

    /*
      @brief Return if the specified key is pressed
    */
    bool isPressed(const String key) {
      return key != "" && getPressed() == key;
    }

    /*
      @brief Wait until the last saved value change
    */
    void waitChange() {
      while (_key == analogRead(_pin));
    }

    /*
      @brief Wait until the last saved key was unpressed
    */
    void waitUnpressed() {
      while (getPressed() != "");
    }
    
    /*
      @brief Wait until one key was pressed

      @return The pressed key name
    */
    String waitPressed() {
      do { _lastKey = getPressed(); } 
      while (_lastKey == "");
      
      return _lastKey;
    }

    /*
      @brief Wait until the specified key was pressed
    */
    void waitPressed(const String key) {
      while (!isPressed(key));
    }

    /*
      @brief Return the names list
    */
    String* keyNames() {
      return _names;
    }
   
    /*
      @brief Return the values list
    */
    uint16_t* keyValues() {
      return _values;
    }
    

  private:
    String _names[MAX_KEYS];
    uint16_t _values[MAX_KEYS];
    String _lastKey;
    uint16_t _noPressed;
    uint8_t _pin,
            _mapSize,
            _key,
            _i;
};

#endif // _ANALOGKEYPAD_H_
