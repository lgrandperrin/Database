#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define NAME_LENGTH_MIN 3
#define NAME_LENGTH_MAX 10
#define TELEPHONE_LENGTH 8
#define FNV_PRIME 16777619
#define OFFSET_BASIS 2166136261
#define BUFSIZE 140

/*********************************************************
			STRUCTURES
*********************************************************/
struct directory_data {
	char last_name[NAME_LENGTH_MAX + 1];
	char first_name[NAME_LENGTH_MAX +1];
	char telephone[TELEPHONE_LENGTH + 1];
};

struct directory {
	struct directory_data **data;
	size_t size;
	size_t capacity;
};

typedef size_t (*index_hash_func_t)(const struct directory_data *data);

struct index_bucket {
	const struct directory_data *data;
	struct index_bucket *next;
};

struct index {
	struct index_bucket **buckets;
	size_t count;
	size_t size;
	index_hash_func_t func;
	
};

struct timeval before, after;

/*********************************************************
		DECLARATION DE FONCTIONS
*********************************************************/

/*Affichage des informations d'une entrée*/
void directory_data_print(const struct directory_data *data);

/*Remplissage d'une entrée avec des données aléatoires*/
void directory_data_random(struct directory_data *data);

/*Initialisation d'un répertoire*/
void directory_create(struct directory *self);

/*Vérification si le répertoire est vide*/
bool directory_is_empty(const struct directory *self);

/*Destruction d'un répertoire*/
void directory_destroy(struct directory *self);

/*Ajout d'une entrée dans le répertoire*/
void directory_add(struct directory *self, struct directory_data *data);

/*Ajoute n entrées aléatoirement dans le répertoire*/
void directory_random (struct directory *self, size_t n);

/*Recherche dans le répertoire (non optimisée)*/
void directory_search(const struct directory *self, const char *last_name);

/*Trie le répertoire en fonction de last_name*/
void directory_sort(struct directory *self);

/*Quicksort*/
ssize_t directory_partition(struct directory *self, ssize_t i, ssize_t j);
void directory_quick_sort_partial(struct directory *self, ssize_t i, ssize_t j);

/*Echange deux entrées dans le répertoire*/
void directory_swap(struct directory *self, size_t i, size_t j);

/*Recherche dans le répertoire (optimisée)*/
void directory_search_opt(const struct directory *self, const char *last_name);

/*Recherche Dichotomique*/
size_t directory_binary_search(const struct directory *self, size_t n, const char *e, size_t lo, size_t hi);

/*Création d'une liste chainée*/
void index_bucket_create (struct index_bucket *self);

/*Ajoute un élément dans une liste chainée*/
struct index_bucket *index_bucket_add (struct index_bucket *self, const struct directory_data *data);

/*Destruction d'une liste chainée*/
void index_bucket_destroy (struct index_bucket *self);

/*Fonction de hashage*/
size_t fnv_hash(const char *key);

/*Fonction de hashage sur le prénom*/
size_t index_first_name_hash (const struct directory_data *data);

/*Fonction de hashage sur le telephone*/
size_t index_telephone_hash (const struct directory_data *data);

/*Création d'un index vide et initialisation de la table de hashage*/
void index_create (struct index *self, index_hash_func_t func);

/*Destruction de l'index*/
void index_destroy (struct index *self);

/*Effectuer un rehash*/
void index_rehash(struct index *self);

/*Ajout d'une entrée dans l'index*/
void index_add(struct index *self, const struct directory_data *data);

/*Remplis l'index avec les données contenus dans le directory*/
void index_fill_with_directory (struct index *self, const struct directory *dir);

/*Recherche et affiche les entrées en fonction d'un prénom grâce à un index*/
void index_search_by_first_name(const struct index *self,const char *first_name);

/*Recherche et affiche les entrées en fonction d'un numéro de téléphone grâce à un index*/
void index_search_by_telephone(const struct index *self, const char *telephone);

/*Supprime le caractère de fin de ligne final et remplace par \0*/
void clean_newline(char *buf, size_t size);

/*Affichage du menu*/
void print_menu();

/*********************************************************
			MAIN
*********************************************************/

