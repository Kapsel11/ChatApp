#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <pthread.h>

using namespace std;

typedef struct data {
    int connections_count;
    vector<int> connections_fd;
}D;


bool check_login(int soket,char buffer[], string* login)
{
    cout << "Check login";
    string path = "/tmp/tmp.A711GNdBTGg/";
    int pocet_pok = 3;
    for(int i = 0; i < pocet_pok; i++)
    {
        cout << "Check-login\n";
        bzero(buffer, 256);
        int f = read(soket,buffer,255);
        if (f < 0)
        {
            printf("Cant read from socket");

        }
        ifstream ifile;
        string b = buffer;
        b = b + ".txt";
        ifile.open(path + b);
        string pas;
        if(ifile)
        {
            write(soket, "1", 1);
            bzero(buffer,256);
            f = read(soket,buffer,255);
            int pom = atoi(buffer);
            ifile >> pas;

            if(pas == buffer)
            {
                write(soket, "1", 1);
                *login = b;
                return true;
            }
            else
            {
                write(soket, "0", 1);
            }
        }
        else
        {
            write(soket, "0", 1);
        }
    }

    return false;
}

void check_register(int soket,char buffer[])
{
    string path = "/tmp/tmp.A711GNdBTGg/";
    int pocet_pok = 3;
    for(int i = 0; i < pocet_pok; i++)
    {
        bzero(buffer, 256);
        int f = read(soket,buffer,255);
        if (f < 0)
        {
            printf("Cant read from socket");
        }

        // Mutex?

        ifstream ifile;
        string login = buffer;
        ifile.open(login +".txt");
        if(ifile) {
            write(soket, "0", 1);
            ifile.close();
        }
        else
        {
            write(soket, "1", 1);
            bzero(buffer,256);
            int f = read(soket,buffer,255);
            if (f < 0)
            {
                printf("Cant read from socket");
            }
            ofstream fajl(path + login + ".txt");
            fajl << buffer << endl;
            fajl << "default," << endl;
            fajl << "default," << endl;
            fajl.close();
            break;
        }
    }
}

void pridaj_kontakt(int soket,char buffer[], string* login, string meno)
{
    //bzero(buffer, 256);
    //read(soket,buffer,255);
    string path = "/tmp/tmp.A711GNdBTGg/";

    ifstream ifile;
    string pom = meno;
    ifile.open(path + pom +".txt");

    if(ifile) { // Podarilo sa nam otvorit subor -> kontakt existuje

        ifile.close();
        ifstream filein(path + *login); //File to read from
        ofstream fileout(path + *login + "1"); //Temporary file

        if (!filein || !fileout)
        {
           // remove(path + *login + "1" + ".txt");
            write(soket, "0", 1);
            return;
        }
        string kontakt = pom;
        string pes = kontakt;
        kontakt.append(",");
        string strTemp;

        int i = 0;
        while (getline(filein,strTemp))
        {
            if (i == 1) {
                strTemp.append(kontakt);
            }
            if (i == 2)
            {
                size_t pee = strTemp.find(kontakt);
                if(pee != string::npos)
                {
                    strTemp.erase(pee, kontakt.length());
                }
            }
            strTemp += "\n";
            fileout << strTemp;
            i++;
        }
        if (i == 1)
            fileout << kontakt;

        fileout.close();
        filein.close();

        bzero(buffer,256);

        string a = path + *login;

        strcpy(buffer, a.c_str());
        char buf[256];
        bzero(buf,256);
        string h = path + *login + "1";
        strcpy(buf,h.c_str());

        if(remove(buffer) != 0)
        {
            cout << "JEE";
        }
        else
        {
            cout << "NEE";
        }
        int result= rename( buf , buffer );

        if ( result == 0 )
        {
            ifstream mm;
            mm.open(path + pes +".txt");

            if(mm) { // Podarilo sa nam otvorit subor -> kontakt existuje

                //ifile.close();
                ofstream fileout(path + pes + ".txt" + "1"); //Temporary file

                if (!fileout)
                {
                    // remove(path + *login + "1" + ".txt");
                    write(soket, "0", 1);
                    return;
                }
                string jee = *login;
                size_t pee = jee.find(".txt");
                jee.erase(pee, 4);

                string soros = jee;
                jee.append(",");
                string strTemp;

                int i = 0;
                while (getline(mm,strTemp))
                {
                    if (i == 1) {
                        strTemp.append(jee);
                    }
                    strTemp += "\n";
                    fileout << strTemp;
                    i++;
                }
                if (i == 1)
                    fileout << jee;

                mm.close();
                fileout.close();

                bzero(buffer,256);

                string a = path + pes + ".txt";

                strcpy(buffer, a.c_str());
                char buf[256];
                bzero(buf,256);
                string h = path + pes + ".txt" + "1";
                strcpy(buf,h.c_str());

                if(remove(buffer) != 0)
                {
                    cout << "JEE";
                }
                else
                {
                    cout << "NEE";
                }
                int result= rename( buf , buffer );

                if ( result == 0 )
                    write(soket, "1", 1);
                else
                    write(soket, "0", 1);
                }
        }
        else
            write(soket, "0", 1);
    }
    else
    {
        write(soket, "0", 1);
    }

}

