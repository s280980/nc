#ifndef eeprom_h
#include <stdint.h>


uint8_t eeprom_get_byte(uint16_t addr);
void eeprom_put_byte(uint16_t addr, uint8_t new_value);
void memcpy_to_eeprom_with_checksum(uint16_t destination, uint8_t *source, uint16_t size);
int memcpy_from_eeprom_with_checksum(uint8_t *destination, uint16_t source, uint16_t size);


#define eeprom_h
#endif
