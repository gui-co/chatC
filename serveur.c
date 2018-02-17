#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include "sock.h"

#define NB_MAX_CLI 10

int nouveauClient(int socket, struct pollfd fds[NB_MAX_CLI+1])
{
  int i;
  i=1;
  while ((i<NB_MAX_CLI+1) && (fds[i].fd != -1))
  {
    i++;
  }
  if (i == NB_MAX_CLI+1)
  {
    return 0;
  }
  fds[i].fd = accept(socket,NULL,NULL);
  fds[i].events = POLLIN;
  return 1;
}


int nouveauMessage(int fdEmet, struct pollfd fds[NB_MAX_CLI+1])
{
  int16_t tailleMessage;
  char message[500];

  switch (read(fds[fdEmet].fd,&tailleMessage,sizeof(int16_t)))
  {
    case sizeof(int16_t):
      fprintf(stderr,"Réception d'un message de %d octets.\n",tailleMessage);
      break;
    default:
      close(fds[fdEmet].fd);
      fds[fdEmet].fd = -1;
      return 0;
      break;
  }

  read(fds[fdEmet].fd,message,tailleMessage);
  for (int i=1 ; i<NB_MAX_CLI+1 ; i++)
  {
    if (fds[i].fd != -1)
    {
      write(fds[i].fd,&tailleMessage,sizeof(int16_t));
      write(fds[i].fd,message,tailleMessage);
    }
  }
  return 1;
}



int main()
{
  int sockServeur;
  struct sockaddr_in adresseServeur;
  struct pollfd fds[NB_MAX_CLI+1];
  int nbPoll;
  int fdCourant;

  sockServeur = creerSocket(&adresseServeur,0);
  listen(sockServeur,SOMAXCONN);

  fprintf(stdout,"Serveur en écoute sur : %d\n",
      ntohs(adresseServeur.sin_port));

  fds[0].fd = sockServeur;
  fds[0].events = POLLIN;

  for (int i=1 ; i<NB_MAX_CLI+1 ; i++)
  {
    fds[i].fd = -1;
  }

  while(1)
  {
    if ((nbPoll = poll(fds,NB_MAX_CLI+1,-1)) > 0)
    {
      fprintf(stderr,"Événement reçu !\n");
      fdCourant = 0;
      while ((nbPoll > 0) && (fdCourant < NB_MAX_CLI+1))
      {
        if (fds[fdCourant].revents & POLLIN)
        {
          nbPoll--;
          switch (fdCourant)
          {
            case 0:
              switch (nouveauClient(sockServeur,fds))
              {
                case 1:
                  fprintf(stderr,"Connexion d'un client.\n");
                  break;
                default:
                  fprintf(stderr,"Déjà le nombre maximal de client. Connexion refusée.\n");
                  break;
              }
              break;
            default:
              switch(nouveauMessage(fdCourant,fds))
              {
                case 1:
                  fprintf(stderr,"Envoi d'un message.\n");
                  break;
                case 0:
                  fprintf(stderr,"Client déconnecté.\n");
                  break;
              }
          }
        }
        fdCourant++;
      }
    }
  }

  return 0;
}