void pridaj_do_nepotvrd(int soket,char buffer[], string* login)
{
    bzero(buffer, 256);
    read(soket,buffer,255);
    string path = "/tmp/tmp.A711GNdBTGg/";

    ifstream ifile;
    string pom = buffer;
    ifile.open(path + pom +".txt");

    if(ifile) { // Podarilo sa nam otvorit subor -> kontakt existuje

        //ifile.close();
        ofstream fileout(path + pom +".txt" + "1"); //Temporary file

        if (!fileout)
        {
            // remove(path + *login + "1" + ".txt");
            write(soket, "0", 1);
            return;
        }
        string jee = *login;
        size_t pee = jee.find(".txt");
        jee.erase(pee, 4);

        string kontakt = jee;
        kontakt.append(",");
        string strTemp;

        int i = -1;
        while (getline(ifile,strTemp))
        {
            if (i == 1) {
                strTemp.append(kontakt);
            }
            strTemp += "\n";
            fileout << strTemp;
            i++;
        }

        ifile.close();
        fileout.close();

        bzero(buffer,256);

        string a = path + pom + ".txt";

        strcpy(buffer, a.c_str());
        char buf[256];
        bzero(buf,256);
        string h = path + pom + ".txt" + "1";
        strcpy(buf,h.c_str());

        if(remove(buffer) != 0)
        {
            cout << "JEE";
        }
        else
        {
            cout << "NEE";
        }
        int result= rename( buf , buffer );

        if ( result == 0 )
            write(soket, "1", 1);
        else
            write(soket, "0", 1);
    }
    else
    {
        write(soket, "0", 1);
    }

}


void zobraz_kontakty(int soket, string* login)
{
    string path = "/tmp/tmp.A711GNdBTGg/";
    char buffer[1024];
    ifstream filein(path + *login);

    string strTemp;
    string f;
    std::getline(filein, f);
    std::getline(filein, f);

    size_t pos = f.find("default,");
    if (pos != std::string::npos)
    {
        f.erase(pos, 8);
    }


    bzero(buffer, 1024);

    strcpy(buffer, f.c_str());
    write(soket,buffer,1023);
}

void posli_spravu(int soket,char buffer[], string* login)
{
    string path = "/tmp/tmp.A711GNdBTGg/";
    bzero(buffer,256);
    read(soket,buffer,255);
    string meno = buffer;

    bzero(buffer,256);
    read(soket,buffer,255);
    string sprava = buffer;

    string f;
    ifstream filein(path + *login);
    std::getline(filein, f);
    std::getline(filein, f);

    filein.close();

    size_t pee = f.find(meno);
    if(pee != string::npos)
    {
        string hiphap = *login;
        string koncovka = ".txt";

        size_t pos = hiphap.find(koncovka);
        if (pos != std::string::npos)
        {
            // If found then erase it from string
            hiphap.erase(pos, koncovka.length());
        }

        //sprava = "\n" + hiphap + ": " + sprava;
        sprava = meno + " " + hiphap + ": " + sprava + "\n";
        ofstream bread(path + *login, ios::app);
        bread << sprava;
        bread.close();

        string f = path + meno + ".txt";

        ifstream ifile;
        ifile.open(f);
        if(ifile)
        {
            ifile.close();
            ofstream fajl(f, ios::app);
            fajl << sprava;
            fajl.close();
            write(soket,"1",1);
        }
        else
        {
            write(soket,"0",1);
        }
    }
    else
    {
        write(soket,"0",1);
    }
}

