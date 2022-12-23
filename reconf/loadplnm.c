#include "strpolinom.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h> // ������� atoi() ��. ����

#define MAX_DW (32)
#define SMB_FOR_SIG (8)

typedef struct PlnmToSend{
    uint16_t SW; //service word
    uint8_t DW[MAX_DW][2]; //data word

}PlnmToSend;

uint16_t transPlnmToMsg(Polinom * plnm, PlnmToSend * mkoSend){
    uint16_t noSw = 0; // ���-�� ��������� ����
    uint16_t noDw = 0; // ���-�� ���� ������
    _Bool leftPart = FALSE;
//----------------------------- Записываем хедеры всех строк ---------------------------------
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
//-------------------------------------------------------------------------------------
    if(noDw >0 || (noDw == 0 &&  leftPart)){
        noSw++;
        noDw=0;
    } // переходим к следующему КС, прпускаем нулевые СД
    leftPart = FALSE;
//----------------------------- Пакуем битовые карты... ---------------------------------
    for (uint16_t iStr=0; iStr<plnm->nStr; iStr++){
        for (uint8_t bm_j=0; bm_j<N_BIT_MAP; bm_j++){
            if(! leftPart){
                mkoSend[noSw].DW[noDw][0] = plnm->strings[iStr].existPQ[bm_j];
                leftPart = TRUE;
            }
            else{
                mkoSend[noSw].DW[noDw][1] = plnm->strings[iStr].existPQ[bm_j];
                leftPart = FALSE;
                noDw++;
                if(noDw>MAX_DW-1){
                    noDw = 0;
                    noSw++;
                }
            }
        }
    }
//-------------------------------------------------------------------------------------
    if(noDw >0 || (noDw == 0 &&  leftPart)){
        noSw++;
        noDw=0;
    } // переходим к следующему КС, прпускаем нулевые СД
    leftPart = FALSE;
//----------------------------- Пакуем символьные карты... ---------------------------------
    for (uint16_t iStr=0; iStr<plnm->nStr; iStr++){
        for (uint8_t sm_j=0; sm_j<N_BIT_MAP; sm_j++){
            if(! leftPart){
                mkoSend[noSw].DW[noDw][0] = plnm->strings[iStr].pq[sm_j];
                leftPart = TRUE;
            }
            else{
                mkoSend[noSw].DW[noDw][1] = plnm->strings[iStr].pq[sm_j];
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
                 hSD = hSD>>8; // ����� hSD = hSD<<8; ?
    mkoSend[noSw].DW[0][1] = (uint8_t)hSD;

    mkoSend[noSw].DW[1][0] = plnm->nEl;

    return noSw;
}

uint16_t  loadPlnmFromMsg (PlnmToSend * mko_send, Polinom * plnm, uint16_t l_str){
   uint16_t  nSw = 0;
   uint16_t jDw = 0;
   _Bool  leftPart = FALSE;
//1. Считываение всех хедеров l_str байт
   for(uint16_t i=0; i<l_str;)
   {
      plnm->strings[i].head = mko_send[ nSw].DW[jDw][0];
      i++;
       leftPart = TRUE;
      if(i>=l_str) break;

      plnm->strings[i].head = mko_send[ nSw].DW[jDw][1];
      i++;
       leftPart = FALSE;
      jDw++;
      if(jDw>MAX_DW-1)
         {jDw = 0;  nSw++; }
   }
// окончание считывания хедеров
   if(jDw >0 || (jDw == 0 &&  leftPart))
      { nSw++; jDw=0;} // переходим к следующему КС, прпускаем нулевые СД
//2. Считываение всех битовых карт существоания элемента exist
    leftPart = FALSE;

   for(uint16_t i=0; i<l_str; i++)
   {
      for(uint8_t j=0; j<N_BIT_MAP; j++)
      {
         if(! leftPart){
                plnm->strings[i].existPQ[j] = mko_send[ nSw].DW[jDw][0];
                 leftPart = TRUE;
         }
         else{
                plnm->strings[i].existPQ[j] = mko_send[ nSw].DW[jDw][1];
                 leftPart = FALSE;
                jDw++;
                if(jDw>MAX_DW-1)
                {jDw = 0;  nSw++;}
         }
      }
   } // конец for, окончание считывания битовых карт

   if(jDw >0 || (jDw == 0 &&  leftPart))
      { nSw++; jDw=0;} // переходим к следующему КС, прпускаем нулевые СД
//3. Считываение всех символьные карт l_str*N_SMB_MAP байт
   //struct bit_map * p_smb_map_byte;
    leftPart = FALSE;

   for(uint16_t i=0; i<l_str;i++)
   {
      for(uint8_t j=0; j<N_BIT_MAP; j++)
      {
          if(! leftPart){
                 plnm->strings[i].pq[j] = mko_send[ nSw].DW[jDw][0];
                  leftPart = TRUE;
          }
          else{
                 plnm->strings[i].pq[j] = mko_send[ nSw].DW[jDw][1];
                  leftPart = FALSE;
                 jDw++;
                 if(jDw>MAX_DW-1)
                 {jDw = 0;  nSw++;}
          }
      }
   }

  return  nSw;
}

void loadPlnmFromFile(char * PlnmFileName, Polinom * polinom)
{
    FILE * filePolinom;
    filePolinom=fopen(PlnmFileName,"r");
    if (filePolinom!=NULL){
        uint16_t CntStr = 0; //number of string in Polinom
        char FailSmb;
        char StrRd[4*N_EL_MAX];
        uint16_t i;

        for(i=0;i<N_STR_MAX;i++){
            if(EOF == fscanf(filePolinom,"%[^\n] %[\n]",StrRd,&FailSmb))
                break;
            CntStr++;
        }

        char Nomer[3]; //index of element, 000 to 999 or _00 to _99
        uint8_t nmb_symb_end = 0; //k текущий pq символ
        uint8_t nmb_symb_beg = 0; //h, предыдущий pq символ
        uint8_t pq_symb_cur = 0; //j_N
        uint8_t str_lng_cur = 0, last_smb =0; // str_lng_cur = a
        _Bool flag_frst = TRUE;  //n_bool
        uint8_t count_smb_in_str = 0;
        uint8_t index_pq = 0; //index_PQ
        uint8_t nEl = 0; //максимальный номер элемента
        uint8_t k,j;
        initPolinom(polinom, CntStr);
        rewind(filePolinom);

        for (uint16_t iStr=0; iStr<CntStr; iStr++){
            nmb_symb_end = 0;
            nmb_symb_beg = 0;
            pq_symb_cur = 0;
            flag_frst = TRUE;
            fscanf(filePolinom,"%[^\n] %[\n]",StrRd,&FailSmb);
            str_lng_cur = 0;
            for (i=0;i<4*N_EL_MAX;i++) {
               if(StrRd[i] == 0)
                   break;
               str_lng_cur++;
            }
      //проверяем первые 6 символов на знак строки, пусть по умолчанию +
            wSig(&polinom->strings[iStr].head, 1);
            for (k=0; k<=SMB_FOR_SIG; k++){
                if (StrRd[k] == '+'){
                    wSig(&polinom->strings[iStr].head, 1);
                    break;
                }

                if (StrRd[k] == '-'){
                    wSig(&polinom->strings[iStr].head, -1);
                    break;
                }
            }

            for(j=0; j<str_lng_cur; j++){
                if (StrRd[j] == 'P')
                    pq_symb_cur = j;
                else if (StrRd[j] == 'Q')
                    pq_symb_cur = j;

                if (pq_symb_cur > nmb_symb_beg && flag_frst){
                           nmb_symb_beg = pq_symb_cur;
                           flag_frst = FALSE;
                }

                if (pq_symb_cur > nmb_symb_beg){
                    nmb_symb_end = pq_symb_cur;
                    nmb_symb_beg++;
                    Nomer[0]=0;Nomer[1]=0;Nomer[2]=0;
                    strncpy(Nomer, &StrRd[nmb_symb_beg],nmb_symb_end-nmb_symb_beg-1);
                    if(atoi(Nomer)>255)
                        printf("Err LoadPlnmFromFile: atoi(Nomer)>255\n");
                    index_pq = (uint8_t)atoi(Nomer);
                    nmb_symb_beg--;
                    if (StrRd[nmb_symb_beg] == 'P')
                        wBit (polinom->strings[iStr].pq, index_pq, P);
                    else if (StrRd[nmb_symb_beg] == 'Q')
                        wBit (polinom->strings[iStr].pq, index_pq, Q);
                    wBit (polinom->strings[iStr].existPQ, index_pq, TRUE); // записываем биты существования
                    nmb_symb_beg = nmb_symb_end;
                    count_smb_in_str++;
                    if(index_pq > nEl)
                        nEl =  index_pq;
                }
           }
           nmb_symb_beg++;
           Nomer[0] = '0'; Nomer[1] = 0; Nomer[2] = 0;
           //---------------- ����� �������� � � ������� � ����� ������ � ��� ----
           if(StrRd[str_lng_cur-1]=='+' || StrRd[str_lng_cur-1]=='-')
              last_smb = 1;
           else last_smb = 0;
           //---------------------------------------------------------------------
           strncpy(Nomer, &StrRd[nmb_symb_beg],str_lng_cur-nmb_symb_beg-last_smb);
           if(atoi(Nomer)>255)
               printf("Err LoadPlnmFromFile: atoi(Nomer)>255\n");
           index_pq = (uint8_t)atoi(Nomer);
           if(index_pq == 0){
               printf("ER: read NULL el of pol in \n");
               printf("LoadPolinomFromFile \n");
           }

           nmb_symb_beg--;
           if (StrRd[nmb_symb_beg] == 'P')
               wBit (polinom->strings[iStr].pq, index_pq, P);
           else if (StrRd[nmb_symb_beg] == 'Q')
               wBit (polinom->strings[iStr].pq, index_pq, Q);
           wBit (polinom->strings[iStr].existPQ, index_pq, TRUE); // записываем последний бит существования
           count_smb_in_str++;
           wLeight(&polinom->strings[iStr].head, count_smb_in_str);
           count_smb_in_str=0;
           if(index_pq > nEl)
              nEl =  index_pq;
     }
     polinom->nEl = nEl;
   }
   fclose(filePolinom);
}

#define MAX_MODE 10

uint32_t findConfiguration(Polinom * polinom, BitConfiguration * ManyConf, uint8_t NumDev, uint8_t NumMode)
{

     float KsiEl[N_EL_MAX];
     uint8_t    p_of_str[N_BIT_MAP];
     uint8_t conf_of_str[MAX_MODE][N_BIT_MAP];
     uint16_t  smb_of_str[N_BIT_MAP];
     uint16_t smb_for_chc[N_BIT_MAP];

     nullBitCnfg(   p_of_str);
     for(uint8_t j=0;j<MAX_MODE;j++)
        nullBitCnfg(&conf_of_str[j][0]);
     nullSmbCnfg(smb_of_str);
     nullSmbCnfg(smb_for_chc);


     _Bool tmb_conf_same = TRUE;
     uint8_t num_max_ksi = 0; //����������� ����������
     uint32_t num_rec = 0; //������� ��������� �� �������� ������������

     uint8_t cur = 0;//������� ������������ � ������

     for(uint16_t i=0;i<polinom->nStr;i++){
        if(rSig(polinom->strings[i].head) == 1){

             for(uint8_t j=1;j<=NumDev;j++)
                 if(rBit(polinom->strings[i].existPQ,j) &&
                    rBit(polinom->strings[i].pq  ,j) == P){
                      wBit(p_of_str,j,1);
                      wSmb(smb_of_str,j,NORM);
                 }
                 else{
                      wBit(p_of_str,j,0);
                      wSmb(smb_of_str,j,DOWN);
                 }
            ///*
             for(uint8_t j=NumDev+1;j<=NumDev+NumMode;j++){
                 wSmb(smb_of_str,j,UP);
                 if(rBit(polinom->strings[i].existPQ,j) &&
                    rBit(polinom->strings[i].pq  ,j) == P)
                      wBit(p_of_str,j,1);
                 else
                      wBit(p_of_str,j,0);
             }

             cur=0;

             if(NumMode>0){
                for(uint8_t j=NumDev+1;j<=NumDev+NumMode;j++){
                        if(rBit(p_of_str,j) == 0){
                           wSmb(smb_of_str,j,DOWN);
                           copySmb(smb_for_chc,smb_of_str);
                           wSmb(smb_of_str,j,UP);
                           for(uint8_t j=1;j<=NumDev;j++){
                                culcAllKsi(smb_for_chc,KsiEl,NEGATIV);

                                num_max_ksi = firstMaxKsi(KsiEl, smb_for_chc);

                                if(num_max_ksi>0 && num_max_ksi<=NumDev)
                                        wSmb(smb_for_chc,num_max_ksi,UP);
                                else
                                        printf("Error choice max el in FindConf \n");

                                if(!(culcP(smb_for_chc)<1.0f)){
                                        for(uint8_t x=1;x<=NumDev;x++)
                                                if(rSmb(smb_for_chc,x) == UP)
                                                        wBit(conf_of_str[cur],x,1);
                                                else
                                                        wBit(conf_of_str[cur],x,0);

                                        for(uint8_t x=NumDev+1;x<=NumDev+NumMode;x++)
                                                if(rSmb(smb_for_chc,x)==DOWN)
                                                        wBit(conf_of_str[cur],x,1);
                                                else
                                                        wBit(conf_of_str[cur],x,0);
                                        cur++;
                                        break;
                                }
                           }//for (1...N_BA)
                        }//p_of_str[j] == 0

                } //for(N_BA...N_MODE)
             }//N_MODE>0
             else{
                for(uint8_t j=1;j<=NumDev;j++){
                        culcAllKsi(smb_of_str,KsiEl,NEGATIV);
                        num_max_ksi = firstMaxKsi(KsiEl, smb_of_str);
                        if(num_max_ksi>0 && num_max_ksi<=NumDev)
                                wSmb(smb_of_str,num_max_ksi,UP);
                        else
                                printf("Error choice max el in FindConf \n");

                        if(!(culcP(smb_of_str)<1.0f)){
                                for(uint8_t x=1;x<=NumDev;x++)
                                        if(rSmb(smb_of_str,x) == UP)
                                                wBit(conf_of_str[cur],x,1);
                                        else
                                                wBit(conf_of_str[cur],x,0);
                                break;
                        }
                }//for (1...N_BA)
                cur++;
             }//

             nullBitCnfg(p_of_str);
             nullSmbCnfg(smb_of_str);
                //2. �������� �� ��������

             for(uint16_t c=0;c<cur;c++){
                for(uint16_t k=0;k<num_rec;k++){
                        tmb_conf_same = TRUE;
                        for(uint8_t j=1;j<=polinom->nEl;j++){
                                if(rBit(conf_of_str[c],j) != rBit(ManyConf[k] ,j)){
                                        tmb_conf_same = FALSE;
                                        break;
                                }
                        }
                        if(tmb_conf_same)
                                break;
                }

                if(!tmb_conf_same ||  num_rec==0){
                //3. ���������� ������������ ��� ���������
                        copy(ManyConf[num_rec] ,conf_of_str[c]);
                               num_rec++;
                }
                nullBitCnfg(conf_of_str[c]);

             }//for(c...cur)
        }
     }//for(...i=0;i<...n_str;i++)

     return num_rec;
}
