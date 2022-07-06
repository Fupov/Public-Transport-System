/* Travail Réaliser Par: 
* Aymane TCHICH 12107196
* Ali BOUNOR 12111353
*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>



//Initialisation des threads
pthread_t pthread_id[4];
pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut3 = PTHREAD_MUTEX_INITIALIZER;

typedef struct User // Structure utilisateur de notre programme qui represente les utilisateurs des transports en commun (bus, taxi, metro)
{
	int IdUser;
	int Dep;
	int Arr;
	int Att;
	int Transfert;
	int AttMax;
}User;
#define CapaciteBus 5 // la capacité d'utilisateurs dans le bus
#define CapaciteMetro 8 //la capacité d'utilisateurs dans le Metro
#define CapaciteTaxi 1 // la capacité d'utilisateurs dans le Taxi
#define MaxUser 25 // le nombre maximal des utilisateurs dans le fichier
#define NBStations 8 // nombre de stations au total
#define nomTube "file" 
int t = 0;
User LBus[CapaciteBus];
User LMetro[CapaciteMetro];
int StationUser[NBStations]; // Nombre des utilisateurs dans chaque station
User Stations[NBStations][MaxUser]; // liste des utilisateurs dans les stations
int NbrUserBus = 0; // nombre des utilisateurs dans le bus
int NbrUserMetro = 0; //nombre des utilisateurs dans le metro;


/*---------------------------------------*/
/* Partie fonctions sur les utilisateurs */
/*---------------------------------------*/



//cette fonction pour affecter chaque utilisateur à sa station de départ 
void AffectUsers(User tab[], int n)
{
	int i=0;
	while(i<n){
		Stations[tab[i].Dep][StationUser[tab[i].Dep]] = tab[i];
		StationUser[tab[i].Dep]++;
		i++;
	}
 
}

void showStations()
{
	int i=0, j=0;
  printf("\n---------------------------------------\n");
	printf("Nb utilisateurs de bus  = %d \n", NbrUserBus);
	printf("Nb utilisateurs de metro = %d \n", NbrUserMetro);
	while(i<NBStations){
		printf("Station n[ %d ] :  \n", i);
		while(j<StationUser[i]){
      		printf("utilisateur n%d [%d -> %d] attente : %d transfert: %d max: %d\n", Stations[i][j].IdUser, Stations[i][j].Dep, Stations[i][j].Arr, Stations[i][j].Att, Stations[i][j].Transfert, Stations[i][j].AttMax);
      j++;
		}
		i++;
	}
  printf("\n---------------------------------------\n");
 
}

//cette fonction c'est boure lire le fichier qui contient les utilisateurs et les affecter.
void ReadFile(FILE * f)
{
  char ligne[30];
	User Users[MaxUser];
	User p;
  char c;
	
	int n = 0;
	
	 while ( fgets( ligne, 30, f) != NULL)
    {
        if (sscanf(ligne,"%s%d%d%d%d%d%d",&c,&p.IdUser,&p.Dep,&p.Arr,&p.Att,&p.Transfert,&p.AttMax) == 7) 
        {
            	Users[n].IdUser=p.IdUser;
             	Users[n].Dep=p.Dep;
             	Users[n].Arr=p.Arr;
             	Users[n].Att=p.Att;
             	Users[n].Transfert=p.Transfert;
             	Users[n].AttMax=p.AttMax;
            	printf(" \n id utilisateur : %d \n",Users[n].IdUser);
                printf(" \n t d'attente : %d \n",Users[n].Att);
            	n++;
             AffectUsers(Users,n);
        }
	}
    showStations();
}



void ViderStation(User arret[], int pos, int* t)
{
	int j=pos;
	if (pos != (*t) + 1 && pos >= 0)
	{
		while(j<(*t)){
			arret[j] = arret[j + 1];
			j++;
		}
		(*t)--;
	}
}
/*------------------------------------*/
/* Partie fonctions sur les Transport */
/*------------------------------------*/

