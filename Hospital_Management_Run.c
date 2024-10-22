#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATIENTS 100
#define MAX_DOCTORS 20
#define MAX_NAME_LENGTH 100
#define MAX_APPOINTMENTS 10

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char specialty[MAX_NAME_LENGTH];
} Doctor;

typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    int age;
    char** appointments; // Dynamic array for appointments
    int appointment_count;
    float total_bill;
    int allocated_room; // Room allocated to the patient
    char allocated_time[MAX_NAME_LENGTH]; // Time allocated for the room
    Doctor assigned_doctor; // Doctor assigned to the patient
} Patient;

Patient patients[MAX_PATIENTS];
Doctor doctors[MAX_DOCTORS];
int patient_count = 0;
int doctor_count = 0;

// Function prototypes
void save_patients();
void load_patients();
void register_patient();
void register_doctor();
Patient* verify_patient(int id);
Doctor* verify_doctor(int id);
void schedule_appointment(Patient* patient);
void delete_appointment(Patient* patient);
void bill_patient(Patient* patient);
void allocate_room(Patient* patient);
void verify_room_allocation(Patient* patient);
void assign_doctor(Patient* patient);
void verify_doctor_assignment(Patient* patient);
void display_patient_info(Patient* patient);
void display_statistics();
void free_memory();

void save_patients() {
    FILE *file = fopen("patients.dat", "wb");
    if (!file) {
        printf("Error saving data.\n");
        return;
    }
    fwrite(patients, sizeof(Patient), patient_count, file);
    fclose(file);
    printf("Data saved successfully.\n");
}

void load_patients() {
    FILE *file = fopen("patients.dat", "rb");
    if (!file) {
        printf("No data file found. Starting fresh.\n");
        return;
    }
    
    patient_count = fread(patients, sizeof(Patient), MAX_PATIENTS, file);
    fclose(file);
    
    for (int i = 0; i < patient_count; i++) {
        patients[i].appointments = malloc(MAX_APPOINTMENTS * sizeof(char*));
        for (int j = 0; j < MAX_APPOINTMENTS; j++) {
            patients[i].appointments[j] = malloc(MAX_NAME_LENGTH * sizeof(char));
        }
    }
    
    printf("Data loaded successfully. %d patients found.\n", patient_count);
}

void register_patient() {
    if (patient_count >= MAX_PATIENTS) {
        printf("Maximum patient limit reached!\n");
        return;
    }

    Patient new_patient;
    new_patient.id = patient_count + 1;

    printf("Enter patient's name: ");
    scanf(" %[^\n]", new_patient.name);

    while (1) {
        printf("Enter patient's age: ");
        if (scanf("%d", &new_patient.age) == 1 && new_patient.age > 0) {
            break;
        } else {
            printf("Invalid input. Please enter a positive integer for age.\n");
            while (getchar() != '\n'); // Clear invalid input
        }
    }

    new_patient.appointments = malloc(MAX_APPOINTMENTS * sizeof(char*));
    for (int j = 0; j < MAX_APPOINTMENTS; j++) {
        new_patient.appointments[j] = malloc(MAX_NAME_LENGTH * sizeof(char));
    }
    
    new_patient.appointment_count = 0;
    new_patient.total_bill = 0.0;

    // Initialize room and doctor details
    new_patient.allocated_room = -1; // Indicates no room allocated initially
    strcpy(new_patient.allocated_time, ""); // No time allocated initially
    new_patient.assigned_doctor.id = -1; // Indicates no doctor assigned initially

    patients[patient_count] = new_patient;
    patient_count++;

    printf("Patient registered successfully! ID: %d\n", new_patient.id);
}

void register_doctor() {
   if (doctor_count >= MAX_DOCTORS) {
       printf("Maximum doctor limit reached!\n");
       return;
   }

   Doctor new_doctor;
   new_doctor.id = doctor_count + 1;

   printf("Enter doctor's name: ");
   scanf(" %[^\n]", new_doctor.name);

   printf("Enter doctor's specialty: ");
   scanf(" %[^\n]", new_doctor.specialty);

   doctors[doctor_count] = new_doctor;
   doctor_count++;

   printf("Doctor registered successfully! ID: %d\n", new_doctor.id);
}

Patient* verify_patient(int id) {
   if (id <= 0 || id > patient_count) {
       return NULL;
   }
   return &patients[id - 1];
}

Doctor* verify_doctor(int id) {
   if (id <= 0 || id > doctor_count) {
       return NULL;
   }
   return &doctors[id - 1];
}

void schedule_appointment(Patient* patient) {
   if (patient->appointment_count >= MAX_APPOINTMENTS) {
       printf("Maximum appointment limit reached for this patient!\n");
       return;
   }

   printf("Enter appointment details: ");
   scanf(" %[^\n]", patient->appointments[patient->appointment_count]);
   
   patient->appointment_count++;
   
   printf("Appointment scheduled successfully.\n");
}

