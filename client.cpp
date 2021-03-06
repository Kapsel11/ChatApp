#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include "client.h"

using namespace std;

int main(int argc, char *argv[])
{
    int SIFRA = 4;
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
    bool prihlaseny = false;

    while(pokracovat)
    {
        int volba;
        cout << "1: Login\n2: Register\n3: Poslanie spravy\n4: Pridanie kontaktu \n5: Odstranenie kontaktu\n6: Potvrdenie ziadosti "
                "\n7: Vytvorit skupinu\n8: Poslanie suboru\n9: Zrusenie konta\n10: Odhlasenie\n"
                "11: Zobrazit spravy\n12: Poslanie spravy skupine\n13: Pridanie do skupiny\n14: Zobrazenie sprav v skupine\n15: Notifikacie";
        cin >> buffer;
        volba = atoi(buffer);
        if(volba == 1)      // Login
        {
            write(sockfd,buffer,2);
            int pocet_pokusov = 3;
            char spravne[10];
            for(int i = 0; i < pocet_pokusov; i++)
            {
                cout << "Zadaj login:";
                bzero(buffer, 256);
                cin >> buffer;


                send(sockfd, n, buffer);
                n = read(sockfd, spravne, 1);

                if (strcmp(spravne,"1") == 0)
                {
                    spravne[0] = '0';
                    cout << "Zadaj heslo:";
                    bzero(buffer, 256);
                    cin >> buffer;
                    string you = buffer;

                    for(int i = 0; i < you.size(); i++)
                    {
                        you[i] = you[i] + SIFRA;
                    }

                    strcpy(buffer,you.c_str());

                    send(sockfd,n,buffer);
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
            write(sockfd,buffer,2);
            int pocet_pokusov = 3;
            char spravne[10];
            for(int i = 0; i < pocet_pokusov; i++)
            {
                cout << "Zadaj login:";
                bzero(buffer, 256);
                cin >> buffer;

                send(sockfd, n, buffer);
                n = read(sockfd, spravne, 1);
                if (strcmp(spravne,"1") == 0)
                {
                    spravne[0] = '0';
                    cout << "Zadaj heslo:";
                    bzero(buffer, 256);
                    cin >> buffer;

                    string you = buffer;

                    for(int i = 0; i < you.size(); i++)
                    {
                        you[i] = you[i] + SIFRA;
                    }

                    strcpy(buffer,you.c_str());
                    send(sockfd,n,buffer);
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
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
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
                    if (a == '\0') {
                        size_t pos = fajnl.find("0");
                        if (pos == std::string::npos)
                        {
                            cout << fajnl << endl;
                        }
                        break;
                    }

                    if (a == ',')
                    {
                        size_t pos = fajnl.find("0");
                        if (pos == std::string::npos)
                        {
                            cout << fajnl << endl;
                        }

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
                send(sockfd, n, buffer);
                bzero(buffer,256);
                cout << "Zadajte spravu:";
                //fgets(buffer,256,stdin);
                std::getline(std::cin,f);
                for(int i = 0; i < f.size(); i++)
                {
                    f[i] = f[i] + SIFRA;
                }
                strcpy(buffer,f.c_str());

                send(sockfd, n, buffer);

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
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }


        }
        else if(volba == 4) // Pridanie kontaktu
        {
            char spravne[10];
            write(sockfd,buffer,2);
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
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
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
                    if (a == '\0') {
                        size_t pos = fajnl.find("0");
                        if (pos == std::string::npos)
                        {
                            cout << fajnl << endl;
                        }
                        break;
                    }

                    if (a == ',')
                    {
                        size_t pos = fajnl.find("0");
                        if (pos == std::string::npos)
                        {
                            cout << fajnl << endl;
                        }

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
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }

        }
        else if(volba == 6)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
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
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }

        }
        else if(volba == 7)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
                int pocet_pokusov = 3;
                char spravne[10];
                for(int i = 0; i < pocet_pokusov; i++)
                {
                    cout << "Zadaj meno skupiny:";
                    bzero(buffer, 256);
                    cin >> buffer;

                    send(sockfd, n, buffer);
                    n = read(sockfd, spravne, 1);
                    if (strcmp(spravne,"1") == 0)
                    {
                        spravne[0] = '0';
                        cout << "Zadaj heslo:";
                        bzero(buffer, 256);
                        cin >> buffer;

                        string you = buffer;

                        for(int i = 0; i < you.size(); i++)
                        {
                            you[i] = you[i] + SIFRA;
                        }

                        strcpy(buffer,you.c_str());
                        send(sockfd,n,buffer);
                        cout << "Skupina bola uspesne zaregistrovany!\n";
                        break;
                    }
                    else
                    {
                        cout << "Skupina s tymto menom uz existuje!\n";
                    }
                }
            }
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }

        }
        else if(volba == 8)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
            }
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }
        }
        else if(volba == 9)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
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
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }
        }
        else if(volba == 10)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
                prihlaseny = false;
                cout << "Boli ste uspesne odhlaseny";
            }
            else
            {
                cout << "Nie ste prihlaseny!\n";
            }
        }

        else if(volba == 11)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
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
                    if (a == '\0') {
                        size_t pos = fajnl.find("0");
                        if (pos == std::string::npos)
                        {
                            cout << fajnl << endl;
                        }
                        break;
                    }

                    if (a == ',')
                    {
                        size_t pos = fajnl.find("0");
                        if (pos == std::string::npos)
                        {
                            cout << fajnl << endl;
                        }

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
                        //cout << buffer;
                        size_t pee = jejeje.find(":");
                        if(pee != std::string::npos)
                        {
                            int i = pee + 2;
                            for(; i < jejeje.size(); i++)
                            {
                                jejeje[i] -= SIFRA;
                            }
                            jejeje[i-1] = '\n';
                        }
                        cout << jejeje;
                    }
                }
                else
                    cout << "Nepodarilo sa najst chat s danym kontaktom!\n";
            }
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }
        }

        else if(volba == 12)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
                char spravne[2];
                char buf[1024];
                bzero(buf,1024);
                read(sockfd,buf,1023);
                string f = buf;
                string fajnl;
                char a;
                int i = 0;
                cout << "Skupiny:\n";
                while (true)
                {
                    a = f[i];
                    if (a == '\0') {
                        size_t pos = fajnl.find("0");
                        if (pos != std::string::npos)
                        {
                            fajnl.erase(0, 1);
                            cout << fajnl << endl;
                        }
                        break;
                    }

                    if (a == ',')
                    {
                        size_t pos = fajnl.find("0");
                        if (pos != std::string::npos)
                        {
                            fajnl.erase(0, 1);
                            cout << fajnl << endl;
                        }

                        fajnl = "";
                        i++;
                        continue;
                    }

                    fajnl += a;
                    i++;
                }
                bzero(buffer, 256);
                cout << "Ktorej skupine chcete poslat spravu:";
                cin >> buffer;
                int n;
                char zzzz;
                zzzz = getchar();
                send(sockfd, n, buffer);
                bzero(buffer,256);
                cout << "Zadajte spravu:";

                std::getline(std::cin,f);
                for(int i = 0; i < f.size(); i++)
                {
                    f[i] = f[i] + SIFRA;
                }
                strcpy(buffer,f.c_str());

                send(sockfd, n, buffer);
                bzero(spravne,2);
                n = read(sockfd, spravne, 1);
                cout << "Sprava bola uspesne poslana\n";
            }
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }
        }

        else if(volba == 13)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
                char spravne[2];
                char buf[1024];
                bzero(buf,1024);
                read(sockfd,buf,1023);
                string f = buf;
                string fajnl;
                char a;
                int i = 0;
                cout << "Skupiny:\n";
                while (true)
                {
                    a = f[i];
                    if (a == '\0') {
                        size_t pos = fajnl.find("0");
                        if (pos != std::string::npos)
                        {
                            fajnl.erase(0, 1);
                            cout << fajnl << endl;
                        }
                        break;
                    }

                    if (a == ',')
                    {
                        size_t pos = fajnl.find("0");
                        if (pos != std::string::npos)
                        {
                            fajnl.erase(0, 1);
                            cout << fajnl << endl;
                        }

                        fajnl = "";
                        i++;
                        continue;
                    }

                    fajnl += a;
                    i++;
                }
                cout << "Zadajte meno noveho clena:";
                bzero(buffer,256);
                cin >> buffer;
                write(sockfd,buffer,255);

                cout << "Zadajte do ktorej skupiny sa ma pridat:";
                bzero(buffer,256);
                cin >> buffer;
                write(sockfd,buffer,255);

                bzero(spravne,2);
                read(sockfd,spravne,1);

                cout  << "Novy clen bol uspesne pridany!\n";
            }
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }
        }

        else if(volba == 14)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
                char spravne[2];
                char buf[1024];
                bzero(buf,1024);
                read(sockfd,buf,1023);
                string f = buf;
                string fajnl;
                char a;
                int i = 0;
                cout << "Skupiny:\n";
                while (true)
                {
                    a = f[i];
                    if (a == '\0') {
                        size_t pos = fajnl.find("0");
                        if (pos != std::string::npos)
                        {
                            fajnl.erase(0, 1);
                            cout << fajnl << endl;
                        }
                        break;
                    }

                    if (a == ',')
                    {
                        size_t pos = fajnl.find("0");
                        if (pos != std::string::npos)
                        {
                            fajnl.erase(0, 1);
                            cout << fajnl << endl;
                        }

                        fajnl = "";
                        i++;
                        continue;
                    }

                    fajnl += a;
                    i++;
                }
                bzero(buffer, 256);
                cout << "Zadajte s ktorou skupinou chcete vidiet chat:";
                cin >> buffer;
                write(sockfd,buffer,255);
                bzero(spravne,2);
                read(sockfd,spravne,1);
                string jejeje;
                if (strcmp(spravne,"1") == 0)
                {
                    cout << "Skupina sa nasla, nacitavam spravy...\n";
                    while(true)
                    {
                        bzero(buffer,256);
                        read(sockfd,buffer,255);
                        jejeje = buffer;
                        if(jejeje == "end")
                            break;
                        //cout << buffer;
                        size_t pee = jejeje.find(":");
                        int i = pee + 2;
                        for(; i < jejeje.size(); i++)
                        {
                            jejeje[i] -= SIFRA;
                        }
                        jejeje[i-1] = '\n';
                        cout << jejeje;
                    }
                }
                else
                    cout << "Nepodarilo sa najst chat s danou skupinou!\n";
            }
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }
        }
        else if(volba == 15)
        {
            if(prihlaseny)
            {
                write(sockfd,buffer,2);
                string jejeje;
                cout << "Nacitavam notifikacie..\n";
                while(true)
                {
                    bzero(buffer,256);
                    read(sockfd,buffer,255);
                    jejeje = buffer;
                    if(jejeje == "end")
                        break;
                    cout << jejeje;
                }
            }
            else
            {
                cout << "Musite sa najskor prihlasit!\n";
            }
        }

        else
        {
            break;
        }
    }

    close(sockfd);
    return 0;
}

void send(int sockfd, int n, char buffer[])
{
    n = write(sockfd, buffer, strlen(buffer));
    if (n = 0)
    {
        printf("ERROR writing to socket");
    }
}