int main(int argc, char *argv[]) {
	if (argc != 2) {/*Verification Argument*/
		printf("Un argument est nécessaire !\n");
		return 1;
	}
	int arg = atoi(argv[1]);

	//srand(time(NULL));/*Initialisation du random*/

	struct directory *d = malloc(sizeof(struct directory));/*Création du répertoire*/
	directory_create(d);
	directory_random(d,arg);

	directory_sort(d);/*Trie du répertoire*/

	struct index *index = malloc(sizeof(struct index));/*Création de l'index*/


	char buf[BUFSIZE];
	for(;;){/*Boucle pour le choix du menu*/
		print_menu();/*Affichage du menu*/
		printf("\n>");
		fgets(buf, BUFSIZE, stdin);
		clean_newline(buf,BUFSIZE);
		switch(buf[0]){
			case '1' :/*Recherche par nom (non optimisée)*/
				printf("Search by last_name (not optimised)\n");
				printf("Which last name ?");
				char lname[NAME_LENGTH_MAX + 1];
				printf("\n>");
				fgets(lname, NAME_LENGTH_MAX + 1, stdin);
				clean_newline(lname,NAME_LENGTH_MAX + 1);
				gettimeofday(&before, NULL);
				directory_search(d,lname);
				gettimeofday(&after, NULL);
				printf("(%ld ms)\n",(after.tv_usec - before.tv_usec));
			break;
			case '2' :/*Recherche par nom (omptimisée)*/
				printf("Search by last_name (optimised)\n");
				printf("Which last name ?");
				char lname2[NAME_LENGTH_MAX + 1];
				printf("\n>");
				fgets(lname2, NAME_LENGTH_MAX + 1, stdin);
				clean_newline(lname2,NAME_LENGTH_MAX + 1);
				gettimeofday(&before, NULL);
				directory_search_opt(d,lname2);
				gettimeofday(&after, NULL);
				printf("(%ld ms)\n",(after.tv_usec - before.tv_usec));
			break;
			case '3' :/*Recherche par prénom*/
				printf("Search by first_name\n");
				printf("Which first name ?");
				char fname[NAME_LENGTH_MAX + 1];
				printf("\n>");
				fgets(fname, NAME_LENGTH_MAX + 1, stdin);
				clean_newline(fname,NAME_LENGTH_MAX + 1);
				gettimeofday(&before, NULL);
				index_create(index, index_first_name_hash);
				index_fill_with_directory(index, d);
				index_search_by_first_name(index,fname);
				index_destroy(index);
				gettimeofday(&after, NULL);
				printf("(%ld ms)\n",(after.tv_usec - before.tv_usec));
			break;
			case '4' :/*Recherche par numéro de téléphone*/
				printf("Search by telephone\n");
				printf("Which number ?");
				char tel[TELEPHONE_LENGTH + 1];
				printf("\n>");
				fgets(tel, TELEPHONE_LENGTH + 1, stdin);
				clean_newline(tel,TELEPHONE_LENGTH + 1);
				gettimeofday(&before, NULL);
				index_create(index, index_telephone_hash);
				index_fill_with_directory(index, d);
				index_search_by_telephone(index, tel);
				index_destroy(index);
				gettimeofday(&after, NULL);
				printf("(%ld ms)\n",(after.tv_usec - before.tv_usec));
			break;
			case 'q' :/*Quitter le programme*/
				directory_destroy(d);/*Libérer, délivrer la mémoire*/
				free(d);
				//index_destroy(index);
				free(index);
				exit(0);
		}
	}
}

/*********************************************************
		PARTIE 1 - FONCTIONS
*********************************************************/

/*Création de manière aléatoire des noms, prenoms et numeros de téléphone*/
void directory_data_random(struct directory_data *data){
	int n = rand()%(NAME_LENGTH_MAX-NAME_LENGTH_MIN+1)+NAME_LENGTH_MIN; /*Tirage aléatoire de la taille de la chaine de caractère (entre 3 et 10)*/
	int m = 0;

	/*Lettres de l'alphabet autorisée*/
	char tab[20] = {'B', 'C', 'D', 'F', 'G', 'H', 'J', 'L', 'M', 'N', 'P', 'R', 'S', 'T', 'V', 'A', 'E', 'I', 'O', 'U'};

	/*Génération du nom*/
	for(int i=0; i<n; ++i){
		if(i%2!=0){	/*Voyelle : random entre 0 et 4 + nombre de consonnes*/
			m = (rand()%4)+15;
			data->last_name[i] = tab[m];
			
		}
		else {	/*Consonne : random entre 0 et 14*/
			m = rand()%15;
			data->last_name[i] = tab[m];
			
		}
	}
	data->last_name[n] = '\0'; /*Caractère de fin de chaine*/
	

	/*Génération du prénom*/
	n = rand()%(NAME_LENGTH_MAX-NAME_LENGTH_MIN+1)+NAME_LENGTH_MIN;
	for(int j=0; j<n; ++j){
		if(j%2!=0){	/*Voyelle : random entre 0 et 4 + nombre de consonnes*/
			m = (rand()%4)+15;
			data->first_name[j] = tab[m];
			
		}
		else {	/*Consonne : random entre 0 et 14*/
			m = rand()%15;
			data->first_name[j] = tab[m];
			
		}
	}
	data->first_name[n] = '\0'; /*Caractère de fin de chaine*/

	/*Generation du numéro de téléphone*/
	for(int k=0 ; k<TELEPHONE_LENGTH ; ++k) {
		data->telephone[k] = (rand()%10)+'0'; /*Random sur le code ASCII des caractères en '0' et '9'*/
	}
	data->telephone[TELEPHONE_LENGTH] = '\0'; /*Caractère de fin de chaine*/
}

