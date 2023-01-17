#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/wait.h>


#pragma region //Model

    #define NAME_LENGTH 51
    #define NAME_MIN_LENGTH 4
    #define MOBILE_LENGTH 12
    #define FILE_NAME "applicants.bin"

    enum profession{
        CARPENTER,
        TILER,
        PROGRAMMER,
        MASON
    };

    struct applicant{
        char name[NAME_LENGTH];
        int age;
        char mobile[MOBILE_LENGTH];
        enum profession prof;
    };

    #define SOA (sizeof(struct applicant))

    void copyApplicant(struct applicant* app1, struct applicant* app2){
        app2->age = app1->age;
        strcpy(app2->mobile,app1->mobile);
        strcpy(app2->name,app1->name);
        app2->prof = app1->prof;
    }

    char* profToString(enum profession* prof){
        switch(*prof){
            case CARPENTER: return "Acs";
            break;
            case TILER: return "Burkolo";
            break;
            case PROGRAMMER: return "Programozo";
            break;
            case MASON: return "Komuves";
            break;
        }
    }

    void writeOutApplicant(struct applicant* app){
        printf("Name: %s, ",app->name);
        printf("Age: %i, ",app->age);
        printf("Mobile: %s, ",app->mobile);
        printf("Prof: %s\n",profToString(&app->prof));
    }

    void setApplicantToZero(struct applicant* app){
        strcpy(app->name,"");
        app->age = 0;
        strcpy(app->mobile,"");
        app->prof = 0;
    }

    int isApplicantZero(struct applicant* app){
        return strcmp(app->name,"") == 0 &&
                strcmp(app->mobile,"") == 0 &&
                app->age == 0 &&
                app->prof == 0;
    }
#pragma endregion

#pragma region //data manipulation

struct applicant* _apps;
int _noa;

void addNewElement(){
    _noa++;
    _apps = (struct applicant*)realloc(_apps,SOA*(_noa));
}

void save(){
    FILE* f = fopen(FILE_NAME,"wb");
    
    if (f == NULL) {perror("Error at opening the file\n");exit(1);}
    
    for (int i = 0; i < _noa;i++){
        if (!isApplicantZero(&_apps[i])){
            if (!fwrite(&_apps[i],sizeof(struct applicant),1,f)){
                perror("There is a mistake in writing\n");
                fclose(f);
                exit(1);
            }
        }
    }
    fclose(f);
}

void init(){
    _noa = 0;//the number of applicants has been read
    _apps = (struct applicant*)malloc(SOA);
    struct applicant* app = (struct applicant*)malloc(SOA);

    FILE* f = fopen(FILE_NAME,"rb");
    if (f == NULL ){perror("Error at opening the file\n");exit(1);}

    while (fread(app,SOA,1,f) >= 1){
        addNewElement();
        copyApplicant(app,&_apps[_noa-1]);
    }
    
    fclose(f);
}

void endGoWest(){
    save();
    free(_apps);
}

#pragma endregion

#pragma region //View

void clearStdin(){
    char endline;
    int nitems = scanf("%c",&endline);
}

int menu(){
    int m,nitems;
    int error = 0; //bool
    do{
        printf("\n\nKerem valasszon az alabbi menupontokbol:\n");
        printf("1- Uj jelentkezes leadasa\n");
        printf("2- Jelentkezes modositasa\n");
        printf("3- Jelentkezes torlese\n");
        printf("4- Jelentkezesek kiirasa\n");
        printf("5- Jelentkezesek kiirasa szakmara szurve\n");
        printf("6- Jelentkezok fuvarozasa\n");
        printf("0- Kilep\n");
        nitems = scanf("%d",&m);
        clearStdin();
        error = nitems == EOF || nitems == 0 || m < 0 || m > 6;
        if (error){
            perror("Hiba tortent a menupont kivalasztasa soran!\nKerem 0 es 4 kozotti szamot adjon meg!\nHiba: ");
        }
    }while(error);
    return m;
}

