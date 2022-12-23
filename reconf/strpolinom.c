//---------------------------------------------------------------------------

#include "strpolinom.h"

#include <stdlib.h> // функция rand() см. ниже
#include <stdio.h>

//------------------------- ОБЕСПЕЧИВАЮЩИЕ ФУНКЦИИ ---------------------------
void nullBitCnfg (BitConfiguration cnfg){
    for(uint8_t i=0;i<N_BIT_MAP;i++){
        cnfg[i] = 0x00;
    }
}

void nullSmbCnfg (SmbConfiguration cnfg){
   for(uint8_t i=0;i<N_BIT_MAP;i++){
       cnfg[i] = 0x0000;
   }
}

uint8_t rBit(const BitConfiguration str, uint8_t j){ // начинаем считать с первого, а не с 0-го
    if(j>BIT_IN_BYTE*N_BIT_MAP || j<1){
        printf("Err r_bit\n");
        // exit(1); для отладки пусть так
    }

    for(uint8_t i=0;i<N_BIT_MAP;i++){
        if(j == (1 + i*BIT_IN_BYTE)){
            return str[i] & 0x01;
        }else if(j == (2 + i*BIT_IN_BYTE)){
            return (str[i]>>1) & 0x01;
        }else if(j == (3 + i*BIT_IN_BYTE)){
            return (str[i]>>2) & 0x01;
        }else if(j == (4 + i*BIT_IN_BYTE)){
            return (str[i]>>3) & 0x01;
        }else if(j == (5 + i*BIT_IN_BYTE)){
            return (str[i]>>4) & 0x01;
        }else if(j == (6 + i*BIT_IN_BYTE)){
            return (str[i]>>5) & 0x01;
        }else if(j == (7 + i*BIT_IN_BYTE)){
            return (str[i]>>6) & 0x01;
        }else if(j == (8 + i*BIT_IN_BYTE)){
            return (str[i]>>7) & 0x01;
        }
    }
    return 0;
}

uint8_t rSmb(const SmbConfiguration str, uint8_t j){ // начинаем считать с первого, а не с 0-го
    if(j>BIT_IN_BYTE*N_BIT_MAP || j<1){
        printf("Err r_smb");
          // exit(1); для отладки пусть так
    }

    for(uint8_t i=0;i<N_BIT_MAP;i++){
        if(j == (1 + i*BIT_IN_BYTE))
            return (str[i] & 0x0003);
        else if(j == (2 + i*BIT_IN_BYTE))
            return ((str[i] & 0x000c)>>2)&0x0003;
        else if(j == (3 + i*BIT_IN_BYTE))
            return ((str[i] & 0x0030)>>4)&0x0003;
        else if(j == (4 + i*BIT_IN_BYTE))
            return ((str[i] & 0x00c0)>>6)&0x0003;
        else if(j == (5 + i*BIT_IN_BYTE))
            return ((str[i] & 0x0300)>>8)&0x0003;
        else if(j == (6 + i*BIT_IN_BYTE))
            return ((str[i] & 0x0c00)>>10)&0x0003;
        else if(j == (7 + i*BIT_IN_BYTE))
            return ((str[i] & 0x3000)>>12)&0x0003;
        else if(j == (8 + i*BIT_IN_BYTE))
            return ((str[i] & 0xc000)>>14)&0x0003;
    }
    return 0;
}

