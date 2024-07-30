#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSGSZ 128

typedef struct msgbuf {
    long mtype;
    char mtext[MSGSZ];
} message_buf;

int main() {
    int msqid;
    key_t key;
    message_buf sbuf, rbuf;
    size_t buf_length;

    key = 1234; // Ключ для очереди сообщений

    if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0) {
        perror("msgget");
        exit(1);
    }

    while (1) {
        // Ввод сообщения пользователем
        printf("Enter your message: ");
        fgets(sbuf.mtext, MSGSZ, stdin);
        sbuf.mtext[strcspn(sbuf.mtext, "\n")] = '\0'; // Убираем символ новой строки

        sbuf.mtype = 1;
        buf_length = strlen(sbuf.mtext) + 1;

        if (msgsnd(msqid, &sbuf, buf_length, 0) < 0) {
            perror("msgsnd");
            exit(1);
        }

        if (strcmp(sbuf.mtext, "Quit") == 0) {
            break;
        }

        // Ожидание сообщения типа 2
        if (msgrcv(msqid, &rbuf, MSGSZ, 2, 0) < 0) {
            perror("msgrcv");
            exit(1);
        }

        printf("Message received: %s\n", rbuf.mtext);

        if (strcmp(rbuf.mtext, "Quit") == 0) {
            break;
        }
    }

    // Удаление очереди сообщений
    msgctl(msqid, IPC_RMID, NULL);

    return 0;
}
