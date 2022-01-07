#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

using namespace std;

void sendUser(int sockfd, int n, char buffer[])
{
    n = write(sockfd, buffer, strlen(buffer));
    if (n = 0)
    {
        printf("ERROR writing to socket");
    }
}

void sendPass(int sockfd, int n, char buffer[])
{
    n = write(sockfd, buffer, strlen(buffer));
    if (n = 0)
    {
        printf("ERROR writing to socket");
    }
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char buffer[256];

    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }

    bool pokracovat = true;
    bool prihlaseny;
    while(pokracovat)
    {
        int volba;
        cout << "1: Login\n2: Register\n3: Poslanie spravy\n4: Pridanie kontaktu \n5: Odstranenie kontaktu\n6: Vytvorit skupinu\n7: Poslanie suboru\n8: Zrusenie konta\n9: Odhlasenie\n10: Ukoncenie\n";
        cin >> buffer;
        write(sockfd,buffer,2);
        volba = atoi(buffer);
        if(volba == 1)      // Login
        {
            int pocet_pokusov = 3;
            char spravne[10];
            for(int i = 0; i < pocet_pokusov; i++)
            {
                cout << "Zadaj login:";
                bzero(buffer, 256);
                cin >> buffer;
                sendUser(sockfd,n,buffer);
                n = read(sockfd, spravne, 1);

                if (strcmp(spravne,"1") == 0)
                {
                    spravne[0] = '0';
                    cout << "Zadaj heslo:";
                    bzero(buffer, 256);
                    cin >> buffer;
                    sendPass(sockfd,n,buffer);
                    n = read(sockfd, spravne, 255);
                    if (strcmp(spravne,"1") == 0)
                    {
                        cout << "Boli ste uspesne prihlaseny!\n";
                        prihlaseny = true;
                        break;
                    }
                    else
                    {
                        cout << "Zle heslo!\n";
                    }

                }
                else
                {
                    cout << "Uzivatel s tymto loginom neexistuje!\n";
                }
            }
        }
        else if(volba == 2) // register
        {
            int pocet_pokusov = 3;
            char spravne[10];
            for(int i = 0; i < pocet_pokusov; i++)
            {
                cout << "Zadaj login:";
                bzero(buffer, 256);
                cin >> buffer;
                sendUser(sockfd,n,buffer);
                n = read(sockfd, spravne, 1);
                if (strcmp(spravne,"1") == 0)
                {
                    spravne[0] = '0';
                    cout << "Zadaj heslo:";
                    bzero(buffer, 256);
                    cin >> buffer;
                    sendPass(sockfd,n,buffer);
                    cout << "Boli ste uspesne zaregistrovany!\n";
                    break;
                }
                else
                {
                    cout << "Uzivatel s tymto loginom uz existuje!\n";
                }
            }

        }
        else if(volba == 3) // Poslanie spravy
        {

        }
        else if(volba == 4) // Pridanie kontaktu
        {
            char spravne[10];
            read(sockfd, spravne, 1);
            if(prihlaseny && (strcmp(spravne,"1") == 0))
            {
                string pom;
                cout << "Zadaj kontakt:";
                bzero(buffer, 256);
                cin >> pom;
               // pom.append(",");
                int n = pom.length();

                strcpy(buffer, pom.c_str());

                //buffer = pom;

                write(sockfd,buffer,255);
                bzero(spravne,10);
                read(sockfd, spravne, 1);

                if((strcmp(spravne,"1") == 0))
                {
                    cout << "Kontakt bol uspesne pridany!\n";
                }
                else
                {
                    cout << "Nepodarilo sa pridat kontakt!\n";
                }
            }
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }
        }
        else if(volba == 5)
        {

        }
        else if(volba == 6)
        {

        }
        else if(volba == 7)
        {

        }
        else if(volba == 8)
        {
            char spravne[10];
            bzero(spravne,10);
            read(sockfd, spravne, 1);
            int f = atoi(spravne);
            switch(f){
                case 0:
                    cout << "Nepodarilo sa odstranit ucet\n";
                    break;
                case 1:
                    cout << "Ucet bol uspesne odstraneny\n";
                    break;
                case 2:
                    cout << "Najskor sa musite prihlasit!\n";
                    break;
            }
        }
        else if(volba == 9)
        {
            prihlaseny = false;
            cout << "Boli ste uspesne odhlaseny";
        }
        else
        {
            break;
        }
    }

    close(sockfd);
    return 0;
}