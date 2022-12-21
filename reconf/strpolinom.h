#ifndef STRPOLINOM_H
#define STRPOLINOM_H

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

typedef enum {NEGATIV = 0,
              POSITIV = 1,
              FULL = 2}
             typeKsi;

typedef enum {GRD_RND = 2,
              RND     = 3,
              GRD_FST = 4,
              GRD_LST = 5,
              GRD_IND = 6}
             typeChoice;

typedef uint8_t BitConfiguration [N_BIT_MAP];

typedef uint16_t SmbConfiguration [N_BIT_MAP];

typedef uint8_t  byte;

typedef struct PolinomString
{
   uint8_t  head;
   uint8_t  existPQ[N_BIT_MAP];
   uint8_t  pq [N_BIT_MAP];
}PolinomString;

typedef struct Polinom
{
   uint32_t nStr; // колическтво строк
   uint8_t nEl; // колическтво элементов <= N_EL_MAX
   PolinomString strings[N_STR_MAX];

} Polinom;

// --------------------------- обеспечивающие функции ------------------------------------------------------
  void nullBitCnfg(BitConfiguration * cnfg);
  void nullBitMap (uint8_t * cnfg); //для всех N_BIT_MAP
  void nullSmbMap (uint16_t * cnfg); //для всех N_BIT_MAP

  uint8_t rBit(uint8_t * str, uint8_t j); // j начинаем считать с первого, а не с 0-го |
  uint8_t rSmb(const uint16_t * str, uint8_t j); // j начинаем считать с первого, а не с 0-го |
  void    wBit(uint8_t * str, uint8_t j, uint8_t bit); //                              |
  void    wSmb(uint16_t * str, uint8_t j, uint8_t smb); //
  void wSig(uint8_t * head, int8_t InSign);
  void wLeight(uint8_t * head, uint8_t InLeight);
  int8_t rSig(uint8_t head);
  uint8_t rLeight(uint8_t head);

  void    wByte(uint8_t * str, uint8_t byte);//
  uint8_t rByte(uint8_t * str);
  void bitToSmb (uint8_t * bitCnfg, uint16_t * smbCnfg);//
  void BitToSmb2 (uint8_t * bitCnfg, uint16_t * smbCnfg);
  void SetCnfg(uint8_t * bitCnfg, uint8_t EnOrDis);
  _Bool wCnfg(uint8_t * bitCnfg, const uint8_t * charCnfg);
  _Bool copy(uint8_t * cnfg1, const uint8_t * cnfg2);    //
  _Bool copySmb(uint16_t * cnfg1, const uint16_t * cnfg2);

  _Bool equl (float f1,float f2);                                      //
  _Bool aMoreB (float a, float b);
  //------ Эта функция вызывается при загрузке полинома из txt------------------
  void polinom_init       (void * pPlnm, uint16_t nStr);
  void PolinomInit       (Polinom * p_plnm);
// -----------------------------------------------------------------------------

// --------------------------- основные функции ------------------------------------------------------------                                                                      |
// через work_conf можно задать вычисление положительного, отрицательного и нормального полинома           |
//относительно элемента j. j начинается с первого, а не с нулевого (!) элемента                            |
        float culcP        (uint16_t * workConf);
        //double culc_pt      (double * pt_el);                                                  //	   |
        //float culc_ksi      (struct smb_map * work_conf, unsigned char j, enum type_ksi type);//           |
        void culcAllKsi   (uint16_t * workConf, float * allKsi, typeKsi type);//           |                                                                                  |
//unsigned char genrt_sing_cnfg(struct smb_map * work_cnfg,/* конфигурация разрешённых к включению элементов */
//                              float * index,             /* ранг элементов для GRD_RNG                     */
//                              struct bit_map * bit_cnfg, /* выбранная рабочая конфигурация                 */
//                              enum type_ksi ksi_type,         /* NEGATIVE или POSITIVE или FULL            */
//                              enum type_choice choice_type    /* GRD_FST или GRD_LST или GRD_IND           */
//                              );
uint8_t firstMaxKsi(float * ksi, uint16_t * cnfg); // начиная с 1
uint8_t findMaxKsi(float * vec, uint16_t * cnfg, uint8_t * manyMax);// начиная с 1
uint8_t findRandKsi(float * vec, uint8_t nEl);// начиная с 1
//-------------------------------------------------------------------------------------
//                               file loadpolinom.c
//-------------------------------------------------------------------------------------
uint32_t FindConfiguration(Polinom * STRUCTURE, BitConfiguration * ManyConf, uint8_t NumDev, uint8_t NumMode);

void LoadPlnmFromFile(char * PlnmFileName, Polinom * Polinom);

#endif