void wBit(BitConfiguration str, uint8_t j, uint8_t bit){
    if(j>BIT_IN_BYTE*N_BIT_MAP || j<1){
        printf("Err w_bit: j - incorrect\n");
        // exit(1); для отладки пусть так
    }

    if(bit>1){
        printf("Err w_bit: bit - incorrect\n");
        // exit(1); для отладки пусть так
    }

    if(j<=BIT_IN_BYTE*N_BIT_MAP && j>0){
        for(uint8_t i=0;i<N_BIT_MAP;i++){
            if(j == (1 + i*BIT_IN_BYTE)){
                bit==1 ? (str[i] |= 0x01) : (str[i] &= ~0x01);break;
            }else if (j == (2 + i*BIT_IN_BYTE)){
                bit==1 ? (str[i] |= 0x02) : (str[i] &= ~0x02); break;
            }else if (j == (3 + i*BIT_IN_BYTE)){
                bit==1 ? (str[i] |= 0x04) : (str[i] &= ~0x04); break;
            }else if (j == (4 + i*BIT_IN_BYTE)){
                bit==1 ? (str[i] |= 0x08) : (str[i] &= ~0x08); break;
            }else if (j == (5 + i*BIT_IN_BYTE)){
                bit==1 ? (str[i] |= 0x10) : (str[i] &= ~0x10); break;
            }else if (j == (6 + i*BIT_IN_BYTE)){
                bit==1 ? (str[i] |= 0x20) : (str[i] &= ~0x20); break;
            }else if (j == (7 + i*BIT_IN_BYTE)){
                bit==1 ? (str[i] |= 0x40) : (str[i] &= ~0x40); break;
            }else if (j == (8 + i*BIT_IN_BYTE)){
                bit==1 ? (str[i] |= 0x80) : (str[i] &= ~0x80); break;
            }
        }
    }
}

void wSmb(SmbConfiguration str, uint8_t j, uint8_t smb){
    if(j==0){
        printf("Err w_smb: j - incorrect\n");
        // exit(1); для отладки пусть так
    }

    if(smb>2){
        printf("Err w_smb: smb - incorrect\n");
        // exit(1); для отладки пусть так
    }

    if(j<=BIT_IN_BYTE*N_BIT_MAP && j>0){
        for(uint8_t i=0;i<N_BIT_MAP;i++){
            if(j == (1 + i*BIT_IN_BYTE)){
                if(1 == smb){
                    str[i] |=  0x0001; str[i] &= ~0x0002;
                }
                else if(0 == smb){
                    str[i] &= ~0x0003;
                }else{
                    str[i] |=  0x0002; str[i] &= ~0x0001;
                }
                break;
            }
            else if (j == (2 + i*BIT_IN_BYTE)){
                if(1 == smb){str[i] |=  0x0004; str[i] &= ~0x0008;}
                else if(0 == smb) str[i] &= ~0x000c;
                else             {str[i] |=  0x0008; str[i] &= ~0x0004; }
                break;
            }
            else if (j == (3 + i*BIT_IN_BYTE)){
                if(1 == smb){str[i] |=  0x0010; str[i] &= ~0x0020;}
                else if(0 == smb) str[i] &= ~0x0030;
                else             {str[i] |=  0x0020; str[i] &= ~0x0010;}
                break;
            }
            else if (j == (4 + i*BIT_IN_BYTE)){
                if(1 == smb){str[i] |=  0x0040; str[i] &= ~0x0080;}
                else if(0 == smb) str[i] &= ~0x00c0;
                else             {str[i] |=  0x0080; str[i] &= ~0x0040;}
                break;
            }
            else if (j == (5 + i*BIT_IN_BYTE)){
                if(1 == smb){str[i] |=  0x0100;str[i] &= ~0x0200;}
                else if(0 == smb) str[i] &= ~0x0300;
                else             {str[i] |=  0x0200;str[i] &= ~0x0100;}
                break;
            }
            else if (j == (6 + i*BIT_IN_BYTE)){
                if(1 == smb){str[i] |=  0x0400; str[i] &= ~0x0800;}
                else if(0 == smb) str[i] &= ~0x0c00;
                else             {str[i] |=  0x0800; str[i] &= ~0x0400;}
                break;
            }
            else if (j == (7 + i*BIT_IN_BYTE)){
                if(1 == smb){str[i] |=  0x1000; str[i] &= ~0x2000;}
                else if(0 == smb) str[i] &= ~0x3000;
                else             {str[i] |=  0x2000; str[i] &= ~0x1000;}
                break;
            }
            else if (j == (8 + i*BIT_IN_BYTE)){
                if(1 == smb){str[i] |=  0x4000; str[i] &= ~0x8000;}
                else if(0 == smb) str[i] &= ~0xc000;
                else             {str[i] |=  0x8000; str[i] &= ~0x4000;}
                break;
            }
        }
    }
}

