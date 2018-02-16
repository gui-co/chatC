#include "sock.h"
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>


int creerSocket(struct sockaddr_in * adresse,int port)
{
  int descripteur;
  socklen_t tailleAdresse;

  descripteur = socket(AF_INET,SOCK_STREAM,0);

  adresse->sin_family = AF_INET;
  adresse->sin_addr.s_addr = htonl(INADDR_ANY);
  adresse->sin_port = htons(port);

  tailleAdresse = sizeof(struct sockaddr_in);

  bind(descripteur,(struct sockaddr *) adresse,tailleAdresse);
  getsockname(descripteur,(struct sockaddr *) adresse,&tailleAdresse);

  return descripteur;
}
