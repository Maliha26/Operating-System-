
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>

struct msg {
    long int type;
    char txt[6];
};


int main() {
    key_t key = ftok(".", 'a');
    int msg_que_id = msgget(key, IPC_CREAT | 0666);
    struct msg message;
    pid_t otp_gen_pid, mail_pros_pid;

    printf("Please enter the workspace name:\n");
    char workspace[10];
    scanf("%s", workspace);

    if (strcmp(workspace, "cse321") != 0) {
        printf("Invalid workspace name\n");
        return 0;
    }

    message.type = 1;
    strcpy(message.txt, workspace);
    msgsnd(msg_que_id, &message, sizeof(message.txt), 0);
    printf("Workspace name sent to otp generator from log in: %s\n", message.txt);

    otp_gen_pid = fork();
    if (otp_gen_pid == 0) { 
        msgrcv(msg_que_id, &message, sizeof(message.txt), 1, 0);
        printf("OTP generator received workspace name from log in: %s\n", message.txt);


        int otp = getpid();
        sprintf(message.txt, "%d", otp);

        message.type = 2;
        msgsnd(msg_que_id, &message, sizeof(message.txt), 0);
        printf("OTP sent to log in from OTP generator: %s\n", message.txt);

   
        message.type = 3;
        msgsnd(msg_que_id, &message, sizeof(message.txt), 0);
        printf("OTP sent to mail from OTP generator: %s\n", message.txt);

        mail_pros_pid = fork();
        if (mail_pros_pid == 0) { 
            msgrcv(msg_que_id, &message, sizeof(message.txt), 3, 0);
            printf("Mail received OTP from OTP generator: %s\n", message.txt);


            message.type = 4;
            msgsnd(msg_que_id, &message, sizeof(message.txt), 0);
            printf("OTP sent to log in from mail: %s\n", message.txt);
            exit(0);
        }
        wait(NULL);
        exit(0);
    }

    wait(NULL);

    msgrcv(msg_que_id, &message, sizeof(message.txt), 2, 0);
    printf("Log in received OTP from OTP generator: %s\n", message.txt);
    char otp_gen[6];
    strcpy(otp_gen, message.txt);


    msgrcv(msg_que_id, &message, sizeof(message.txt), 4, 0);
    printf("Log in received OTP from mail: %s\n", message.txt);
    char otp_mail[6];
    strcpy(otp_mail, message.txt);


    if (strcmp(otp_gen, otp_mail) == 0) {
        printf("OTP Verified\n");
    } else {
        printf("OTP Incorrect\n");
    }

    msgctl(msg_que_id, IPC_RMID, NULL);
    return 0;
}