int8_t rSig(uint8_t head){
    uint8_t Sign = head & 0x03;
    if(0x03 == Sign)
        return -1;
    else if(0x01 == Sign)
        return 1;
    else{
        printf("Err r_sig: head.sig - incorrect\n");
        // exit(1); для отладки пусть так
        return 1;
    }
}

uint8_t rLeight(uint8_t head){
    uint8_t Leight = (head>>2) & 0x3f;
    return Leight;
}

void wSig(uint8_t * head, int8_t inSign){
    if(!(-1 == inSign || 1 == inSign)){
          printf("Err w_sig: InSign - incorrect\n");
          // exit(1); для отладки пусть так
    }
    uint8_t locHead = *head;
    if(-1 == inSign){
        locHead = locHead | 0x03;
        *head = locHead;
    }
    else if(1 == inSign){
        locHead = locHead | 0x01;
        *head = locHead;
    }
}

void wLeight(uint8_t * head, uint8_t inLeight){
    if(inLeight>0x3f){
          printf("Err wLeight: InLeight - incorrect\n");
          // exit(1); для отладки пусть так
    }
    uint8_t locSign = *head & 0x03;
    * head = (uint8_t)(inLeight<<2);
    if(0x01 == locSign)
        wSig(head,1);
    else if(0x03 == locSign)
        wSig(head,-1);
    else{
        printf("Err wLeight: head.sig - incorrect\n");
        // exit(1); для отладки пусть так
    }
}

void wByte(uint8_t * str, uint8_t byte){
    if(str == NULL){
        printf("Err w_byte\n");
        //exit(1);
    }
    * str = byte;
}

uint8_t rByte(uint8_t * str){
    if(str == NULL){
        printf("Err r_byte\n");
        //exit(1);
    }
    uint8_t resByte = (* str);
    return resByte;
}

_Bool copy(uint8_t * cnfg1, const uint8_t * cnfg2){
    for(uint8_t i=0;i<N_BIT_MAP;i++){
        cnfg1[i] = cnfg2[i];
    }
    return 0;
}

_Bool copySmb(uint16_t * cnfg1, const uint16_t * cnfg2){
    for(uint8_t i=0;i<N_BIT_MAP;i++){
        cnfg1[i] = cnfg2[i];
    }
    return 0;
}

void bitToSmb (BitConfiguration bitCnfg, SmbConfiguration smbCnfg){
    for(uint8_t i=1; i<=N_EL_MAX;i++){
        if(rBit(bitCnfg,i) == 1){
            wSmb(smbCnfg,i,NORM);
        }else{
            wSmb(smbCnfg,i,DOWN);
        }
    }
}

void bitToSmb2 (BitConfiguration bitCnfg, SmbConfiguration smbCnfg){
    for(uint8_t i=1; i<=N_EL_MAX;i++){
        if(rBit(bitCnfg,i) == P){
            wSmb(smbCnfg,i,UP);
        }else{
            wSmb(smbCnfg,i,DOWN);
        }
    }
}

void setCnfg(BitConfiguration bitCnfg, uint8_t enOrDis){
    if(ENBL == enOrDis){
        for(uint8_t i=0;i<N_BIT_MAP;i++)
            wByte(&bitCnfg[i], 0xff);
    }
    else if (DISBL == enOrDis) {
        for(uint8_t i=0;i<N_BIT_MAP;i++)
            wByte(&bitCnfg[i], 0x00);
    }
}