void chooseProfession(enum profession* prof){
    int m, nitems;
    int error = 0;
    do{
        printf("Kerem valasszon a foglalkozasok kozul: \n");
        printf("1- acs\n");
        printf("2- burkolo\n");
        printf("3- programozo\n");
        printf("4- komuves\n");
        nitems = scanf("%i",&m);
        clearStdin();
        
        error = nitems == EOF || nitems == 0 || m < 1 || m > 4;
        if (error){
            perror("Hiba a szakma kivalasztasa soran!\nKerem 1 es 4 kozotti szamot adjon meg!\n");
        }
    }while(error);
    *prof = m-1; //-1 because of the enum indexed from 0
}

void newApplicantView(char* name, int* age, char* mobile, enum profession* prof){
    int nitems;
    int error = 0;

    printf("\n------------Uj jelentkezes leadasa-------------\n");
    do{
        //clearStdin();
        printf("Kerem adja meg a nevet: ");
        fflush(stdout);
        fgets(name,NAME_LENGTH,stdin);
        name[strlen(name)-1] = '\0';

        printf("Kerem adja meg a korat: ");
        nitems = scanf("%i",age);
        clearStdin();

        printf("Adja mega telefon szamat: ");
        fgets(mobile,MOBILE_LENGTH,stdin);

        chooseProfession(prof);
        error = strchr(name,' ') == NULL || nitems == 0 || *age < 18 || strlen(mobile)+1 != MOBILE_LENGTH || strlen(name) < NAME_MIN_LENGTH;
        if (error){
            perror("Hiba az adatok megadasa soran!\n\tNevben szerepel legalabb 1 szokoz!\n\tEletkor legalabb 18\n\tTelefonszam hossza 11 karakter!");
        }
    }while(error);
}

void writeApplicantsView(enum profession* prof){
    int count = 0;
    if (prof == NULL){
        printf("\n--------------Jelentkezok kiirasa--------------\n");
        for (int i = 0; i < _noa;i++){
            count++;
            writeOutApplicant(&_apps[i]);
        }
    }
    else{
        printf("\n--------%s jelentkezok kiirasa--------\n",profToString(prof));
        for (int i = 0; i < _noa; i++){
            if (_apps[i].prof == *prof){
                count++;
                writeOutApplicant(&_apps[i]);
            }
        }
    }
    if (count == 0){
        printf("A file ures volt, vagy nem volt a keresett szakmabol jelentkezo!\n");
    }
}

void readApplicantNameMobile(char* appName, char* appMobile){
    int error = 0;
    printf("\n-----------------Jelentkezes torlese/modositasa-----------------\n");
    do{
        printf("Kerem adja meg a torlendo/modositando jelentkezo nevet: ");
        fgets(appName,NAME_LENGTH,stdin);
        printf("Kerem adja meg a torlendo/modositando jelentkezo telefonszamat: ");
        fgets(appMobile,MOBILE_LENGTH,stdin);
        clearStdin();
        
        error = strchr(appName,' ') == NULL || strlen(appName) < NAME_MIN_LENGTH || strlen(appMobile)+1 < MOBILE_LENGTH;
        if (error){
            perror("Hiba a nev vagy telefonszam beolvasasa soran!");
        }
    }while(error);
}

void modApplicantView(struct applicant* modApp){
    int nitems;
    int error = 0;

    printf("\n------------Jelentkezes adatok modositasa-------------\n");
    printf("Kerem adja meg az osszes adatat helyesen, mert az itt megadott adatokra lesz frissitve a jelentkezese.\n");
    do{
        //clearStdin();
        printf("Kerem adja meg a nevet: ");
        fflush(stdout);
        fgets(modApp->name,NAME_LENGTH,stdin);
        modApp->name[strlen(modApp->name)-1] = '\0';

        printf("Kerem adja meg a korat: ");
        nitems = scanf("%i",&modApp->age);
        clearStdin();

        printf("Adja mega telefon szamat: ");
        fgets(modApp->mobile,MOBILE_LENGTH,stdin);
        clearStdin();

        chooseProfession(&modApp->prof);
        error = strchr(modApp->name,' ') == NULL || strlen(modApp->name) < NAME_MIN_LENGTH ||
                 nitems == 0 || modApp->age < 18 ||
                 strlen(modApp->mobile)+1 != MOBILE_LENGTH;
        if (error){
            perror("Hiba az adatok megadasa soran!\n\tNevben szerepel legalabb 1 szokoz!\n\tEletkor legalabb 18\n\tTelefonszam hossza 11 karakter!");
        }
    }while(error);
}

