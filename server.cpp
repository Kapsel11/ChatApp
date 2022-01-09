#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <cstring>
#include <limits.h>
#include "server.h"

using namespace std;

pthread_mutex_t *mutex;
string path = "/tmp/tmp.A711GNdBTGg/";

int main(int argc, char *argv[])
{
    int newsockfd;
    char cwd[PATH_MAX];
    if(getcwd(cwd, sizeof(cwd)))
    {
        path = cwd;
        string cmake = "cmake-build-debug";

        size_t pee = path.find(cmake);
        if(pee != string::npos)
        {
            path.erase(pee, cmake.length());
        }
    }

    signal(SIGPIPE, SIG_IGN);
    pthread_mutex_t moutex;
    pthread_mutex_init(&moutex,NULL);
    mutex = &moutex;
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

    while(true)
    {
        newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
        pthread_t test;
        pthread_create(&test,NULL,&accept_connection,&newsockfd);
    }
    pthread_mutex_destroy(mutex);
    close(sockfd);
    return 0;
}

bool check_login(int soket,char buffer[], string* login)
{
    int pocet_pok = 3;
    for(int i = 0; i < pocet_pok; i++)
    {
        bzero(buffer, 256);
        int f = read(soket,buffer,255);
        if (f < 0)
        {
            printf("Cant read from socket");
        }
        ifstream ifile;
        string b = buffer;
        b = b + ".txt";
        pthread_mutex_lock(mutex);
        ifile.open(path + b);
        string pas;
        if(ifile)
        {
            write(soket, "1", 1);
            bzero(buffer,256);
            f = read(soket,buffer,255);
            int pom = atoi(buffer);
            ifile >> pas;
            ifile.close();
            if(pas == buffer)
            {
                write(soket, "1", 1);
                *login = b;
                pthread_mutex_unlock(mutex);
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
        pthread_mutex_unlock(mutex);
    }

    return false;
}

void check_register(int soket,char buffer[], string* login, int volba)
{
    int pocet_pok = 3;
    string logdan;
    if(volba != 1)
    {
        for(int i = 0; i < pocet_pok; i++)
        {
            bzero(buffer, 256);
            int f = read(soket,buffer,255);
            if (f < 0)
            {
                printf("Cant read from socket");
            }
            ifstream ifile;
            logdan = buffer;

            pthread_mutex_lock(mutex);

            ifile.open(logdan +".txt");
            if(ifile) {
                write(soket, "0", 1);
                ifile.close();
                pthread_mutex_unlock(mutex);
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
                ofstream fajl(path + logdan + ".txt");
                fajl << buffer << endl;
                fajl << "default," << endl;
                fajl << "default," << endl;
                fajl.close();
                pthread_mutex_unlock(mutex);
                break;
            }
        }
    }
    else
    {
        for(int i = 0; i < pocet_pok; i++)
        {
            bzero(buffer, 256);
            int f = read(soket,buffer,255);
            if (f < 0)
            {
                printf("Cant read from socket");
            }
            ifstream ifile;
            logdan = buffer;
            logdan = "0" + logdan;
            pthread_mutex_lock(mutex);

            ifile.open(path + logdan +".txt");
            if(ifile) {
                write(soket, "0", 1);
                ifile.close();
                pthread_mutex_unlock(mutex);
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
                ofstream fajl(path + logdan + ".txt");
                fajl << buffer << endl;
                fajl << "default," << endl;
                fajl << "default," << endl;
                fajl.close();
                pthread_mutex_unlock(mutex);
                break;
            }
        }
        pridaj_kontakt(soket, buffer, login, logdan);
    }
}

void pridaj_kontakt(int soket,char buffer[], string* login, string meno)
{

    ifstream ifile;
    string pom = meno;
    pthread_mutex_lock(mutex);
    ifile.open(path + pom +".txt");

    if(ifile) { // Podarilo sa nam otvorit subor -> kontakt existuje

        ifile.close();
        ifstream filein(path + *login); //File to read from
        ofstream fileout(path + *login + "1"); //Temporary file

        if (!filein || !fileout)
        {
           // remove(path + *login + "1" + ".txt");
            pthread_mutex_unlock(mutex);
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

        remove(buffer);

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
                    pthread_mutex_unlock(mutex);
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

                remove(buffer);

                int result= rename( buf , buffer );

                if ( result == 0 )
                    write(soket, "1", 1);
                else
                    write(soket, "0", 1);
                }
        }
        else
            write(soket, "0", 1);
        pthread_mutex_unlock(mutex);
    }
    else
    {
        pthread_mutex_unlock(mutex);
        write(soket, "0", 1);
    }

}

void pridaj_do_nepotvrd(int soket,char buffer[], string* login)
{
    bzero(buffer, 256);
    read(soket,buffer,255);

    ifstream ifile;
    string pom = buffer;
    pthread_mutex_lock(mutex);
    ifile.open(path + pom +".txt");

    if(ifile) { // Podarilo sa nam otvorit subor -> kontakt existuje

        //ifile.close();
        ofstream fileout(path + pom +".txt" + "1"); //Temporary file

        if (!fileout)
        {
            // remove(path + *login + "1" + ".txt");
            pthread_mutex_unlock(mutex);
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

        remove(buffer);

        int result= rename( buf , buffer );

        pthread_mutex_unlock(mutex);

        if ( result == 0 )
            write(soket, "1", 1);
        else
            write(soket, "0", 1);
    }
    else
    {
        pthread_mutex_unlock(mutex);
        write(soket, "0", 1);
    }

}


void zobraz_kontakty(int soket, string* login)
{
    char buffer[1024];
    pthread_mutex_lock(mutex);
    ifstream filein(path + *login);

    string strTemp;
    string f;
    std::getline(filein, f);
    std::getline(filein, f);
    filein.close();
    pthread_mutex_unlock(mutex);

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
    bzero(buffer,256);
    read(soket,buffer,255);
    string meno = buffer;

    bzero(buffer,256);
    read(soket,buffer,255);
    string sprava = buffer;

    string f;
    pthread_mutex_lock(mutex);
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

        pthread_mutex_unlock(mutex);

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
        pthread_mutex_unlock(mutex);
    }
    else
    {
        pthread_mutex_unlock(mutex);
        write(soket,"0",1);
    }
}

void posli_spravu_skupine(int soket,char buffer[], string* login)
{
    bzero(buffer,256);
    read(soket,buffer,255);
    string meno = buffer;
    meno = "0" + meno;

    bzero(buffer,256);
    read(soket,buffer,255);
    string sprava = buffer;

    string jee = *login;
    size_t pee = jee.find(".txt");
    jee.erase(pee, 4);

    sprava = jee + ": " + sprava + "\n";

    string f;
    pthread_mutex_lock(mutex);
    ifstream filein(path + *login);
    std::getline(filein, f);
    std::getline(filein, f);

    filein.close();

     pee = f.find(meno);
    if(pee != string::npos)
    {
        ofstream subor_skupiny(path + meno + ".txt", ios::app);
        if(subor_skupiny)
        {
            subor_skupiny << sprava;
            subor_skupiny.close();
            pthread_mutex_unlock(mutex);
            write(soket,"1",1);
        }
        else
        {
            pthread_mutex_unlock(mutex);
            write(soket,"0",1);
        }
    }
    else
    {
        pthread_mutex_unlock(mutex);
        write(soket,"0",1);
    }
}

void potvrd_ziadosti(int soket, string* login)
{
    char buffer[1024];
    pthread_mutex_lock(mutex);
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

    pthread_mutex_unlock(mutex);

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

        string jee = *login;
        size_t pee = jee.find(".txt");
        jee.erase(pee, 4);
        jee.append(",");

        ofstream fajv(path + meno + ".txt",ios::app);
        string sprava = "Pouzivatel " + jee + " prijal vasu ziadost!\n";
        if(fajv)
        {
            fajv << sprava;
            fajv.close();
        }
    }
    else
    {
        write(soket,"0",1);
    }
}