static Polinom * plnm; //
//-------------------------------- ОСНОВНЫЕ ФУНКЦИИ -------------------------------
void setLibPolinom (Polinom * inPlnm){ //polinomInit
    if(inPlnm == NULL){
        printf("setLibPolinom error : inPlnm = NULL \n");
        return;
    }
    plnm  = inPlnm;
}

void initPolinom (Polinom * inPlnm, uint16_t nStr){
    plnm  = inPlnm;
    plnm->nStr = nStr;
    for(uint16_t i=0; i<plnm->nStr; i++){
        wSig(&plnm->strings[i].head, 1);
        wLeight(&plnm->strings[i].head, 0);
        for(uint8_t j=1; j<=N_EL_MAX; j++){
            wBit(plnm->strings[i].existPQ,j,FALSE);
            wBit(plnm->strings[i].pq,j,Q);
        }
    }
}

//------------------------------- ОБЕСП. ФУНКЦИЯ ---------------------------------
float culcStrP(PolinomString * str, uint16_t * workCnfg){
    float pStrRes = 1.0;
    uint8_t k=0;

    for(uint8_t i=1;i<=N_EL_MAX;i++){
         if(rBit(str->existPQ,i)){ // может быть три варианта в зависимости от bit_work_conf[i] и p_or_q
            if(rSmb(workCnfg, i) == NORM)
                pStrRes=pStrRes * 0.5f;
            else if(rSmb(workCnfg, i) == DOWN && rBit(str->pq,i) == P)
                return 0.0;
            else if(rSmb(workCnfg, i) == UP   && rBit(str->pq,i) == Q)
                return 0.0;
            k++;
        }
        if(k >= rLeight(str->head))
            return pStrRes;
    }
    return pStrRes;
}
//-----------------------------------------------------------------------------------
float culcP    (uint16_t * workCnfg){
    float pRes = 0.0;
    for(uint16_t i=0; i<plnm->nStr; i++){
        if(rSig(plnm->strings[i].head) == 1)
            pRes = pRes + culcStrP(&(plnm->strings[i]), workCnfg);
        else if(rSig(plnm->strings[i].head) == -1)
            pRes = pRes - culcStrP(&(plnm->strings[i]), workCnfg);
    }
    return pRes;
}

//------------------------------- ОБЕСП. ФУНКЦИЯ ---------------------------------
double culcStrPt(PolinomString * str, double * elPt){
    double pStrRes = 1.0;
    uint8_t k=0;

    for(uint8_t i=1;i<=plnm->nEl;i++){
        if(rBit(str->existPQ,i)){
            if(rBit(str->pq,i) == P)
                pStrRes=pStrRes*elPt[i-1];
            else if(rBit(str->pq,i) == Q)
                pStrRes=pStrRes*(1.0-elPt[i-1]);
            k++;
        }
        if(k >= rLeight(str->head) )
            return pStrRes;
    }
    return pStrRes;
}

double culcPt  (double * elPt){
    double pRes = 0.0;

    for(uint16_t i=0; i<plnm->nStr; i++){
        if(rSig(plnm->strings[i].head) == 1)
            pRes = pRes + culcStrPt(&(plnm->strings[i]), elPt);
        else if(rSig(plnm->strings[i].head) == -1)
            pRes = pRes - culcStrPt(&(plnm->strings[i]), elPt);
    }
    return pRes;
}

