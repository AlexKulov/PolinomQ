#include "strpolinom.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h> // функция atoi() см. ниже

#define MAX_DW (32)
#define SMB_FOR_SIG (8)

typedef struct PlnmToSend{
    uint16_t SW; //service word
    uint8_t DW[MAX_DW][2]; //data word

}PlnmToSend;

uint16_t transPlnmToMsg(Polinom * plnm, PlnmToSend * mkoSend){
    uint16_t noSw = 0; // кол-во командных слов
    uint16_t noDw = 0; // кол-во слов данных
    _Bool leftPart = FALSE;
//------------------------------------------------------------
    for (uint16_t iStr=0; iStr<plnm->nStr; iStr++){
        if(iStr%2 == 0){
            mkoSend[noSw].DW[noDw][0] = plnm->strings[iStr].head;
            leftPart = TRUE;
         }
         else{
             mkoSend[noSw].DW[noDw][1] = plnm->strings[iStr].head;
             leftPart = FALSE;
             noDw++;
            if(noDw>MAX_DW-1){
                noDw = 0;
                noSw++;
            }
         }
    }
//-----------------------------------------------------------
    if(noDw >0 || (noDw == 0 &&  leftPart)){
        noSw++;
        noDw=0;
    }
    leftPart = FALSE;
//-----------------------------------------------------------
    for (uint16_t iStr=0; iStr<plnm->nStr; iStr++){
        for (uint8_t jBitMap=0; jBitMap<N_BIT_MAP; jBitMap++){
            if(! leftPart){
                mkoSend[noSw].DW[noDw][0] = plnm->strings[iStr].existPQ[jBitMap];
                leftPart = TRUE;
            }
            else{
                mkoSend[noSw].DW[noDw][1] = plnm->strings[iStr].existPQ[jBitMap];
                leftPart = FALSE;
                noDw++;
                if(noDw>MAX_DW-1){
                    noDw = 0;
                    noSw++;
                }
            }
        }
    }
//---------------------------------------------------------------
    if(noDw >0 || (noDw == 0 &&  leftPart)){
        noSw++;
        noDw=0;
    }
    leftPart = FALSE;
//---------------------------------------------------------------
    for (uint16_t iStr=0; iStr<plnm->nStr; iStr++){
        for (uint8_t jSmbMap=0; jSmbMap<N_BIT_MAP; jSmbMap++){
            if(! leftPart){
                mkoSend[noSw].DW[noDw][0] = plnm->strings[iStr].pq[jSmbMap];
                leftPart = TRUE;
            }
            else{
                mkoSend[noSw].DW[noDw][1] = plnm->strings[iStr].pq[jSmbMap];
                leftPart = FALSE;
                noDw++;
                if(noDw>MAX_DW-1){
                    noSw++;
                    noDw = 0;
                }
           }
        }
    }

    if(noDw >0){
        noSw++;
        noDw=0;
    }
    noSw++;
    uint16_t     hSD = plnm->nStr;
                 hSD = hSD & 0x00ff;
    mkoSend[noSw].DW[0][0] = (uint8_t)hSD;
                 hSD = plnm->nStr;
                 hSD = hSD>>8; // может hSD = hSD<<8; ?
    mkoSend[noSw].DW[0][1] = (uint8_t)hSD;

    mkoSend[noSw].DW[1][0] = plnm->nEl;

    return noSw;
}

