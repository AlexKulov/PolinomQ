#include "reconf\reconfiguration.h"


void ShowCnfg(bit_configuration CnfgIn,unsigned char Nel){
    printf("\n");
    printf("Cnfg : ");
    unsigned char i;
    for(i=1;i<=Nel;i++){
            if(r_bit(CnfgIn.adr,i))
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
    bit_configuration Con;
    bit_configuration ConConst;
    unsigned char Elements  =17;
    unsigned char Mode = 3;

    Con = InitFswRcnf(Elements,Mode); //10 - элементов, 4 - режима
    copy(ConConst.adr,Con.adr);
    ShowCnfg(Con,Elements+Mode);

    bit_configuration Cserv;
    SetCnfg(Cserv.adr,ENBL);
    long SumExctMode=0;
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
                    if(ENBL == r_bit(Con.adr,i)){
                        w_bit(Cserv.adr,i,DISBL);
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
                SumExctMode = (long)Reconfiguration(Cserv, Tcur,&Con);
                if(SumExctMode == 0){
                    printf("\n NP %f", Tsim);
                    break;
                }
                else
                    ShowCnfg(Con,Elements+Mode);
            }

            Tsim = Tsim + dTsim;
            for (i=1;i<=Elements;i++)
                if(ENBL == r_bit(Con.adr,i))
                    Tcur[i] = Tcur[i] + dTsim;
        }
        Tsim = 0;
        Tfail = 100;
        over = FALSE;
        SetCnfg(Cserv.adr,ENBL);
        copy(Con.adr, ConConst.adr);
        if(decr)
            iStart--;
        else
            iStart++;
        if(iStart == Elements)
            decr = TRUE;
    }

    return 0;
}
