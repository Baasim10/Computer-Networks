#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
//#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include<stddef.h>
//#include<libproc.h>
#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;
char *path = "./fsock";

int recv_fd(int fd) {

    struct iovec iov[1];
    char ptr[2];
    int newfd;

    struct msghdr m;

    iov[0].iov_base = ptr;
    iov[0].iov_len = sizeof(ptr);
    m.msg_iov = iov;
    m.msg_iovlen = 1;
    m.msg_name = NULL;
    m.msg_namelen = 0;

   
    cmptr = malloc(CONTROLLEN);
    m.msg_control = cmptr;
    m.msg_controllen = CONTROLLEN;

    if(recvmsg(fd, &m, 0) < 0) {
        perror("recvmsg");
        return -1;
    }

    newfd = *(int *)CMSG_DATA(cmptr);

    return newfd;
}


int cli_conn(char *name) {
    int fd, len;
    struct sockaddr_un u, su;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    unlink(name);
    u.sun_family = AF_UNIX;
    strcpy(u.sun_path, name);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
    if(bind(fd, (struct sockaddr*)&u, len)) {
        perror("bind");
        exit(1);
    }

    su.sun_family = AF_UNIX;
    strcpy(su.sun_path, path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    if(connect(fd, (struct sockaddr*)&su, len) < 0) {
        perror("connect");
        exit(1);
    }

    return fd;
}


int main() {

printf("Performance: \n");

    int ufd, rfd;
    char *name = "./csock";

    ufd = cli_conn(name);
    if((rfd = recv_fd(ufd)) < 0) {
        perror("recv_fd");
        exit(1);
    }
    char buff[100];
    read(rfd, buff, 30);
    printf("%s\n", buff);
   // buff[0] = '\0';
   // send(ufd, buff, sizeof(buff), 0);

  // unlink(name);
   
    // notify A send signal to A
  //  printf("Notify A \n");
   
    int fd;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    char arr1[80], arr2[80];
  
        close(fd);

        // Open FIFO for Read only
        fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        read(fd, arr1, sizeof(arr1));


int process_id=atoi(arr1);
//printf("process id : %d\n",process_id);
        // Print the read message
      //  printf("User2: %s\n", arr1);
        close(fd);
    
   kill(process_id,SIGUSR1);
  // printf("kill signal\n");
   
   
   int fd1;

    // FIFO file path
    char * myfif = "/tmp/myfi";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfif, 0666);

    char arr3[80], arr4[80];
   // while (1)
    {
        

        // Open FIFO for Read only
        fd1 = open(myfif, O_RDONLY);

        // Read from FIFO
        read(fd1, arr3, sizeof(arr3));

        // Print the read message
        printf("Answer from participant : %s\n", arr3);
        close(fd1);
    }
   

   
   return 0;
}