uint16_t  loadPlnmFromMsg (PlnmToSend * mko_send, Polinom * plnm, uint16_t l_str){
    uint16_t  nSw = 0;
    uint16_t jDw = 0;
    _Bool  leftPart = FALSE;

    for(uint16_t i=0; i<l_str;){
        plnm->strings[i].head = mko_send[ nSw].DW[jDw][0];
        i++;
        leftPart = TRUE;
        if(i>=l_str){
            break;
        }

        plnm->strings[i].head = mko_send[ nSw].DW[jDw][1];
        i++;
        leftPart = FALSE;
        jDw++;
        if(jDw>MAX_DW-1){
            jDw = 0;
            nSw++;
        }
    }

    if(jDw >0 || (jDw == 0 &&  leftPart)){
        nSw++;
        jDw=0;
    }

    leftPart = FALSE;

    for(uint16_t i=0; i<l_str; i++){
        for(uint8_t j=0; j<N_BIT_MAP; j++){
            if(! leftPart){
                plnm->strings[i].existPQ[j] = mko_send[ nSw].DW[jDw][0];
                leftPart = TRUE;
            }
            else{
                plnm->strings[i].existPQ[j] = mko_send[ nSw].DW[jDw][1];
                leftPart = FALSE;
                jDw++;
                if(jDw>MAX_DW-1){
                    jDw = 0;
                    nSw++;
                }
            }
        }
    }

    if(jDw >0 || (jDw == 0 &&  leftPart)){
        nSw++;
        jDw=0;
    }

    leftPart = FALSE;

    for(uint16_t i=0; i<l_str;i++){
        for(uint8_t j=0; j<N_BIT_MAP; j++){
            if(! leftPart){
                plnm->strings[i].pq[j] = mko_send[ nSw].DW[jDw][0];
                leftPart = TRUE;
            }
            else{
                plnm->strings[i].pq[j] = mko_send[ nSw].DW[jDw][1];
                leftPart = FALSE;
                jDw++;
                if(jDw>MAX_DW-1){
                    jDw = 0;
                    nSw++;
                }
            }
        }
    }

    return  nSw;
}