#pragma endregion

#pragma region //Controller

void newApplicantController(){
    struct applicant *napp = (struct applicant*)malloc(sizeof(struct applicant));
    
    newApplicantView(napp->name,&(napp->age),napp->mobile,&(napp->prof));
    
    addNewElement();
    copyApplicant(napp,&_apps[_noa-1]);

    free(napp);
    save();
}

void modApplicantController(){
    int i = 0;
    char* appName = (char*)malloc(sizeof(char)*NAME_LENGTH);
    char* appMobile = (char*)malloc(sizeof(char)*MOBILE_LENGTH);
    readApplicantNameMobile(appName,appMobile);
    appName[strlen(appName)-1] = '\0';

    while(i < _noa && (strcmp(appName,_apps[i].name) != 0 && strcmp(appMobile,_apps[i].mobile) != 0)){
        i++;
    }

    if (i >= _noa) {printf("Nem talalhato ilyen nev es telefonszam paros!");return;}

    struct applicant* modApp = (struct applicant*)malloc(sizeof(struct applicant));
    modApplicantView(modApp);
    
    copyApplicant(modApp,&_apps[i]);
    save();
    free(appName);
    free(appMobile);
    free(modApp);
}

void delApplicantsController(){
    int found = 0;
    char* appName = (char*)malloc(sizeof(char)*NAME_LENGTH);
    char* appMobile = (char*)malloc(sizeof(char)*MOBILE_LENGTH);
    readApplicantNameMobile(appName,appMobile);
    appName[strlen(appName)-1] = '\0';

    for (int i = 0; i < _noa; i++){
        if (strcmp(appName,_apps[i].name) == 0 && strcmp(appMobile,_apps[i].mobile) == 0){
            found++;
            setApplicantToZero(&_apps[i]);
        }
    }
    if (found == 0){
        printf("Nem volt a keresesnek megfelelo jelentkezo!\n");
    }
    save();
    free(appName);
    free(appMobile);
}

void writeApplicantsController(char *mode)
{
    if (_noa != 0){
        if (strcmp(mode,"all") == 0){
            
            writeApplicantsView(NULL);
        }
        else if (strcmp(mode,"pro") == 0){
            enum profession prof;
            chooseProfession(&prof);
            writeApplicantsView(&prof);
        }
    }
    else{
        printf("Nem voltak jelentkezok a fileban!\n");
    }

    save();
}

void sendOneVehicle(int dProf,int appsAm){ //melyik szakma, hány jelentkező
    int pipePTC[2]; //pipe parent to child
    int pipeCTP[2]; //child to parent
    char* vehicle;
    pid_t pid;
    if (appsAm > 4){
        vehicle = "KisBusz";
    }
    else{
        vehicle = "Auto";
    }
    

    if (pipe(pipePTC) == -1){
        perror("Hiba a PTC pipe nyitasakor!");
        return;
    }
    if (pipe(pipeCTP) == -1){
        perror("Hiba a CTP pipe nyitasakor!");
        return;
    }

    pid = fork();
    if (pid == -1){
        perror("Jarmu inditasi hiba");
        return;
    }

    if (pid == 0) // gyerek
    {
        close(pipePTC[1]); //szülő -> gyerek író bezár
        close(pipeCTP[0]); //gyerek -> szülő olvasó bezár
        int haveToRead;
        read(pipePTC[0],&haveToRead,sizeof(int));

        printf("Emberek elszallitva: \n");
        struct applicant readApp;
        for (int i = 0; i < haveToRead; i++){
            read(pipePTC[0],&readApp,sizeof(struct applicant));
            writeOutApplicant(&readApp);
        }
        close(pipePTC[0]); // szülő -> gyerek olvasó bezár

        write(pipeCTP[1],"OK",3);
        close(pipeCTP[1]);  //gyerek -> szülő író bezár
        exit(0); // mivel a programom ciklikusan fut ezért a fork hívásnál a "további rész" is lemásolódik,
                 //tehát a főprogram tovább fut a gyerek szálon is, miután az befejezte a működését
                 // ezért bezárom azt szálat, hogy ne legyen ez 
    }
    else //szülő
    {
        close(pipePTC[0]); //szülő -> gyerek olvasó bezár
        close(pipeCTP[1]); //gyerek -> szülő író bezár
        write(pipePTC[1],&appsAm,sizeof(int)); //átküldöm, hány applicantot kell kiolvasni a csőből
        int appsWritten = 1;
        for (int i = 0; i < _noa && appsWritten <= appsAm && appsWritten <= 7;i++){
            if (_apps[i].prof == dProf){
                write(pipePTC[1],&_apps[i],sizeof(struct applicant));
                appsWritten++;
            }
        }
        close(pipePTC[1]); //szülő -> gyerek író
        fflush(NULL);
        wait(NULL);
        char childMsg[3];
        read(pipeCTP[0],childMsg,3);
        printf("%s uzenete: %s\n", vehicle, childMsg);
        close(pipeCTP[0]); // gyerek -> szülő olvasó
    }
}

