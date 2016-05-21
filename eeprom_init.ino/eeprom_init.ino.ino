/* Grallator
 * https://grallator.wordpress.com/2013/12/04/arduino-keypad-entry-system/
 */

#include <EEPROM.h>
/* initialise EEPROM with default code */
void setup()
{
    EEPROM.write(0,'1');
    EEPROM.write(1,'2');
    EEPROM.write(2,'3');
    EEPROM.write(3,'4');
}
void loop()
{
}
