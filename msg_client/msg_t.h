#define MAXBUF 512

typedef struct {
    long mtype;
    char buf[MAXBUF];
} message_t;
