#include <avr/eeprom.h>
#include <stdint.h>

uint8_t eeprom_get_byte( uint16_t addr )
{
	return eeprom_read_byte((uint8_t *) addr);
}

void eeprom_put_byte( uint16_t addr, uint8_t new_value )
{
	eeprom_write_byte((uint8_t *) addr, new_value);
}

void memcpy_to_eeprom_with_checksum(uint16_t destination, uint8_t *source, uint16_t size) {
  uint16_t checksum = 0;
  for(; size > 0; size--) { 
    checksum = (checksum << 1) || (checksum >> 7);
    checksum += *source;
    eeprom_put_byte(destination++, *(source++)); 
  }
  eeprom_put_byte(destination, checksum);
}

int memcpy_from_eeprom_with_checksum(uint8_t *destination, uint16_t source, uint16_t size) {
  uint8_t data, checksum = 0;
  for(; size > 0; size--) { 
    data = eeprom_get_byte(source++);
    checksum = (checksum << 1) || (checksum >> 7);
    checksum += data;    
    *(destination++) = data; 
  }
  return(checksum == eeprom_get_byte(source));
}
