#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stddef.h>
#include <fcntl.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip_icmp.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))
#define BUFFER_SIZE 4096

char *path1 = "./esock";

static struct cmsghdr *cmptr = NULL;
char *path = "./esock";

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

    unlink(path1);
    u.sun_family = AF_UNIX;
    strcpy(u.sun_path, path1);
    len = offsetof(struct sockaddr_un, sun_path) + strlen(path1);
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

#define h_addr h_addr_list[0]
//#define PORT 51720
#define LISTEN_PORT 12345  // Port number for Process J1

struct sockaddr_in source,dest;
FILE *logfile;

struct sockaddr_in serv_addr, cli_addr;
unsigned short chksum(void *b, int len) 
{    
    unsigned short *buf = b; 
    unsigned int sum = 0; 
    unsigned short result; 

    for(sum = 0;len > 1;len -= 2) 
        sum += *buf++; 
    if (len == 1) 
        sum += *(unsigned char*)buf; 
    sum = (sum >> 16) + (sum & 0xFFFF); 
    sum += (sum >> 16); 
    result = ~sum; 
    return result; 
}



int client_connect(int cli_port)
{
    int sfd;
    char *name = "localhost";
    
    struct hostent *server_details;
    server_details = gethostbyname(name);
    
    memset(&cli_addr, 0, sizeof(cli_addr));
    cli_addr.sin_family = AF_INET;
    cli_addr.sin_port = htons(cli_port);
    bcopy((char *) server_details->h_addr, (char *) &cli_addr.sin_addr.s_addr, server_details->h_length);

    if((sfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) 
    {
        perror("socket");
        exit(1);
    }

    int opt = 1;
    if((setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt))) < 0)
    {
        perror("setsockopt");
        exit(1);
    }

    if(bind(sfd, (struct sockaddr*)&cli_addr, sizeof(cli_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    return sfd;
}

int main() 
{
    // logfile=fopen("log.txt","w");
   // if(logfile==NULL) 
    //{
      //  printf("Unable to create file.");
    //}

int sock_raw;
    int optval = 1;
    struct sockaddr_in saddr;
    unsigned char buffer[100];
    int data_size;

    // create a raw socket
    sock_raw = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock_raw == -1) {
        perror("socket");
        exit(1);
    }

    // set socket options to receive all packets
    if (setsockopt(sock_raw, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0) {
        perror("setsockopt");
        exit(1);
    }
    
    

    // Set up address for listening
    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.2");  // INADDR_ANY
    saddr.sin_port = htons(LISTEN_PORT);
    if (bind(sock_raw, (struct sockaddr *)&saddr, sizeof(saddr)) < 0) {
        perror("bind");
        exit(1);
    }
    
 //   printf("Process 1 is listening on port %d\n", LISTEN_PORT);
    
    struct sockaddr_in serv_addr;
    serv_addr.sin_family= AF_INET;
    serv_addr.sin_addr.s_addr= inet_addr("127.0.0.1");
    serv_addr.sin_port= htons(51720);
    char buff[100];
        int len = sizeof(serv_addr);
        int r;
      //  printf("Recieving");
        if((r = recvfrom(sock_raw, buff, 100, 0, (struct sockaddr *)&serv_addr, &len)) < 0)
        {
            perror("recvfrom");
            exit(1);
        }
     //extracting ip header
        struct iphdr *iph = (struct iphdr*)buff;

        //converting ip address from number to . format
    //    char buff1[100];
    //    struct in_addr in;
     //   in.s_addr = iph->saddr;
     //   strcpy(buff1, inet_ntoa(in));
        
        printf("%s\n", buff + sizeof(struct iphdr) + sizeof(struct udphdr));
    
 //   printf("send\n");
 char str8[]="Ask Question and send score";
// char str2[]=
  if(strcmp(str8,buff + sizeof(struct iphdr) + sizeof(struct udphdr)) == 0 ){
   printf("Asking question :");
    int fd;

    // FIFO file path
    char * myfifo = "/tmp/myfif";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);

    char arr1[80], arr2[80];
  //  while (1)
    {
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Take an input arr2ing from user.
        // 80 is maximum length
        fgets(arr2, 80, stdin);

        // Write the input arr2ing on FIFO
        // and close it
        write(fd, arr2, strlen(arr2)+1);
        close(fd);
    }
    }
 /*   struct sockaddr_un cli_addr;
    int len, ufd, nufd;

    len = sizeof(cli_addr);
    ufd = serv_listen();
    if((nufd = accept(ufd, (struct sockaddr*)&cli_addr, &len)) < 0) {
        perror("accept");
        exit(1);
    }

   int fd1 = open("question.txt", O_RDONLY);
    char buff[100];
  //  read(fd1, buff, 7);
  //  printf("%s\n", buff);
printf("Sending");
   if(send_fd(nufd, fd1) < 0) {
        perror("send_fd");
        exit(1);
    }
    
    unlink(path1);
/*
char buff[BUFFER_SIZE];

while (1) {
        fd_set readfds;
        int max_fd;

        // Clear the file descriptor set
        FD_ZERO(&readfds);

        // Add the socket file descriptor to the set
        FD_SET(sock_raw, &readfds);
        max_fd = sock_raw;

        // Set the timeout (0 for non-blocking, NULL for blocking)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;

        // Wait for activity on the socket
        int activity = select(max_fd + 1, &readfds, NULL, NULL, &timeout);
        if (activity < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Check if the socket has data to read
        if (FD_ISSET(sock_raw, &readfds)) {
            // Receive data from the socket
            int num_bytes = recv(sock_raw, buff, sizeof(buffer), 0);
            if (num_bytes < 0) {
                perror("recv");
                exit(EXIT_FAILURE);
            } else if (num_bytes == 0) {
                printf("Connection closed by peer\n");
                break;
            } else {
                // Process the received data
                printf("Received %d bytes: %.*s\n", num_bytes, num_bytes, buff);
            }
        }
    }
/*

fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(sock_raw, &read_fds);
    int n = select(sock_raw + 1, &read_fds, NULL, NULL, NULL);
    if (n < 0) {
        perror("select");
        exit(EXIT_FAILURE);
    }
    printf("working");
    if (FD_ISSET(sock_raw, &read_fds)) {
        printf("Data received on socket!\n");
        
        // receive packets and print their content
  // while (1) {
        data_size = recv(sock_raw, buffer, 100, 0);
        if (data_size < 0) {
            perror("recv");
            exit(1);
      }
        printf("Received packet of size %d\n", data_size);
        // process the packet data here
        printf("ask question: in less tan 100 words");
        int qfd = open("question.txt", O_RDWR);
        char buf1[100];
        fgets(buf1, 100, stdin);
         write(qfd, buf1, strlen(buf1));
        int nufd=serv_listen();
         if(send_fd(nufd, qfd) < 0) {
        perror("send_fd");
        exit(1);
    }
    recv(nufd, buf1, 100, 0);
  //  read(fd1, buff, 7);
  //  printf("%s\n", buff);
   
    unlink(path);
        
       // close(qfd);
        
   // }
    }
    */
    
    printf("score from J1: ");
    
     int fd1;

    // FIFO file path
    char * myfif = "/tmp/myf";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfif, 0666);

    char arr3[80], arr4[80];
  //  while (1)
    {
        // Open FIFO for write only
        fd1 = open(myfif, O_WRONLY);

        // Take an input arr2ing from user.
        // 80 is maximum length
        fgets(arr4, 80, stdin);

        // Write the input arr2ing on FIFO
        // and close it
        write(fd1, arr4, strlen(arr4)+1);
        close(fd1);
    }

    

    // close the socket
    close(sock_raw);
    return 0;

   // int sfd = client_connect(PORT);
   // int i=0;
  /*  while(1) 
    {
        char buff[100];
        int len = sizeof(serv_addr);
        int r;
        if((r = recvfrom(sfd, buff, 100, 0, (struct sockaddr *)&serv_addr, &len)) < 0)
        {
            perror("recvfrom");
            exit(1);
        }
        //extracting ip header
        struct iphdr *iph = (struct iphdr*)buff;

        //converting ip address from number to . format
        char buff1[100];
        struct in_addr in;
        in.s_addr = iph->saddr;
        strcpy(buff1, inet_ntoa(in));
        printf("%s from : %s\n", buff + sizeof(struct iphdr) + sizeof(struct tcphdr), buff1);
        if(i==0)
        {
            print_ip_header(buff);
            // close(fileno(logfile));
            i++;
        }
    }
*/



   // return 0;
}




