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


int main()
{
  int sockClient;
  struct sockaddr_in adresseServeur;
  struct sockaddr_in adresseClient;
  socklen_t tailleAdresse;

  adresseServeur.sin_family = AF_INET;
  adresseServeur.sin_addr.s_addr = inet_addr("127.0.0.1");
  adresseServeur.sin_port = htons(45037);
  sockClient = creerSocket(&adresseClient,0);
  tailleAdresse = sizeof(adresseServeur);
  connect(sockClient,(struct sockaddr *)&adresseServeur,tailleAdresse);

  initscr();
  curs_set(0);
  noecho();
  WINDOW * chat;
  WINDOW * envoi;
  chat = subwin(stdscr,LINES-3,COLS,0,0);
  box(chat, ACS_VLINE, ACS_HLINE);
  envoi = subwin(stdscr,3,COLS,LINES-3,0);
  box(envoi, ACS_VLINE, ACS_HLINE);
  scrollok(envoi, 1);
  refresh();

  struct pollfd fds[2];
  fds[0].fd = fileno(stdin);
  fds[0].events = POLLIN;
  fds[1].fd = sockClient;
  fds[1].events = POLLIN;
  char messageEnvoi[500];
  char messageRecu[500];
  int16_t l=0;
  int16_t g;
  int nbMessagesRecus = 1;
  char c;


  while(1)
  {
    if (poll(fds,2,-1) > 0)
    {
      if (fds[0].revents & POLLIN)
      {
        switch (c = getch())
        {
          case '\n':
            write(sockClient,&l,sizeof(int16_t));
            write(sockClient,messageEnvoi,l);
            werase(envoi);
            wrefresh(envoi);
            l=0;
            break;
          case 127:
            l= l>0?l-1:0;
            mvwprintw(envoi,1,l+1," ");
            wrefresh(envoi);
            break;
          default:
            messageEnvoi[l] = c;
            mvwprintw(envoi,1,l+1,"%c",c);
            wrefresh(envoi);
            l++;
            break;
        }
      }
      if (fds[1].revents & POLLIN)
      {
        read(sockClient,&g,sizeof(int16_t));
        read(sockClient,messageRecu,g);
        messageRecu[g]='\0';
        mvwprintw(chat,nbMessagesRecus,1,"%s",messageRecu);
        wrefresh(chat);
        beep();
        nbMessagesRecus++;
      }
    }
  }

  return 0;
}