void potvrd_ziadosti(int soket, string* login)
{
    string path = "/tmp/tmp.A711GNdBTGg/";
    char buffer[1024];
    ifstream filein(path + *login);

    string strTemp;
    string f;
    std::getline(filein, f);
    std::getline(filein, f);
    std::getline(filein, f);

    size_t pos = f.find("default,");
    if (pos != std::string::npos)
    {
        f.erase(pos, 8);
    }

    filein.close();

    bzero(buffer, 1024);

    strcpy(buffer, f.c_str());
    write(soket,buffer,1023);
    bzero(buffer,1024);
    read(soket,buffer,1023);
    string meno = buffer;

    size_t pee = f.find(meno);
    if(pee != string::npos)
    {
        pridaj_kontakt(soket,buffer,login,meno);
    }
    else
    {
        write(soket,"0",1);
    }
}

void vymaz_kontakt(int soket,char buffer[], string* login)
{
    string path = "/tmp/tmp.A711GNdBTGg/";
    bzero(buffer,256);
    read(soket,buffer,255);
    string meno_kont = buffer;
    string meno_bez = buffer;
    meno_kont.append(",");

    ifstream filein(path + *login);

    string f;
    std::getline(filein, f);
    std::getline(filein, f);

    filein.close();

    size_t pos = f.find(meno_kont);
    if (pos != std::string::npos)
    {
        f.erase(pos, meno_kont.size());

        ifstream file_input(path + *login);
        ofstream fileout(path + *login + "1"); //Temporary file
        if (!filein || !fileout)
        {
            // remove(path + *login + "1" + ".txt");
            write(soket, "0", 1);
            return;
        }
        string strTemp;

        int i = 0;
        while (getline(file_input,strTemp))
        {
            if (i == 1) {
                strTemp = f;
            }
            strTemp += "\n";
            cout << strTemp;
            fileout << strTemp;
            i++;
        }

        fileout.close();
        file_input.close();

        bzero(buffer,256);

        string a = path + *login;

        strcpy(buffer, a.c_str());
        char buf[256];
        bzero(buf,256);
        string h = path + *login + "1";
        strcpy(buf,h.c_str());

        remove(buffer);

        rename( buf , buffer );

        ifstream fyle(path + meno_bez + ".txt");
        if(fyle)
        {
            ofstream fyle_out(path + meno_bez + ".txt" + "1");
            string f;
            std::getline(fyle, f);
            std::getline(fyle, f);

            fyle.close();

            string jee = *login;
            size_t pee = jee.find(".txt");
            jee.erase(pee, 4);
            jee.append(",");

            size_t pos = f.find(jee);
            if (pos != std::string::npos)
            {
                f.erase(pos,jee.length());
            }

            ifstream losos(path + meno_bez + ".txt");

            string strPemp;

            int i = 0;
            while (getline(losos,strPemp))
            {
                if (i == 1) {
                    strPemp = f;
                }
                strPemp += "\n";
                fyle_out << strPemp;
                i++;
            }

            losos.close();

            bzero(buffer,256);

            string a = path + meno_bez + ".txt";

            strcpy(buffer, a.c_str());
            char buf[256];
            bzero(buf,256);
            string h = path + meno_bez + ".txt" + "1";
            strcpy(buf,h.c_str());

            remove(buffer);

            int result= rename( buf , buffer );

            if ( result == 0 )
                write(soket, "1", 1);
            else
                write(soket, "0", 1);

        }
        else
        {
            write(soket, "0", 1);
            return;
        }

    }
    else
    {
        write(soket, "0", 1);
    }
}

