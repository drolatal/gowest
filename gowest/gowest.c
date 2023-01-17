
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "lib/gowestlib.h"

void deliverApplicants();

int main(){
    init();

    printf("___________________________________GoWest______________________________________");
    printf("Üdvözöljük az állás jelentkező szoftverünkben!");

    int m;
    do {
        m = menu();
        switch(m){
            case 1: newApplicantController();
            break;
            case 2: modApplicantController();
            break;
            case 3: delApplicantsController();
            break;
            case 4: writeApplicantsController("all");
            break;
            case 5: writeApplicantsController("pro");
            break;
            case 6: deliverApplicants();
            break;
        }
    }while(m != 0);

    endGoWest();
}