void vymaz_kontakt(int soket,char buffer[], string* login)
{
    bzero(buffer,256);
    read(soket,buffer,255);
    string meno_kont = buffer;
    string meno_bez = buffer;
    meno_kont.append(",");

    pthread_mutex_lock(mutex);

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
            pthread_mutex_unlock(mutex);
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
            fyle_out.close();
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

            ofstream fajv(buffer,ios::app);
            string sprava = "Pouzivatel " + jee + " si vas odstranil z kontaktov!\n";
            if(fajv)
            {
                fajv << sprava;
                fajv.close();
            }
            if ( result == 0 )
                write(soket, "1", 1);
            else
                write(soket, "0", 1);
        }
        else
        {
            pthread_mutex_unlock(mutex);
            write(soket, "0", 1);
            return;
        }
        pthread_mutex_unlock(mutex);
    }
    else
    {
        pthread_mutex_unlock(mutex);
        write(soket, "0", 1);
    }
    pthread_mutex_unlock(mutex);
}

void zobraz_spravy(int soket,char buffer[], string* login)
{
    bzero(buffer,256);
    read(soket,buffer,255); // Pride meno kontaktu

    string meno_kontaktu = buffer;
    //meno_kontaktu += ":";

    string jee = *login;
    size_t pee = jee.find(".txt");
    jee.erase(pee, 4);
    //jee += ":";

    pthread_mutex_lock(mutex);

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
        pthread_mutex_unlock(mutex);
        bzero(buffer,256);
        write(soket,"end",3);

    }
    else
    {
        pthread_mutex_unlock(mutex);
      write(soket,"0",1);
    }

}