float culcKsi      (uint16_t * workCnfg, uint8_t j, typeKsi type){
    float ksi = 0.0;
    if(j>plnm->nEl){
        return -2.0; // ошибка, j задана некорректно
    }
    if(type == NEGATIV){
        ksi = culcP(workCnfg);
        wSmb(workCnfg, j,DOWN);
        ksi = ksi - culcP(workCnfg);
        wSmb(workCnfg, j,NORM);
        return ksi;
    }
    else if(type == POSITIV){
        wSmb(workCnfg, j,UP);
        ksi = culcP(workCnfg);
        wSmb(workCnfg, j,NORM);
        ksi = ksi - culcP(workCnfg);
        return ksi;
    }
    else if(type == FULL){
        wSmb(workCnfg, j,UP);
        ksi = culcP(workCnfg);
        wSmb(workCnfg, j,DOWN);
        ksi = ksi - culcP(workCnfg);
        wSmb(workCnfg, j,NORM);
        return ksi;
    }
    return 0.0;
}
// договоримся что кси которые не надо вычислять будут заданы значениями 2 и -2 в зависимости от
// работоспособен этот элемент или уже включен в конфигурацию
void culcAllKsi  (uint16_t * workCnfg, float * allKsi, typeKsi type){
    for(uint8_t i=1;i<=plnm->nEl;i++){ // i должны начинаться с 1 и до n_el включительно
        if(rSmb(workCnfg, i) == NORM){
            allKsi[i-1] = culcKsi(workCnfg, i, type);
        }else if(rSmb(workCnfg, i) == DOWN){
            allKsi[i-1] = -2.0;
        }else if(rSmb(workCnfg, i) == UP){
            allKsi[i-1] = 2.0;
        }
    }
}

#define EQUL_ZONE (5E-7f)

_Bool equl(float f1,float f2){
    float mF = (f1+f2)/2.0f;
    if(f1>= (mF - EQUL_ZONE) &&  f1<= (mF + EQUL_ZONE) &&
       f2>= (mF - EQUL_ZONE) &&  f2<= (mF + EQUL_ZONE)){
        return TRUE;
    }else{
        return FALSE;
    }
}

_Bool aMoreB (float a, float b){ //т.е b< а <= b+eps
    float a_b = a - b;
    if(a_b>EQUL_ZONE){
        return TRUE;
    }else{
        return FALSE;
    }
}

uint8_t findMaxKsi(float * vec, uint16_t * cnfg, uint8_t * manyMax){
    float ksiMax = -1.0;
    uint8_t nMax = 0;
    _Bool existMax = FALSE;

    for(uint8_t i=1;i<=plnm->nEl;i++){
        if(rSmb(cnfg,i) == NORM){
            if(vec[i-1]>ksiMax){
                ksiMax = vec[i-1];
                existMax = TRUE;
            }
        }
    }

    if(!existMax)
        return 0;

    for(uint8_t i=1;i<=plnm->nEl;i++){
        if(rSmb(cnfg,i) == NORM && equl(vec[i-1],ksiMax) ){
            manyMax[nMax] = i;
            nMax++;
        }
    }
    return nMax;
}

uint8_t findRandKsi(float * vec, uint8_t nEl){
    float aKsi[N_EL_MAX];
    float summKsi=0;
    uint8_t i=0;
    for(i=0;i<nEl;i++){
        if(-2.0f<vec[i] && vec[i]<2.0f)
            summKsi=summKsi+vec[i];
    }
    aKsi[0] = 0.0;

    for(i=1;i<=nEl;i++){
        if(-2.0f<vec[i-1] && vec[i-1]<2.0f)
            aKsi[i] = aKsi[i-1] + vec[i-1]/summKsi;
        else
            aKsi[i] = aKsi[i-1];
    }
    uint8_t numResEl=0;
    float randVal=0.0;
    randVal = (float)(rand()%1000);
    randVal = randVal/1000;

    for(i=1;i<=nEl;i++){
        if(randVal<aKsi[i]){
            numResEl = i;
            break;
        }
    }
    return numResEl;
}

uint8_t firstMaxKsi(float * ksi, uint16_t * cnfg){
     float ksiMax = -1.0;
     uint8_t res = 0;
     for(uint8_t i=1;i<=plnm->nEl;i++){
         if(rSmb(cnfg,i) == NORM){
             if(ksi[i-1]>ksiMax){
                 ksiMax = ksi[i-1];
                 res = i;
             }
         }
     }
     return res;
}


