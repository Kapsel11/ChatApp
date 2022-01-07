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
        cout << "1: Login\n2: Register\n3: Poslanie spravy\n4: Pridanie kontaktu \n5: Odstranenie kontaktu\n6: Potvrdenie ziadosti "
                "\n7: Vytvorit skupinu\n8: Poslanie suboru\n9: Zrusenie konta\n10: Odhlasenie\n11: Zobrazit spravy\n12: Poslanie spravy skupine\n";
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
            char spravne[2];
            char buf[1024];
            bzero(buf,1024);
            read(sockfd,buf,1024);
            string f = buf;
            string fajnl;
            char a;
            int i = 0;
            cout << "Kontakty:\n";
            while (true)
            {
                a = f[i];
                if (a == '\0')
                {
                    cout << fajnl << endl;
                    break;
                }

                if (a == ',')
                {
                    cout << fajnl << endl;
                    fajnl = "";
                    i++;
                    continue;
                }

                fajnl += a;
                i++;
            }
            bzero(buffer, 256);
            cout << "Komu chcete poslat spravu:";
            cin >> buffer;
            //fgets(buffer,256,stdin);
           // std::getline(std::cin,f);
            int n;
            char zzzz;
            zzzz = getchar();
            sendUser(sockfd,n,buffer);
            bzero(buffer,256);
            cout << "Zadajte spravu:";
           //fgets(buffer,256,stdin);
            std::getline(std::cin,f);

            strcpy(buffer,f.c_str());

            sendUser(sockfd,n,buffer);

            n = read(sockfd, spravne, 1);
            if (strcmp(spravne,"1") == 0)
            {
                cout << "Sprava bola uspesne poslana\n";
            }
            else
            {
                cout << "Nastala chyba pri posielani spravy!\n";
            }

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
                    cout << "Kontakt bol uspesne pridany do nepotvrdenych kontaktov!\n";
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
            char spravne[2];
            char buf[1024];
            bzero(buf,1024);
            read(sockfd,buf,1024);
            string f = buf;
            string fajnl;
            char a;
            int i = 0;
            cout << "Kontakty:\n";
            while (true)
            {
                a = f[i];
                if (a == '\0')
                {
                    cout << fajnl << endl;
                    break;
                }

                if (a == ',')
                {
                    cout << fajnl << endl;
                    fajnl = "";
                    i++;
                    continue;
                }

                fajnl += a;
                i++;
            }
            bzero(buffer, 256);
            cout << "Zadajte ktory kontakt sa ma vymazat:";
            cin >> buffer;
            write(sockfd,buffer,255);
            bzero(spravne,2);
            read(sockfd,spravne,2);
            if (strcmp(spravne,"1") == 0)
            {
                cout << "Kontakt bol uspesne vymazany!\n";
            }
            else
                cout << "Nepodarilo sa vymazat kontakt!\n";
        }
        else if(volba == 6)
        {
            char spravne[2];
            char buf[1024];
            bzero(buf,1024);
            read(sockfd,buf,1024);
            string f = buf;
            string fajnl;
            char a;
            int i = 0;
            cout << "Ludia ktory sa s vami chcu spojit:\n";
            while (true)
            {
                a = f[i];
                if (a == '\0')
                {
                    cout << fajnl << endl;
                    break;
                }

                if (a == ',')
                {
                    cout << fajnl << endl;
                    fajnl = "";
                    i++;
                    continue;
                }

                fajnl += a;
                i++;
            }
            string pom;
            bzero(buffer, 256);
            cout << "Zadajte kto sa ma pridat do kontaktov:";
            cin >> pom;
            strcpy(buffer, pom.c_str());
            write(sockfd,buffer,255);
            bzero(spravne,2);
            read(sockfd, spravne, 1);

            if (strcmp(spravne,"1") == 0)
            {
                // Urobit nieco
                cout << "Kontakt bol uspesne pridany!\n";
            }
            else
                cout << "Nepodarilo sa pridat kontakt!\n";

        }
        else if(volba == 7)
        {

        }
        else if(volba == 8)
        {

        }
        else if(volba == 9)
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
        else if(volba == 10)
        {
            prihlaseny = false;
            cout << "Boli ste uspesne odhlaseny";
        }

        else if(volba == 11)
        {
            char spravne[2];
            char buf[1024];
            bzero(buf,1024);
            read(sockfd,buf,1023);
            string f = buf;
            string fajnl;
            char a;
            int i = 0;
            cout << "Kontakty:\n";
            while (true)
            {
                a = f[i];
                if (a == '\0')
                {
                    cout << fajnl << endl;
                    break;
                }

                if (a == ',')
                {
                    cout << fajnl << endl;
                    fajnl = "";
                    i++;
                    continue;
                }

                fajnl += a;
                i++;
            }
            bzero(buffer, 256);
            cout << "Zadajte s ktorym kontaktom chcete vidiet chat:";
            cin >> buffer;
            write(sockfd,buffer,255);
            bzero(spravne,2);
            read(sockfd,spravne,1);
            string jejeje;
            if (strcmp(spravne,"1") == 0)
            {
                cout << "Kontakt sa nasiel, nacitavam spravy...\n";
                while(true)
                {
                    bzero(buffer,256);
                    read(sockfd,buffer,255);
                    jejeje = buffer;
                    if(jejeje == "end")
                        break;
                    cout << buffer;
                }
            }
            else
                cout << "Nepodarilo sa najst chat s danym kontaktom!\n";
        }

        else if(volba == 12)
        {

        }

        else
        {
            break;
        }
    }

    close(sockfd);
    return 0;
}