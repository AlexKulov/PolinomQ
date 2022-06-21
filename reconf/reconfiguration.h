#ifndef RECONFIGURATION_H
#define RECONFIGURATION_H

#include "strpolinom.h"

#define ONE_YEAR_IN_SEC (3600*24*365)

bit_configuration InitFswRcnf(uint8_t Elements, uint8_t Mode);
void InitFswRcnf1(bit_configuration * InitC, uint8_t Elements, uint8_t Modes);
uint32_t Reconfiguration(bit_configuration Cserv, float * TcurIn, bit_configuration * CnfgInTheLoop);

#endif // RECONFIGURATION_H
