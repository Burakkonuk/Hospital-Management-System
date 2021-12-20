#include <semaphore.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <time.h>
#define maxPatientNum 100

void *patient(void *num);
sem_t reg_office;
sem_t gp_office;
sem_t pharmacy;
sem_t bloodlab;
sem_t operatingroom;
sem_t surgeons;
sem_t nurses;
sem_t cafe;
sem_t restroom;


// The number of registration desks that are available.
int REGISTRATION_SIZE = 10;
// The number of restrooms that are available.
int RESTROOM_SIZE = 10;
// The number of cashiers in cafe that are available.
int CAFE_NUMBER = 10;
// The number of General Practitioner that are available.
int GP_NUMBER = 10;
// The number of cashiers in pharmacy that are available.
int PHARMACY_NUMBER = 10;
// The number of assistants in blood lab that are available.
int BLOOD_LAB_NUMBER = 10;
// The number of operating rooms, surgeons and nurses that are available.
int OR_NUMBER = 10;
int SURGEON_NUMBER = 30;
int NURSE_NUMBER = 30;

int SURGEON_LIMIT = 5;
int NURSE_LIMIT = 5;
int PATIENT_NUMBER[maxPatientNum];
int HOSPITAL_WALLET = 0;
int WAIT_TIME = 100;
int REGISTRATION_TIME = 100;
int GP_TIME = 200;
int PHARMACY_TIME = 100;
int BLOOD_LAB_TIME = 200;
int SURGERY_TIME = 500;
int CAFE_TIME = 100;
int RESTROOM_TIME = 100;
int REGISTRATION_COST = 100;
int PHARMACY_COST = 200; // Calculated randomly between 1 and given value.
int BLOOD_LAB_COST = 200;
int SURGERY_OR_COST = 200;
int SURGERY_SURGEON_COST = 100;
int SURGERY_NURSE_COST = 50;
int CAFE_COST = 200; // Calculated randomly between 1 and given value.

int HUNGER_INCREASE_RATE = 10;
int RESTROOM_INCREASE_RATE = 10;



int counter = 0;

struct patient {
  int id;
  char disease[30];
  int Hunger_Meter;
  int Restroom_Meter;
  };

void randwait(int millisecs){
    int r =(rand() % millisecs) + 1;
    usleep(1000*r);
}
void hungerRestroomMeterInc(struct patient patients){
    //printf("%d %d patient \n",patients.Hunger_Meter, patients.id);
    //printf("%d %d patient\n",patients.Restroom_Meter, patients.id);
    int HUNGER_INCREASE = (rand() % HUNGER_INCREASE_RATE) + 1;
    patients.Hunger_Meter = patients.Hunger_Meter + HUNGER_INCREASE;
    int RESTROOM_INCREASE = (rand() % HUNGER_INCREASE_RATE) + 1;
    patients.Restroom_Meter = patients.Restroom_Meter + RESTROOM_INCREASE;
    //printf("%d %d patient\n",patients.Hunger_Meter, patients.id);
    //printf("%d %d patient\n",patients.Restroom_Meter, patients.id);
    if (patients.Restroom_Meter>=100)
    {
        while(1){ 
        if(sem_trywait(&restroom)==0){
            printf("Patient (%d) goes to restroom\n", patients.id);
            randwait(RESTROOM_TIME); // Waiting for registration process.
            printf("Patient (%d) exits restroom\n", patients.id);
            patients.Restroom_Meter = 0; // I HAVE TO USE POINTERS HERE TO CHANGE VALUE BACK.
            sem_post(&restroom);
            break;
        }
        else{
            printf("Patient (%d) is waiting for restroom to be available.\n", patients.id);
            randwait(WAIT_TIME); // Waiting for registration queue.
            
        }

    }
    }
    if (patients.Hunger_Meter>=100)
    {
        while(1){ 
        if(sem_trywait(&cafe)==0){
            printf("Patient (%d) goes to cafe\n", patients.id);
            randwait(CAFE_TIME); // Waiting for registration process.
            printf("Patient (%d) exits cafe\n", patients.id);
            HOSPITAL_WALLET += rand() % 200 + 1;
            patients.Hunger_Meter = 0;
            sem_post(&cafe);
            break;
        }
        else{
            printf("Patient (%d) is waiting for cafe to be available.\n", patients.id);
            randwait(WAIT_TIME); // Waiting for registration queue.
            
        }

    }
    }
    
    
}
int main(int argc, char* argv[]){
   srand ( time(NULL) ); 
  pthread_t patientTid[maxPatientNum];
  sem_init(&reg_office, 0, REGISTRATION_SIZE);
  sem_init(&gp_office, 0, GP_NUMBER);
  sem_init(&pharmacy, 0, PHARMACY_NUMBER);
  sem_init(&bloodlab,0, BLOOD_LAB_NUMBER);
  sem_init(&operatingroom,0,OR_NUMBER);
  sem_init(&surgeons,0, SURGEON_NUMBER);
  sem_init(&nurses,0,NURSE_NUMBER);
  sem_init(&cafe,0,CAFE_NUMBER);
  sem_init(&restroom,0,RESTROOM_SIZE);


    
    
    for (int i = 0; i < maxPatientNum; i++)  //create patients
    {
        pthread_create(&patientTid[i], NULL, patient, (void*)&PATIENT_NUMBER[i]);
    }

      for (int i = 0; i < maxPatientNum; i++)
    {
        pthread_join(patientTid[i],NULL);
    }

}