/*Affichage nom, prenom, téléphone*/
void directory_data_print(const struct directory_data *data){
	printf("%s %s : %s \n", data->last_name , data->first_name, data->telephone);
}


/*Initialisation du répertoire*/
void directory_create(struct directory *self){
	assert(self);
	self->capacity = 10;
	self->data = calloc(self->capacity, sizeof(struct directory_data*));
	self->size = 0;
	
}

/*Test si le répertoire est vide*/
bool directory_is_empty(const struct directory *self){
	return (self->data == NULL);//1 si vide
}

/*Destruction des données*/
void directory_destroy(struct directory *self){
	for(int i = 0; i < self->size ;++i){
		free(self->data[i]);
	}
	self->size=0;
	free(self->data);
}


/*Ajout de l'adresse de la structure contenant le nom, prénom et téléphone à la fin du tableau contenant toute les adresses enregistrées*/
void directory_add(struct directory *self, struct directory_data *data){
	if(self->size == self->capacity) {
		self->capacity *= 2;
		struct directory_data **other = calloc(self->capacity, sizeof(struct directory_data*));
		memcpy(other, self->data, self->size * sizeof(struct directory_data*));
		free(self->data);
		self->data = other;
	}
	self->data[self->size]=data;
	++self->size;
}

/*Création et stockage d'un nombre n de données (nom, prenom, telephone) de façon aléatoire*/
void directory_random (struct directory *self, size_t n) {
	
	for(size_t i = 0; i<n ; ++i) {
		struct directory_data *data = malloc(sizeof(struct directory_data));
		directory_data_random(data);
		directory_add(self, data);	
	}
	
}

/*********************************************************
		PARTIE 2 - FONCTIONS
*********************************************************/

void directory_search(const struct directory *self,const char *last_name){
	printf("\nResults :\n");
	size_t nb = 0;
	for(size_t i = 0; i < self->size; ++i){/*Parcours de l'annuaire*/
		if(strcmp(self->data[i]->last_name, last_name)==0) {
			directory_data_print(self->data[i]);
			++nb;
		}
	}
	if(nb == 0){
		printf("Last_name not found !\n");
	}
}

void directory_sort(struct directory *self){
	directory_quick_sort_partial(self, 0, self->size -1);
}

void directory_quick_sort_partial(struct directory *self, ssize_t i, ssize_t j) {
	if (i < j) {
		ssize_t p = directory_partition(self, i, j);
		directory_quick_sort_partial(self, i, p - 1);
		directory_quick_sort_partial(self, p + 1, j);
	}
}

ssize_t directory_partition(struct directory *self, ssize_t i, ssize_t j) {
	ssize_t pivot_index = i;
	struct directory_data *pivot = self->data[pivot_index];
	directory_swap(self, pivot_index, j);
	ssize_t l = i;
	for (ssize_t k = i; k < j; ++k) {
		if (strcmp(self->data[k]->last_name, pivot->last_name)<0) {
			directory_swap(self, k, l);
			l++;
		}
	}
	directory_swap(self, l, j);
	return l;
}

void directory_swap(struct directory *self, size_t i, size_t j){
	struct directory_data *tmp = self->data[i];
	self->data[i] = self->data[j];
	self->data[j] = tmp;
}


void directory_search_opt(const struct directory *self, const char *last_name){
	size_t i = directory_binary_search(self, self->size, last_name, 0, self->size);
	printf("\nResults:\n");
	if(i==self->size){
		printf("Last_name not found !\n");
		return;
	}
	if(i>=1){
		while(strcmp(self->data[i-1]->last_name,last_name)==0 && i>=1){
			i--;
		}
	}
	directory_data_print(self->data[i]);
	if(i<self->size-1){
		while(strcmp(self->data[i+1]->last_name,last_name)==0 && i < self->size-1){
			i++;
			directory_data_print(self->data[i]);
		}
	}
}

