
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
#include <sys/stat.h>
#include <sys/types.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
//#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<stddef.h>
#include<netinet/udp.h>

#define BUFFER_SIZE 1024

//int port1=51720;
//int port2=51721;
//int port3=51722;

#define DESTINATION_IP1 "127.0.0.2"  // IP address of Process J1
#define DESTINATION_IP2 "127.0.0.3"  // IP address of Process J2
#define DESTINATION_IP3 "127.0.0.4"  // IP address of Process J3

#define DESTINATION_PORT1 12345  // Port number of Process J1
#define DESTINATION_PORT2 23456  // Port number of Process J2
#define DESTINATION_PORT3 34567  // Port number of Process J3


unsigned short calculate_checksum(unsigned short *ptr, int nbytes) {
    unsigned long sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes == 1) {
        sum += *(unsigned char *)ptr;
    }

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);

    return (unsigned short)(~sum);
}

struct sigaction act;
		
void handler(int sig, siginfo_t *siginfo, void *context)
{
	printf("Signal received by Process with PID: %ld, UID: %ld\n",(long) siginfo->si_pid, (long) siginfo->si_uid);
}

int main()
{
 int fd1;

    // FIFO file path
    char * myfifo = "/tmp/myfifo";

    //
    mkfifo(myfifo, 0666);

    char str1[80], str2[80];
   // while (1)
    
        fd1 = open(myfifo,O_WRONLY);
        
        int pid=getpid();
     //   printf("process id: %d\n",pid);
        sprintf(str2,"%d",pid);
      //  fgets(str2, 80, stdin);
        write(fd1, str2, strlen(str2)+1);
        close(fd1);
    

//printf("Process P2 with PID: %d started!\n",pid);
	act.sa_sigaction = &handler;
	act.sa_flags = SA_SIGINFO;

	int k = sigaction(SIGUSR1 , &act , NULL);
	sleep(1);
	if(k < 0)
	{
		printf("Error in SIGUSR1\n");
		return 1;
	}
	else
	{
		printf("Performance completed.\n");
	}
	//sleep(5);
	//while(1);


//write(1,"From A: He\n",10);
   
  // signal(SIGUSR1,handler);
//printf("Performance is not completed");

   // printf("PID of anchor.c = %d\n", getpid());
int rsfd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
if(rsfd < 0)
    {
        perror("rsfd");
        exit(1);
    }
int one = 1;
    const int *val = &one;
    if (setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
        perror("setsockopt() error");
        return -1;
    }

//int raw_sockets[3] = { rsfd1, rsfd2, rsfd3 };

//int random_index = rand() % 3;

srand(time(NULL));
int destination = rand() % 3;

struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_addr.s_addr = inet_addr("127.0.0.1");
addr.sin_port = htons(51720); // change  source port

 if (bind(rsfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }
    

//int result = connect(raw_sockets[random_index], (struct sockaddr *)&addr, sizeof(addr));
//if (result == -1) {
  //  perror("connect failed");
  //  exit(1);
//}
   
    int  ret;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in dest_addr;
  //  struct iphdr *ip_header;


    // Set destination address
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
   // dest_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

 if (destination == 0) {
        dest_addr.sin_addr.s_addr = inet_addr(DESTINATION_IP1);
        dest_addr.sin_port = htons(DESTINATION_PORT1);
    } else if (destination == 1) {
        dest_addr.sin_addr.s_addr = inet_addr(DESTINATION_IP2);
        dest_addr.sin_port = htons(DESTINATION_PORT2);
    } else {
        dest_addr.sin_addr.s_addr = inet_addr(DESTINATION_IP3);
        dest_addr.sin_port = htons(DESTINATION_PORT3);
    }

//if (connect(rsfd, (struct sockaddr*)&dest_addr, sizeof(dest_addr)) == -1) {
  //      perror("connect");
    //    close(rsfd);
      //  exit(1);
    //}
    
   // printf("Connected to process %d\n", destination + 1);
//char message[4096] = "Ask question";
//if (sendto(rsfd, message, strlen(message)+1,0,(struct sockaddr *)&dest_addr,sizeof(dest_addr)) < 0) {
  //      perror("Message sending failed");
    //   exit(1);
    //}

   // printf("Message sent to the client\n");
    


// Set up the IP header
 char packet[4096];
    memset(packet, 0, sizeof(packet));
    
    struct iphdr *ip = (struct iphdr *)packet;
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen("Ask Question and send score");
    if (destination == 0) {
       ip->id = htons(DESTINATION_PORT1);
    } else if (destination == 1) {
        ip->id = htons(DESTINATION_PORT2);
    } else {
        ip->id = htons(DESTINATION_PORT3);
    }
    
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_RAW;  // change
    ip->check = 0;
    ip->saddr = inet_addr("127.0.0.1");
      if (destination == 0) {
       ip->daddr = inet_addr(DESTINATION_IP1);
    } else if (destination == 1) {
        ip->daddr = inet_addr(DESTINATION_IP2);
    } else {
        ip->daddr = inet_addr(DESTINATION_IP3);
    }
  //  ip->daddr = inet_addr(DEST_IP);

    // Set up the UDP header
    struct udphdr *udp = (struct udphdr *)(packet + sizeof(struct iphdr));
    udp->source = htons(51720);
    
    if (destination == 0) {
      udp->dest  = htons(DESTINATION_PORT1);
    } else if (destination == 1) {
       udp->dest  = htons(DESTINATION_PORT2);
    } else {
       udp->dest  = htons(DESTINATION_PORT3);
    }
  //  udp->dest = htons(DEST_PORT);
    udp->len = htons(sizeof(struct udphdr) + strlen("Ask Question and send score"));
    udp->check = 0;

    // Copy the payload
    char *payload = packet + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(payload, "Ask Question and send score");

    // Calculate the IP checksum
    ip->check = calculate_checksum((unsigned short *)packet, ip->tot_len);

    // Set up the destination address
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip->daddr;

    // Send the packet
    if (sendto(rsfd, packet, ip->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto() error");
        return -1;
    }

  //  printf("Packet sent successfully.\n");

if(destination == 0)
{
char packet1[4096];
    memset(packet1, 0, sizeof(packet1));
    
    struct iphdr *ip1 = (struct iphdr *)packet1;
    ip1->ihl = 5;
    ip1->version = 4;
    ip1->tos = 0;
    ip1->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen("send score");
  //  if (destination == 0) {
    //   ip->id = htons(DESTINATION_PORT1);
    //} else if (destination == 1) {
        ip1->id = htons(DESTINATION_PORT2);
   // } else {
     //   ip->id = htons(DESTINATION_PORT3);
   // }
    
    ip1->frag_off = 0;
    ip1->ttl = 64;
    ip1->protocol = IPPROTO_RAW;  // change
    ip1->check = 0;
    ip1->saddr = inet_addr("127.0.0.1");
  //    if (destination == 0) {
    //   ip->daddr = inet_addr(DESTINATION_IP1);
   // } else if (destination == 1) {
        ip1->daddr = inet_addr(DESTINATION_IP2);
   // } else {
     //   ip->daddr = inet_addr(DESTINATION_IP3);
    //}
  //  ip->daddr = inet_addr(DEST_IP);

    // Set up the UDP header
    struct udphdr *udp1 = (struct udphdr *)(packet1 + sizeof(struct iphdr));
    udp1->source = htons(51720);
    
  //  if (destination == 0) {
    //  udp->dest  = htons(DESTINATION_PORT1);
  //  } else if (destination == 1) {
       udp1->dest  = htons(DESTINATION_PORT2);
   // } else {
     //  udp->dest  = htons(DESTINATION_PORT3);
  //  }
  //  udp->dest = htons(DEST_PORT);
    udp1->len = htons(sizeof(struct udphdr) + strlen("send score"));
    udp1->check = 0;

    // Copy the payload
    char *payload1 = packet1 + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(payload1, "send score");

    // Calculate the IP checksum
    ip1->check = calculate_checksum((unsigned short *)packet1, ip1->tot_len);

    // Set up the destination address
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip1->daddr;

    // Send the packet
    if (sendto(rsfd, packet1, ip1->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto() error");
        return -1;
    }

char packet2[4096];
    memset(packet2, 0, sizeof(packet2));
    
    struct iphdr *ip2 = (struct iphdr *)packet2;
    ip2->ihl = 5;
    ip2->version = 4;
    ip2->tos = 0;
    ip2->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen("send score");
  //  if (destination == 0) {
    //   ip->id = htons(DESTINATION_PORT1);
   // } else if (destination == 1) {
     //   ip->id = htons(DESTINATION_PORT2);
   // } else {
        ip2->id = htons(DESTINATION_PORT3);
  //  }
    
    ip2->frag_off = 0;
    ip2->ttl = 64;
    ip2->protocol = IPPROTO_RAW;  // change
    ip2->check = 0;
    ip2->saddr = inet_addr("127.0.0.1");
    //  if (destination == 0) {
      // ip->daddr = inet_addr(DESTINATION_IP1);
   // } else if (destination == 1) {
     //   ip->daddr = inet_addr(DESTINATION_IP2);
    //} else {
        ip2->daddr = inet_addr(DESTINATION_IP3);
    //}
  //  ip->daddr = inet_addr(DEST_IP);

    // Set up the UDP header
    struct udphdr *udp2 = (struct udphdr *)(packet2 + sizeof(struct iphdr));
    udp2->source = htons(51720);
    
   // if (destination == 0) {
     // udp->dest  = htons(DESTINATION_PORT1);
  //  } else if (destination == 1) {
    //   udp->dest  = htons(DESTINATION_PORT2);
  //  } else {
       udp2->dest  = htons(DESTINATION_PORT3);
  //  }
  //  udp->dest = htons(DEST_PORT);
    udp2->len = htons(sizeof(struct udphdr) + strlen("and send score"));
    udp2->check = 0;

    // Copy the payload
    char *payload2 = packet2 + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(payload2, "send score");

    // Calculate the IP checksum
    ip2->check = calculate_checksum((unsigned short *)packet2, ip2->tot_len);

    // Set up the destination address
   // struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip2->daddr;

    // Send the packet
    if (sendto(rsfd, packet2, ip2->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto() error");
        return -1;
    }
}
else if(destination==1)
{
char packet3[4096];
    memset(packet3, 0, sizeof(packet3));
    
    struct iphdr *ip3 = (struct iphdr *)packet3;
    ip3->ihl = 5;
    ip3->version = 4;
    ip3->tos = 0;
    ip3->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen("send score");
   // if (destination == 0) {
       ip3->id = htons(DESTINATION_PORT1);
  //  } else if (destination == 1) {
    //    ip->id = htons(DESTINATION_PORT2);
  //  } else {
    //    ip->id = htons(DESTINATION_PORT3);
  //  }
    
    ip3->frag_off = 0;
    ip3->ttl = 64;
    ip3->protocol = IPPROTO_RAW;  // change
    ip3->check = 0;
    ip3->saddr = inet_addr("127.0.0.1");
   //   if (destination == 0) {
       ip3->daddr = inet_addr(DESTINATION_IP1);
   // } else if (destination == 1) {
     //   ip->daddr = inet_addr(DESTINATION_IP2);
   // } else {
     //   ip->daddr = inet_addr(DESTINATION_IP3);
    //}
  //  ip->daddr = inet_addr(DEST_IP);

    // Set up the UDP header
    struct udphdr *udp3 = (struct udphdr *)(packet3 + sizeof(struct iphdr));
    udp3->source = htons(51720);
    
  //  if (destination == 0) {
      udp3->dest  = htons(DESTINATION_PORT1);
 //   } else if (destination == 1) {
   //    udp->dest  = htons(DESTINATION_PORT2);
  //  } else {
    //   udp->dest  = htons(DESTINATION_PORT3);
  //  }
  //  udp->dest = htons(DEST_PORT);
    udp3->len = htons(sizeof(struct udphdr) + strlen("send score"));
    udp3->check = 0;

    // Copy the payload
    char *payload3 = packet3 + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(payload3, "send score");

    // Calculate the IP checksum
    ip3->check = calculate_checksum((unsigned short *)packet3, ip3->tot_len);

    // Set up the destination address
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip3->daddr;

    // Send the packet
    if (sendto(rsfd, packet3, ip3->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto() error");
        return -1;
    }
char packet4[4096];
    memset(packet4, 0, sizeof(packet4));
    
    struct iphdr *ip4 = (struct iphdr *)packet4;
    ip4->ihl = 5;
    ip4->version = 4;
    ip4->tos = 0;
    ip4->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen("send score");
 //   if (destination == 0) {
   //    ip->id = htons(DESTINATION_PORT1);
   // } else if (destination == 1) {
     //   ip->id = htons(DESTINATION_PORT2);
  //  } else {
        ip4->id = htons(DESTINATION_PORT3);
  //  }
    
    ip4->frag_off = 0;
    ip4->ttl = 64;
    ip4->protocol = IPPROTO_RAW;  // change
    ip4->check = 0;
    ip4->saddr = inet_addr("127.0.0.1");
    //  if (destination == 0) {
      // ip->daddr = inet_addr(DESTINATION_IP1);
//    } else if (destination == 1) {
  //      ip->daddr = inet_addr(DESTINATION_IP2);
  //  } else {
        ip4->daddr = inet_addr(DESTINATION_IP3);
   // }
  //  ip->daddr = inet_addr(DEST_IP);

    // Set up the UDP header
    struct udphdr *udp4 = (struct udphdr *)(packet4 + sizeof(struct iphdr));
    udp4->source = htons(51720);
    
  //  if (destination == 0) {
    //  udp->dest  = htons(DESTINATION_PORT1);
  //  } else if (destination == 1) {
    //   udp->dest  = htons(DESTINATION_PORT2);
   // } else {
       udp4->dest  = htons(DESTINATION_PORT3);
  //  }
  //  udp->dest = htons(DEST_PORT);
    udp4->len = htons(sizeof(struct udphdr) + strlen("send score"));
    udp4->check = 0;

    // Copy the payload
    char *payload4 = packet4 + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(payload4, "send score");

    // Calculate the IP checksum
    ip4->check = calculate_checksum((unsigned short *)packet4, ip4->tot_len);

    // Set up the destination address
  //  struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip4->daddr;

    // Send the packet
    if (sendto(rsfd, packet4, ip4->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto() error");
        return -1;
    }
}
else if(destination == 2)
{
char packet5[4096];
    memset(packet5, 0, sizeof(packet5));
    
    struct iphdr *ip5 = (struct iphdr *)packet5;
    ip5->ihl = 5;
    ip5->version = 4;
    ip5->tos = 0;
    ip5->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen("send score");
   // if (destination == 0) {
       ip5->id = htons(DESTINATION_PORT1);
    //} else if (destination == 1) {
      //  ip->id = htons(DESTINATION_PORT2);
  //  } else {
     //   ip->id = htons(DESTINATION_PORT3);
   // }
    
    ip5->frag_off = 0;
    ip5->ttl = 64;
    ip5->protocol = IPPROTO_RAW;  // change
    ip5->check = 0;
    ip5->saddr = inet_addr("127.0.0.1");
    //  if (destination == 0) {
       ip5->daddr = inet_addr(DESTINATION_IP1);
  //  } else if (destination == 1) {
    //    ip->daddr = inet_addr(DESTINATION_IP2);
  //  } else {
      //  ip->daddr = inet_addr(DESTINATION_IP3);
  //  }
  //  ip->daddr = inet_addr(DEST_IP);

    // Set up the UDP header
    struct udphdr *udp5 = (struct udphdr *)(packet5 + sizeof(struct iphdr));
    udp5->source = htons(51720);
    
  //  if (destination == 0) {
      udp5->dest  = htons(DESTINATION_PORT1);
  //  } else if (destination == 1) {
  //     udp->dest  = htons(DESTINATION_PORT2);
    //} else {
      // udp->dest  = htons(DESTINATION_PORT3);
   // }
  //  udp->dest = htons(DEST_PORT);
    udp5->len = htons(sizeof(struct udphdr) + strlen("send score"));
    udp5->check = 0;

    // Copy the payload
    char *payload5 = packet5 + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(payload5, "send score");

    // Calculate the IP checksum
    ip5->check = calculate_checksum((unsigned short *)packet5, ip5->tot_len);

    // Set up the destination address
    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip5->daddr;

    // Send the packet
    if (sendto(rsfd, packet5, ip5->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto() error");
        return -1;
    }
char packet6[4096];
    memset(packet6, 0, sizeof(packet6));
    
    struct iphdr *ip6 = (struct iphdr *)packet6;
    ip6->ihl = 5;
    ip6->version = 4;
    ip6->tos = 0;
    ip6->tot_len = sizeof(struct iphdr) + sizeof(struct udphdr) + strlen("send score");
  //  if (destination == 0) {
    //   ip->id = htons(DESTINATION_PORT1);
  //  } else if (destination == 1) {
        ip6->id = htons(DESTINATION_PORT2);
  //  } else {
    //    ip->id = htons(DESTINATION_PORT3);
  //  }
    
    ip6->frag_off = 0;
    ip6->ttl = 64;
    ip6->protocol = IPPROTO_RAW;  // change
    ip6->check = 0;
    ip6->saddr = inet_addr("127.0.0.1");
    //  if (destination == 0) {
    //   ip->daddr = inet_addr(DESTINATION_IP1);
  //  } else if (destination == 1) {
        ip6->daddr = inet_addr(DESTINATION_IP2);
  //  } else {
    //    ip->daddr = inet_addr(DESTINATION_IP3);
  //  }
  //  ip->daddr = inet_addr(DEST_IP);

    // Set up the UDP header
    struct udphdr *udp6 = (struct udphdr *)(packet6 + sizeof(struct iphdr));
    udp6->source = htons(51720);
    
  //  if (destination == 0) {
    //  udp->dest  = htons(DESTINATION_PORT1);
    //} else if (destination == 1) {
       udp6->dest  = htons(DESTINATION_PORT2);
 //   } else {
   //    udp->dest  = htons(DESTINATION_PORT3);
   // }
  //  udp->dest = htons(DEST_PORT);
    udp6->len = htons(sizeof(struct udphdr) + strlen("send score"));
    udp6->check = 0;

    // Copy the payload
    char *payload6 = packet6 + sizeof(struct iphdr) + sizeof(struct udphdr);
    strcpy(payload6, "send score");

    // Calculate the IP checksum
    ip6->check = calculate_checksum((unsigned short *)packet6, ip6->tot_len);

    // Set up the destination address
  //  struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = ip6->daddr;

    // Send the packet
    if (sendto(rsfd, packet6, ip6->tot_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("sendto() error");
        return -1;
    }
}

 int fd2;

    // FIFO file path
    char * myfif = "/tmp/myf";

    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfif, 0666);

    char arr1[80], arr2[80];
    int count=0;
    while (count<3)
    {
        
        // Open FIFO for Read only
        fd2 = open(myfif, O_RDONLY);

        // Read from FIFO
        read(fd2, arr1, sizeof(arr1));

        // Print the read message
        printf("Score : %s\n", arr1);
        close(fd2);
        
        count++;
    }
    





 
   
   close(rsfd);
   
    return 0;
}




