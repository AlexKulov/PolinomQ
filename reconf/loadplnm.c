#include <string.h>
#include "strpolinom.h"

#define SD_MAX 32
#define SMB_FOR_SIG 8

typedef struct
{
    uint16_t SW; //service word
    uint8_t DW[SD_MAX][2]; //data word

}PlnmToSend;

uint16_t TransPlnmToMsg(polinom * plnm, PlnmToSend * mko_send)
{
  uint16_t no_ks = 0;
  uint16_t no_sd = 0;
  _Bool left_part = FALSE;
//----------------------------- Записываем хедеры всех строк ---------------------------------
  for (uint16_t str_i=0; str_i<plnm->n_str; str_i++)
  {
       if(str_i%2 == 0)
       {
          mko_send[no_ks].DW[no_sd][0] = plnm->strings[str_i].head;
          left_part = TRUE;
       }
       else
       {
          mko_send[no_ks].DW[no_sd][1] = plnm->strings[str_i].head;
          left_part = FALSE;
          no_sd++;
          if(no_sd>SD_MAX-1)
          { no_sd = 0; no_ks++;}
       }
  }
//-------------------------------------------------------------------------------------
  if(no_sd >0 || (no_sd == 0 && left_part))
      {no_ks++; no_sd=0;} // переходим к следующему КС, прпускаем нулевые СД
  left_part = FALSE;
//----------------------------- Пакуем битовые карты... ---------------------------------
  for (uint16_t str_i=0; str_i<plnm->n_str; str_i++)
  {
     for (uint8_t bm_j=0; bm_j<N_BIT_MAP; bm_j++)
     {
       if(!left_part)
       {
          mko_send[no_ks].DW[no_sd][0] = plnm->strings[str_i].exist_pq[bm_j];
          left_part = TRUE;
       }
       else
       {
          mko_send[no_ks].DW[no_sd][1] = plnm->strings[str_i].exist_pq[bm_j];
          left_part = FALSE;
          no_sd++;
          if(no_sd>SD_MAX-1)
          { no_sd = 0; no_ks++;}
       }
     }
  }
//-------------------------------------------------------------------------------------
  if(no_sd >0 || (no_sd == 0 && left_part))
      {no_ks++; no_sd=0;} // переходим к следующему КС, прпускаем нулевые СД
  left_part = FALSE;
//----------------------------- Пакуем символьные карты... ---------------------------------
  for (uint16_t str_i=0; str_i<plnm->n_str; str_i++)
  {
     for (uint8_t sm_j=0; sm_j<N_BIT_MAP; sm_j++)
     {
       if(!left_part)
       {
          mko_send[no_ks].DW[no_sd][0] = plnm->strings[str_i].p_or_q[sm_j];
          left_part = TRUE;
       }
       else
       {
          mko_send[no_ks].DW[no_sd][1] = plnm->strings[str_i].p_or_q[sm_j];
          left_part = FALSE;
          no_sd++;
          if(no_sd>SD_MAX-1)
          { no_sd = 0; no_ks++;}
       }
     }
  }

  if(no_sd >0)
      {no_ks++; no_sd=0;}
  no_ks++;
  uint16_t       hSD = plnm->n_str;
                 hSD = hSD & 0x00ff;
  mko_send[no_ks].DW[0][0] = (uint8_t)hSD;
                 hSD = plnm->n_str;
                 hSD = hSD>>8; // ����� hSD = hSD<<8; ?
  mko_send[no_ks].DW[0][1] = (uint8_t)hSD;

  mko_send[no_ks].DW[1][0] = plnm->n_el;

  return no_ks;
}

