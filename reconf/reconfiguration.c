#include "reconfiguration.h"
#include <stdio.h>

#define CNFG_ALL_MODE_TEST 0x0f
//#define CNFG_ALL_MODE 0x07

#define ON_ALL_MODE 3

#define ALG_FULL_ENUM 1
#define ALG_RAND_RSCH 2

/* #ifndef NAME_SPACE
    #define STATIC static
#else
    #define STATIC
#endif */

//namespace ValFswRcnf{
   static Polinom scPolinim;
   static BitConfiguration cOn; // now execute  device
   static float tCur[N_EL_MAX];
   static float tMax[N_EL_MAX];
   static uint8_t numOnboardDevice, numMode;
   static uint8_t exctMode;
   static long sumExctMode;
   static long flagAlgReCnfg;
   static long maxCnfgRnd;
   static uint32_t maxInAllCnfg;
   static BitConfiguration allCnfg[N_STR_MAX];
   static BitConfiguration manyCnfg[N_STR_MAX];
   static BitConfiguration manyCnfgForMode[ON_ALL_MODE][N_STR_MAX];
//}

void formExctMode(BitConfiguration servCnfg)
{
    BitConfiguration cnfgOneMode;
    copy(cnfgOneMode, servCnfg);
    uint8_t iMode=0;
    for(iMode=numOnboardDevice+1;iMode<=(numOnboardDevice+numMode);iMode++){
        wBit(cnfgOneMode,iMode,ENBL);
    }
    SmbConfiguration smbCnfgOneMode;
    sumExctMode = 0;
    for (iMode=numOnboardDevice+1;iMode<=(numOnboardDevice+numMode);iMode++){
        wBit(cnfgOneMode,iMode,DISBL);
        bitToSmb2(cnfgOneMode,smbCnfgOneMode);
        if(culcP(smbCnfgOneMode)>0.0f){
            wBit(&exctMode,iMode-numOnboardDevice,ENBL);
            sumExctMode++;
        }
        else
            wBit(&exctMode,iMode-numOnboardDevice,DISBL);
        wBit(cnfgOneMode,iMode,ENBL);
    }
}

uint32_t randGnrtCnfg(BitConfiguration servCnfg){

    float testP=0.f;
    SmbConfiguration smbCnfgStart;
    bitToSmb(servCnfg,smbCnfgStart);
    uint8_t iMode=0;
    for(iMode=numOnboardDevice+1;iMode<=numOnboardDevice+numMode;iMode++)
        if(ENBL == rBit(&exctMode,iMode-numOnboardDevice)){
            wSmb(smbCnfgStart,iMode,DOWN);
            wBit(servCnfg,iMode,DISBL); //27.02.2022
        }
        else{ //23.03.2022
            wSmb(smbCnfgStart,iMode,UP); //23.03.2022
            wBit(servCnfg,iMode,ENBL); //23.03.2022
        }

    float vecKsi[N_EL_MAX]; //N_EL_MAX
    uint8_t stepChoice = 0;
    for(uint16_t iCnfg=0;iCnfg<maxCnfgRnd;iCnfg++){
        nullBitCnfg(manyCnfg[iCnfg]);
    }

    uint32_t resNumCnfg=0;
    for(uint8_t iCnfg=0;iCnfg<maxCnfgRnd;iCnfg++){
        for(uint8_t step=0;step<numOnboardDevice+numMode;step++){
            culcAllKsi(smbCnfgStart, vecKsi, FULL);
            //------------------- Выбираем элемент ---------------------------------
            stepChoice = findRandKsi(vecKsi, numOnboardDevice);// начиная с 1;
            wBit(manyCnfg[iCnfg], stepChoice, ENBL);
        //----------- Поднимаем флаг как UP и выбираем новый элемент
          wSmb(smbCnfgStart, stepChoice, UP);
        // если выбранный элемент позволил включить систему, то выход
          testP = culcP(smbCnfgStart);
          if(equl (testP,1.0f)){
              resNumCnfg++;
              break;
              // step; количество элементов вкл в конфигурацию
          }
        }//generate single configuration cicle
        bitToSmb(servCnfg,smbCnfgStart);
        for(iMode=numOnboardDevice+1;iMode<=numOnboardDevice+numMode;iMode++)
            if(ENBL == rBit(&exctMode,iMode-numOnboardDevice))
                wSmb(smbCnfgStart,iMode,DOWN);
            else
                wSmb(smbCnfgStart,iMode,UP);
    }//generate configurations cicle
    return resNumCnfg;
}

