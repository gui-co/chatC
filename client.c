#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "sock.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <curses.h>
#include <string.h>
#include <stdio.h>
#include <curses.h>
#include <poll.h>

void ajoutPseudo(char * message, char * pseudo, int16_t * tailleMessage)
{
  char tmp[1000];
  char * ptr;
  int i;

  ptr = message;
  i=0;
  
  while ((tmp[i++] = *pseudo++));
  *tailleMessage += i--;
  tmp[i++] = '>';
  
  while ((tmp[i++] = *message++));
  
  i=0;
  while ((*ptr++ = tmp[i++]));
}

void fenetreOptions(char * ip, int * port, char * pseudo)
{
  WINDOW *options, *ipW, *portW, *pseudoW;
  options = newwin(5,34,(LINES-5)/2,(COLS-34)/2);
  box(options,ACS_VLINE,ACS_HLINE);
  ipW = derwin(options,1,15,1,17);
  portW = derwin(options,1,6,2,19);
  pseudoW = derwin(options,1,15,3,10);
  mvwprintw(options,1,1,"IP du serveur : ");
  mvwprintw(options,2,1,"Port du serveur : ");
  mvwprintw(options,3,1,"Pseudo : ");
  wrefresh(options);
  do
  {
    werase(ipW);
    wrefresh(ipW);
    mvwscanw(ipW,0,0,"%15s",ip);
  }
  while (!inet_aton(ip,NULL));

  do
  {
    werase(portW);
    wrefresh(portW);
    mvwscanw(portW,0,0,"%d",port);
  }
  while (*port < 0 || *port > 65535);

  mvwscanw(pseudoW,0,0,"%15s",pseudo);
  werase(options);
  wrefresh(options);
}


int main()
{
  /* connexion */
  int sockClient;
  struct sockaddr_in adresseServeur;
  struct sockaddr_in adresseClient;
  socklen_t tailleAdresse;
  char ip[20];
  int port;

  /* chat */
  char pseudo[20];
  struct pollfd fds[2];
  char messageEnvoi[500];
  char messageRecu[500];
  int16_t tMessEnvoi;
  int16_t tMessRecu;
  int nbMessagesRecus;
  char c;

  /* ncurses */
  WINDOW * chat;
  WINDOW * chatCadre;
  WINDOW * envoi;
  WINDOW * message;
  
  initscr();

  fenetreOptions(ip,&port,pseudo);

  curs_set(0);
  noecho();
  adresseServeur.sin_family = AF_INET;
  adresseServeur.sin_addr.s_addr = inet_addr(ip);
  adresseServeur.sin_port = htons(port);
  sockClient = creerSocket(&adresseClient,0);
  tailleAdresse = sizeof(adresseServeur);
  connect(sockClient,(struct sockaddr *)&adresseServeur,tailleAdresse);

  chatCadre = subwin(stdscr,LINES-3,COLS,0,0);
  box(chatCadre, ACS_VLINE, ACS_HLINE);
  chat = subwin(chatCadre,LINES-5,COLS-3,1,1);
  envoi = subwin(stdscr,3,COLS,LINES-3,0);
  box(envoi, ACS_VLINE, ACS_HLINE);
  message = derwin(envoi,1,COLS-2,1,1);
  scrollok(chat,1);
  idlok(chat,1);
  refresh();

  fds[0].fd = fileno(stdin);
  fds[0].events = POLLIN;
  fds[1].fd = sockClient;
  fds[1].events = POLLIN;
 
  tMessEnvoi = 0;
  nbMessagesRecus = 0;

  while(1)
  {
    if (poll(fds,2,-1) > 0)
    {
      if (fds[0].revents & POLLIN)
      {
        switch (c = getch())
        {
          case '\n':
            ajoutPseudo(messageEnvoi,pseudo,&tMessEnvoi);
            write(sockClient,&tMessEnvoi,sizeof(int16_t));
            write(sockClient,messageEnvoi,tMessEnvoi);
            werase(message);
            wrefresh(message);
            tMessEnvoi=0;
            break;
          case 127:
            tMessEnvoi= tMessEnvoi>0?tMessEnvoi-1:0;
            mvwprintw(message,0,tMessEnvoi+1," ");
            wrefresh(message);
            break;
          default:
            messageEnvoi[tMessEnvoi] = c;
            mvwprintw(message,0,tMessEnvoi+1,"%c",c);
            wrefresh(message);
            tMessEnvoi++;
            break;
        }
      }
      if (fds[1].revents & POLLIN)
      {
        read(sockClient,&tMessRecu,sizeof(int16_t));
        read(sockClient,messageRecu,tMessRecu);
        messageRecu[tMessRecu]='\0';
        wprintw(chat,"%s\n",messageRecu);
        wrefresh(chat);
        beep();
        nbMessagesRecus++;
      }
    }
  }

  return 0;
}