void loadPlnmFromFile(char * plnmFileName, Polinom * polinom){
    FILE * filePolinom;
    filePolinom=fopen(plnmFileName,"r");
    if (filePolinom!=NULL){
        uint16_t CntStr = 0; //number of string in Polinom
        char failSmb;
        char scanString[4*N_EL_MAX];
        uint16_t i;

        for(i=0;i<N_STR_MAX;i++){
            if(EOF == fscanf(filePolinom,"%[^\n] %[\n]",scanString,&failSmb))
                break;
            CntStr++;
        }

        char Nomer[3]; //index of element, 000 to 999 or _00 to _99
        uint8_t nSmbEnd = 0; //k nmb_symb_end
        uint8_t nSmbBegin = 0; //h nmb_symb_beg
        uint8_t nCurPQ = 0; //j_N nCurPQ
        uint8_t stringLength = 0, lastSmb =0; // stringLength
        _Bool isFirstPQ = TRUE;  //n_bool
        uint8_t nSmbInString = 0;
        uint8_t indexPQ = 0; //index_PQ
        uint8_t nEl = 0;
        uint8_t k,j;
        initPolinom(polinom, CntStr);
        rewind(filePolinom);

        for (uint16_t iStr=0; iStr<CntStr; iStr++){
            nSmbEnd = 0;
            nSmbBegin = 0;
            nCurPQ = 0;
            isFirstPQ = TRUE;
            fscanf(filePolinom,"%[^\n] %[\n]",scanString,&failSmb);
            stringLength = 0;
            for (i=0;i<4*N_EL_MAX;i++) {
               if(scanString[i] == 0)
                   break;
               stringLength++;
            }

            wSig(&polinom->strings[iStr].head, 1);
            for (k=0; k<=SMB_FOR_SIG; k++){
                if (scanString[k] == '+'){
                    wSig(&polinom->strings[iStr].head, 1);
                    break;
                }

                if (scanString[k] == '-'){
                    wSig(&polinom->strings[iStr].head, -1);
                    break;
                }
            }

            for(j=0; j<stringLength; j++){
                if (scanString[j] == 'P')
                    nCurPQ = j;
                else if (scanString[j] == 'Q')
                    nCurPQ = j;

                if (nCurPQ > nSmbBegin && isFirstPQ){
                    nSmbBegin = nCurPQ;
                    isFirstPQ = FALSE;
                }

                if (nCurPQ > nSmbBegin){
                    nSmbEnd = nCurPQ;
                    nSmbBegin++;
                    Nomer[0]=0;Nomer[1]=0;Nomer[2]=0;
                    strncpy(Nomer, &scanString[nSmbBegin],nSmbEnd-nSmbBegin-1);
                    if(atoi(Nomer)>255)
                        printf("Err LoadPlnmFromFile: atoi(Nomer)>255\n");
                    indexPQ = (uint8_t)atoi(Nomer);
                    nSmbBegin--;
                    if (scanString[nSmbBegin] == 'P')
                        wBit (polinom->strings[iStr].pq, indexPQ, P);
                    else if (scanString[nSmbBegin] == 'Q')
                        wBit (polinom->strings[iStr].pq, indexPQ, Q);
                    wBit (polinom->strings[iStr].existPQ, indexPQ, TRUE);
                    nSmbBegin = nSmbEnd;
                    nSmbInString++;
                    if(indexPQ > nEl)
                        nEl =  indexPQ;
                }
            }
            nSmbBegin++;
            Nomer[0] = '0';
            Nomer[1] = 0;
            Nomer[2] = 0;
            //---------------- чтобы работало и с знаками в конце строки и без ----
            if(scanString[stringLength-1]=='+' || scanString[stringLength-1]=='-'){
                lastSmb = 1;
            }else{
                lastSmb = 0;
            }
            //---------------------------------------------------------------------
            strncpy(Nomer, &scanString[nSmbBegin],stringLength-nSmbBegin-lastSmb);
            if(atoi(Nomer)>255){
                printf("Err LoadPlnmFromFile: atoi(Nomer)>255\n");
            }
            indexPQ = (uint8_t)atoi(Nomer);
            if(indexPQ == 0){
                printf("ER: read NULL el of pol in \n");
                printf("LoadPolinomFromFile \n");
            }

            nSmbBegin--;
            if (scanString[nSmbBegin] == 'P'){
                wBit (polinom->strings[iStr].pq, indexPQ, P);
            }else if (scanString[nSmbBegin] == 'Q'){
                wBit (polinom->strings[iStr].pq, indexPQ, Q);
            }
            wBit (polinom->strings[iStr].existPQ, indexPQ, TRUE);
            nSmbInString++;
            wLeight(&polinom->strings[iStr].head, nSmbInString);
            nSmbInString=0;
            if(indexPQ > nEl){
               nEl =  indexPQ;
            }
        }
        polinom->nEl = nEl;
    }
    fclose(filePolinom);
}

#define MAX_MODE 10