void choiseCnfgFromAll(BitConfiguration servCnfg, uint32_t * numCnfg){
    uint8_t iMode=0;
    for(iMode=numOnboardDevice+1;iMode<=numOnboardDevice+numMode;iMode++){
        //if(ENBL == r_bit(&ExctMode,iMode-numOnboardDevice))
            wBit(servCnfg,iMode,DISBL); //ENBL 27.02.2022
            numCnfg[iMode-numOnboardDevice-1]=0;
    }
    //unsigned int ResNumCnfg = 0;
    uint32_t iCnfg = 0;
    uint8_t iBit = 0;
    _Bool FlagCnfg = TRUE;
    for(iMode=0;iMode<numMode;iMode++){//iMode=numOnboardDevice+1;iMode<=numOnboardDevice+NumMode;iMode++
        if(ENBL == rBit(&exctMode,iMode+1)){
            wBit(servCnfg,iMode+numOnboardDevice+1,ENBL);
            for(iCnfg=0;iCnfg<maxInAllCnfg;iCnfg++) {
                for(iBit=1;iBit<=numOnboardDevice+numMode;iBit++){
                    if(DOWN == rBit(servCnfg,iBit) &&
                       ENBL == rBit(allCnfg[iCnfg],iBit)){
                         FlagCnfg = FALSE;
                         break;
                    }
                }
                if(FlagCnfg){
                    copy(manyCnfgForMode[iMode][numCnfg[iMode]] , allCnfg[iCnfg] );
                    numCnfg[iMode]++;
                }
                FlagCnfg = TRUE;
            }
            wBit(servCnfg,iMode,DISBL);
        }
    }

    for(iMode=numOnboardDevice+1;iMode<=numOnboardDevice+numMode;iMode++){
        if(ENBL == rBit(&exctMode,iMode-numOnboardDevice))
            wBit(servCnfg,iMode,ENBL);
    }
   // return ResNumCnfg;
}

void cnfgEqulTimeRand(uint32_t NumCnfg){

    float MaxValIndex = 0;
    float CurValIndex = 0;
    uint32_t ChoiseCnfg = N_STR_MAX;
    uint32_t iCnfg=0;
    uint8_t iBit=0;
    uint8_t SummEnbl=0;
    float ci = 0;
    for(iCnfg=0;iCnfg<NumCnfg;iCnfg++){
        CurValIndex = 0;
        SummEnbl=0;
        for(iBit=1;iBit<=numOnboardDevice;iBit++){ // numOnboardDevice + ??? NumMode
            if(ENBL == rBit(manyCnfg[iCnfg] ,iBit)){
                ci = (tMax[iBit-1] - tCur[iBit-1])/tMax[iBit-1];
                CurValIndex = CurValIndex + ci;
                SummEnbl++;
            }
        }
        CurValIndex = CurValIndex/SummEnbl;
        if(CurValIndex>MaxValIndex){
            MaxValIndex = CurValIndex;
            ChoiseCnfg = iCnfg;
        }
    }
    if(N_STR_MAX == ChoiseCnfg)
        printf("\n Erroe:CnfgEqulTime()");
    else
        copy(cOn ,manyCnfg[ChoiseCnfg] ); // конфигурация сразу для всех исп. режимов
}

