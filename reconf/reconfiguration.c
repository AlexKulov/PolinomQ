#include <stdio.h>
#include "reconfiguration.h"

#define CNFG_ALL_MODE_TEST 0x0f
//#define CNFG_ALL_MODE 0x07

#define ON_ALL_MODE 3

#define ALG_FULL_ENUM 1
#define ALG_RAND_RSCH 2

#ifndef NAME_SPACE
    #define STATIC static
#else
    #define STATIC
#endif

//namespace ValFswRcnf{
   STATIC polinom ScPolinim;
   STATIC bit_configuration Con; // now execute  device
   STATIC float Tcur[N_EL_MAX];
   STATIC float Tmax[N_EL_MAX];
   STATIC uint8_t NumOnboardDevice, NumMode;
   STATIC uint8_t ExctMode;
   STATIC long SumExctMode;
   STATIC long FlagAlgReCnfg;
   STATIC long MaxCnfgRnd;
   STATIC uint32_t MaxInAllCnfg;
   bit_configuration AllCnfg[N_STR_MAX];
   STATIC bit_configuration ManyCnfg[N_STR_MAX];
   STATIC bit_configuration ManyCnfgForMode[ON_ALL_MODE][N_STR_MAX];
//}

void FormExctMode(bit_configuration Cserv)
{
    bit_configuration CnfgOneMode;
    copy(CnfgOneMode.adr, Cserv.adr);
    uint8_t iMode=0;
    for(iMode=NumOnboardDevice+1;iMode<=(NumOnboardDevice+NumMode);iMode++){
        w_bit(CnfgOneMode.adr,iMode,ENBL);
    }
    smb_configuration SmbCnfgOneMode;
    SumExctMode = 0;
    for (iMode=NumOnboardDevice+1;iMode<=(NumOnboardDevice+NumMode);iMode++){
        w_bit(CnfgOneMode.adr,iMode,DISBL);
        BitToSmb2(CnfgOneMode.adr,SmbCnfgOneMode.adr);
        if(culc_p(SmbCnfgOneMode.adr)>0.0f){
            w_bit(&ExctMode,iMode-NumOnboardDevice,ENBL);
            SumExctMode++;
        }
        else
            w_bit(&ExctMode,iMode-NumOnboardDevice,DISBL);
        w_bit(CnfgOneMode.adr,iMode,ENBL);
    }
}

uint32_t RandGnrtCnfg(bit_configuration Cserv){

    float testP=0.f;
    smb_configuration SmbCnfgStart;
    bit_to_smb(Cserv.adr,SmbCnfgStart.adr);
    uint8_t iMode=0;
    for(iMode=NumOnboardDevice+1;iMode<=NumOnboardDevice+NumMode;iMode++)
        if(ENBL == r_bit(&ExctMode,iMode-NumOnboardDevice)){
            w_smb(SmbCnfgStart.adr,iMode,DOWN);
            w_bit(Cserv.adr,iMode,DISBL); //27.02.2022
        }
        else{ //23.03.2022
            w_smb(SmbCnfgStart.adr,iMode,UP); //23.03.2022
            w_bit(Cserv.adr,iMode,ENBL); //23.03.2022
        }

    float VecKsi[N_EL_MAX]; //N_EL_MAX
    uint8_t step_choice = 0;
    for(uint16_t iCnfg=0;iCnfg<MaxCnfgRnd;iCnfg++){
        null_bit_cnfg(&ManyCnfg[iCnfg]);
    }

    uint32_t ResNumCnfg=0;
    for(uint8_t iCnfg=0;iCnfg<MaxCnfgRnd;iCnfg++){
        for(uint8_t step=0;step<NumOnboardDevice+NumMode;step++)
        {
          culc_all_ksi(SmbCnfgStart.adr, VecKsi, FULL);
          //------------------- Выбираем элемент ---------------------------------
          step_choice = FindRandKsi(VecKsi, NumOnboardDevice);// начиная с 1;
          w_bit(ManyCnfg[iCnfg].adr, step_choice, ENBL);
        //----------- Поднимаем флаг как UP и выбираем новый элемент
          w_smb(SmbCnfgStart.adr, step_choice, UP);
        // если выбранный элемент позволил включить систему, то выход
          testP = culc_p(SmbCnfgStart.adr);
          if(equl (testP,1.0f)){
              ResNumCnfg++;
              break;
              // step; количество элементов вкл в конфигурацию
          }
        }//generate single configuration cicle
        bit_to_smb(Cserv.adr,SmbCnfgStart.adr);
        for(iMode=NumOnboardDevice+1;iMode<=NumOnboardDevice+NumMode;iMode++)
            if(ENBL == r_bit(&ExctMode,iMode-NumOnboardDevice))
                w_smb(SmbCnfgStart.adr,iMode,DOWN);
            else
                w_smb(SmbCnfgStart.adr,iMode,UP);
    }//generate configurations cicle
    return ResNumCnfg;
}

