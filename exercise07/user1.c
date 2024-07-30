#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MSGSZ 128
#define QUEUE_NAME "/chat_queue"

typedef struct msgbuf {
    long mtype;
    char mtext[MSGSZ];
} message_buf;

int main() {
    mqd_t mq;
    struct mq_attr attr;
    message_buf sbuf, rbuf;
    unsigned int priority;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MSGSZ;
    attr.mq_curmsgs = 0;

    mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    while (1) {
        printf("Enter your message: ");
        fgets(sbuf.mtext, MSGSZ, stdin);
        sbuf.mtext[strcspn(sbuf.mtext, "\n")] = '\0';

        if (mq_send(mq, sbuf.mtext, MSGSZ, 1) == -1) {
            perror("mq_send");
            exit(1);
        }

        if (strcmp(sbuf.mtext, "Quit") == 0) {
            break;
        }

        if (mq_receive(mq, rbuf.mtext, MSGSZ, &priority) == -1) {
            perror("mq_receive");
            exit(1);
        }

        printf("Message received: %s\n", rbuf.mtext);

        if (strcmp(rbuf.mtext, "Quit") == 0) {
            break;
        }
    }

    mq_close(mq);
    mq_unlink(QUEUE_NAME);

    return 0;
}