void cnfgEqulTimeMode(uint32_t * NumCnfg){

    float MaxValIndex = 0;
    float CurValIndex = 0;
    uint32_t ChoiseCnfg[ON_ALL_MODE];
    uint32_t iCnfg=0;
    uint8_t iBit=0;
    uint8_t SummEnbl=0;
    float ci = 0;
    uint8_t iMode=0;
    for(iMode=0;iMode<numMode;iMode++){
        ChoiseCnfg[iMode] = N_STR_MAX;
        if(ENBL == rBit(&exctMode,iMode+1)){
            for(iCnfg=0;iCnfg<NumCnfg[iMode];iCnfg++){
                CurValIndex = 0;
                SummEnbl=0;
                for(iBit=1;iBit<=numOnboardDevice;iBit++){
                    if(ENBL == rBit(manyCnfgForMode[iMode][iCnfg] ,iBit)){
                        ci = (tMax[iBit-1] - tCur[iBit-1])/tMax[iBit-1];
                        CurValIndex = CurValIndex + ci;
                        SummEnbl++;
                    }
                }
                CurValIndex = CurValIndex/SummEnbl;
                if(CurValIndex>MaxValIndex){
                    MaxValIndex = CurValIndex;
                    ChoiseCnfg[iMode] = iCnfg;
                }
            }
            MaxValIndex = 0;
        }
    }

    _Bool FlagErr = TRUE;
    for(iMode=0;iMode<numMode;iMode++)
            if(N_STR_MAX > ChoiseCnfg[iMode])
                FlagErr = FALSE;
    if(TRUE==FlagErr)
            printf("Erroe:CnfgEqulTimeMode()");

    for(iMode=0;iMode<numMode;iMode++){
        if(N_STR_MAX != ChoiseCnfg[iMode]){
            copy(cOn ,manyCnfgForMode[iMode][ChoiseCnfg[iMode]] );
            break;
        }
    }
    iMode++;
    for(       ;iMode<numMode;iMode++){
            if(N_STR_MAX != ChoiseCnfg[iMode]){
                wBit(cOn ,iMode+numOnboardDevice+1,ENBL);
                for(iBit=1;iBit<=numOnboardDevice;iBit++){
                    if(DISBL == rBit(cOn ,iBit) &&
                       ENBL == rBit(manyCnfgForMode[iMode][ChoiseCnfg[iMode]] ,iBit) )
                        wBit(cOn ,iBit,ENBL);
                }
            }
    }

    SmbConfiguration SmbCnfgOn;
    bitToSmb(cOn ,SmbCnfgOn );
    for(iBit=1;iBit<=numOnboardDevice;iBit++){
          wSmb(SmbCnfgOn ,iBit,DOWN);
            if(culcP(SmbCnfgOn )>0.0f){
                wBit(cOn ,iBit,DOWN);
            }
          bitToSmb(cOn ,SmbCnfgOn );
    }
}

void formCnfgOn(BitConfiguration cServ){

    if(ALG_RAND_RSCH == flagAlgReCnfg){
      uint32_t NumCnfgRand = 0;
      NumCnfgRand = randGnrtCnfg(cServ);
      cnfgEqulTimeRand(NumCnfgRand);
    }else if (ALG_FULL_ENUM == flagAlgReCnfg){
      uint32_t NumCnfgMode[ON_ALL_MODE];
      choiseCnfgFromAll(cServ, NumCnfgMode);
      cnfgEqulTimeMode(NumCnfgMode);
    }
}

void includeModes(BitConfiguration * cnfg){
    uint8_t iMode=0;
    for(iMode=numOnboardDevice+1;iMode<=numOnboardDevice+numMode;iMode++)
        if(ENBL == rBit(&exctMode,iMode-numOnboardDevice))
            wBit(*cnfg,iMode,ENBL);
        else
            wBit(*cnfg,iMode,DISBL);
}