size_t directory_binary_search(const struct directory *self, size_t n, const char *e, size_t lo, size_t hi) {
	
	if (lo == hi) {
		return n;
	}
	size_t mid = (lo + hi) / 2;
	if (strcmp(e, self->data[mid]->last_name) < 0) {
		return directory_binary_search(self, n, e, lo, mid);
	}
	if (strcmp(self->data[mid]->last_name,e) < 0) {
		return directory_binary_search(self, n, e, mid + 1, hi);
	}
	return mid;
}

/*********************************************************
		PARTIE 3 - FONCTIONS
*********************************************************/

/*Ajout d'un élément à la fin de la liste*/
struct index_bucket *index_bucket_add (struct index_bucket *self, const struct directory_data *data) {
	struct index_bucket *other = malloc(sizeof(struct index_bucket));
	other->data = data;
	other->next = self;
	return other;
}

/*Détruit tous les éléments d'une liste*/
void index_bucket_destroy (struct index_bucket *self) {
	
	if(self==NULL){
		return;
	}

	struct index_bucket *other = self->next;
	while(other!=NULL){
		self->next=other->next;
		free(other);
		other = self->next;
	}
	
	free(self);

}

size_t fnv_hash(const char *key) {  
     size_t hash = OFFSET_BASIS;

    for (int i = 0; i < strlen(key); ++i)    {
        hash = hash ^ (key[i]);
        hash = hash * FNV_PRIME;
    }

    return hash;
}

size_t index_first_name_hash (const struct directory_data *data){
	return fnv_hash(data->first_name);
}

size_t index_telephone_hash (const struct directory_data *data) {
	return fnv_hash(data->telephone);
}

void index_create (struct index *self, index_hash_func_t func) {
	self->count=0;
	self->size=10;
	self->buckets=calloc(self->size, sizeof(struct index_bucket));
	self->func = func;/*Choix entre first_name_hash et telephone_hash*/
	
}

void index_destroy (struct index *self){

	for(size_t i = 0; i<self->size; ++i){
		index_bucket_destroy(self->buckets[i]);
	}
	free(self->buckets);
}	

void index_rehash(struct index *self) {
	self->size *=2;
	struct index_bucket **new = calloc(self->size, sizeof(struct index_bucket*)); /* Création de la nouvelle table de hashage */

	for (size_t i = 0; i < self->size/2; ++i){
		struct index_bucket *other = self->buckets[i];
		while(self->buckets[i]!=NULL){
			/*Calcul du nouvel index avec la fonction FNV*/
			size_t index = self->func(self->buckets[i]->data)%self->size;
			new[index] = index_bucket_add(new[index], self->buckets[i]->data);
			self->buckets[i] = self->buckets[i]->next;
		}
		index_bucket_destroy(other);
		free(self->buckets[i]);
	}
	free(self->buckets);
	self->buckets = new;
}



void index_add(struct index *self, const struct directory_data *data) {

	size_t index = self->func(data)%self->size;
	self->buckets[index] = index_bucket_add(self->buckets[index], data);
	++self->count;

	if(self->count/self->size > 0.5){
		index_rehash(self);
	}
}


void index_fill_with_directory (struct index *self, const struct directory *dir) { 
	for(size_t i = 0 ; i < dir->size ; ++i){
		index_add(self, dir->data[i]);
	}
}

void index_search_by_first_name(const struct index *self,const char *first_name){
	size_t index = fnv_hash(first_name)%self->size;
	printf("\nResults:\n");
	if(self->buckets[index]==NULL){
		printf("First_name not found !\n");
		return;
	}
	struct index_bucket *other = self->buckets[index];
	while(other!=NULL){
		if(strcmp(other->data->first_name, first_name) == 0) {
			directory_data_print(other->data);
		}
		
		other = other->next;
	}
}

void index_search_by_telephone(const struct index *self, const char *telephone){
	size_t index = fnv_hash(telephone)%self->size;
	printf("\nResults:\n");
	if(self->buckets[index]==NULL){
		printf("First_name not found !\n");
		return;
	}
	struct index_bucket *other = self->buckets[index];
	while(other!=NULL){
		if(strcmp(other->data->telephone, telephone) == 0) {
			directory_data_print(other->data);
		}
		
		other = other->next;
	}
}

/*********************************************************
		PARTIE 4 - FONCTIONS
*********************************************************/

void clean_newline(char *buf, size_t size){
	char *i = strchr(buf , '\n');
	if(i){
		*i = 0;
	}
}

void print_menu(){
printf("\nWhat do you want to do ?\n");
printf("	1: Search by last_name (not optimised)\n");
printf("	2: Search by last_name (optimised)\n");
printf("	3: Search by first_name\n");
printf("	4: Search by telephone\n");
printf("	q: Quit\n");
}

//Le reste des questions se situe essentiellement dans le Main()


