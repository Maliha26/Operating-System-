#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define NUM_STUDENTS 10
#define WAITING_CHAIRS 3

sem_t stu_sem;    
sem_t st_sem;          
pthread_mutex_t mutex; 

int waiting_stu = 0;
int stu_served = 0;
void* stu_thread(void* arg) {
    int id = *(int*)arg;
    int waiting_time = rand() % 3 + 1;
    sleep(waiting_time);
    
    pthread_mutex_lock(&mutex);
    while (1) {
        if (waiting_stu < WAITING_CHAIRS) {
            waiting_stu++;
            printf("Student %d started waiting for consultation\n", id);
            
            if (waiting_stu == 1) {
                sem_post(&st_sem);
            }
            pthread_mutex_unlock(&mutex);
            
            sem_wait(&stu_sem);
            printf("Student %d is getting consultation\n", id);
            sleep(1);
            
            pthread_mutex_lock(&mutex);
            stu_served++;
            printf("Student %d finished getting consultation and left\n", id);
            printf("Number of served students: %d\n", stu_served);
            pthread_mutex_unlock(&mutex);
            break;
        } else {
            printf("No chairs remaining in lobby. Student %d Leaving...\n", id);
            pthread_mutex_unlock(&mutex);
            break;
        }
    }
    return NULL;
}

void* st_thread(void* arg) {
    while (stu_served < NUM_STUDENTS) {
        sem_wait(&st_sem);
        pthread_mutex_lock(&mutex);
        
        while (waiting_stu > 0) {
            waiting_stu--;
            printf("A waiting student started getting consultation\n"
                   "Number of students now waiting: %d\n"
                   "ST giving consultation\n", waiting_stu);
            pthread_mutex_unlock(&mutex);
            
            sem_post(&stu_sem);
            sleep(1);
            
            pthread_mutex_lock(&mutex);
        }
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}
int main() {
    srand(time(NULL));
    sem_init(&stu_sem, 0, 0);
    sem_init(&st_sem, 0, 0);
    pthread_mutex_init(&mutex, NULL);

    pthread_t st;
    pthread_create(&st, NULL, st_thread, NULL);

    pthread_t students[NUM_STUDENTS];
    int student_ids[NUM_STUDENTS];
    
    for (int i = 0; i < NUM_STUDENTS; i++) {
        student_ids[i] = i;
        pthread_create(&students[i], NULL, stu_thread, &student_ids[i]);
    }
    for (int i = 0; i < NUM_STUDENTS; i++) {
        pthread_join(students[i], NULL);
    }
    pthread_join(st, NULL);
  
    sem_destroy(&stu_sem);
    sem_destroy(&st_sem);
    pthread_mutex_destroy(&mutex);
    
    return 0;
}