void initFswRcnf1(BitConfiguration * initC, uint8_t Elements, uint8_t Modes)
{
    wByte(&exctMode,CNFG_ALL_MODE_TEST);
    numOnboardDevice = Elements; //4Gyro+2Fss+2St+2Mag+4Whl+3Mtb = 17
    numMode = Modes; //LVHL, SunOriuntation, ProgrammReturn = 3
    sumExctMode = numMode;
    maxCnfgRnd = 20; //пусть будет столько пока 3.03.2022

    char NamePolinomFile[128] = "polynom/PolEk.txt";
    // char NamePolinomFile[128] = "SimplePol.txt";
    loadPlnmFromFile(NamePolinomFile,&scPolinim);
    setLibPolinom(&scPolinim);
    flagAlgReCnfg = ALG_FULL_ENUM;
    if(ALG_FULL_ENUM == flagAlgReCnfg){
        maxInAllCnfg = findConfiguration(&scPolinim, allCnfg, numOnboardDevice, numMode);
        for(uint32_t i=0;i<maxInAllCnfg;i++){
                        printf("Cnfg No(%i) : ",i+1);
                        for(uint8_t j=1;j<=numOnboardDevice+numMode;j++){
                                if(rBit(allCnfg[i] ,j))
                                    printf("P%i ",j);
                        }
                        printf("\n");
        }
    }

    flagAlgReCnfg = ALG_RAND_RSCH;
    BitConfiguration servCnfg;//serviceable device
    setCnfg(servCnfg,ENBL);//for Initial
    uint8_t i=0;
    for (i=0;i<Elements;i++){
        tCur[i] = 0.0;
        tMax[i] = 200.0; //tDevmax[i] - можно подать на вход
    }
    formCnfgOn(servCnfg);
    includeModes(&cOn);
    //(*initC) = cOn;
    copy(*initC,cOn);
}

BitConfiguration * initFswRcnf(char * namePolinomFile, uint8_t elements, uint8_t modes)
{
    wByte(&exctMode,CNFG_ALL_MODE_TEST);
    numOnboardDevice = elements; //4Gyro+2Fss+2St+2Mag+4Whl+3Mtb = 17
    numMode = modes; //LVHL, SunOriuntation, ProgrammReturn = 3
    sumExctMode = numMode;
    maxCnfgRnd = 10; //пусть будет столько пока 3.03.2022

    //char NamePolinomFile[128] = "polynom/PolEk.txt";

    loadPlnmFromFile(namePolinomFile,&scPolinim);
    setLibPolinom(&scPolinim);
    flagAlgReCnfg = ALG_FULL_ENUM;
    if(ALG_FULL_ENUM == flagAlgReCnfg){
        maxInAllCnfg = findConfiguration(&scPolinim, allCnfg, numOnboardDevice, numMode);
        for(uint32_t i=0;i<maxInAllCnfg;i++){
                        printf("Cnfg No(%i) : ",i+1);
                        for(uint8_t j=1;j<=numOnboardDevice+numMode;j++){
                                if(rBit(allCnfg[i] ,j))
                                    printf("P%i ",j);
                        }
                        printf("\n");
        }
    }

    flagAlgReCnfg = ALG_RAND_RSCH;
    BitConfiguration servCnfg;//serviceable device
    setCnfg(servCnfg,ENBL);//for Initial
    uint8_t i=0;
    for (i=0;i<elements;i++){
        tCur[i] = 0.0;
        tMax[i] = ONE_YEAR_IN_SEC; //tDevmax[i] - можно подать на вход
    }
    formCnfgOn(servCnfg);
    includeModes(&cOn);
    return &cOn;
}

uint32_t reconfiguration(BitConfiguration servCnfg, float * tCurIn, BitConfiguration * cnfgInTheLoop)
{
    formExctMode(servCnfg);
    if(sumExctMode>0){
        uint8_t i;
        for (i=0;i<numOnboardDevice;i++)
            tCur[i] = tCurIn[i];
        formCnfgOn(servCnfg);
    }
    else
        return 0;

    copy(*cnfgInTheLoop, cOn);
    includeModes(cnfgInTheLoop);
    uint32_t res = (uint32_t)sumExctMode;
    return res;
}
