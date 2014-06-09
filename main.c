#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>

#include <errno.h>
#include <pthread.h>


void *sendICMP()
{
	struct sockaddr_in sa;
	struct hostent *he;
	struct protoent *pr;
	struct icmphdr *icmp;
	struct iphdr *ip;
	char *host;
	int s, n;
	unsigned char pqt[4096];
	long sum;
	unsigned short *p;
	unsigned short impar;

	char *noseascabron = "192.168.0.1";

	if(inet_aton(noseascabron, sa.sin_addr) == 0)
	{
		printf("\nNEL con inet_aton\n");
		return -1;
	}

	sa.sin_family = AF_INET;
	sa.sin_port = 0;

	if(!(pr = getprotobyname("icmp")))
	{
		perror("ERROR AL OBTENER PROTOCOLO");
		exit(1);
	}

	icmp = (struct icmp *)pqt;

	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->checksum = 0;
	icmp->un.echo.id = getpid();
	int i=0;
	while(1)
	{		

	icmp->un.echo.sequence = i;
	if((s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("ERROR AL OBTENER SOCKET");
		exit(1);
	}
	if(sendto(s, &pqt, sizeof(struct icmphdr), 0, (struct sockaddr *) &sa, sizeof(sa)) < 0)
	{
		perror("Error al enviar paquete");
		exit(1);
	}
	i++;

	close(s);
	sleep(1);

	}
	
	//Envio


}

void recvICMP()


{

	struct sockaddr_in sa;
	struct hostent *he;
	struct protoent *pr;
	struct icmphdr *icmp;
	struct iphdr *ip;
	char *host;
	int s, n;
	unsigned char pqt[4096];
	long sum;
	unsigned short *p;
	unsigned short impar;
	do
	{
		bzero(&sa, sizeof(sa));
		bzero(pqt, sizeof(pqt));

	int aa;
	if((aa = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{
		perror("ERROR AL OBTENER SOCKET");
		exit(1);
	}

		if(recvfrom(aa, &pqt, sizeof(pqt), 0, (struct sockaddr_in *)&sa, &n) < 0)
		{
			perror("Error al recibir");
			exit(1);
		}

		ip = (struct iphdr *)&pqt;
		icmp = (struct  icmphdr *)(&pqt[0] + (ip->ihl << 2));
		if((icmp->type == ICMP_ECHOREPLY) && (icmp->un.echo.id == getpid()))
		{
			printf("\nRecibido ECHO REPLY\n");
			//fprintf(stdout, "Recibido ECHO REPLY desde %s\n", host?host:argv[1]);
		} 
		else 
		{
			fprintf(stdout, "Tipo: %i Codigo: %i Id: %i Secuencia: %i\n", 
				icmp->type, icmp->code, icmp->un.echo.id, icmp->un.echo.sequence);
		}
	}while(1);
}

pthread_t tid[2];

int main(int argc, char *argv[])
{
	int err;

    err = pthread_create(&(tid[0]), NULL, &sendICMP, NULL);
        if (err != 0)
            printf("\ncan't create thread :[%s]", strerror(err));
        else
            printf("\n Sending...\n");

    recvICMP();
    return 0;
}