_Bool checkCopyCnfg (const uint16_t * smbCnfg, uint16_t * smbCnfgStart){
    uint8_t iSmb = 0;
    for(uint8_t i=1;i<=plnm->nEl;i++){
        iSmb = rSmb(smbCnfg,i);
        if(iSmb == NORM){
            wSmb(smbCnfgStart, i, NORM/*UP*/);
        }else if (iSmb == DOWN){
            wSmb(smbCnfgStart, i, DOWN);
        }else{
            return FALSE; // задана некорректная smb_cnfg
        }
   }
   return TRUE;
}

uint8_t genrtSingCnfg(uint16_t * workCnfg,/* конфигурация разрешённых к включению элементов */
                         float * index,             /* ранг элементов для GRD_RNG           */
                       uint8_t * bitCnfg, /* выбранная рабочая конфигурация                 */
                       typeKsi   ksiType,         /* NEGATIV или POSITIV или FULL           */
                    typeChoice   choiceType    /* GRD_FST или GRD_LST или GRD_IND           */){

    uint16_t smbCnfgStart[N_BIT_MAP]; // копируем элементы из smb_cnfg в smb_cnfg_start
    float vecKsi[N_EL_MAX];

    if(!(checkCopyCnfg (workCnfg, smbCnfgStart))){ // проверка и запись cnfg_start
        return 1;
    }

    for(uint8_t j=0;j<N_BIT_MAP;j++){ // на всякий случай обнулим конфигурации
        wByte(&(bitCnfg[j]), 0x00); // вдруг этого не сделали в предыдущей функции
    }

    if (culcP(smbCnfgStart)<EQUL_ZONE){  // проверка работоспоосбности, если включить все раб-сп-е элементы
        return 0;                                // рабочей конфигурации не существует
    }
    // ------------------------- ОСНВНОЙ ЦИКЛ ----------------------------------
    for(uint8_t step=0;step<plnm->nEl;step++){
        culcAllKsi(smbCnfgStart, vecKsi, ksiType);
        //------------------- Выбираем элемент ---------------------------------
        uint8_t manyMax[N_EL_MAX];
        //Находим номер элемента у которого максимальное кси
        uint8_t nMax = findMaxKsi(vecKsi, smbCnfgStart, manyMax);
        uint8_t stepChoice = 0;
        float maxIndex = -1.0;

        if(nMax == 1){ // если элемент один, то он выбирается
            stepChoice = manyMax[0];
        // если элементов несколько, то выбираем в зависимости от choice_type
        }else if (choiceType == GRD_FST || choiceType == GRD_LST){
            stepChoice = manyMax[0];
            for(uint8_t loc_i=1;loc_i<nMax;loc_i++)
                if(choiceType == GRD_FST && manyMax[loc_i] < stepChoice)
                    stepChoice = manyMax[loc_i];
                else if (choiceType == GRD_LST && manyMax[loc_i] > stepChoice)
                    stepChoice = manyMax[loc_i];
        }else if (choiceType == GRD_IND){
            stepChoice = manyMax[0];
            maxIndex = index[stepChoice];
            for(uint8_t loc_i=1;loc_i<nMax;loc_i++){
                if(index[manyMax[loc_i]] > maxIndex){
                    stepChoice = manyMax[loc_i];
                    maxIndex = index[manyMax[loc_i]];
                }
            }
        }

        wBit(bitCnfg, stepChoice, 1);
        //----------- Поднимаем флаг как UP и выбираем новый элемент
        wSmb(smbCnfgStart, stepChoice, UP);
        //если выбранный элемент позволил включить систему, то выход
        if(culcP(smbCnfgStart) + EQUL_ZONE > 1.0f){
            return step; // количество элементов вкл в контур
        }
    } //ОСНВНОЙ ЦИКЛ

    return 0; // если ничего так и не включилось то ошибка реализации функции
}
