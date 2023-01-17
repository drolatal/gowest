#ifndef GOWESTLIB_DEFINED

   #define GOWESTLIB_DEFINED

    #define NAME_LENGTH 51
    #define MOBILE_LENGTH 12

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

    //void countProfessions(int appsInProfs[]);
    //void getTwoGreatest(int appsInProfs[],int* fGreate,int* sGreate);
    void writeOutApplicant(struct applicant* app);
    void init();
    void save();
    void endGoWest();
    void newApplicantController();
    void modApplicantController();
    void delApplicantsController();
    void writeApplicantsController(char *mode);
    void deliverApplicants();
    int menu();
    void chooseProfesion(enum profession* prof);
    void newApplicantView(char* name, int* age, char* mobile, enum profession*);

#endif