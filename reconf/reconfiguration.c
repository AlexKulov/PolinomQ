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
   BitConfiguration allCnfg[N_STR_MAX];
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
        BitToSmb2(cnfgOneMode,smbCnfgOneMode);
        if(culcP(smbCnfgOneMode)>0.0f){
            wBit(&exctMode,iMode-numOnboardDevice,ENBL);
            sumExctMode++;
        }
        else
            wBit(&exctMode,iMode-numOnboardDevice,DISBL);
        wBit(cnfgOneMode.adr,iMode,ENBL);
    }
}

uint32_t RandGnrtCnfg(BitConfiguration servCnfg){

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
        nullBitCnfg(&manyCnfg[iCnfg]);
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
              ResNumCnfg++;
              break;
              // step; количество элементов вкл в конфигурацию
          }
        }//generate single configuration cicle
        bitToSmb(servCnfg,smbCnfgStart);
        for(iMode=numOnboardDevice+1;iMode<=numOnboardDevice+numMode;iMode++)
            if(ENBL == r_bit(&exctMode,iMode-numOnboardDevice))
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
            wBit(Cserv.adr,iMode,DISBL); //ENBL 27.02.2022
            NumCnfg[iMode-numOnboardDevice-1]=0;
    }
    //unsigned int ResNumCnfg = 0;
    uint32_t iCnfg = 0;
    uint8_t iBit = 0;
    _Bool FlagCnfg = TRUE;
    for(iMode=0;iMode<numMode;iMode++){//iMode=numOnboardDevice+1;iMode<=numOnboardDevice+NumMode;iMode++
        if(ENBL == rBit(&ExctMode,iMode+1)){
            wBit(servCnfg,iMode+numOnboardDevice+1,ENBL);
            for(iCnfg=0;iCnfg<MaxInAllCnfg;iCnfg++) {
                for(iBit=1;iBit<=numOnboardDevice+NumMode;iBit++){
                    if(DOWN == rBit(servCnfg,iBit) &&
                       ENBL == rBit(AllCnfg[iCnfg].adr,iBit)){
                         FlagCnfg = FALSE;
                         break;
                    }
                }
                if(FlagCnfg){
                    copy(ManyCnfgForMode[iMode][NumCnfg[iMode]].adr, AllCnfg[iCnfg].adr);
                    NumCnfg[iMode]++;
                }
                FlagCnfg = TRUE;
            }
            wBit(Cserv.adr,iMode,DISBL);
        }
    }

    for(iMode=numOnboardDevice+1;iMode<=numOnboardDevice+NumMode;iMode++){
        if(ENBL == rBit(&exctMode,iMode-numOnboardDevice))
            wBit(Cserv.adr,iMode,ENBL);
    }
   // return ResNumCnfg;
}

