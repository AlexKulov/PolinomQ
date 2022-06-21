//---------------------------------------------------------------------------

#include "strpolinom.h"

//------------------------- ОБЕСПЕЧИВАЮЩИЕ ФУНКЦИИ ---------------------------
void null_bit_cnfg(bit_configuration * cnfg)
{
   for(uint8_t i=0;i<N_BIT_MAP;i++)
       w_byte(&cnfg->adr[i],0x00);
}

void null_bit_map (uint8_t    * cnfg)
{
   for(uint8_t i=0;i<N_BIT_MAP;i++)
       cnfg[i] = 0x00;
}

void null_smb_map (uint16_t    * cnfg)
{
   for(uint8_t i=0;i<N_BIT_MAP;i++)
       cnfg[i] = 0x0000;
}

uint8_t r_smb(const uint16_t * str, uint8_t j) // начинаем считать с первого, а не с 0-го
{
  if(j>BIT_IN_BYTE*N_BIT_MAP || j<1){
          printf("Err r_smb");
          // exit(1); для отладки пусть так
  }

  for(uint8_t i=0;i<N_BIT_MAP;i++)
  {
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

uint8_t r_bit(uint8_t * str, uint8_t j) // начинаем считать с первого, а не с 0-го
{
  if(j>BIT_IN_BYTE*N_BIT_MAP || j<1){
          printf("Err r_bit\n");
          // exit(1); для отладки пусть так
  }

  for(uint8_t i=0;i<N_BIT_MAP;i++)
  {
     if(j == (1 + i*BIT_IN_BYTE))
        return str[i] & 0x01;
     else if(j == (2 + i*BIT_IN_BYTE))
        return (str[i]>>1) & 0x01;
     else if(j == (3 + i*BIT_IN_BYTE))
        return (str[i]>>2) & 0x01;
     else if(j == (4 + i*BIT_IN_BYTE))
        return (str[i]>>3) & 0x01;
     else if(j == (5 + i*BIT_IN_BYTE))
        return (str[i]>>4) & 0x01;
     else if(j == (6 + i*BIT_IN_BYTE))
        return (str[i]>>5) & 0x01;
     else if(j == (7 + i*BIT_IN_BYTE))
        return (str[i]>>6) & 0x01;
     else if(j == (8 + i*BIT_IN_BYTE))
        return (str[i]>>7) & 0x01;
  }
  return 0;
}

void          w_bit(uint8_t * str, uint8_t j, uint8_t bit)
{
  if(j>BIT_IN_BYTE*N_BIT_MAP || j<1){
        printf("Err w_bit: j - incorrect\n");
        // exit(1); для отладки пусть так
  }

  if(bit>1){
        printf("Err w_bit: bit - incorrect\n");
        // exit(1); для отладки пусть так
  }

  if(j<=BIT_IN_BYTE*N_BIT_MAP && j>0)
    for(uint8_t i=0;i<N_BIT_MAP;i++)
    {
     if(j == (1 + i*BIT_IN_BYTE))
        {bit==1 ? (str[i] |= 0x01) : (str[i] &= ~0x01); break;}
     else if (j == (2 + i*BIT_IN_BYTE))
        {bit==1 ? (str[i] |= 0x02) : (str[i] &= ~0x02); break;}
     else if (j == (3 + i*BIT_IN_BYTE))
        {bit==1 ? (str[i] |= 0x04) : (str[i] &= ~0x04); break;}
     else if (j == (4 + i*BIT_IN_BYTE))
        {bit==1 ? (str[i] |= 0x08) : (str[i] &= ~0x08); break;}
     else if (j == (5 + i*BIT_IN_BYTE))
        {bit==1 ? (str[i] |= 0x10) : (str[i] &= ~0x10); break;}
     else if (j == (6 + i*BIT_IN_BYTE))
        {bit==1 ? (str[i] |= 0x20) : (str[i] &= ~0x20); break;}
     else if (j == (7 + i*BIT_IN_BYTE))
        {bit==1 ? (str[i] |= 0x40) : (str[i] &= ~0x40); break;}
     else if (j == (8 + i*BIT_IN_BYTE))
        {bit==1 ? (str[i] |= 0x80) : (str[i] &= ~0x80); break;}
    }
}
void          w_smb(uint16_t * str, uint8_t j, uint8_t smb)
{
  if(j==0){
        printf("Err w_smb: j - incorrect\n");
        // exit(1); для отладки пусть так
  }

  if(smb>2){
        printf("Err w_smb: smb - incorrect\n");
        // exit(1); для отладки пусть так
  }

  if(j<=BIT_IN_BYTE*N_BIT_MAP && j>0)
    for(uint8_t i=0;i<N_BIT_MAP;i++)
    {
     if(j == (1 + i*BIT_IN_BYTE)){
              if(1 == smb){str[i] |=  0x0001; str[i] &= ~0x0002;}
         else if(0 == smb) str[i] &= ~0x0003;
         else             {str[i] |=  0x0002; str[i] &= ~0x0001;}
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

int8_t r_sig(uint8_t head)
{
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

uint8_t r_leight(uint8_t head)
{
    uint8_t Leight = (head>>2) & 0x3f;
    return Leight;
}

void w_sig(uint8_t * head, int8_t InSign)
{
    if(!(-1 == InSign || 1 == InSign)){
          printf("Err w_sig: InSign - incorrect\n");
          // exit(1); для отладки пусть так
    }
    uint8_t loc_head = *head;
    if(-1 == InSign){
        loc_head = loc_head | 0x03;
        *head = loc_head;
    }
    else if(1 == InSign){
        loc_head = loc_head | 0x01;
        *head = loc_head;
    }
}

void w_leight(uint8_t * head, uint8_t InLeight)
{
    if(InLeight>0x3f){
          printf("Err w_leight: InLeight - incorrect\n");
          // exit(1); для отладки пусть так
    }
    uint8_t loc_sign = *head & 0x03;
    * head = (uint8_t)(InLeight<<2);
    if(0x01 == loc_sign)
        w_sig(head,1);
    else if(0x03 == loc_sign)
        w_sig(head,-1);
    else{
        printf("Err w_leight: head.sig - incorrect\n");
        // exit(1); для отладки пусть так
    }
}

void        w_byte(uint8_t * str, uint8_t byte)
{
    if(str == 0){
        printf("Err w_byte\n");
        //exit(1);
    }
    * str = byte;
}

uint8_t r_byte(uint8_t * str)
{
    if(str == 0){
        printf("Err r_byte\n");
        //exit(1);
    }
    uint8_t ResByte = (* str);
    return ResByte;
}

_Bool     w_cnfg(uint8_t * bit_cnfg, const uint8_t * char_cnfg)
{
  for(uint8_t i=0;i<N_BIT_MAP;i++)
        w_byte(&bit_cnfg[i], char_cnfg[i]);
  return 0;
}

_Bool     copy(uint8_t * cnfg1, const uint8_t * cnfg2)
{
  for(uint8_t i=0;i<N_BIT_MAP;i++)
        cnfg1[i] = cnfg2[i];
  return 0;
}

_Bool     copy_smb(uint16_t * cnfg1, const uint16_t * cnfg2)
{
  for(uint8_t i=0;i<N_BIT_MAP;i++)
        cnfg1[i] = cnfg2[i];
  return 0;
}

void bit_to_smb (uint8_t * bit_cnfg, uint16_t * smb_cnfg)
{
    for(uint8_t i=1; i<=N_EL_MAX;i++){
        if(r_bit(bit_cnfg,i) == 1)
           w_smb(smb_cnfg,i,NORM);
        else
           w_smb(smb_cnfg,i,DOWN);
    }
}

void BitToSmb2 (uint8_t * bit_cnfg, uint16_t * smb_cnfg)
{
    for(uint8_t i=1; i<=N_EL_MAX;i++)
        if(r_bit(bit_cnfg,i) == P)
           w_smb(smb_cnfg,i,UP);
        else
           w_smb(smb_cnfg,i,DOWN);
}

void SetCnfg(uint8_t * bit_cnfg, uint8_t EnOrDis)
{
    if(ENBL == EnOrDis){
        for(uint8_t i=0;i<N_BIT_MAP;i++)
              w_byte(&bit_cnfg[i], 0xff);
    }
    else if (DISBL == EnOrDis) {
        for(uint8_t i=0;i<N_BIT_MAP;i++)
              w_byte(&bit_cnfg[i], 0x00);
    }
}

polinom * plnm; //
//-------------------------------- ОСНОВНЫЕ ФУНКЦИИ -------------------------------
void PolinomInit       (polinom * p_plnm)
{
    plnm  = (polinom *) p_plnm;
}

void polinom_init       (void * p_plnm, uint16_t in_n_str)
{
  plnm  = (polinom *) p_plnm;
  plnm->n_str = in_n_str;
  for(uint16_t i=0; i<plnm->n_str; i++)
  {
    w_sig(&plnm->strings[i].head, 1);
    w_leight(&plnm->strings[i].head, 0);
    for(uint8_t j=1; j<=N_EL_MAX; j++)
    {
      w_bit(plnm->strings[i].exist_pq,j,FALSE);
      w_bit(plnm->strings[i].p_or_q,j,Q);
    }
  }
}

//------------------------------- ОБЕСП. ФУНКЦИЯ ---------------------------------
float culc_p_str(string_of_polinom* str, uint16_t * work_conf)
{
  float p_str_res = 1.0;
  uint8_t k=0;

  for(uint8_t i=1;i<=N_EL_MAX;i++)
  {
     if(r_bit(str->exist_pq,i)) // может быть три варианта в зависимости от bit_work_conf[i] и p_or_q
     {
       if(r_smb(work_conf, i) == NORM)
            p_str_res=p_str_res * 0.5f;
       else if(r_smb(work_conf, i) == DOWN && r_bit(str->p_or_q,i) == P)
		   return 0.0;
       else if(r_smb(work_conf, i) == UP   && r_bit(str->p_or_q,i) == Q)
		   return 0.0;
       k++;
     }
     if(k >= r_leight(str->head))
       return p_str_res;
  }
  return p_str_res;
}
//-----------------------------------------------------------------------------------
float culc_p    (uint16_t * work_conf)
{
    float p_res = 0.0;
    for(uint16_t i=0; i<plnm->n_str; i++){
        if(r_sig(plnm->strings[i].head) == 1)
            p_res = p_res + culc_p_str(&(plnm->strings[i]), work_conf);
        else if(r_sig(plnm->strings[i].head) == -1)
            p_res = p_res - culc_p_str(&(plnm->strings[i]), work_conf);
    }
    return p_res;
}

//------------------------------- ОБЕСП. ФУНКЦИЯ ---------------------------------
double culc_pt_str(string_of_polinom * str, double * pt_el)
{
   double p_str_res = 1.0;
   uint8_t k=0;

   for(uint8_t i=1;i<=plnm->n_el;i++){
      if(r_bit(str->exist_pq,i))
      {
        if(r_bit(str->p_or_q,i) == P)
                p_str_res=p_str_res*pt_el[i-1];
        else if(r_bit(str->p_or_q,i) == Q)
                p_str_res=p_str_res*(1.0-pt_el[i-1]);
        k++;
      }
      if(k >= r_leight(str->head) )
        return p_str_res;
   }
   return p_str_res;
}

double culc_pt      (double * pt_el)
{
    double p_res = 0.0;

    for(uint16_t i=0; i<plnm->n_str; i++){
        if(r_sig(plnm->strings[i].head) == 1)
                p_res = p_res + culc_pt_str(&(plnm->strings[i]), pt_el);
        else if(r_sig(plnm->strings[i].head) == -1)
                p_res = p_res - culc_pt_str(&(plnm->strings[i]), pt_el);
    }
    return p_res;
}

float culc_ksi      (uint16_t * work_cnfg, uint8_t j, enum type_ksi type)
{
      float ksi = 0.0;
      if(j>plnm->n_el)
         return -2.0; // ошибка, j задана некорректно

      if(type == NEGATIV){
             ksi = culc_p(work_cnfg);
             w_smb(work_cnfg, j,DOWN);
             ksi = ksi - culc_p(work_cnfg);
             w_smb(work_cnfg, j,NORM);
             return ksi;
      }
      else if(type == POSITIV){
             w_smb(work_cnfg, j,UP);
             ksi = culc_p(work_cnfg);
             w_smb(work_cnfg, j,NORM);
             ksi = ksi - culc_p(work_cnfg);
             return ksi;
      }
      else if(type == FULL){
             w_smb(work_cnfg, j,UP);
             ksi = culc_p(work_cnfg);
             w_smb(work_cnfg, j,DOWN);
             ksi = ksi - culc_p(work_cnfg);
             w_smb(work_cnfg, j,NORM);
             return ksi;
      }
      return 0.0;
}
// договоримся что кси которые не надо вычислять будут заданы значениями 2 и -2 в зависимости от
// работоспособен этот элемент или уже включен в конфигурацию
void culc_all_ksi  (uint16_t * work_cnfg, float * all_ksi, enum type_ksi type)
{
    for(uint8_t i=1;i<=plnm->n_el;i++) // i должны начинаться с 1 и до n_el включительно
    {
       if(r_smb(work_cnfg, i) == NORM)
         all_ksi[i-1] = culc_ksi(work_cnfg, i, type);
       else if(r_smb(work_cnfg, i) == DOWN)
         all_ksi[i-1] = -2.0;
       else if(r_smb(work_cnfg, i) == UP)
         all_ksi[i-1] = 2.0;
    }
}

#define EQUL_ZONE (5E-7f)

_Bool equl(float f1,float f2)
{
   float m_f = (f1+f2)/2.0f;
   if(f1>= (m_f - EQUL_ZONE) &&  f1<= (m_f + EQUL_ZONE) &&
      f2>= (m_f - EQUL_ZONE) &&  f2<= (m_f + EQUL_ZONE))
      return TRUE;
   else
      return FALSE;
}

_Bool a_more_b (float a, float b) //т.е b< а <= b+eps
{
    float a_b = a - b;

    if(a_b>EQUL_ZONE)
        return TRUE;
    else
        return FALSE;
}

uint8_t find_max_ksi(float * vec, uint16_t * cnfg, uint8_t * many_max)
{
  float ksi_max = -1.0;
  uint8_t n_max = 0;
  _Bool exist_max = FALSE;

  for(uint8_t i=1;i<=plnm->n_el;i++)
     if(r_smb(cnfg,i) == NORM)
        if(vec[i-1]>ksi_max)
        {
          ksi_max = vec[i-1];
          exist_max = TRUE;
        }

  if(!exist_max)
        return 0;

  for(uint8_t i=1;i<=plnm->n_el;i++)
     if(r_smb(cnfg,i) == NORM && equl(vec[i-1],ksi_max) )
     {
       many_max[n_max] = i;
       n_max++;
     }
  return n_max;
}

#include <stdlib.h> // функция rand() см. ниже

uint8_t FindRandKsi(float * vec, uint8_t n_el)
{
    float a_ksi[N_EL_MAX];
    float SummKsi=0;
    uint8_t i=0;
    for(i=0;i<n_el;i++){
        if(-2.0f<vec[i] && vec[i]<2.0f)
            SummKsi=SummKsi+vec[i];
    }
    a_ksi[0] = 0.0;

    for(i=1;i<=n_el;i++){
        if(-2.0f<vec[i-1] && vec[i-1]<2.0f)
            a_ksi[i] = a_ksi[i-1] + vec[i-1]/SummKsi;
        else
            a_ksi[i] = a_ksi[i-1];
    }
    uint8_t NumResEl=0;
    float RandVal=0.0;

    RandVal = (float)(rand()%1000);
    RandVal = RandVal/1000;

    for(i=1;i<=n_el;i++){
        if(RandVal<a_ksi[i]){
            NumResEl = i;
            break;
        }
    }

    return NumResEl;
}

uint8_t first_max_ksi(float * ksi, uint16_t * cnfg)
{
     float ksi_max = -1.0;
     uint8_t res = 0;
     for(uint8_t i=1;i<=plnm->n_el;i++)
        if(r_smb(cnfg,i) == NORM)
           if(ksi[i-1]>ksi_max){
                ksi_max = ksi[i-1];
                res = i;
           }
     return res;
}


_Bool check_copy_cnfg (const uint16_t * smb_cnfg, uint16_t * smb_cnfg_start)
{
   uint8_t smb_i = 0;
   for(uint8_t i=1;i<=plnm->n_el;i++)
   {
      smb_i = r_smb(smb_cnfg,i);
      if(smb_i == NORM)
        w_smb(smb_cnfg_start, i, NORM/*UP*/);
      else if (smb_i == DOWN)
        w_smb(smb_cnfg_start, i, DOWN);
      else
        return FALSE; // задана некорректная smb_cnfg
   }
   return TRUE;
}

uint8_t genrt_sing_cnfg(uint16_t * work_cnfg,/* конфигурация разрешённых к включению элементов */
                              float * index,             /* ранг элементов для GRD_RNG                     */
                              uint8_t * bit_cnfg, /* выбранная рабочая конфигурация                 */
                              enum type_ksi ksi_type,         /* NEGATIV или POSITIV или FULL                 */
                              enum type_choice choice_type    /* GRD_FST или GRD_LST или GRD_IND                */
                              )
{
   uint16_t smb_cnfg_start[N_BIT_MAP]; // копируем элементы из smb_cnfg в smb_cnfg_start
   float vec_ksi[N_EL_MAX];

   if(!(check_copy_cnfg (work_cnfg, smb_cnfg_start))) // проверка и запись cnfg_start
        return 1;

   for(uint8_t j=0;j<N_BIT_MAP;j++) // на всякий случай обнулим конфигурации
        w_byte(&(bit_cnfg[j]), 0x00); // вдруг этого не сделали в предыдущей функции

   if (culc_p(smb_cnfg_start)<EQUL_ZONE)  // проверка работоспоосбности, если включить все раб-сп-е элементы
       return 0;                                // рабочей конфигурации не существует
// ------------------------- ОСНВНОЙ ЦИКЛ ----------------------------------
 for(uint8_t step=0;step<plnm->n_el;step++)
 {
   culc_all_ksi(smb_cnfg_start, vec_ksi, ksi_type);
   //------------------- Выбираем элемент ---------------------------------
   uint8_t many_max[N_EL_MAX];
   //Находим номер элемента у которого максимальное кси
   uint8_t n_max = find_max_ksi(vec_ksi, smb_cnfg_start, many_max);
   uint8_t step_choice = 0;
   float max_index = -1.0;

   if(n_max == 1) // если элемент один, то он выбирается
      step_choice = many_max[0];
   // если элементов несколько, то выбираем в зависимости от choice_type
   else if (choice_type == GRD_FST || choice_type == GRD_LST)
   {
      step_choice = many_max[0];
      for(uint8_t loc_i=1;loc_i<n_max;loc_i++)
        if(choice_type == GRD_FST && many_max[loc_i] < step_choice)
           step_choice = many_max[loc_i];
        else if (choice_type == GRD_LST && many_max[loc_i] > step_choice)
           step_choice = many_max[loc_i];
   }
   else if (choice_type == GRD_IND)
   {
      step_choice = many_max[0];
      max_index = index[step_choice];
      for(uint8_t loc_i=1;loc_i<n_max;loc_i++)
          if(index[many_max[loc_i]] > max_index)
          {
             step_choice = many_max[loc_i];
             max_index = index[many_max[loc_i]];
          }
   }

   w_bit(bit_cnfg, step_choice, 1);
 //----------- Поднимаем флаг как UP и выбираем новый элемент
   w_smb(smb_cnfg_start, step_choice, UP);
 // если выбранный элемент позволил включить систему, то выход
   if(culc_p(smb_cnfg_start) + EQUL_ZONE > 1.0f)
      return step; // количество элементов вкл в контур
 } //ОСНВНОЙ ЦИКЛ

  return 0; // если ничего так и не включилось то ошибка реализации функции
}