/* embarquement des utilisateurs dans le bus*/
void BoardingBus(int NStation)
{
	int n=0;
	while(n < StationUser[NStation] && NbrUserBus < CapaciteBus){
	
		if (Stations[NStation][n].Dep == NStation)
		{
			printf("[bus] : embarque l'utilisateur {%d} \n", Stations[NStation][n].IdUser);
			LBus[NbrUserBus] = Stations[NStation][n];
			NbrUserBus++;
			//Retirer l'utilisateur de sa station de depart
			ViderStation(Stations[NStation], n, &StationUser[NStation]);
		}
		n++;
	}
}
//embarquement des utilisateurs dans le metro
void BoardingMetro(int NStation)
{
	int i=0;
	while(i < StationUser[NStation] && NbrUserMetro < CapaciteMetro)
	{
		if (Stations[NStation][i].Dep == NStation)
		{
			printf("[Metro] : embarque l'utilisateur {%d} \n", Stations[NStation][i].IdUser);
			LMetro[NbrUserMetro] = Stations[NStation][i];
			NbrUserMetro++;
			//Retirer l'utilisateurs de sa station de depart
			ViderStation(Stations[NStation], i, &StationUser[NStation]);
		}
		i++;
	}

}
// debarquement des utilisateurs du bus 
void LandingBus(int NStation)
{
	if (NbrUserBus > 0) //voir si le bus n'est pas vide
	{
		int i = 0;
			
			while(i < NbrUserBus)
			{
				if (LBus[i].Arr > 4) //si on est arrive au terminus on debarque tout les utilisateurs.
				{
					printf("[bus] : debarque l'utilisateur {%d} \n", LBus[i].IdUser);
					LBus[i].Dep = 5; // la station de depart de l'utilisateur i devient 5 ,ie pour se deplacer à la station de metro 
					Stations[5][StationUser[5]] = LBus[i];
					StationUser[5]++; // on va augmenter le nombre des utilisateurs de la station 5 
					ViderStation(LBus, i, &NbrUserBus);
					i--;
				}else if(LBus[i].Arr == NStation)//si le User i est arrivé à sa station d'arrivé
        		{
       	  			printf("[bus] : debarque l'utilisateur {%d} (arrive)\n", LBus[i].IdUser);//le bus va debarquement l'utilisateur
          			ViderStation(LBus, i, &NbrUserBus);// on va supprimer l'utilisateur de notre liste 
		      		i--;
        		}
				i++;
			}
	}
	
	
}



// debarquement des utilisateurs du Metro 
void LandingMetro(int NStation)
{
	if (NbrUserMetro > 0)
	{
		int i = 0, j = 0;
		if (NStation == 0)
		{
			while(i < NbrUserMetro){
				if (LMetro[i].Arr < 5)//on déplace l'utilisateurs i du station de metro 5 à la station 0 de bus
				{
					printf("[Metro] : debarque l'utilisateur {%d} \n", LMetro[i].IdUser);
					LMetro[i].Dep = 0;
					Stations[0][StationUser[0]] = LMetro[i];
					StationUser[0]++;
					ViderStation(LMetro, i, &NbrUserMetro);
					i--;
				}
				i++;
			}
		}
		while(j < NbrUserMetro){
			if (LMetro[j].Arr == NStation)
			{
				printf("[Metro] : debarque l'utilisateur {%d} (arrive)\n", LMetro[j].IdUser);
				ViderStation(LMetro, j, &NbrUserMetro);
				j--;
			}
			j++;
		}

	}


}


void * TrajetMetro()
{
	int station = 5;
	int cmpt = 1;
	while (1)
	{
		pthread_mutex_lock(&mut1);

		printf("\n Le metro est a la station %d \n", station);
		LandingMetro(station);
		BoardingMetro(station);
		if (cmpt)
		{
			station++;
			if (station >= 7)
				cmpt = 0;
		}
		else
		{
			--station;
			if (station <= 5)
				cmpt = 1;
		}
		sleep(1);
	}
}

void * TrajetBus() {
	int station = 0;
	while (1)
	{
		pthread_mutex_lock(&mut2);
		printf("\n Le bus est a la station %d \n", station);
		LandingBus(station);
		BoardingBus(station);
		station++;
		if (station > 5)
		{
			station = 0;
		}
		pthread_mutex_unlock(&mut1);
		sleep(1);
	}
}