void CnfgEqulTimeRand(uint32_t NumCnfg){

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
            if(ENBL == rBit(ManyCnfg[iCnfg].adr,iBit)){
                ci = (Tmax[iBit-1] - Tcur[iBit-1])/Tmax[iBit-1];
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
        copy(Con.adr,ManyCnfg[ChoiseCnfg].adr); // конфигурация сразу для всех исп. режимов
}

void CnfgEqulTimeMode(uint32_t * NumCnfg){

    float MaxValIndex = 0;
    float CurValIndex = 0;
    uint32_t ChoiseCnfg[ON_ALL_MODE];
    uint32_t iCnfg=0;
    uint8_t iBit=0;
    uint8_t SummEnbl=0;
    float ci = 0;
    uint8_t iMode=0;
    for(iMode=0;iMode<NumMode;iMode++){
        ChoiseCnfg[iMode] = N_STR_MAX;
        if(ENBL == rBit(&exctMode,iMode+1)){
            for(iCnfg=0;iCnfg<NumCnfg[iMode];iCnfg++){
                CurValIndex = 0;
                SummEnbl=0;
                for(iBit=1;iBit<=numOnboardDevice;iBit++){
                    if(ENBL == rBit(ManyCnfgForMode[iMode][iCnfg].adr,iBit)){
                        ci = (Tmax[iBit-1] - Tcur[iBit-1])/Tmax[iBit-1];
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
    for(iMode=0;iMode<NumMode;iMode++)
            if(N_STR_MAX > ChoiseCnfg[iMode])
                FlagErr = FALSE;
    if(TRUE==FlagErr)
            printf("Erroe:CnfgEqulTimeMode()");

    for(iMode=0;iMode<NumMode;iMode++){
        if(N_STR_MAX != ChoiseCnfg[iMode]){
            copy(Con.adr,ManyCnfgForMode[iMode][ChoiseCnfg[iMode]].adr);
            break;
        }
    }
    iMode++;
    for(       ;iMode<NumMode;iMode++){
            if(N_STR_MAX != ChoiseCnfg[iMode]){
                wBit(Con.adr,iMode+numOnboardDevice+1,ENBL);
                for(iBit=1;iBit<=numOnboardDevice;iBit++){
                    if(DISBL == rBit(Con.adr,iBit) &&
                       ENBL == rBit(ManyCnfgForMode[iMode][ChoiseCnfg[iMode]].adr,iBit) )
                        wBit(Con.adr,iBit,ENBL);
                }
            }
    }

    smb_configuration SmbCnfgOn;
    bit_to_smb(Con.adr,SmbCnfgOn.adr);
    for(iBit=1;iBit<=numOnboardDevice;iBit++){
          w_smb(SmbCnfgOn.adr,iBit,DOWN);
            if(culc_p(SmbCnfgOn.adr)>0.0f){
                wBit(Con.adr,iBit,DOWN);
            }
          bit_to_smb(Con.adr,SmbCnfgOn.adr);
    }
}

void FormCnfgOn(bit_configuration Cserv){

    if(ALG_RAND_RSCH == flagAlgReCnfg){
      uint32_t NumCnfgRand = 0;
      NumCnfgRand = RandGnrtCnfg(Cserv);
      CnfgEqulTimeRand(NumCnfgRand);
    }else if (ALG_FULL_ENUM == flagAlgReCnfg){
      uint32_t NumCnfgMode[ON_ALL_MODE];
      ChoiseCnfgFromAll(Cserv, NumCnfgMode);
      CnfgEqulTimeMode(NumCnfgMode);
    }
}

void IncludeModes(bit_configuration * Cnfg){
    uint8_t iMode=0;
    for(iMode=numOnboardDevice+1;iMode<=numOnboardDevice+NumMode;iMode++)
        if(ENBL == rBit(&exctMode,iMode-numOnboardDevice))
            wBit(Cnfg->adr,iMode,ENBL);
        else
            wBit(Cnfg->adr,iMode,DISBL);
}

void InitFswRcnf1(bit_configuration * InitC, uint8_t Elements, uint8_t Modes)
{
    wByte(&exctMode,CNFG_ALL_MODE_TEST);
    numOnboardDevice = Elements; //4Gyro+2Fss+2St+2Mag+4Whl+3Mtb = 17
    NumMode = Modes; //LVHL, SunOriuntation, ProgrammReturn = 3
    SumExctMode = NumMode;
    MaxCnfgRnd = 20; //пусть будет столько пока 3.03.2022

    char NamePolinomFile[128] = "polynom/PolEk.txt";
    // char NamePolinomFile[128] = "SimplePol.txt";
    LoadPlnmFromFile(NamePolinomFile,&ScPolinim);
    PolinomInit(&ScPolinim);
    flagAlgReCnfg = ALG_FULL_ENUM;
    if(ALG_FULL_ENUM == flagAlgReCnfg){
        MaxInAllCnfg = FindConfiguration(&ScPolinim, AllCnfg, numOnboardDevice, NumMode);
        for(uint32_t i=0;i<MaxInAllCnfg;i++){
                        printf("Cnfg No(%i) : ",i+1);
                        for(uint8_t j=1;j<=numOnboardDevice+NumMode;j++){
                                if(rBit(AllCnfg[i].adr,j))
                                    printf("P%i ",j);
                        }
                        printf("\n");
        }
    }

    flagAlgReCnfg = ALG_RAND_RSCH;
    bit_configuration Cserv;//serviceable device
    SetCnfg(Cserv.adr,ENBL);//for Initial
    uint8_t i=0;
    for (i=0;i<Elements;i++){
        Tcur[i] = 0.0;
        Tmax[i] = 200.0; //tDevmax[i] - можно подать на вход
    }
    FormCnfgOn(Cserv);
    IncludeModes(&Con);
    (*InitC) = Con;
    //return Con;
}

bit_configuration InitFswRcnf(uint8_t Elements, uint8_t Modes)
{
    wByte(&ExctMode,CNFG_ALL_MODE_TEST);
    numOnboardDevice = Elements; //4Gyro+2Fss+2St+2Mag+4Whl+3Mtb = 17
    NumMode = Modes; //LVHL, SunOriuntation, ProgrammReturn = 3
    SumExctMode = NumMode;
    MaxCnfgRnd = 10; //пусть будет столько пока 3.03.2022

    char NamePolinomFile[128] = "polynom/PolEk.txt";
    // char NamePolinomFile[128] = "SimplePol.txt";
    LoadPlnmFromFile(NamePolinomFile,&ScPolinim);
    PolinomInit(&ScPolinim);
    flagAlgReCnfg = ALG_FULL_ENUM;
    if(ALG_FULL_ENUM == flagAlgReCnfg){
        MaxInAllCnfg = FindConfiguration(&ScPolinim, AllCnfg, numOnboardDevice, NumMode);
        for(uint32_t i=0;i<MaxInAllCnfg;i++){
                        printf("Cnfg No(%i) : ",i+1);
                        for(uint8_t j=1;j<=numOnboardDevice+NumMode;j++){
                                if(rBit(AllCnfg[i].adr,j))
                                    printf("P%i ",j);
                        }
                        printf("\n");
        }
    }

    flagAlgReCnfg = ALG_RAND_RSCH;
    bit_configuration Cserv;//serviceable device
    SetCnfg(Cserv.adr,ENBL);//for Initial
    uint8_t i=0;
    for (i=0;i<Elements;i++){
        Tcur[i] = 0.0;
        Tmax[i] = ONE_YEAR_IN_SEC; //tDevmax[i] - можно подать на вход
    }
    FormCnfgOn(Cserv);
    IncludeModes(&Con);
    return Con;
}

uint32_t Reconfiguration(bit_configuration Cserv, float * TcurIn, bit_configuration * CnfgInTheLoop)
{
    FormExctMode(Cserv);
    if(sumExctMode>0){
        uint8_t i;
        for (i=0;i<numOnboardDevice;i++)
            Tcur[i] = TcurIn[i];
        FormCnfgOn(Cserv);
    }
    else
        return 0;

    copy(CnfgInTheLoop->adr, Con.adr);
    IncludeModes(CnfgInTheLoop);
    uint32_t res = (uint32_t)sumExctMode;
    return res;
}