void sendTwoVehicle(int fProf, int sProf, int fAppAm, int sAppAm){
    int pipePTC[2]; //pipe parent to car
    int pipeCTP[2]; //car to parent
    int pipePTB[2]; //parent to minibus
    int pipeBTP[2]; //minibus to parent
    pid_t pidC, pidB;

    if (pipe(pipePTC) == -1){perror("Hiba a PTC pipe nyitasakor!"); return;}
    if (pipe(pipeCTP) == -1){perror("Hiba a CTP pipe nyitasakor!"); return;}
    if (pipe(pipePTB) == -1){perror("Hiba a PTB pipe nyitasakor!"); return;}
    if (pipe(pipeBTP) == -1){perror("Hiba a BTP pipe nyitasakor!"); return;}

    pidC = fork();
    if (pidC == -1){perror("Auto inditasi hiba"); return;}
    
    if (pidC > 0){
        pidB = fork();
        if (pidB == -1){perror("Kisbusz inditasi hiba"); return;}
    }

    if (pidC == 0 ) // gyerek
    {
        close(pipePTC[1]); //szülő -> gyerek író bezár
        close(pipeCTP[0]); //gyerek -> szülő olvasó bezár
        close(pipePTB[0]);close(pipePTB[1]);close(pipeBTP[0]);close(pipeBTP[1]);
        int haveToRead;
        read(pipePTC[0],&haveToRead,sizeof(int));

        printf("Emberek elszallitva autoval: \n");
        struct applicant readApp;
        for (int i = 0; i < haveToRead; i++){
            read(pipePTC[0],&readApp,sizeof(struct applicant));
            writeOutApplicant(&readApp);
        }
        close(pipePTC[0]); // szülő -> gyerek olvasó bezár

        write(pipeCTP[1],"OK",3);
        close(pipeCTP[1]);  //gyerek -> szülő író bezár
        exit(0);
    }
    else if (pidB == 0){
        close(pipePTB[1]); //szülő -> gyerek író bezár
        close(pipeBTP[0]); //gyerek -> szülő olvasó bezár
        close(pipePTC[0]);close(pipePTC[1]);close(pipeCTP[0]);close(pipeCTP[1]);
        int haveToRead;
        read(pipePTB[0],&haveToRead,sizeof(int));

        printf("Emberek elszallitva kisbusszal: \n");
        struct applicant readApp;
        for (int i = 0; i < haveToRead; i++){
            read(pipePTB[0],&readApp,sizeof(struct applicant));
            writeOutApplicant(&readApp);
        }
        close(pipePTB[0]); // szülő -> gyerek olvasó bezár

        write(pipeBTP[1],"OK",3);
        close(pipeBTP[1]);  //gyerek -> szülő író bezár
        exit(0);
    }
    else if (pidB > 0 && pidC > 0)//szülő
    {
        close(pipePTC[0]); //szülő -> gyerek olvasó bezár
        close(pipeCTP[1]); //gyerek -> szülő író bezár
        close(pipePTB[0]); //szülő -> gyerek olvasó bezár
        close(pipeBTP[1]); //gyerek -> szülő író bezár
        write(pipePTC[1],&sAppAm,sizeof(int)); //átküldöm, hány applicantot kell kiolvasni a csőből
        write(pipePTB[1],&fAppAm,sizeof(int));
        int fAppsWritten = 1, sAppsWritten = 1;
        for (int i = 0; i < _noa && sAppsWritten <=sAppAm && sAppsWritten <= 4;i++){
            if (_apps[i].prof == sProf){
                write(pipePTC[1],&_apps[i],sizeof(struct applicant));
                sAppsWritten++;
            }
        }
        for (int i = 0; i < _noa && fAppsWritten <= fAppAm && fAppsWritten <= 7;i++){
            if (_apps[i].prof == fProf){
                write(pipePTB[1],&_apps[i],sizeof(struct applicant));
                fAppsWritten++;
            }
        }
        close(pipePTC[1]); //szülő -> gyerek író
        close(pipePTB[1]); //szülő -> gyerek író
        fflush(NULL);
        wait(NULL);
        char carMsg[3], busMsg[3];
        read(pipeCTP[0],carMsg,3);
        read(pipeBTP[0],busMsg,3);
        printf("Auto uzenete: %s\n",carMsg);
        printf("Minibusz uzenete: %s\n",busMsg);
        close(pipeCTP[0]); // gyerek -> szülő olvasó
        close(pipeBTP[0]); // gyerek -> szülő olvasó
    }
}


