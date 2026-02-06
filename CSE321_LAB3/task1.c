#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>

struct shared {
    char sel[100];
    int b;
};

int main() {
    int memory_id;
    struct shared *data ;
    int message_pipe[2];
    pid_t pid;
  
    
    if (pipe(message_pipe) < 0) { perror("pipe"); exit(1); }

    memory_id = shmget(IPC_PRIVATE, sizeof(struct shared), IPC_CREAT | 0666);
    data = (struct shared *)shmat(memory_id, NULL, 0);
    data ->b = 1000;


    printf("Provide Your Input From Given Options:\n");
    printf("1. Type a to Add Money\n");
    printf("2. Type w to Withdraw Money\n");
    printf("3. Type c to Check Balance\n");
    scanf("%s", data ->sel);
    printf("Your selection: %s\n", data ->sel);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    } else if (pid == 0) { 
        close(message_pipe[0]); 

        if (strcmp(data ->sel, "a") == 0) {
            int amount;
            printf("Enter amount to be added:\n");
            scanf("%d", &amount);
            if (amount > 0) {
                data ->b += amount;
                printf("Balance added successfully\n");
                printf("Updated balance after addition:\n%d\n", data ->b);
            } else {
                printf("Adding failed, Invalid amount\n");
            }
        } else if (strcmp(data ->sel, "w") == 0) {
            
            
            int amount;
            printf("Enter amount to be withdrawn:\n");
            scanf("%d", &amount);
            if (amount > 0 && amount <= data ->b) {
                data ->b -= amount;
                printf("Balance withdrawn successfully\n");
                printf("Updated balance after withdrawal:\n%d\n", data ->b);
            } else {
                printf("Withdrawal failed, Invalid amount\n");
            }
        } else if (strcmp(data ->sel, "c") == 0) {
            printf("Your current balance is:\n%d\n", data ->b);
        } else {
            printf("Invalid selection\n");
        }

        write(message_pipe[1], "Thank you for using", 20);
        close(message_pipe[1]);
        exit(0);
    } else { 
        close(message_pipe[1]); 
        wait(NULL);

        char buffer[100];
        read(message_pipe[0], buffer, sizeof(buffer));
        printf("%s\n", buffer);

        close(message_pipe[0]);
        shmdt(data);
        shmctl(memory_id, IPC_RMID, NULL);
    }

    return 0;
}


