#ifndef STRPOLINOM_H
#define STRPOLINOM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif
/****** N_EL_MAX - multiple 8 *******/
#define N_EL_MAX  40
#define BIT_IN_BYTE 8
#define N_BIT_MAP (N_EL_MAX/BIT_IN_BYTE)
#define N_STR_MAX 10000
#define MAX_CNFG N_STR_MAX

enum          {      Q = 0,           P = 1           }; // для p_or_q
enum          {  DISBL = 0,        ENBL = 1           };
enum          {   DOWN = 0,        NORM = 1,    UP = 2};
enum          {   ZERO = 0,        PLUS = 1, MINUS = 3};
enum type_ksi {NEGATIV = 0, POSITIV = 1,  FULL = 2};
enum type_choice {GRD_RND = 2, RND = 3, GRD_FST=4, GRD_LST=5, GRD_IND =6};

//typedef struct bit_map bit_configuration [N_BIT_MAP];
typedef struct
{
  uint8_t adr [N_BIT_MAP];
} bit_configuration;
//typedef struct smb_map smb_configuration [N_BIT_MAP];
typedef struct 
{
  uint16_t adr [N_BIT_MAP];
} smb_configuration;

typedef uint8_t  byte;

typedef struct
{
   uint8_t  head;
   uint8_t  exist_pq[N_BIT_MAP];
   uint8_t  p_or_q [N_BIT_MAP];
}string_of_polinom;

typedef struct
{
   uint16_t n_str; // колическтво строк
   uint8_t n_el; // колическтво элементов <= N_EL_MAX
   string_of_polinom strings[N_STR_MAX];

} polinom; 

// --------------------------- обеспечивающие функции ------------------------------------------------------
  void null_bit_cnfg(bit_configuration * cnfg);
  void null_bit_map (uint8_t * cnfg); //для всех N_BIT_MAP
  void null_smb_map (uint16_t * cnfg); //для всех N_BIT_MAP

  uint8_t r_bit(uint8_t * str, uint8_t j); // j начинаем считать с первого, а не с 0-го |
  uint8_t r_smb(const uint16_t * str, uint8_t j); // j начинаем считать с первого, а не с 0-го |
  void    w_bit(uint8_t * str, uint8_t j, uint8_t bit); //                              |
  void    w_smb(uint16_t * str, uint8_t j, uint8_t smb); //
  void w_sig(uint8_t * head, int8_t InSign);
  void w_leight(uint8_t * head, uint8_t InLeight);
  int8_t r_sig(uint8_t head);
  uint8_t r_leight(uint8_t head);

  void    w_byte(uint8_t * str, uint8_t byte);//
  uint8_t r_byte(uint8_t * str);
  void bit_to_smb (uint8_t * bit_cnfg, uint16_t * smb_cnfg);//
  void BitToSmb2 (uint8_t * bit_cnfg, uint16_t * smb_cnfg);
  void SetCnfg(uint8_t * bit_cnfg, uint8_t EnOrDis);
  _Bool w_cnfg(uint8_t * bit_cnfg, const uint8_t * char_cnfg);
  _Bool copy(uint8_t * cnfg1, const uint8_t * cnfg2);    //
  _Bool copy_smb(uint16_t * cnfg1, const uint16_t * cnfg2);

  _Bool equl (float f1,float f2);                                      //
  _Bool a_more_b (float a, float b);
  //------ Эта функция вызывается при загрузке полинома из txt------------------
  void polinom_init       (void * p_plnm, uint16_t in_n_str);
  void PolinomInit       (polinom * p_plnm);
// -----------------------------------------------------------------------------

// --------------------------- основные функции ------------------------------------------------------------                                                                      |
// через work_conf можно задать вычисление положительного, отрицательного и нормального полинома           |
//относительно элемента j. j начинается с первого, а не с нулевого (!) элемента                            |
        float culc_p        (uint16_t * work_conf);
        //double culc_pt      (double * pt_el);                                                  //	   |
        //float culc_ksi      (struct smb_map * work_conf, unsigned char j, enum type_ksi type);//           |
        void culc_all_ksi   (uint16_t * work_conf, float * all_ksi, enum type_ksi type);//           |                                                                                  |
//unsigned char genrt_sing_cnfg(struct smb_map * work_cnfg,/* конфигурация разрешённых к включению элементов */
//                              float * index,             /* ранг элементов для GRD_RNG                     */
//                              struct bit_map * bit_cnfg, /* выбранная рабочая конфигурация                 */
//                              enum type_ksi ksi_type,         /* NEGATIVE или POSITIVE или FULL            */
//                              enum type_choice choice_type    /* GRD_FST или GRD_LST или GRD_IND           */
//                              );
uint8_t first_max_ksi(float * ksi, uint16_t * cnfg); // начиная с 1
uint8_t find_max_ksi(float * vec, uint16_t * cnfg, uint8_t * many_max);// начиная с 1
uint8_t FindRandKsi(float * vec, uint8_t n_el);// начиная с 1
//-------------------------------------------------------------------------------------
//                               file loadpolinom.c
//-------------------------------------------------------------------------------------
uint32_t FindConfiguration(polinom * STRUCTURE, bit_configuration * ManyConf, uint8_t NumDev, uint8_t NumMode);

void LoadPlnmFromFile(char * PlnmFileName, polinom * Polinom);

#endif