void delete_appointment(Patient* patient) {
   if (patient->appointment_count == 0) {
       printf("No appointments to delete for this patient.\n");
       return;
   }

   printf("Enter appointment index to delete (1 to %d): ", patient->appointment_count);
   
   int index;
   scanf("%d", &index);
   if (index < 1 || index > patient->appointment_count) {
       printf("Invalid index.\n");
       return;
   }

   // Shift appointments to remove the one at index
   free(patient->appointments[index - 1]); // Free memory of the deleted appointment
   for (int i = index - 1; i < patient->appointment_count - 1; i++) {
       strcpy(patient->appointments[i], patient->appointments[i + 1]);
   }
   
   patient->appointment_count--;
   printf("Appointment deleted successfully.\n");
}

void bill_patient(Patient* patient) {
   float amount;
   printf("Enter billing amount: ");
   while (scanf("%f", &amount) != 1 || amount < 0) {
       printf("Invalid input. Please enter a positive billing amount.\n");
       while (getchar() != '\n'); // Clear invalid input
   }
   
   patient->total_bill += amount;

   printf("Billing updated. Total bill for %s: $%.2f\n", patient->name, patient->total_bill);
}

void allocate_room(Patient* patient) {
   if (patient->allocated_room != -1) {
       printf("Room already allocated to this patient.\n");
       return;
   }

   int room_number;
   char time_allocated[MAX_NAME_LENGTH];

   printf("Enter room number: ");
   scanf("%d", &room_number);

   printf("Enter time allocated for the room: ");
   scanf(" %[^\n]", time_allocated);

   // Allocate room and set time
   patient->allocated_room = room_number;
   strcpy(patient->allocated_time, time_allocated);

   printf("Room %d allocated to %s at %s.\n", room_number, patient->name, time_allocated);
}

void verify_room_allocation(Patient* patient) {
   if (patient->allocated_room == -1) {
       printf("No room allocated to this patient.\n");
       return;
   }

   printf("Patient %s is allocated Room %d at %s.\n", 
          patient->name, 
          patient->allocated_room, 
          patient->allocated_time);
}

void assign_doctor(Patient* patient) {
   if (patient->assigned_doctor.id != -1) {
       printf("Doctor already assigned to this patient.\n");
       return;
   }

   int doctor_id;

   printf("Enter Doctor ID to assign: ");
   scanf("%d", &doctor_id);

   Doctor* doctor = verify_doctor(doctor_id);
   
   if (doctor) {
       // Assign doctor to the patient
       patient->assigned_doctor = *doctor;
       printf("Doctor %s assigned to Patient %s.\n", 
              doctor->name, 
              patient->name);
       
       return;
       
     } else {
         printf("Doctor not found.\n");
     }
}

void verify_doctor_assignment(Patient* patient) {
     if (patient->assigned_doctor.id == -1) {
         printf("No doctor assigned to this patient.\n");
         return;
     }

     printf("Patient %s is assigned Doctor %s with specialty %s.\n",
            patient->name,
            patient->assigned_doctor.name,
            patient->assigned_doctor.specialty);
}

void display_patient_info(Patient* patient) {
     printf("\nPatient ID: %d\n", patient->id);
     printf("Name: %s\n", patient->name);
     printf("Age: %d\n", patient->age);
     printf("Total Bill: $%.2f\n", patient->total_bill);

     // Display allocated room information
     if (patient->allocated_room != -1) {
         printf("Allocated Room Number: %d\n", 
                patient->allocated_room);
         printf("Allocated Time: %s\n", 
                patient->allocated_time);
     } else {
         printf("No room allocated.\n");
     }

     // Display assigned doctor information
     if (patient->assigned_doctor.id != -1) {
         printf("Assigned Doctor ID: %d\n", 
                patient->assigned_doctor.id);
         printf("Assigned Doctor Name: %s\n",
                patient->assigned_doctor.name);
         printf("Assigned Doctor Specialty: %s\n",
                patient->assigned_doctor.specialty);
     } else {
         printf("No doctor assigned.\n");
     }

     // Display appointments
     printf("\nAppointments:\n");
     
     for (int i = 0; i < patient->appointment_count; i++) {
         printf("\t%d: %s\n", i + 1, 
                patient->appointments[i]);
     }
}

void display_statistics() {
     float total_billing = 0.0;

     for (int i = 0; i < patient_count; i++) {
         total_billing += patients[i].total_bill;
     }

     float average_billing = (patient_count > 0) ? total_billing / 
                             ((float)(patient_count)) : 0;

     printf("\nTotal Registered Patients: %d\n", 
            patient_count);
     printf("\tTotal Registered Doctors: %d\n",
            doctor_count);     
     printf("\tAverage Billing Amount: $%.2f\n",
            average_billing);     
}