void zobraz_spravy(int soket,char buffer[], string* login)
{
    string path = "/tmp/tmp.A711GNdBTGg/";
    bzero(buffer,256);
    read(soket,buffer,255); // Pride meno kontaktu

    string meno_kontaktu = buffer;
    //meno_kontaktu += ":";

    string jee = *login;
    size_t pee = jee.find(".txt");
    jee.erase(pee, 4);
    //jee += ":";


    ifstream ifile(path + *login);
    if(ifile)
    {
        write(soket,"1",1);

        string strPemp;

        getline(ifile,strPemp);
        getline(ifile,strPemp);
        getline(ifile,strPemp);

        size_t p1;
        size_t p2;

        while (getline(ifile,strPemp))
        {
            p1 = strPemp.find(meno_kontaktu);
            p2 = strPemp.find(jee);
            strPemp += "\n";

            if((p1 != string::npos && p2 != string::npos) && (p1 < 25 && p2 < 25))
            {
                int i = 0;
                while(strPemp[i] != ' ')
                {
                    i++;
                }
                strPemp.erase(0,i+1);
                bzero(buffer,256);
                strcpy(buffer,strPemp.c_str());
                write(soket,buffer,255);
            }
        }
        ifile.close();
        bzero(buffer,256);
        write(soket,"end",3);

    }
    else
    {
      write(soket,"0",1);
    }

}
void* accept_connection(void *p) {
    string path = "/tmp/tmp.A711GNdBTGg/";
    bool uspesne_prih;
    string login;
    cout << "accept-connection\n";
    int newsockfd;
    newsockfd = * ((int *) p);
    char buffer[256];

    while(true)
    {
        bzero(buffer,256);
        read(newsockfd,buffer,255);
        int volba = atoi(buffer);
        if(volba == 1)
        {
            uspesne_prih = check_login(newsockfd,buffer,&login);
        }

        else if(volba == 2)
        {
            check_register(newsockfd,buffer);
        }

        else if(volba == 3)
        {
            zobraz_kontakty(newsockfd,&login);
            posli_spravu(newsockfd,buffer, &login);
        }
        else if(volba == 4)
        {
            if(uspesne_prih)
            {
                write(newsockfd, "1", 1);
                //pridaj_kontakt(newsockfd,buffer, &login);
                pridaj_do_nepotvrd(newsockfd,buffer, &login);
            }
            else
                write(newsockfd, "0", 1);
        }
        else if(volba == 5)
        {
            zobraz_kontakty(newsockfd,&login);
            vymaz_kontakt(newsockfd,buffer, &login);
        }
        else if(volba == 6)
        {
            potvrd_ziadosti(newsockfd,&login);
        }
        else if(volba == 7)
        {

        }
        else if(volba == 8)
        {

        }
        else if(volba == 9)
        {
            if(uspesne_prih)
            {
                int n = path.length() + login.length();
                string strom =  path + login;
                char char_array[n + 1];
                strcpy(char_array, strom.c_str());

                if( remove(char_array) != 0 )
                    write(newsockfd, "0", 1);
                else
                {
                    write(newsockfd, "1", 1);
                    uspesne_prih = false;
                }
            }
            else
                write(newsockfd, "2", 1);
        }
        else if(volba == 10)
        {
            uspesne_prih = false;
        }
        else if(volba == 11)
        {
            zobraz_kontakty(newsockfd,&login);
            zobraz_spravy(newsockfd,buffer,&login);
        }
        else if(volba == 12)
        {

        }
        else
        {
            break;
        }
    }
    cout << close(newsockfd);
}

int main(int argc, char *argv[])
{
    int sockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, 5);

    cli_len = sizeof(cli_addr);
    int newsockfd;

    while(true)
    {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
        pthread_t test;
        pthread_create(&test,NULL,&accept_connection,&newsockfd);
        cout << "Padam";
    }
    cout << "Letim";

}

