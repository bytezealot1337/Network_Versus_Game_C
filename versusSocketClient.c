#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio_ext.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int playerCards[7][2];
    int opponentCards[7][2];
    int x, y;
    int playerItemToPlay[1];
    int opponentItemToPlay[1];
    int playerScore[1];
    int opponentScore[1];
    int winner[1];

    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;


    printf("\033c");

    printf("*****************************\n");
    printf("*                           *\n");
    printf("*       VERSUS V1.0         *\n");
    printf("*                           *\n");
    printf("*****************************\n\n");

    if(argc < 3)
    {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0)
    {
        error("ERROR opening socket");
        exit(0);
    }

    server = gethostbyname(argv[1]);
    if(server == NULL)
    {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if(connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR connecting");
        exit(0);
    }

    sleep(3);

    printf("\033c");

    n = read(sockfd, opponentCards, 14*sizeof(int));
    if(n < 0)
    {
         error("ERROR reading from socket");
    }

    n = read(sockfd, playerCards, 14*sizeof(int));
    if(n < 0)
    {
         error("ERROR reading from socket");
    }

    for(int iTurn = 0; iTurn < 7; iTurn++)
    {
        printf("\nYour hand is:\n\n");

        for(y = 0; y < 7; y++)
        {
          printf("  [%d][%d]  ", playerCards[y][0], playerCards[y][1]);
        }

        printf("\n\nOpponent hand is:\n\n");

        for(y = 0; y < 7; y++)
        {
          printf("  [%d][%d]  ", opponentCards[y][0], opponentCards[y][1]);
        }

        printf("\n\n");

        printf("Please wait while your opponent makes his play...\n");

        n = read(sockfd, opponentItemToPlay, sizeof(int));
        if(n < 0)
        {
             error("ERROR reading from socket");
        }

        printf("\nYour opponent chose to play [%d][%d]\n",
         opponentCards[opponentItemToPlay[0]][0],
          opponentCards[opponentItemToPlay[0]][1]);

        do
        {
            printf("\nChoose the number of the item you want to play (1 to 7): ");

            __fpurge(stdin);
            scanf("%d", &(playerItemToPlay[0]));

            (playerItemToPlay[0])--;

            if( (playerItemToPlay[0] >= 0) && (playerItemToPlay[0] <= 6) && (playerCards[playerItemToPlay[0]][0] != 0) )
            {
              printf("\nYou chose to play [%d][%d]\n\n", playerCards[playerItemToPlay[0]][0], playerCards[playerItemToPlay[0]][1]);
            }
            else if( (playerItemToPlay[0] < 0) || (playerItemToPlay[0] > 6) || (playerCards[playerItemToPlay[0]][0] == 0) )
            {
              printf("\nInvalid choice.\n");
            }

        } while( (playerItemToPlay[0] < 0) || (playerItemToPlay[0] > 6) || (playerCards[playerItemToPlay[0]][0] == 0) );

        n = write(sockfd, playerItemToPlay, sizeof(int));
        if(n < 0)
        {
            error("ERROR writing to socket");
            close(sockfd);
        }

        n = read(sockfd, winner, sizeof(int));
        if(n < 0)
        {
             error("ERROR reading from socket");
        }

        if(winner[0] == -1)
        {
          printf("\nEquality! No Harm no Foul...");
        }
        else
        {
          printf("\nAnd the Winner is ... ");
        }

        sleep(2);

        if(winner[0] == 1)
        {
          printf("YOU!!!\n");
        }
        else if (winner[0] == 0)
        {
          printf("Your opponent...\n");
        }

        sleep(2);

        playerCards[playerItemToPlay[0]][0] = 0;
        playerCards[playerItemToPlay[0]][1] = 0;

        opponentCards[opponentItemToPlay[0]][0] = 0;
        opponentCards[opponentItemToPlay[0]][1] = 0;

        printf("\033c");
    }

    n = read(sockfd, playerScore, sizeof(int));
    if(n < 0)
    {
         error("ERROR reading from socket");
    }

    n = read(sockfd, opponentScore, sizeof(int));
    if(n < 0)
    {
         error("ERROR reading from socket");
    }

    close(sockfd);

    printf("\nYour final score is %d.\n", playerScore[0]);

    printf("\nOpponent final score is %d.\n\n", opponentScore[0]);

    sleep(3);

    printf("\nPress any key to exit...\n");

    __fpurge(stdin);
    getchar();

    printf("\033c");

    return 0;
}
