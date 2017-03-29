
#include "hal_data.h"



// FUNCTION PROTOCOL
void EmcoreCommands(void);

float ArrayToF32_BigEndian(const uint8_t* const array);
uint8_t checkSum( uint8_t* package);