void countProfessions(int appsInProfs[]){
    //*cc = 0; *ct = 0; *cm = 0; *cp = 0;
    for (int i = 0; i < _noa;i++){
        if (_apps[i].prof == CARPENTER) appsInProfs[0]++;
        else if (_apps[i].prof == TILER) appsInProfs[1]++;
        else if (_apps[i].prof == PROGRAMMER) appsInProfs[2]++;
        else if (_apps[i].prof == MASON) appsInProfs[3]++;
    }
}

void getTwoGreatest(int appsInProfs[],int* fGreate,int* sGreate){
    *fGreate = 0;
    *sGreate = 0;
    for (int i = 1; i < 4; i++){
        if (appsInProfs[*fGreate] < appsInProfs[i]){ // appsInProfs[*fGreate] < appsInProfs[i] -> appsInProfs[*sGreate] < appsInProfs[i] && appsInProfs[*sGreate] < appsInProfs[*fGreate]
            *sGreate = *fGreate;
            *fGreate = i;
        }
        else if (appsInProfs[*sGreate] < appsInProfs[i]){ // appsInProfs[*fGreate] > appsInProfs[i] && appsInProfs[*sGreate] < appsInProfs[i]
            *sGreate = i;
        }
    }
    printf("Nincs segfault\n");
}

void deliverApplicants(){
    printf ("----------------------Jelentkezők szállítása-------------------------\n");
    int appsInProfs[4];
    int fGreate, sGreate;
    for (int i = 0; i < 4; i++) appsInProfs[i] = 0;

    printf ("Jelentkezok szamlalasa...\n");
    countProfessions(appsInProfs);

    printf ("Ket szakmabol valo legtobb jelentkezesek keresese...\n");
    getTwoGreatest(appsInProfs,&fGreate,&sGreate);

    printf("Jarmu(vek) inditasa...\n");
    if (appsInProfs[fGreate] >= 5){
        if (appsInProfs[sGreate] >= 3){
            sendTwoVehicle(fGreate,sGreate,appsInProfs[fGreate],appsInProfs[sGreate]);
        }
        else{
            sendOneVehicle(fGreate,appsInProfs[fGreate]);
        }
    }
    else if (appsInProfs[fGreate] >= 3){
        sendOneVehicle(fGreate,appsInProfs[fGreate]);
    }
    else{
        printf("Nem volt eleg jelentkezo\n");
    }
}

#pragma endregion