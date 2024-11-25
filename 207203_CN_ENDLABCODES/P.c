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
#include <sys/stat.h>
#include <sys/types.h>
//#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<stddef.h>
#include<netinet/udp.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))
static struct cmsghdr *cmptr = NULL;
char *path = "./fsock";
char *path1="./esock";

int send_fd(int fd, int fd_to_send) {
    struct iovec iov[1];
    char ptr[2] = {0, 0};
    int newfd;

    struct msghdr m;

    iov[0].iov_base = ptr;
    iov[0].iov_len = 2;
    m.msg_iov = iov;
    m.msg_iovlen = 1;
    m.msg_name = NULL;
    m.msg_namelen = 0;

    cmptr = malloc(CONTROLLEN);
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    cmptr->cmsg_len = CONTROLLEN;
    m.msg_control = cmptr;
    m.msg_controllen = CONTROLLEN;
    *(int *)CMSG_DATA(cmptr) = fd_to_send;

    if(sendmsg(fd, &m, 0) < 0) {
        perror("sendmsg");
        return -1;
    }

    return 0;
}

int serv_listen() {
    int fd, len;
    struct sockaddr_un u;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    unlink(path);
    u.sun_family = AF_UNIX;
    strcpy(u.sun_path, path);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path);
    if(bind(fd, (struct sockaddr*)&u, len)) {
        perror("bind");
        exit(1);
    }

    if(listen(fd, 10) < 0) {
        perror("listen");
        exit(1);
    }

    return fd;
}

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
    strcpy(su.sun_path, path1);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path1);
    if(connect(fd, (struct sockaddr*)&su, len) < 0) {
        perror("connect");
        exit(1);
    }

    return fd;
}

int main() {
    struct sockaddr_un cli_addr;
    int len, ufd, nufd;

    len = sizeof(cli_addr);
    ufd = serv_listen();
    if((nufd = accept(ufd, (struct sockaddr*)&cli_addr, &len)) < 0) {
        perror("accept");
        exit(1);
    }

    int fd1 = open("performance.txt", O_RDONLY);
    char buff[100];
  //  read(fd1, buff, 7);
  //  printf("%s\n", buff);

    if(send_fd(nufd, fd1) < 0) {
        perror("send_fd");
        exit(1);
    }
    unlink(path);
  
    
    int fd;

    // FIFO file path
    char * myfifo = "/tmp/myfif";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    char arr1[80], arr2[80];
   // while (1)
    {
        

        // Open FIFO for Read only
        fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        read(fd, arr1, sizeof(arr1));

        // Print the read message
        printf("Judge's question : %s\n", arr1);
        close(fd);
    }
    
     int fd2;
printf("Answer : ");
    // FIFO file path
    char * myfif = "/tmp/myfi";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfif, 0666);

    char arr3[80], arr4[80];
  //  while (1)
    {
        // Open FIFO for write only
        fd2 = open(myfif, O_WRONLY);

        // Take an input arr2ing from user.
        // 80 is maximum length
        fgets(arr4, 80, stdin);

        // Write the input arr2ing on FIFO  // and close it
        write(fd2, arr4, strlen(arr4)+1);
        close(fd2);
    }
  
    
    
   
}