void free_memory() { 
      for (int i = 0; i < MAX_PATIENTS; i++) { 
          free(patients[i].appointments); 
      } 
} 

int main() { 
      load_patients(); 

      int choice, id; 

      while (1) { 
          printf("\nHospital Management System\n"); 
          printf("\t1. Register Patient\n"); 
          printf("\t2. Register Doctor\n"); 
          printf("\t3. Verify Patient\n"); 
          printf("\t4. Verify Doctor\n"); 
          printf("\t5. Schedule Appointment\n"); 
          printf("\t6. Delete Appointment\n"); 
          printf("\t7. Bill Patient\n"); 
          printf("\t8. Allocate Room\n"); 
          printf("\t9. Verify Room Allocation\n"); 
          printf("\t10. Assign Doctor\n"); 
          printf("\t11. Verify Doctor Assignment\n"); 
          printf("\t12. Display Patient Info\n"); 
          printf("\t13. Display Statistics\n"); 
          printf("\t14. Save Data\n"); 
          printf("\t15. Exit\n"); 

          if (scanf("%d", &choice) != 1) { 
              while(getchar() != '\n'); // Clear invalid input 
              continue; 
          } 

          switch(choice){  
              case 1:
                  register_patient();
                  break;  
              case 2:
                  register_doctor();
                  break;  
              case 3:
                  printf ("Enter Patient ID to verify:\n ");  
                  scanf ("%d",&id);  
                  Patient *p=verify_patient(id);  
                  if(p){  
                      display_patient_info(p);  
                  }else{  
                      puts ("Patient not found.");  
                  }  
                  break;  
              case 4:
                  puts ("Enter Doctor ID to verify:\n ");  
                  scanf ("%d",&id);  
                  Doctor *d=verify_doctor(id);  
                  if(d){  
                      puts ("Doctor found.");  
                      puts(d -> name);  
                      puts(d -> specialty);  
                  }else{  
                      puts ("Doctor not found.");  
                  }  
                  break;    
              case 5:
                  puts ("Enter Patient ID to schedule appointment:\n ");  
                  scanf ("%d",&id);  
                  p=verify_patient(id);  
                  if(p){  
                      schedule_appointment(p);  
                  }else{  
                      puts ("Patient not found.");  
                  }  
                  break;   
              case 6:
                   puts ("Enter Patient ID to delete appointment:\n ");  
                   scanf ("%d",&id);  
                   p=verify_patient(id);  
                   if(p){  
                       delete_appointment(p);   
                   }else{   
                       puts ("Patient not found.");   
                   }   
                   break;   
              case 7:
                   puts ("Enter Patient ID to bill:\n ");   
                   scanf ("%d",&id);   
                   p=verify_patient(id);   
                   if(p){   
                       bill_patient(p);   
                   }else{   
                       puts ("Patient not found.");   
                   }   
                   break;      
              case 8:
                   puts ("Enter Patient ID to allocate room:\n ");   
                   scanf ("%d",&id);   
                   p=verify_patient(id);   
                   if(p){   
                       allocate_room(p);   
                   }else{   
                       puts ("Patient not found.");   
                   }   
                   break;      
              case 9:
                   puts ("Enter Patient ID to verify room allocation:\n ");    
                   scanf ("%d",&id);    
                   p=verify_patient(id);    
                   if(p){    
                       verify_room_allocation(p);    
                   }else{    
                       puts ("Patient not found.");    
                   }    
                   break;      
              case 10:
                    puts ("Enter Patient ID to assign a doctor:\n ");    
                    scanf ("%d",&id);    
                    p=verify_patient(id);    
                    if(p){    
                        assign_doctor(p);    
                    }else{    
                        puts ("Patient not found.");    
                    }    
                    break;      
              case 11:
                    puts ("Enter Patient ID to verify doctor assignment:\n ");     
                    scanf ("%d",&id);     
                    p=verify_patient(id);     
                    if(p){     
                        verify_doctor_assignment(p);     
                    }else{     
                        puts ("Patient not found.");     
                    }     
                    break;      
              case 12:
                    puts ("Enter Patient ID to display info:\n ");     
                    scanf ("%d",&id);     
                    p=verify_patient(id);     
                    if(p){     
                        display_patient_info(p);     
                    }else{     
                        puts ("Patient not found.");     
                    }     
                    break;      
              case 13:
                     display_statistics();      
                     break;      
              case 14:
                     save_patients();      
                     break;      
              case 15:
                     free_memory();      
                     exit(0);      
              default:
                     puts ("Invalid choice. Please try again.");      
             }      
         }      

      return 0;      
}