uint32_t findConfiguration(Polinom * polinom, BitConfiguration * manyConf, uint8_t numDev, uint8_t numMode){

    float KsiEl[N_EL_MAX];
    uint8_t    pOfStr[N_BIT_MAP];
    uint8_t cnfgOfStr[MAX_MODE][N_BIT_MAP];
    uint16_t  smbOfStr[N_BIT_MAP];
    uint16_t smbForChoice[N_BIT_MAP];

    nullBitCnfg(   pOfStr);
    for(uint8_t j=0;j<MAX_MODE;j++){
        nullBitCnfg(&cnfgOfStr[j][0]);
    }
    nullSmbCnfg(smbOfStr);
    nullSmbCnfg(smbForChoice);


    _Bool isSameCnfg = TRUE;
    uint8_t nMaxKsi = 0; //максимальна¤ значимость
    uint32_t nCnfg = 0; //счЄтчик выбранных из полинома конфигураций

    uint8_t cur = 0;//счЄтчик конфигураций в строке

    for(uint16_t i=0;i<polinom->nStr;i++){
        if(rSig(polinom->strings[i].head) == 1){
            for(uint8_t j=1;j<=numDev;j++){
                if(rBit(polinom->strings[i].existPQ,j) &&
                   rBit(polinom->strings[i].pq  ,j) == P){
                        wBit(pOfStr,j,1);
                        wSmb(smbOfStr,j,NORM);
                }
                else{
                        wBit(pOfStr,j,0);
                        wSmb(smbOfStr,j,DOWN);
                }
            }
            ///*
            for(uint8_t j=numDev+1;j<=numDev+numMode;j++){
                wSmb(smbOfStr,j,UP);
                if(rBit(polinom->strings[i].existPQ,j) &&
                   rBit(polinom->strings[i].pq  ,j) == P){
                    wBit(pOfStr,j,1);
                }else{
                    wBit(pOfStr,j,0);
                }
            }

            cur=0;

            if(numMode>0){
                for(uint8_t j=numDev+1;j<=numDev+numMode;j++){
                    if(rBit(pOfStr,j) == 0){
                        wSmb(smbOfStr,j,DOWN);
                        copySmb(smbForChoice,smbOfStr);
                        wSmb(smbOfStr,j,UP);
                        for(uint8_t j=1;j<=numDev;j++){
                            culcAllKsi(smbForChoice,KsiEl,NEGATIV);

                            nMaxKsi = firstMaxKsi(KsiEl, smbForChoice);

                            if(nMaxKsi>0 && nMaxKsi<=numDev){
                                wSmb(smbForChoice,nMaxKsi,UP);
                            }else{
                                printf("Error choice max el in FindConf \n");
                            }

                            if(!(culcP(smbForChoice)<1.0f)){
                                for(uint8_t x=1;x<=numDev;x++){
                                    if(rSmb(smbForChoice,x) == UP){
                                        wBit(cnfgOfStr[cur],x,1);
                                    }else{
                                        wBit(cnfgOfStr[cur],x,0);
                                    }
                                }

                                for(uint8_t x=numDev+1;x<=numDev+numMode;x++){
                                    if(rSmb(smbForChoice,x)==DOWN){
                                        wBit(cnfgOfStr[cur],x,1);
                                    }else{
                                        wBit(cnfgOfStr[cur],x,0);
                                    }
                                }
                                cur++;
                                break;
                            }
                        }//for (1...N_BA)
                    }//pOfStr[j] == 0
                } //for(N_BA...N_MODE)
            }//N_MODE>0
            else{
                for(uint8_t j=1;j<=numDev;j++){
                    culcAllKsi(smbOfStr,KsiEl,NEGATIV);
                    nMaxKsi = firstMaxKsi(KsiEl, smbOfStr);
                    if(nMaxKsi>0 && nMaxKsi<=numDev){
                        wSmb(smbOfStr,nMaxKsi,UP);
                    }else{
                        printf("Error choice max el in FindConf \n");
                    }

                    if(!(culcP(smbOfStr)<1.0f)){
                        for(uint8_t x=1;x<=numDev;x++){
                            if(rSmb(smbOfStr,x) == UP){
                                wBit(cnfgOfStr[cur],x,1);
                            }else{
                                wBit(cnfgOfStr[cur],x,0);
                            }
                        }
                        break;
                    }
                }//for (1...N_BA)
                cur++;
            }//

            nullBitCnfg(pOfStr);
            nullSmbCnfg(smbOfStr);
            //2. провер¤ем на схожесть
            for(uint16_t c=0;c<cur;c++){
                for(uint16_t k=0;k<nCnfg;k++){
                    isSameCnfg = TRUE;
                    for(uint8_t j=1;j<=polinom->nEl;j++){
                        if(rBit(cnfgOfStr[c],j) != rBit(manyConf[k] ,j)){
                            isSameCnfg = FALSE;
                            break;
                        }
                    }
                    if(isSameCnfg){
                        break;
                    }
                }

                if(!isSameCnfg ||  nCnfg==0){
                //3. «аписываем конфигурацию как очередную
                    copy(manyConf[nCnfg] ,cnfgOfStr[c]);
                    nCnfg++;
                }
                nullBitCnfg(cnfgOfStr[c]);
            }//for(c...cur)
        }//если строка положительная
    }//for(...i=0;i<...n_str;i++)

    return nCnfg;
}