void * verificateur() {
	int i=0,j=0;
	while (1)
	{
		t++;
		if (t % 30 == 0) // chaque 30 seconde on va afficher toute les stations et verifier si les stations sont vide	
		{
			showStations();
			if(NbrUserBus==0 && NbrUserMetro==0){
				printf("\n---------------------------------------\n");
				printf("Tout les utilisateurs sont arrivée a leurs destination\n");
				printf("Fin du programme");
				printf("\n---------------------------------------\n");
			
			    exit(EXIT_SUCCESS);
		}

		}
		
		while(i < NBStations)
		{
			while(j < StationUser[i])
			{
				Stations[i][j].Att++; //incrémenter le t d'attente des utilisateurs dans chaque itération
				if (Stations[i][j].Att > Stations[i][j].AttMax) //une fois l'utilisateur atteint son t d'attente maximale il va prendre un taxi
				{
					int fd;
					char message[250];
          printf("\n Verificateur : transfert du User {%d} vers le taxi \n ",Stations[i][j].IdUser);
          //on va récuperer les details de l'utilisateurs pour qu'on puisse l'ajouter au tube ie on puisse l'ajouter après à la station correspondante au taxi
					sprintf(message, "# %d %d %d %d %d %d", Stations[i][j].IdUser, Stations[i][j].Dep, Stations[i][j].Arr, Stations[i][j].Att, Stations[i][j].Transfert, Stations[i][j].AttMax);
					if ((fd = open(nomTube, O_WRONLY)) == -1)
					{
						fprintf(stderr, "Impossible d'ouvrir l'entrée du tube nommé.\n");
						exit(EXIT_FAILURE);
					}
					else {
						write(fd, message, strlen(message));
					}
					close (fd) ;
          //enfin on va supprimer l'utilisateur de la station ou il était
					ViderStation(Stations[i], i, &StationUser[i]);
				}
				j++;
			}
			i++;
		}
		pthread_mutex_unlock(&mut2);
		sleep(2);
	}
}

void * TrajetTaxi() {
	while (1)
	{
		pthread_mutex_lock(&mut3);
    usleep(10);
		int SortieTube;
		char message[250];

		if ((SortieTube = open (nomTube, O_RDONLY)) == -1)
		{
			printf("erreur ouverture du tube.\n");
			exit(EXIT_FAILURE);
		}
   
		read(SortieTube, message, 250);
    char c;
		User p;
    sscanf(message,"%s %d %d %d %d %d %d",&c,&p.IdUser,&p.Dep,&p.Arr,&p.Att,&p.Transfert,&p.AttMax);
		printf("taxi#{1} : utilisateur {%d} est rendu a la station {%d}\n", p.IdUser, p.Arr);

		pthread_mutex_unlock(&mut3);
		sleep(5);
	}
}



int main(int argc, char *argv[])
{
  system("[ -e file ] && rm  file");
	FILE * f;
	if ( argc == 2 ) {
		f = fopen(argv[1], "r");
	} else
	{
		f = fopen("Users.txt", "r");
	}
	ReadFile(f);
	fclose (f);


	// Création du tube
	if (mkfifo(nomTube, 0666 | O_NONBLOCK) == -1)
	{
		fprintf(stderr, "Erreur de création tube");
		exit(EXIT_FAILURE);
	}
  // Création des threads 
 	if (pthread_create((pthread_id + 0), NULL, TrajetBus, NULL) == -1) {
		fprintf(stderr, "Erreur pthread du Bus\n" );
	}
	if (pthread_create((pthread_id + 1), NULL, TrajetMetro, NULL) == -1) {
		fprintf(stderr, "Erreur pthread de Metro\n");
	}


	if (pthread_create((pthread_id + 2), NULL, verificateur, NULL) == -1) {
		fprintf(stderr, "Erreur thread du verificateur\n" );
	}

	if (pthread_create((pthread_id + 3), NULL, TrajetTaxi, NULL) == -1) {
		fprintf(stderr, "Erreur pthread du taxi 1\n" );
	}

  for (int i=0; i<4 ;i++)
  {
    	if (pthread_join(pthread_id[i], NULL)) {
  
  		fprintf(stderr, "Erreur jointure du thread\n");
  	}
  
  }
 
	return 0;
}
