#ifndef CHATAPP_SERVER_H
#define CHATAPP_SERVER_H

void* accept_connection(void *p);
void notifikacie(int soket,char buffer[], std::string* login);
void zobraz_skupinove_spravy(int soket,char buffer[], std::string* login);
void pridaj_do_skupiny(int soket,char buffer[], std::string* login);
void zobraz_spravy(int soket,char buffer[], std::string* login);
void vymaz_kontakt(int soket,char buffer[], std::string* login);
void potvrd_ziadosti(int soket, std::string* login);
void posli_spravu_skupine(int soket,char buffer[], std::string* login);
void posli_spravu(int soket,char buffer[], std::string* login);
void zobraz_kontakty(int soket, std::string* login);
void pridaj_do_nepotvrd(int soket,char buffer[], std::string* login);
void pridaj_kontakt(int soket,char buffer[], std::string* login, std::string meno);
void check_register(int soket,char buffer[], std::string* login, int volba = 0);
bool check_login(int soket,char buffer[], std::string* login);

#endif //CHATAPP_SERVER_H
