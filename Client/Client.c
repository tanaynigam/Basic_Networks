#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
#define MAX_LINE 80

int main(int argc, char * argv[])
{
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char *fname;
	char buf[MAX_LINE];
	char buf2[MAX_LINE];
	int s;
	int slen;
	struct timeval tv;

	if (argc==3) {
		host = argv[1];
		fname= argv[2];
	}
	else {
		fprintf(stderr, "Usage: ./a.out host filename\n");
		exit(1);
	}
        hp = gethostbyname(host); // it is converting ip address fed into hosten and stored in hp
	/* translate host name into peerâ€™s IP address */
	if (!hp) { // Checks if there exists a hostent
		fprintf(stderr, "Unknown host: %s\n", host);
		exit(1);
	}

	fp = fopen(fname, "r"); // we read a file with filename provided and gives read only access
	if (fp==NULL){
		fprintf(stderr, "Can't open file: %s\n", fname);
		exit(1);
	}

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin)); // same as server code
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length); //bcopy copies any data (strcpy copies just string). copying address of structure of host(ip address of server) into address of structure of client socket.
	sin.sin_port = htons(SERVER_PORT);

       /* active open */
       if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) { //same as server code
       	perror("Socket");
       	close(s);
	exit(1);
       }
       	tv.tv_sec = 1;
 	tv.tv_usec = 0;
  
	srandom(time(0)); // generate random number
	socklen_t sock_len= sizeof sin;

	
	/* main loop: get and send lines of text */
	while(fgets(buf, 80, fp) != NULL){ // get values from the file fp and store in the socket  buffer
		buf2[0] = 0x00;
		slen = strlen(buf);
		buf[slen] ='\0'; // last value of the buffer at buffer address length+1 be added as /0 to indicate the end of buffer
		if(random()>RAND_MAX/4){
        	if(sendto(s, buf, slen+1, 0, (struct sockaddr *)&sin, sock_len)<0){ // if -1 then show error (socket id, buffer, length f the buffer+/0, flag, socket address, socket length)
			perror("SendTo Error\n");
			exit(1);
		printf(buf,"\n");
		}}
	while(buf2[0]==0x00){
	if (setsockopt(s,SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
 	perror("PError");
 	}
	if(recvfrom(s,buf2,sizeof(buf2),0,(struct sockaddr *)&sin, &sock_len)<0){
	perror("Perror");
	}
	if(buf2[0]==0x01){
	break;
	}
	else{ 
		 if(random()>RAND_MAX/4){
               if(sendto(s, buf, slen+1, 0, (struct sockaddr *)&sin, sock_len)<0){ // if -1 then show error (socket id, buffer, length f the buffer+/0, flag, socket address, socket length)
                        perror("SendTo Error\n");
                        exit(1);
                }}
		}

	}
	}

	
	*buf = 0x02;	// send 0x02 as the last value to denote end of transmission
        if(sendto(s, buf, 1, 0, (struct sockaddr *)&sin, sock_len)<0){
		perror("SendTo Error\n");
		exit(1);
	}
	fclose(fp);
        close(s); //close socket and file
}

