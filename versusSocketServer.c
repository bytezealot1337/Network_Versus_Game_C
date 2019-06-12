#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio_ext.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int playerCards[7][2];
    int opponentCards[7][2];
    int x, y;
    int playerItemToPlay[1];
    int opponentItemToPlay[1];
    int randomResultPlayer;
    int randomResultOpponent;
    int winner[1];
    int opponentScore[1];
    int playerScore[1];

    int sockfd, newsockfd, portno;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    int n;


    if(argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if(sockfd < 0)
    {
        error("ERROR opening socket");
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        error("ERROR on binding");
        exit(0);
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if(newsockfd < 0)
    {
        error("ERROR on accept");
        exit(0);
    }

    printf("\033c");

    printf("*****************************\n");
    printf("*                           *\n");
    printf("*   VERSUS V1.0 (C)ZAPPS    *\n");
    printf("*                           *\n");
    printf("*****************************\n\n");

    sleep(3);

    printf("\033c");

    srand( time(NULL) );  /* initialize random seed */

    for(y = 0; y < 7; y++)
    {
        for(x = 0; x < 2; x++)
        {
          playerCards[y][x] = 0;
          opponentCards[y][x] = 0;
        }
    }

    for(y = 0; y < 7; y++)
    {
        for(x = 0; x < 2; x++)
        {
          playerCards[y][x] = rand()%9+1;
          opponentCards[y][x] = rand()%9+1;
        }
    }

    n = write(newsockfd, playerCards, 14*sizeof(int));
    if(n < 0)
    {
        error("ERROR writing to socket");
        close(newsockfd);
        close(sockfd);
    }

    n = write(newsockfd, opponentCards, 14*sizeof(int));
    if(n < 0)
    {
        error("ERROR writing to socket");
        close(newsockfd);
        close(sockfd);
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

        n = write(newsockfd, playerItemToPlay, sizeof(int));
        if(n < 0)
        {
            error("ERROR writing to socket");
            close(newsockfd);
            close(sockfd);
        }

        n = read(newsockfd, opponentItemToPlay, sizeof(int));
        if(n < 0)
        {
            error("ERROR reading from socket");
            close(newsockfd);
            close(sockfd);
        }

        randomResultPlayer = rand()%(playerCards[playerItemToPlay[0]][0])+1;
        randomResultOpponent = rand()%(opponentCards[opponentItemToPlay[0]][0])+1;

        if(randomResultPlayer > randomResultOpponent)
        {
            playerScore[0] = playerScore[0] + playerCards[playerItemToPlay[0]][1];
            winner[0] = 0;
        }
        else if(randomResultPlayer < randomResultOpponent)
        {
            opponentScore[0] = opponentScore[0] + opponentCards[opponentItemToPlay[0]][1];
            winner[0] = 1;
        }
        else if(randomResultPlayer == randomResultOpponent)
        {
            winner[0] = -1;
        }

        n = write(newsockfd, winner, sizeof(int));
        if(n < 0)
        {
            error("ERROR writing to socket");
            close(newsockfd);
            close(sockfd);
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

        if(winner[0] == 0)
        {
          printf("YOU!!!\n");
        }
        else if (winner[0] == 1)
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

    n = write(newsockfd, opponentScore, sizeof(int));
    if(n < 0)
    {
        error("ERROR writing to socket");
        close(newsockfd);
        close(sockfd);
    }

    n = write(newsockfd, playerScore, sizeof(int));
    if(n < 0)
    {
        error("ERROR writing to socket");
        close(newsockfd);
        close(sockfd);
    }

    close(newsockfd);
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
