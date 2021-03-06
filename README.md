# IRChat - Projet de programmation réseau (RE216)

Ce projet consiste en la réalisation d'un grand classique de la programmation réseau, un cas pratique de discussion instantanée de type client/serveur. A titre d'exemple, vous pouvez jeter un coup d'œil au protocole IRC (Internet Relay Chat) défini originellement par la RFC1459.

Le projet a pour objectif la réalisation d'une application de chat client/serveur en C permettant d'échanger des messages entre deux utilisateurs, entre plusieurs utilisateurs, ou à destination de la totalité des utilisateurs connectés sur le réseau, ainsi que de s'envoyer des fichiers.
L'objectif sous-jacent de ce projet est la manipulation de l'API socket POSIX en C vue en cours, ainsi que la mise œuvre de communications sur TCP/IP.

# Jalons 

## Jalon 1 - Client-serveur TCP et serveur multi-clients 
[Top](#re216-\--projet-de-programmation-réseau)

### Description 

Dans ce jalon, vous allez vous concentrer sur la réalisation d'un modèle client/serveur IPv4 où le serveur renvoie au client la chaîne de caractère précédemment envoyée.
Dans ce modèle de communication, il y a une partie client et une partie serveur. Pour la première partie, le client devra créer une socket TCP (avec une adresse IPv4 et un numéro port renseigné en argument du programme), établir une connexion sur la socket du serveur. L'envoi et la réception de données pourra alors se faire pour le client.
Parallèlement, du côté du serveur, il faudra créer une socket, la lier à un port d'écoute et repérer la nouvelle connexion. Une fois la connexion acceptée, la réception et l'envoi de données peut se faire.
Dans ce jalon, une fois la connexion établie, le client doit envoyer une chaîne de caractère tapée au clavier au serveur. En réponse, le serveur récupère cette chaîne et la renvoie directement à l'utilisateur en question qui doit l'afficher sur son écran.

Le serveur doit être capable gérer plusieurs clients en parallèle. Pour cela, il faut utiliser un unique processus pour réaliser toutes les tâches en utilisant la fonction **poll()**. Les informations des clients (descripteur de fichier de la socket client et adresse/port) doivent être stockés par le serveur dans une liste chaînée.

Le client doit pouvoir gérer en même temps les messages provenant du serveur et le texte tapé au clavier. Pour cela, il faut également utiliser la fonction poll() côté client sur l'entrée standard du programme et la socket pour la communication avec le serveur.

### Exigences

Les exigences/requirements pour ce premier jalon sont définis comme suit :

**Req1.1** : Création d'un client qui se connecte en TCP à un serveur renseigné par un port et un nom de domaine passés comme arguments du programme comme suit :
```
./client <server_name> <server_port>
```

**Req1.2** : Création d'un server avec une socket d'écoute qui accepte la connexion et gère les données entrantes. Le port du serveur doit être passé comme argument du programme.
```
./server <server_port>
```

**Req1.3** : Le serveur doit être capable d’accepter les connexions et de répondre à plusieurs clients en même temps.

**Req1.4** : Le client doit pouvoir prendre une chaîne de caractère tapée au clavier, l'envoyer au serveur et recevoir de ce dernier la même chaîne de caractère.

**Req1.5** : Le client doit pouvoir gérer les chaînes de caractère tapées au clavier et les messages provenant du serveur en même temps.

**Req1.6** : Le serveur, en recevant une chaîne de caractère depuis un client, doit répéter cette chaîne uniquement à ce même client.

**Req1.7** : La connexion doit se couper lorsque le client envoi '/quit'. Que ce soit du coté serveur ou du coté client, les sockets créés doivent être fermées et la mémoire allouée aux structures de données doit être libérée.

**Req1.8** : Le serveur doit stocker les informations des clients (descripteur de fichiers et adresse/port remplis par la fonction **accept()**) dans une liste chaînée.


## Jalon 2 - Les utilisateurs 
[Top](#re216-\--projet-de-programmation-réseau)

### Description

L'objectif de ce jalon est de permettre au serveur de récupérer, stocker et utiliser des informations relatives aux utilisateurs du service de messagerie instantanée. Le serveur n'est plus, comme dans le jalon 1, un serveur répétitif mais plutôt l'intermédiaire entre les utilisateurs pour l'envoi de messages. 
Grâce à cela, les utilisateurs seront capable de choisir un pseudo, de voir les pseudos des autres utilisateurs connectés, de récupérer des informations sur ces utilisateurs, d’envoyer des messages privés à un utilisateur et d’envoyer des messages à tous les utilisateurs connectés.
Pour faire cela, les utilisateurs devront taper des commandes avant leur message (/nick, /who, /whois <pseudo>, /msgall <msg>, /msg <pseudo> <msg>) qui seront interprétées par le client et le serveur.

À partir de ce jalon, il vous est demandé de ne plus envoyer de simple chaînes de caractères mais d’utiliser la structure suivante pour vos messages : 

```C
#define NICK_LEN 128
#define INFOS_LEN 128

enum msg_type { 
	NICKNAME_NEW,
	NICKNAME_LIST,
	NICKNAME_INFOS,
	ECHO_SEND,
	UNICAST_SEND, 
	BROADCAST_SEND,
	MULTICAST_CREATE,
	MULTICAST_LIST,
	MULTICAST_JOIN,
	MULTICAST_SEND,
	MULTICAST_QUIT,
	FILE_REQUEST,
	FILE_ACCEPT,
	FILE_REJECT,
	FILE_SEND,
	FILE_ACK
};

struct message {
	int pld_len;
	char nick_sender[NICK_LEN];
	enum msg_type type;
	char infos[INFOS_LEN];
};

static char* msg_type_str[] = {
	"NICKNAME_NEW",
	"NICKNAME_LIST",
	"NICKNAME_INFOS",
	"ECHO_SEND",
	"UNICAST_SEND", 
	"BROADCAST_SEND",
	"MULTICAST_CREATE",
	"MULTICAST_LIST",
	"MULTICAST_JOIN",
	"MULTICAST_SEND",
	"MULTICAST_QUIT",
	"FILE_REQUEST",
	"FILE_ACCEPT",
	"FILE_REJECT",
	"FILE_SEND",
	"FILE_ACK"
};
```

Cette structure est déclarée dans le fichier msg_struct.h.

La structure s’utilise de la façon suivante:

- Le champ **pld_len** contient le taille des données utiles à envoyer (généralement, la taille du message à envoyer). S’il n’y a pas de données utiles (par exemple la requête en question n’a pas de données de message ou de fichier), **pld_len** doit valoir 0.
- Le champ **nick_sender** contient le pseudo de l’utilisateur qui envoie le message.
- Le champ **type** contient le type de message (cf la liste dans enum msg_type). Ce gens va être utilisé pour différencier les actions demandées par l’utilisateur.
- Le champ **infos** va contenir des informations à envoyer. Ces informations dépendent du champ **type**.

Juste après ce message, il est possible d’envoyer un ensemble d'octet contenant le payload (i.e. les données utiles) de taille **pld_len**.

Dans ce jalon, il faut utiliser les types : NICKNAME_NEW, NICKNAME_LIST, NICKNAME_INFOS, ECHO_SEND, UNICAST_SEND, BROADCAST_SEND.

Les champs **infos** doivent contenir les valeurs suivantes en fonction des cas : 
 - Pour **NICKNAME_NEW**, le champ **infos** contient le nouveau pseudo. Le champ **nick_sender** contient une chaîne de caractères vide si l’utilisateur n’a pas encore de pseudo, ou le pseudo actuel si l’utilisateur change de pseudo.
- Pour **NICKNAME_LIST**, le champ **infos** contient une chaîne de caractères vide.
- Pour  **NICKNAME_INFOS**, le champ **infos** contient le pseudo de l’utilisateur dont on veut les informations.
- Pour **ECHO_SEND**, le champ **infos** contient une chaîne de caractères vide.
- Pour **UNICAST_SEND**, le champ **infos** contient le pseudo de l’utilisateur à qui on veut envoyer le message.
- Pour **BROADCAST_SEND**, le champ **infos** contient une chaîne de caractères vide.

### Exigences

**Req2.0** : Les messages échangés entre le client et le serveur doivent impérativement respecter la structure donnée dans le sujet. Dans ce sujet, la seule façon de transmettre un message de taille non-déterminée à l'avance est d'utiliser 2 messages : Un premier contenant les octets de la _struct message_ avec le champ **pld_len** qui indique la taille du second message, un second message de taille **pld_len** contenant les octets utiles.

**Req2.1** : Une fois la connexion établie avec le serveur, le client doit s'identifier par son pseudo (avec la commande /nick <pseudo>, type NICKNAME_NEW). Le cas particulier où un utilisateur se connecte avec un pseudo trop long ou un pseudo avec des espaces et autres caractères spéciaux (autre que chiffres ou lettres de l'alphabet) doivent être gérés.
```
Connecting to server ... done!
[Server] : please login with /nick <your pseudo>
/nick CoolestUserEver
[Serveur] : Welcome on the chat CoolestUserEver
```
**Req2.2** : Un utilisateur doit recevoir une erreur si le pseudo qu’il désire est déjà attribué.

**Req2.3** : Le serveur doit gérer plusieurs utilisateurs et plusieurs connexions. Les utilisateurs (pseudo) et les informations liées à leur connexion (numéro de socket et structure d’adresse) doivent être stockés par le serveur dans une liste chaînée.

**Req2.4** : Le serveur doit tenir compte du changement de pseudo d’un utilisateur (réutilisation de la commande /nick <pseudo>).

**Req2.5** : un client doit pouvoir obtenir du serveur la liste des utilisateurs connectés. (commande /who, type NICKNAME_LIST)

```
/who
[Server] : Online users are
                          - User1
                          - User2
                          - CoolestUserEver

```

**Req2.6** : un client doit pouvoir obtenir du serveur des informations sur un utilisateur en particulier (avec la commande /whois, type NICKNAME_INFOS). Les informations à donner sont, la date de connexion, l'adresse IP et le numéro de port utilisés par le client.

```
/whois  User1
[Server] : User1 connected since 2014/09/29@19:23 with IP address 192.168.3.165 and port number 52322
```

**Req2.7** : Un utilisateur doit pouvoir envoyer un message à tous les autres utilisateurs à la fois (commande /msgall <msg>, type BROADCAST_SEND).

```
%terminal_user0>  /msgall Hello all
                    %terminal_user1 > [user0] : Hello all
                                    %terminal_user2 > [user0] : Hello all
```

**Req2.8** : Un message envoyé ne doit pas être retransmis à l'expéditeur.

**Req2.9** : Un utilisateur doit pouvoir envoyer un message privé à un autre utilisateur (commande /msg <pseudo> <msg>, type UNICAST_SEND).

```
%terminal_user0> /msg user1 Hello
                    %terminal_user1 > [user0] : Hello
```

**Req2.10** : Le serveur doit traiter les requetes de type UNICAST_SEND lorsque l'utilisateur spécifié n'existe pas. Le serveur doit alors renvoyer à l'émetteur une information pertinente.
```
%terminal_user0> /msg userKJHDQ Hello
                    %terminal_user1 > [Server] : user userKJHDQ does not exist
```

**Req2.11** : Le serveur doit considerer sa fonction “echo” (i.e. renvoyer le message à l’utilisateur) si aucune commande n’est tapée avant le message (type ECHO_SEND).


## Jalon 3 - Les salons de discussion
[Top](#re216-\--projet-de-programmation-réseau)

### Description

Ce jalon a pour objectif la réalisation des messages entre les utilisateurs afin que votre application devienne une application de messagerie instantanée à part entière. Jusqu'à présent, le serveur ne permettait que d’envoyer des messages privés et des messages en à tout le monde. Dorénavant, vos utilisateurs pourront interagir avec des salons de discussion.
Dans ce contexte, un utilisateur peut créer un salon. Les utilisateurs ont alors la possibilité de rejoindre ce salon, et une fois inscrits les utilisateurs du salon peuvent s'échanger des messages entre eux. Les utilisateurs peuvent quitter le salon ou changer de salon quand ils le souhaitent.

Dans ce jalon, il faut utiliser les types : MULTICAST_CREATE, MULTICAST_LIST, MULTICAST_JOIN, MULTICAST_SEND et MULTICAST_QUIT.

Les champs **infos** doivent contenir les valeurs suivantes en fonction des cas : 
 - Pour **MULTICAST_CREATE**, le champ **infos** contient le nom du salon à créer.
- Pour **MULTICAST_LIST**, le champ **infos** contient une chaîne de caractères vide.
- Pour  **MULTICAST_JOIN**, le champ **infos** contient le nom du salon à rejoindre.
- Pour **MULTICAST_SEND**, le champ **infos** contient le nom du salon dans lequel on veut envoyer le message.
- Pour **MULTICAST_QUIT**, le champ **infos** contient le nom du salon à quitter.

### Exigences

**Req3.1** : Un utilisateur doit pouvoir créer un salon (commande /create <channel_name>, type MULTICAST_CREATE). Les cas particuliers où un utilisateur déclare un salon avec des espaces ou des caractères spéciaux (i.e. autre que les lettres de l'alphabet et des chiffres) doivent être gérés.

**Req3.2** : L'utilisateur doit rejoindre automatiquement le salon qu’il vient de créer, en quittant le salon dans lequel il se trouvait le cas échéant.

**Req3.3** : L'utilisateur doit pouvoir demander la liste des salons (commande /channel_list, type MULTICAST_LIST).

**Req3.4** : Le serveur doit retourner un message d'erreur à l'utilisateur qui demande la création d'un salon déjà existant.

**Req3.5** : L'utilisateur doit pouvoir rejoindre et quitter un salon (commandes /join et /quit, type MULTICAST_JOIN et MULTICAST_QUIT).

**Req3.6** : L'utilisateur inscrit à un salon doit pouvoir changer de salon, ce qui lui fait quitter le salon en cours.

**Req3.7** : Le serveur doit détruire le salon lorsque son dernier occupant le quitte et doit notifier le dernier utilisateurs de la destruction du salon en même temps.

**Req3.8** : L'utilisateur envoie des messages dans le salon dans lequel il se trouve en tapant directement une chaine de caractère. Un message envoyé dans un salon ne doit pas être transmis à d'autres utilisateurs que ceux présents dans le salon (multicast, type MULTICAST_SEND).

**Req3.9** : Les utilisateurs qui composent un salon doivent être notifié de chaque départ/arrivé d'utilisateurs dans le salon dans lequel ils se trouvent. 

Exemple de fonctionnement des salons : 

```
%terminal_user0> /create channel_name
%terminal_user0> You have created channel channel_name
%terminal_user0[channel_name]> You have joined channel_name
                    %terminal_user1> /join channel_name
                    %terminal_user1[channel_name]> INFO> You have joined channel_name
%terminal_user0[channel_name]> INFO> user1 has joined channel_name
%terminal_user0[channel_name]>  I'm downtown
                    %terminal_user1[channel_name]> user0> : I'm downtown
%terminal_user0[channel_name] > /quit channel_name
					%terminal_user1[channel_name] INFO> user0 has quit channel_name
                    %terminal_user1[channel_name] > /quit channel_name
					%terminal_user0> INFO> You were the last user in this channel, channel_name has been destroyed

```




## Jalon 4 - Les transferts de fichiers
[Top](#re216-\--projet-de-programmation-réseau)

### Description

L'objectif de ce jalon est de permettre à un utilisateur d'envoyer un fichier à un autre utilisateur. Plusieurs schémas sont possibles pour implémenter cette fonctionnalité. Nous retiendrons le mode P2P (pair à pair) ne nécessitant pas de passer le serveur pour échanger des données. Le serveur n'est utilisé que pour mettre en relation les utilisateurs.

Les différentes étapes d’un échange de fichiers sont les suivantes : 
- L’émetteur envoie une demande d’envoi de fichier au serveur, en précisant le nom du récepteur et le nom du fichier (commande /send, type FILE_REQUEST).
- Le serveur transmet cette demande à l’utilisateur récepteur.
- L’utilisateur récepteur peut accepter le téléchargement ou non.
- La réponse est renvoyée à l’émetteur par le biais du serveur (type FILE_ACCEPT ou FILE_REJECT).
- Si le récepteur a refusé l’envoi, l’émetteur en est informé et le fichier n’est pas envoyé.
- Si le récepteur a accepté, la réponse doit contenir l’adresse IP  et le port d’écoute du récepteur pour la connection pair-à-pair. 
- L’émetteur se connecte alors directement au récepteur, et lui envoie le fichier en direct, sans passer par le serveur (type FILE_SEND).
- Une fois le fichier reçu, le récepteur confirme la réception (type FILE_ACK).

Dans ce jalon, il faut utiliser les types : FILE_REQUEST, FILE_ACCEPT, FILE_REJECT, FILE_SEND et FILE_ACK.

Les champs **infos** doivent contenir les valeurs suivantes en fonction des cas : 
 - Pour **FILE_REQUEST**, le champ **infos** contient le pseudo de l’utilisateur à qui envoyer le fichier. Le nom du fichier se trouvera dans le payload.
- Pour  **FILE_ACCEPT**, le champ **infos** contient le pseudo de l’utilisateur qui avait envoyé le message FILE_REQUEST. Le payload du message contient l’adresse et le port sur lequel le récepteur écoutera la connexion de l’émetteur (de la forme “addr:port”, par exemple : “127.0.0.1:8081”).
- Pour **FILE_REJECT**, le champ **infos** contient le pseudo de l’utilisateur qui avait envoyé le message FILE_REQUEST.
- Pour **FILE_SEND**, le champ **infos** contient le nom du fichier qui est envoyé. Le payload contient les données du fichier lui-même. Attention, ce fichier n’est pas obligatoire un fichier texte et peut contenir des caractères ‘\0’. Il ne faut donc pas utiliser de fonctions propres à la lecture des chaînes de caractères sur ce payload (comme strlen(), strcpy(), strcmp(), etc.).
- Pour **FILE_ACK**, le champ **infos** contient le nom du fichier qui a été correctement reçu par le récepteur.

### Exigences
**Req 5.1** : Un utilisateur (l'émetteur) doit pouvoir envoyer un fichier à un autre utilisateur (le récepteur).

**Req 5.2** : Lors du transfert d'un fichier, le récepteur doit donner son approbation.

**Req 5.3** : Si le récepteur accepte l’échange de fichier, l’émetteur soit pouvoir se connecter directement au récepteur le temps de lui envoyer le fichier.

**Req 5.4** : Si le récepteur refuse l’échange de fichier, l’émetteur doit en être informé.

**Req 5.5** : Lors du transfert d'un fichier, le récepteur et l'émetteur doivent avoir confirmation que l'envoi s'est déroulé correctement.

Exemple de fonctionnement : 

```
%terminal_user1> /send user2 "/home/user/file.txt"
                            %terminal_user2> user1 wants you to accept the
                            transfer of the file named "file.txt". Do you
                            accept? [Y/N]
                            %terminal_user2> Y
%terminal_user1> user2 accepted file transfert.
%terminal_user1> Connecting to user2 and sending the file...
                            %terminal_user2> Receiving the file from 
                            user1...
                            %terminal_user2> file.txt saved in 
                            .re216/inbox/file.txt
%terminal_user1> user2 has received the file.

%terminal_user1> /send user2 "/home/user/correction_du_projet.txt"
                            %terminal_user2> user1 wants you to accept the 
                            transfer of the file named 
                            "correction_du_projet.txt". Do you accept? 
                            [Y/N]
                            %terminal_user2> N
%terminal_user1> user2 cancelled file transfer.
```

[Top](#re216-\--projet-de-programmation-réseau)