uint16_t  LoadPlnmFromMsg (PlnmToSend * mko_send, polinom * plnm, uint16_t l_str)
{
   uint16_t n_ks = 0;
   uint16_t j_sd = 0;
   _Bool left_part = FALSE;
//1. Считываение всех хедеров l_str байт
   for(uint16_t i=0; i<l_str;)
   {
      plnm->strings[i].head = mko_send[n_ks].DW[j_sd][0];
      i++;
      left_part = TRUE;
      if(i>=l_str) break;

      plnm->strings[i].head = mko_send[n_ks].DW[j_sd][1];
      i++;
      left_part = FALSE;
      j_sd++;
      if(j_sd>SD_MAX-1)
         {j_sd = 0; n_ks++; }
   }
// окончание считывания хедеров
   if(j_sd >0 || (j_sd == 0 && left_part))
      {n_ks++; j_sd=0;} // переходим к следующему КС, прпускаем нулевые СД
//2. Считываение всех битовых карт существоания элемента exist
   left_part = FALSE;

   for(uint16_t i=0; i<l_str; i++)
   {
      for(uint8_t j=0; j<N_BIT_MAP; j++)
      {
         if(!left_part){
                plnm->strings[i].exist_pq[j] = mko_send[n_ks].DW[j_sd][0];
                left_part = TRUE;
         }
         else{
                plnm->strings[i].exist_pq[j] = mko_send[n_ks].DW[j_sd][1];
                left_part = FALSE;
                j_sd++;
                if(j_sd>SD_MAX-1)
                {j_sd = 0; n_ks++;}
         }
      }
   } // конец for, окончание считывания битовых карт

   if(j_sd >0 || (j_sd == 0 && left_part))
      {n_ks++; j_sd=0;} // переходим к следующему КС, прпускаем нулевые СД
//3. Считываение всех символьные карт l_str*N_SMB_MAP байт
   //struct bit_map * p_smb_map_byte;
   left_part = FALSE;

   for(uint16_t i=0; i<l_str;i++)
   {
      for(uint8_t j=0; j<N_BIT_MAP; j++)
      {
          if(!left_part){
                 plnm->strings[i].p_or_q[j] = mko_send[n_ks].DW[j_sd][0];
                 left_part = TRUE;
          }
          else{
                 plnm->strings[i].p_or_q[j] = mko_send[n_ks].DW[j_sd][1];
                 left_part = FALSE;
                 j_sd++;
                 if(j_sd>SD_MAX-1)
                 {j_sd = 0; n_ks++;}
          }
      }
   }

  return n_ks;
}