void ChoiseCnfgFromAll(bit_configuration Cserv, uint32_t * NumCnfg){
    uint8_t iMode=0;
    for(iMode=NumOnboardDevice+1;iMode<=NumOnboardDevice+NumMode;iMode++){
        //if(ENBL == r_bit(&ExctMode,iMode-NumOnboardDevice))
            w_bit(Cserv.adr,iMode,DISBL); //ENBL 27.02.2022
            NumCnfg[iMode-NumOnboardDevice-1]=0;
    }
    //unsigned int ResNumCnfg = 0;
    uint32_t iCnfg = 0;
    uint8_t iBit = 0;
    _Bool FlagCnfg = TRUE;
    for(iMode=0;iMode<NumMode;iMode++){//iMode=NumOnboardDevice+1;iMode<=NumOnboardDevice+NumMode;iMode++
        if(ENBL == r_bit(&ExctMode,iMode+1)){
            w_bit(Cserv.adr,iMode+NumOnboardDevice+1,ENBL);
            for(iCnfg=0;iCnfg<MaxInAllCnfg;iCnfg++) {
                for(iBit=1;iBit<=NumOnboardDevice+NumMode;iBit++){
                    if(DOWN == r_bit(Cserv.adr,iBit) &&
                       ENBL == r_bit(AllCnfg[iCnfg].adr,iBit)){
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
            w_bit(Cserv.adr,iMode,DISBL);
        }
    }

    for(iMode=NumOnboardDevice+1;iMode<=NumOnboardDevice+NumMode;iMode++){
        if(ENBL == r_bit(&ExctMode,iMode-NumOnboardDevice))
            w_bit(Cserv.adr,iMode,ENBL);
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
        for(iBit=1;iBit<=NumOnboardDevice;iBit++){ // NumOnboardDevice + ??? NumMode
            if(ENBL == r_bit(ManyCnfg[iCnfg].adr,iBit)){
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
        if(ENBL == r_bit(&ExctMode,iMode+1)){
            for(iCnfg=0;iCnfg<NumCnfg[iMode];iCnfg++){
                CurValIndex = 0;
                SummEnbl=0;
                for(iBit=1;iBit<=NumOnboardDevice;iBit++){
                    if(ENBL == r_bit(ManyCnfgForMode[iMode][iCnfg].adr,iBit)){
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
                w_bit(Con.adr,iMode+NumOnboardDevice+1,ENBL);
                for(iBit=1;iBit<=NumOnboardDevice;iBit++){
                    if(DISBL == r_bit(Con.adr,iBit) &&
                       ENBL == r_bit(ManyCnfgForMode[iMode][ChoiseCnfg[iMode]].adr,iBit) )
                        w_bit(Con.adr,iBit,ENBL);
                }
            }
    }

    smb_configuration SmbCnfgOn;
    bit_to_smb(Con.adr,SmbCnfgOn.adr);
    for(iBit=1;iBit<=NumOnboardDevice;iBit++){
          w_smb(SmbCnfgOn.adr,iBit,DOWN);
            if(culc_p(SmbCnfgOn.adr)>0.0f){
                w_bit(Con.adr,iBit,DOWN);
            }
          bit_to_smb(Con.adr,SmbCnfgOn.adr);
    }
}

void FormCnfgOn(bit_configuration Cserv){

    if(ALG_RAND_RSCH == FlagAlgReCnfg){
      uint32_t NumCnfgRand = 0;
      NumCnfgRand = RandGnrtCnfg(Cserv);
      CnfgEqulTimeRand(NumCnfgRand);
    }else if (ALG_FULL_ENUM == FlagAlgReCnfg){
      uint32_t NumCnfgMode[ON_ALL_MODE];
      ChoiseCnfgFromAll(Cserv, NumCnfgMode);
      CnfgEqulTimeMode(NumCnfgMode);
    }
}

void IncludeModes(bit_configuration * Cnfg){
    uint8_t iMode=0;
    for(iMode=NumOnboardDevice+1;iMode<=NumOnboardDevice+NumMode;iMode++)
        if(ENBL == r_bit(&ExctMode,iMode-NumOnboardDevice))
            w_bit(Cnfg->adr,iMode,ENBL);
        else
            w_bit(Cnfg->adr,iMode,DISBL);
}

void InitFswRcnf1(bit_configuration * InitC, uint8_t Elements, uint8_t Modes)
{
    w_byte(&ExctMode,CNFG_ALL_MODE_TEST);
    NumOnboardDevice = Elements; //4Gyro+2Fss+2St+2Mag+4Whl+3Mtb = 17
    NumMode = Modes; //LVHL, SunOriuntation, ProgrammReturn = 3
    SumExctMode = NumMode;
    MaxCnfgRnd = 20; //пусть будет столько пока 3.03.2022

    char NamePolinomFile[128] = "polynom/PolEk.txt";
    // char NamePolinomFile[128] = "SimplePol.txt";
    LoadPlnmFromFile(NamePolinomFile,&ScPolinim);
    PolinomInit(&ScPolinim);
    FlagAlgReCnfg = ALG_FULL_ENUM;
    if(ALG_FULL_ENUM == FlagAlgReCnfg){
        MaxInAllCnfg = FindConfiguration(&ScPolinim, AllCnfg, NumOnboardDevice, NumMode);
        for(uint32_t i=0;i<MaxInAllCnfg;i++){
                        printf("Cnfg No(%i) : ",i+1);
                        for(uint8_t j=1;j<=NumOnboardDevice+NumMode;j++){
                                if(r_bit(AllCnfg[i].adr,j))
                                    printf("P%i ",j);
                        }
                        printf("\n");
        }
    }

    FlagAlgReCnfg = ALG_RAND_RSCH;
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
    w_byte(&ExctMode,CNFG_ALL_MODE_TEST);
    NumOnboardDevice = Elements; //4Gyro+2Fss+2St+2Mag+4Whl+3Mtb = 17
    NumMode = Modes; //LVHL, SunOriuntation, ProgrammReturn = 3
    SumExctMode = NumMode;
    MaxCnfgRnd = 10; //пусть будет столько пока 3.03.2022

    char NamePolinomFile[128] = "polynom/PolEk.txt";
    // char NamePolinomFile[128] = "SimplePol.txt";
    LoadPlnmFromFile(NamePolinomFile,&ScPolinim);
    PolinomInit(&ScPolinim);
    FlagAlgReCnfg = ALG_FULL_ENUM;
    if(ALG_FULL_ENUM == FlagAlgReCnfg){
        MaxInAllCnfg = FindConfiguration(&ScPolinim, AllCnfg, NumOnboardDevice, NumMode);
        for(uint32_t i=0;i<MaxInAllCnfg;i++){
                        printf("Cnfg No(%i) : ",i+1);
                        for(uint8_t j=1;j<=NumOnboardDevice+NumMode;j++){
                                if(r_bit(AllCnfg[i].adr,j))
                                    printf("P%i ",j);
                        }
                        printf("\n");
        }
    }

    FlagAlgReCnfg = ALG_RAND_RSCH;
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
    if(SumExctMode>0){
        uint8_t i;
        for (i=0;i<NumOnboardDevice;i++)
            Tcur[i] = TcurIn[i];
        FormCnfgOn(Cserv);
    }
    else
        return 0;

    copy(CnfgInTheLoop->adr, Con.adr);
    IncludeModes(CnfgInTheLoop);
    uint32_t res = (uint32_t)SumExctMode;
    return res;
}
