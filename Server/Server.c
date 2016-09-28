#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <fcntl.h>

#define SERVER_PORT 5432
#define MAX_LINE 256

int main(int argc, char * argv[])
{
	char *fname;
        char buf[MAX_LINE];
	char buf2[MAX_LINE];
        struct sockaddr_in sin;
        int len;
        int s, i;
        struct timeval tv;
	char seq_num = 1; 
	FILE *fp;

        if (argc==2) {
                fname = argv[1];
        }
        else {
                fprintf(stderr, "usage: ./client_udp filename\n");
        	exit(1);
        }


        /* build address data structure */
        bzero((char *)&sin, sizeof(sin)); // cleaning -> makes sin all 0
        sin.sin_family = AF_INET;  // sin belongs to AFINET family. socket sin is addressed to AFINET family. ipv_4 internet protocol
        sin.sin_addr.s_addr = INADDR_ANY; // can take any legal address. You can add an ip address through this command. INADDR_ANY is 0
        sin.sin_port = htons(SERVER_PORT); // obtains server port (5432) htons converts integer port number to hardware port number. uses big endian or small endian according to the system.

        /* setup passive open */
        if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) { //PF_INET used, although AFINET can be used too. PFINET addresses a wider range of protocol family. this command creates a socket s and gives it arguments. SOCK_DGRAM uses udp datagram. And protocol number is 0 in usual cases.
                perror("simplex-talk: socket");
                close(s);
		 exit(1); // when error exist socket() returns value -1. hence the if statement. and exit the program.
        }
        if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) { // bind (socket file descriptor (int), sin memory location, size of sin). technically assigning name to the socket. 
                perror("simplex-talk: bind");
                close(s);
		 exit(1); // error if -1 is returned.
        }

// now the server connection is established
	socklen_t sock_len = sizeof sin; // getting size of sin and storing it
	srandom(time(NULL)); // random number generated

	fp = fopen(fname, "w"); //opening a file input later. we want to open and write the file.
        if (fp==NULL){
                printf("Can't open file\n"); // if fp is still NULL, then we couldn't open the file.
                exit(1);
        }
	
	while(1){
		len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &sock_len); //the server is now listening. it recieves messages from socket.(socket name, buffer, size of buffer, socket address and socket length)

          	if(len == -1){
            perror("PError");
		} 
	
		else if(len == 1){ // the last value after transmission completion writes at buffer address 0x02. hence after reading it, transmission completes.
			if (buf[0] == 0x02){
        	    		printf("Transmission Complete\n");
				break;
			}
        	    	else{ // if the last value after transmission completion writes at some other value, there is an error.
				perror("Error: Short packet\n");
			}
		}	
		else if(len > 1){ // here the server receives data from the buffer of the socket and writes it in a file fp.
			if(fputs((char *) buf, fp) < 1){
				printf("fputs() error\n");
		}}
	*buf2 = 0x01;
	if(sendto(s,buf2,1,0,(struct sockaddr *)&sin, sock_len)<0){
	perror("Sendto Error\n");
	exit(1);
	}
        }
	fclose(fp); //it is necessary to close filename
	close(s); // it is necessary to close socket or else ram fills fast and it may cause crashes.
}