void *patient(void *number){
    int num =*(int*)number;
    
    struct patient patient1; // Constructing patient's struct.
    patient1.id = counter; // Basic id incrementer.
    patient1.Hunger_Meter = (rand() % 100) + 1;
    patient1.Restroom_Meter = (rand() % 100) + 1;
    counter++;
    int r = rand() % 4;
    if (r==0)  // Random diseases for each patient.
    {
        strcpy(patient1.disease, "Headache"); 
    }
    if (r==1)
    {
        strcpy(patient1.disease, "Toothace"); 
    }
    if (r==2)
    {
        strcpy(patient1.disease, "Fever"); 
    }
    if (r==3)
    {
        strcpy(patient1.disease, "diarrhea"); 
    }
    
    
    //printf("Patient id = %d  Patient disease = %s \n", patient1.id, patient1.disease);
    
    while(1){   // Patients enters to registration desk if it is empty else they wait for REGISTRATION_TIME ms.
        if(sem_trywait(&reg_office)==0){
            printf("Patient (%d) found an available registration desk.\n", patient1.id);
            randwait(REGISTRATION_TIME); // Waiting for registration process.
            hungerRestroomMeterInc(patient1);
            printf("Patient (%d) registered successfully\n", patient1.id);
            HOSPITAL_WALLET += REGISTRATION_COST;
            sem_post(&reg_office);
            break;
        }
        else{
            printf("Patient (%d) is waiting in the registration room.\n", patient1.id);
            randwait(WAIT_TIME); // Waiting for registration queue.
            hungerRestroomMeterInc(patient1);
            
        }

    }

    while(1){  // Patients enters to GP's office if it is empty else they wait for GP_TIME ms.
        if(sem_trywait(&gp_office)==0){
            printf("Patient (%d) found an available doctor to be examined\n", patient1.id  );
            randwait(GP_TIME); // Waiting for examination progress.
            hungerRestroomMeterInc(patient1);
            printf("Patient (%d) has been examined.\n", patient1.id);
            sem_post(&gp_office);
            break;
        }
        else
        {
            printf("Patient (%d) is waiting for examination.\n",patient1.id);
            randwait(WAIT_TIME); // Waiting for GP's queue.
            hungerRestroomMeterInc(patient1);
        }
        
    }

    int departmentRandom = rand() % 3; // Patient goes on of the three departments.
    if (departmentRandom == 0) //PHARMACY DEPARTMENT
    {
        printf("Patient (%d) has been directed to the 'PHARMACY DEPARTMENT'\n",patient1.id);
        while (1)
        {
            if(sem_trywait(&pharmacy)==0){
            printf("Patient (%d) has found an empty pharmacy registration.\n", patient1.id  );
            randwait(PHARMACY_TIME); // Waiting for examination progress.
            hungerRestroomMeterInc(patient1);
            printf("Patient (%d) has bought medicines.\n", patient1.id);
            HOSPITAL_WALLET += rand()%200 + 1; 
            sem_post(&pharmacy);
            break;
        }
        else
        {
            printf("Patient (%d) is waiting for pharmacy registration.\n",patient1.id);
            randwait(WAIT_TIME); // Waiting for Pharmacy's queue.
            hungerRestroomMeterInc(patient1);
        }
        }
    }
    if (departmentRandom == 1) //BLOOD TEST DEPARTMENT
    {
        printf("Patient (%d) has been directed to the 'BLOOD TEST DEPARTMENT'\n",patient1.id);
        while (1)
        {
            if(sem_trywait(&bloodlab)==0){
            printf("Patient (%d) has found an available bloodlab assistant.\n", patient1.id  );
            randwait(BLOOD_LAB_TIME); // Waiting for examination progress.
            hungerRestroomMeterInc(patient1);
            printf("Patient (%d) has given blood.\n", patient1.id);
            HOSPITAL_WALLET += BLOOD_LAB_COST;
            sem_post(&bloodlab);
            break;
        }
        else
        {
            printf("Patient (%d) is waiting for an available bloodlab assistant.\n",patient1.id);
            randwait(WAIT_TIME); // Waiting for BloodLab's queue.
            hungerRestroomMeterInc(patient1);
        }
        }

        printf("Patient (%d) returns to Doctor's Office\n", patient1.id);

        while(1){  // Patients reenters to GP's office if it is empty else they wait for GP_TIME ms.
        if(sem_trywait(&gp_office)==0){
            printf("Patient (%d) found an available doctor to show blood results\n", patient1.id  );
            randwait(GP_TIME); // Waiting for examination progress.
            hungerRestroomMeterInc(patient1);
            printf("Patient (%d) has shown blood results to doctor.\n", patient1.id);
            sem_post(&gp_office);
            break;
        }
        else
        {
            printf("Patient (%d) is waiting to show blood results to doctor.\n",patient1.id);
            randwait(WAIT_TIME); // Waiting for GP's queue.
            hungerRestroomMeterInc(patient1);
        }
        
    }

        int isPrescribed = rand() % 2;
        if (isPrescribed == 0) // IF PATIENT NEEDS MEDICINES AFTER BLOOD TEST
        {
            printf("Patient (%d) is prescribed after blood test and has been directed to the 'PHARMACY DEPARTMENT'\n",patient1.id);
        while (1)
        {
            if(sem_trywait(&pharmacy)==0){
            printf("Patient (%d) has found an empty pharmacy registration.\n", patient1.id  );
            randwait(PHARMACY_TIME); // Waiting for examination progress.
            hungerRestroomMeterInc(patient1);
            printf("Patient (%d) has bought medicines.\n", patient1.id);
            HOSPITAL_WALLET += rand()%200 + 1; 
            sem_post(&pharmacy);
            break;
        }
        else
        {
            printf("Patient (%d) is waiting for pharmacy registration.\n",patient1.id);
            randwait(WAIT_TIME); // Waiting for Pharmacy's queue.
            hungerRestroomMeterInc(patient1);
        }
        }
        }
        if (isPrescribed == 1) // IF PATIENT DOESN'T NEED MEDICINES AFTER BLOOD TEST
        {
            printf("Patient (%d) isn't prescribed after blood test and is on way to home\n", patient1.id);
        }
    }
    if (departmentRandom == 2) //SURGERY DEPARTMENT
    {
        int surgeonNum = rand() % SURGEON_LIMIT + 1;
        int nurseNum = rand() % NURSE_LIMIT + 1;
        printf("Patient (%d) has been directed to the 'SURGERY DEPARTMENT'\n",patient1.id);
        while (1)
        {
            if (surgeonNum<=SURGEON_NUMBER && nurseNum<=NURSE_NUMBER && OR_NUMBER>0)
            {
                if (sem_trywait(&operatingroom) == 0)
                {
                    SURGEON_NUMBER = SURGEON_NUMBER-surgeonNum;
                    NURSE_NUMBER = NURSE_NUMBER-nurseNum;
                    OR_NUMBER = OR_NUMBER - 1;
                    printf("Patient (%d) is on surgery.\n", patient1.id);
                    randwait(SURGERY_TIME);
                    hungerRestroomMeterInc(patient1);
                    printf("Patient (%d) had a successfull operation.\n", patient1.id);
                    SURGEON_NUMBER = SURGEON_NUMBER+surgeonNum;
                    NURSE_NUMBER = NURSE_NUMBER+nurseNum;
                    OR_NUMBER = OR_NUMBER + 1;
                    int surgerycost = SURGERY_OR_COST +(surgeonNum*SURGERY_SURGEON_COST)+ (nurseNum * SURGERY_NURSE_COST);
                    HOSPITAL_WALLET+= surgerycost;
                    sem_post(&operatingroom);
                    
                    break;
                }
                 else
                {
                    printf("Patient (%d) is waiting for surgery room.\n",patient1.id);
                    randwait(WAIT_TIME); 
                    hungerRestroomMeterInc(patient1);
                }
            }
             else
            {
                printf("Patient (%d) is waiting for surgery surgeons and nurses to be avaliable.\n",patient1.id);
                randwait(WAIT_TIME); // Waiting for Pharmacy's queue.
                hungerRestroomMeterInc(patient1);
            }
            
        }

        int isPrescribed = rand() % 2;
        if (isPrescribed == 0) // IF PATIENT NEEDS MEDICINES AFTER SURGERY
        {
            printf("Patient (%d) is prescribed after surgeryand has been directed to the 'PHARMACY DEPARTMENT'\n",patient1.id);
        while (1)
        {
            if(sem_trywait(&pharmacy)==0){
            printf("Patient (%d) has found an empty pharmacy registration.\n", patient1.id  );
            randwait(PHARMACY_TIME); // Waiting for examination progress.
            hungerRestroomMeterInc(patient1);
            printf("Patient (%d) has bought medicines.\n", patient1.id);
            HOSPITAL_WALLET += rand()%200 + 1; 
            sem_post(&pharmacy);
            break;
        }
        else
        {
            printf("Patient (%d) is waiting for pharmacy registration.\n",patient1.id);
            randwait(WAIT_TIME); // Waiting for Pharmacy's queue.
            hungerRestroomMeterInc(patient1);
        }
        }
        }
        if (isPrescribed == 1) // IF PATIENT DOESN'T NEED MEDICINES AFTER SURGERY
        {
            printf("Patient (%d) isn't prescribed after surgery and is on way to home\n", patient1.id);
        }
        
    }
    


    
    
    

}