void pridaj_do_skupiny(int soket,char buffer[], string* login)
{
    bzero(buffer,256);
    read(soket,buffer,255);
    string meno_kontaktu = buffer;

    bzero(buffer,256);
    read(soket,buffer,255);
    string meno_skupiny = buffer;
    meno_skupiny = "0" + meno_skupiny;

    pthread_mutex_lock(mutex);
    ifstream subor_skupiny;
    subor_skupiny.open(path + meno_skupiny + ".txt");
    string f;
    string login_bez = *login;
    size_t pee = login_bez.find(".txt");
    login_bez.erase(pee, 4);

    if(subor_skupiny)
    {
        getline(subor_skupiny,f);
        getline(subor_skupiny,f);
        subor_skupiny.close();
        pee = f.find(login_bez);
        if(pee != string::npos)
        {
            pthread_mutex_unlock(mutex);
            string helping = meno_skupiny + ".txt";
            pridaj_kontakt(soket,buffer,&helping,meno_kontaktu);
        }
        else
        {
            pthread_mutex_unlock(mutex);
            write(soket,"0",1);
        }
    }
    else
    {
        pthread_mutex_unlock(mutex);
        write(soket,"0",1);
    }

}

void zobraz_skupinove_spravy(int soket,char buffer[], string* login)
{
    bzero(buffer,256);
    read(soket,buffer,255); // Pride meno skupiny

    string meno_kontaktu = buffer;
    meno_kontaktu = "0" + meno_kontaktu;
    //meno_kontaktu += ":";

    string jee = *login;
    size_t pee = jee.find(".txt");
    jee.erase(pee, 4);
    //jee += ":";

    pthread_mutex_lock(mutex);

    ifstream ifile(path + meno_kontaktu + ".txt");
    if(ifile)
    {
        write(soket,"1",1);

        string strPemp;

        getline(ifile,strPemp);
        getline(ifile,strPemp);

        pee = strPemp.find(jee);

        if(pee != string::npos)
        {
            getline(ifile,strPemp);

            while (getline(ifile,strPemp))
            {
                strPemp += "\n";
                bzero(buffer,256);
                strcpy(buffer,strPemp.c_str());
                write(soket,buffer,255);
            }
            ifile.close();
            pthread_mutex_unlock(mutex);
            bzero(buffer,256);
            write(soket,"end",3);
        }
        else
        {
            pthread_mutex_unlock(mutex);
            write(soket,"end",3);
            return;
        }
    }
    else
    {
        pthread_mutex_unlock(mutex);
        write(soket,"0",1);
    }
}

void notifikacie(int soket,char buffer[], string* login)
{

    string jee = *login;
    size_t pee = jee.find(".txt");
    jee.erase(pee, 4);

    pthread_mutex_lock(mutex);

    ifstream ifile(path + *login);
    if(ifile)
    {

        string strPemp;

        getline(ifile,strPemp);
        getline(ifile,strPemp);
        getline(ifile,strPemp);

        size_t p1;
        size_t p2;

        while (getline(ifile,strPemp))
        {
            p1 = strPemp.find("Pouzivatel ");
            strPemp += "\n";

            if((p1 != string::npos) && (p1 < 25 ))
            {
                bzero(buffer,256);
                strcpy(buffer,strPemp.c_str());
                write(soket,buffer,255);
            }
        }
        ifile.close();
        pthread_mutex_unlock(mutex);
        bzero(buffer,256);
        write(soket,"end",3);

    }
    else
    {
        pthread_mutex_unlock(mutex);
    }
}

void* accept_connection(void *p) {

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
            check_register(newsockfd,buffer,&login);
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
            check_register(newsockfd,buffer,&login,1);

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
                {
                    write(newsockfd, "0", 1);
                    // odstran z kontaktov
                }

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
            zobraz_kontakty(newsockfd,&login);
            posli_spravu_skupine(newsockfd,buffer,&login);
        }
        else if(volba == 13)
        {
            zobraz_kontakty(newsockfd,&login);
            pridaj_do_skupiny(newsockfd,buffer,&login);
        }
        else if(volba == 14)
        {
            zobraz_kontakty(newsockfd,&login);
            zobraz_skupinove_spravy(newsockfd,buffer,&login);
        }
        else if(volba == 15)
        {
            notifikacie(newsockfd,buffer,&login);;
        }

        else
        {
            break;
        }
    }
    close(newsockfd);
}



