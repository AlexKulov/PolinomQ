#ifndef RECONFIGURATION_H
#define RECONFIGURATION_H

#include "strpolinom.h"

#define ONE_YEAR_IN_SEC (3600*24*365)

BitConfiguration * initFswRcnf(char * namePolinomFile, uint8_t elements, uint8_t modes);
void initFswRcnf1(BitConfiguration * initCnfg, uint8_t elements, uint8_t modes);
uint32_t reconfiguration(BitConfiguration servCnfg, float * tCurIn, BitConfiguration * cnfgInTheLoop);

#endif // RECONFIGURATION_H