void LoadPlnmFromFile(char * PlnmFileName, polinom * Polinom)
{
    FILE * FilePolinom;
    FilePolinom=fopen(PlnmFileName,"r");
    if (FilePolinom!=NULL){
        uint16_t CntStr = 0; //number of string in Polinom
        char FailSmb;
        char StrRd[4*N_EL_MAX];
        uint16_t i;

        for(i=0;i<N_STR_MAX;i++){
            if(EOF == fscanf(FilePolinom,"%[^\n] %[\n]",StrRd,&FailSmb))
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
        uint8_t n_el = 0; //максимальный номер элемента
        uint8_t k,j;
        polinom_init((void*)/*&*/Polinom, CntStr);
        rewind(FilePolinom);

        for (uint16_t str_i=0; str_i<CntStr; str_i++){
            nmb_symb_end = 0;
            nmb_symb_beg = 0;
            pq_symb_cur = 0;
            flag_frst = TRUE;
            fscanf(FilePolinom,"%[^\n] %[\n]",StrRd,&FailSmb);
            str_lng_cur = 0;
            for (i=0;i<4*N_EL_MAX;i++) {
               if(StrRd[i] == 0)
                   break;
               str_lng_cur++;
            }
      //проверяем первые 6 символов на знак строки, пусть по умолчанию +
            w_sig(&Polinom->strings[str_i].head, 1);
            for (k=0; k<=SMB_FOR_SIG; k++){
                if (StrRd[k] == '+'){
                    w_sig(&Polinom->strings[str_i].head, 1);
                    break;
                }

                if (StrRd[k] == '-'){
                    w_sig(&Polinom->strings[str_i].head, -1);
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
                        w_bit (Polinom->strings[str_i].p_or_q, index_pq, P);
                    else if (StrRd[nmb_symb_beg] == 'Q')
                        w_bit (Polinom->strings[str_i].p_or_q, index_pq, Q);
                    w_bit (Polinom->strings[str_i].exist_pq, index_pq, TRUE); // записываем биты существования
                    nmb_symb_beg = nmb_symb_end;
                    count_smb_in_str++;
                    if(index_pq > n_el)
                        n_el =  index_pq;
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
               w_bit (Polinom->strings[str_i].p_or_q, index_pq, P);
           else if (StrRd[nmb_symb_beg] == 'Q')
               w_bit (Polinom->strings[str_i].p_or_q, index_pq, Q);
           w_bit (Polinom->strings[str_i].exist_pq, index_pq, TRUE); // записываем последний бит существования
           count_smb_in_str++;
           w_leight(&Polinom->strings[str_i].head, count_smb_in_str);
           count_smb_in_str=0;
           if(index_pq > n_el)
              n_el =  index_pq;
     }
     Polinom->n_el = n_el;
   }
   fclose(FilePolinom);
}

#define MAX_MODE 10

uint32_t FindConfiguration(polinom * STRUCTURE, bit_configuration * ManyConf, uint8_t NumDev, uint8_t NumMode)
{

     float KsiEl[N_EL_MAX];
     uint8_t    p_of_str[N_BIT_MAP];
     uint8_t conf_of_str[MAX_MODE][N_BIT_MAP];
     uint16_t  smb_of_str[N_BIT_MAP];
     uint16_t smb_for_chc[N_BIT_MAP];

     null_bit_map(   p_of_str);
     for(uint8_t j=0;j<MAX_MODE;j++)
        null_bit_map(&conf_of_str[j][0]);
     null_smb_map(smb_of_str);
     null_smb_map(smb_for_chc);


     _Bool tmb_conf_same = TRUE;
     uint8_t num_max_ksi = 0; //����������� ����������
     uint32_t num_rec = 0; //������� ��������� �� �������� ������������

     uint8_t cur = 0;//������� ������������ � ������

     for(uint16_t i=0;i<STRUCTURE->n_str;i++){
        if(r_sig(STRUCTURE->strings[i].head) == 1){

             for(uint8_t j=1;j<=NumDev;j++)
                 if(r_bit(STRUCTURE->strings[i].exist_pq,j) &&
                    r_bit(STRUCTURE->strings[i].p_or_q  ,j) == P){
                      w_bit(p_of_str,j,1);
                      w_smb(smb_of_str,j,NORM);
                 }
                 else{
                      w_bit(p_of_str,j,0);
                      w_smb(smb_of_str,j,DOWN);
                 }
            ///*
             for(uint8_t j=NumDev+1;j<=NumDev+NumMode;j++){
                 w_smb(smb_of_str,j,UP);
                 if(r_bit(STRUCTURE->strings[i].exist_pq,j) &&
                    r_bit(STRUCTURE->strings[i].p_or_q  ,j) == P)
                      w_bit(p_of_str,j,1);
                 else
                      w_bit(p_of_str,j,0);
             }

             cur=0;

             if(NumMode>0){
                for(uint8_t j=NumDev+1;j<=NumDev+NumMode;j++){
                        if(r_bit(p_of_str,j) == 0){
                           w_smb(smb_of_str,j,DOWN);
                           copy_smb(smb_for_chc,smb_of_str);
                           w_smb(smb_of_str,j,UP);
                           for(uint8_t j=1;j<=NumDev;j++){
                                culc_all_ksi(smb_for_chc,KsiEl,NEGATIV);

                                num_max_ksi = first_max_ksi(KsiEl, smb_for_chc);

                                if(num_max_ksi>0 && num_max_ksi<=NumDev)
                                        w_smb(smb_for_chc,num_max_ksi,UP);
                                else
                                        printf("Error choice max el in FindConf \n");

                                if(!(culc_p(smb_for_chc)<1.0f)){
                                        for(uint8_t x=1;x<=NumDev;x++)
                                                if(r_smb(smb_for_chc,x) == UP)
                                                        w_bit(conf_of_str[cur],x,1);
                                                else
                                                        w_bit(conf_of_str[cur],x,0);

                                        for(uint8_t x=NumDev+1;x<=NumDev+NumMode;x++)
                                                if(r_smb(smb_for_chc,x)==DOWN)
                                                        w_bit(conf_of_str[cur],x,1);
                                                else
                                                        w_bit(conf_of_str[cur],x,0);
                                        cur++;
                                        break;
                                }
                           }//for (1...N_BA)
                        }//p_of_str[j] == 0

                } //for(N_BA...N_MODE)
             }//N_MODE>0
             else{
                for(uint8_t j=1;j<=NumDev;j++){
                        culc_all_ksi(smb_of_str,KsiEl,NEGATIV);
                        num_max_ksi = first_max_ksi(KsiEl, smb_of_str);
                        if(num_max_ksi>0 && num_max_ksi<=NumDev)
                                w_smb(smb_of_str,num_max_ksi,UP);
                        else
                                printf("Error choice max el in FindConf \n");

                        if(!(culc_p(smb_of_str)<1.0f)){
                                for(uint8_t x=1;x<=NumDev;x++)
                                        if(r_smb(smb_of_str,x) == UP)
                                                w_bit(conf_of_str[cur],x,1);
                                        else
                                                w_bit(conf_of_str[cur],x,0);
                                break;
                        }
                }//for (1...N_BA)
                cur++;
             }//

             null_bit_map(   p_of_str);
             null_smb_map(smb_of_str);
                //2. �������� �� ��������

             for(uint16_t c=0;c<cur;c++){
                for(uint16_t k=0;k<num_rec;k++){
                        tmb_conf_same = TRUE;
                        for(uint8_t j=1;j<=STRUCTURE->n_el;j++){
                                if(r_bit(conf_of_str[c],j) != r_bit(ManyConf[k].adr,j)){
                                        tmb_conf_same = FALSE;
                                        break;
                                }
                        }
                        if(tmb_conf_same)
                                break;
                }

                if(!tmb_conf_same ||  num_rec==0){
                //3. ���������� ������������ ��� ���������
                        copy(ManyConf[num_rec].adr,conf_of_str[c]);
                               num_rec++;
                }
                null_bit_map(conf_of_str[c]);

             }//for(c...cur)
        }
     }//for(...i=0;i<...n_str;i++)

     return num_rec;
}
