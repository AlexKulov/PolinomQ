#include "reconf/reconfiguration.h"

#include <stdio.h>
#include <string.h>

void showCnfg(BitConfiguration cnfgIn,unsigned char nEl){
    printf("\n Cnfg : ");
    unsigned char i;
    for(i=1;i<=nEl;i++){
            if(rBit(cnfgIn,i))
                printf("P%i ",i);
    }
}

int main(int argc, char *argv[])
{

    //float Tmax[N_EL_MAX];
    float Tcur[N_EL_MAX];
    unsigned char i;
    for (i=0;i<N_EL_MAX;i++)
        Tcur[i] = 0;
    BitConfiguration * Con;
    BitConfiguration ConConst;
    unsigned char Elements  =17;
    unsigned char Mode = 3;

    char polinomName[127];
    if(argc == 1){
        strcpy(polinomName,"polynom/Neuro.txt");
    }else if(argc == 2){
        strcpy(polinomName,argv[1]);
    }else{
        printf("Please, next time add input parameters for programm \n");
        return -1;
    }

    Con = initFswRcnf(polinomName,Elements,Mode); //10 - элементов, 4 - режима
    copy(ConConst,*Con);
    showCnfg(*Con,Elements+Mode);

    BitConfiguration servCnfg;
    setCnfg(servCnfg,ENBL);
    long sumExctMode=0;
    float Tsim=0;
    float dTsim=5;
    float Tfail=100;
    long over = FALSE;
    unsigned char iStart = 1;
    long decr = FALSE;

    for(;;){

        if(iStart == 0)
            break;
        else
            printf("\n iStart = %i", iStart);

        for(;Tsim<2000;){
            if(Tsim>Tfail){
                for (i=iStart;i<=Elements && i>0;){
                    if(ENBL == rBit(*Con,i)){
                        wBit(servCnfg,i,DISBL);
                        printf("\n Fail : %i", i);
                        Tfail = Tsim + 100;
                        break;
                    }
                    if(decr){
                        i++;
                        if(i>Elements){
                            i=1;
                            over = TRUE;
                        }
                        if(i==iStart && over)
                            break;
                    }
                    else
                        i++;
                }
                sumExctMode = (long)reconfiguration(servCnfg, Tcur,Con);
                if(sumExctMode == 0){
                    printf("\n NP %f", (double)Tsim);
                    break;
                }
                else
                    showCnfg(*Con,Elements+Mode);
            }

            Tsim = Tsim + dTsim;
            for (i=1;i<=Elements;i++)
                if(ENBL == rBit(*Con ,i))
                    Tcur[i] = Tcur[i] + dTsim;
        }
        Tsim = 0;
        Tfail = 100;
        over = FALSE;
        setCnfg(servCnfg ,ENBL);
        copy(*Con , ConConst );
        if(decr)
            iStart--;
        else
            iStart++;
        if(iStart == Elements)
            decr = TRUE;
    }

    return 0;
}
