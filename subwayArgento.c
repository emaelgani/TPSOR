#include <stdio.h>      // libreria estandar
#include <stdlib.h>     // para usar exit y funciones de la libreria standard
#include <string.h>
#include <pthread.h>    // para usar threads
#include <semaphore.h>  // para usar semaforos
#include <unistd.h>


#define LIMITE 50
pthread_mutex_t mutexSalero = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexSarten = PTHREAD_MUTEX_INITIALIZER;
sem_t s_Horno; //Utilizo un semaforo global para el horno ya que pueden acceder dos hilos

void escribir();
//creo estructura de semaforos 
struct semaforos {
    sem_t sem_mezclar;
	sem_t sem_salar;
    sem_t sem_agregarCarne;
    sem_t sem_empanar;
    sem_t sem_cocinarMilanesa;
    sem_t sem_armarSanguche1; //Hay 3 acciones que tienen que habilitar la función de finalizar el sanguche.
    sem_t sem_armarSanguche2;
    sem_t sem_armarSanguche3;
	//poner demas semaforos aqui
};

//creo los pasos con los ingredientes
struct paso {
   char accion [LIMITE];
   char ingredientes[4][LIMITE];
   
};

//creo los parametros de los hilos 
struct parametro {
 int equipo_param;
 struct semaforos semaforos_param;
 struct paso pasos_param[13];
 int tiempo;
};

//funcion para imprimir las acciones y los ingredientes de la accion
void* imprimirAccion(void *data, char *accionIn) {
	struct parametro *mydata = data;
	//calculo la longitud del array de pasos 
	int sizeArray = (int)( sizeof(mydata->pasos_param) / sizeof(mydata->pasos_param[0]));
	//indice para recorrer array de pasos 
	int i;
	for(i = 0; i < sizeArray; i ++){
		//pregunto si la accion del array es igual a la pasada por parametro (si es igual la funcion strcmp devuelve cero)
		if(strcmp(mydata->pasos_param[i].accion, accionIn) == 0){
		printf("\tEquipo %d - accion %s \n " , mydata->equipo_param, mydata->pasos_param[i].accion);
		//calculo la longitud del array de ingredientes
		int sizeArrayIngredientes = (int)( sizeof(mydata->pasos_param[i].ingredientes) / sizeof(mydata->pasos_param[i].ingredientes[0]) );
		//indice para recorrer array de ingredientes
		int h;
		
		printf("\tEquipo %d -----------ingredientes : ----------\n",mydata->equipo_param );
		//
		
			for(h = 0; h < sizeArrayIngredientes; h++) {
				
				//consulto si la posicion tiene valor porque no se cuantos ingredientes tengo por accion 
				if(strlen(mydata->pasos_param[i].ingredientes[h]) != 0) {
				
							printf("\tEquipo %d ingrediente  %d : %s \n",mydata->equipo_param,h,mydata->pasos_param[i].ingredientes[h]);
				
				}
			}
		
		}
	}
}
//funcion para tomar de ejemplo
void* cortarCebollaYdemas(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar otros ingredientes";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
    
	//BLOQUE QUE ESCRIBE EN EL ARCHIVO SALIDA.TXT
	   int t= *((int *) &mydata->tiempo);
    escribir("¡Cortando lechuga, tomate, cebolla morada y pepino!", data);
	mydata->tiempo=t+1; //Finalizó la acción, por lo tanto le sumo 1
	//FIN DEL BLOQUE
	
	
	imprimirAccion(mydata,accion); //Imprime la salida a la terminal
	
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar cebolla  me habilita armar sanguche)
    sem_post(&mydata->semaforos_param.sem_armarSanguche1);
	
    pthread_exit(NULL);
}


//funcion para tomar de ejemplo
void* cortar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
    
	//BLOQUE QUE ESCRIBE EN EL ARCHIVO SALIDA.TXT
    int t= *((int *) &mydata->tiempo);
    escribir("¡Cortando ajo y perejil!", data);
	mydata->tiempo=t+1;
	//FIN DEL BLOQUE
	
	imprimirAccion(mydata,accion);
	
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);
    pthread_exit(NULL);
}


//funcion para tomar de ejemplo
void* mezclar(void *data) {
	
	//creo el nombre de la accion de la funcion 
	char *accion = "mezclar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_mezclar);
	
	//Bloque que habilida escribir en salida.txt
	int t= *((int *) &mydata->tiempo);
    escribir("¡Mezclando ajo y perejil con 1 huevo!", data);
	mydata->tiempo=t+1;
	
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (mezclar me habilita salar)
    sem_post(&mydata->semaforos_param.sem_salar);
	
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* salar(void *data) {
	
	//creo el nombre de la accion de la funcion 
	char *accion = "salar mezcla";
  
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_salar);
	pthread_mutex_lock(&mutexSalero);
	
	//bloque que escribe la salida en txt
	int t= *((int *) &mydata->tiempo);
    escribir("¡Salando!", data);
	mydata->tiempo=t+1;
	//fin del bloque que escribe en txt
	
	  printf("\n");
    printf("\t¡Equipo %d  %s \n" ,mydata->equipo_param, ",Salando!");
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	 printf("\n");
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );

	//Escribe de nuevo en el txt para informar que se habilitó el salero
	int t2= *((int *) &mydata->tiempo);
    escribir("¡Dejando de Salar!", data);
	mydata->tiempo=t2+1;
	
      printf("\n");
    printf("\t¡Equipo %d  %s \n" ,mydata->equipo_param, ",Terminó de salar!");
	printf("\n");
	//doy la señal a la siguiente accion (salar me habilita agregar carne)
	pthread_mutex_unlock(&mutexSalero);
	sem_post(&mydata->semaforos_param.sem_agregarCarne);
    pthread_exit(NULL);
}


//funcion para tomar de ejemplo
void* agregarCarne(void *data) {
	
	//creo el nombre de la accion de la funcion 
	char *accion = "agregar carne a la mezcla";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_agregarCarne);
	
	int t= *((int *) &mydata->tiempo);
    escribir("¡Agregando la carne a la mezcla!", data);
	mydata->tiempo=t+1;
	
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );


	//doy la señal a la siguiente accion (cortar me habilita mezclar)
	
     sem_post(&mydata->semaforos_param.sem_empanar);
    pthread_exit(NULL);
}


void* empanar(void *data) {
	
	//creo el nombre de la accion de la funcion 
	char *accion = "empanar milanesa";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_empanar);
	
	//Bloque que escribe en el archivo .txt
	int t= *((int *) &mydata->tiempo);
    escribir("¡Empanando la milanesa!", data);
	mydata->tiempo=t+1;
	
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );


	//doy la señal a la siguiente accion (empanar me habilita a cocinar milanesa)
	
    sem_post(&mydata->semaforos_param.sem_cocinarMilanesa);
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* cocinarMilanesa(void *data) {
	
	//creo el nombre de la accion de la funcion 
	char *accion = "cocinar milanesa";
    
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
    
	sem_wait(&mydata->semaforos_param.sem_cocinarMilanesa);
    pthread_mutex_lock(&mutexSarten);

	int t= *((int *) &mydata->tiempo);
    escribir("¡Cocinando la milanesa!", data);
	mydata->tiempo=t+1;
	
    printf("\n");
    printf("\t¡Equipo %d  %s \n" ,mydata->equipo_param, ",Cocinando Milanesa!");
    
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	
	printf("\n");
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );

	int t2= *((int *) &mydata->tiempo);
    escribir("¡Terminó de cocinar la milanesa!", data);
	mydata->tiempo=t2+1;
	
	
    printf("\n");
    printf("\t¡Equipo %d  %s \n" ,mydata->equipo_param, ",Terminó de cocinar la milanesa!");
    printf("\n");
    

	//doy la señal a la siguiente accion (cocinar milanesa me habilita armar sanguche)
    pthread_mutex_unlock(&mutexSarten);
	sem_post(&mydata->semaforos_param.sem_armarSanguche2);
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* hornear(void *data) {
	
	//creo el nombre de la accion de la funcion 
	char *accion = "hornear pan";
  
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	sem_wait(&s_Horno);
	
	int t= *((int *) &mydata->tiempo);
    escribir("¡Horneando panes!", data);
	mydata->tiempo=t+1;
	
	
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	printf("\n");
    printf("\t¡Equipo %d  %s \n" ,mydata->equipo_param, ",Horneando panes!");
	imprimirAccion(mydata,accion);
	printf("\n");
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	
		int t2= *((int *) &mydata->tiempo);
    escribir("¡Terminó de hornear los panes!", data);
	mydata->tiempo=t2+1;
	
	
    printf("\n");
    printf("\t¡Equipo %d  %s \n" ,mydata->equipo_param, ",El pan está listo!");
    printf("\n");
	//doy la señal a la siguiente accion (hornear pan me habilita a armar sanguche)
	sem_post(&s_Horno);
	 sem_post(&mydata->semaforos_param.sem_armarSanguche3);
    pthread_exit(NULL);
}

//funcion para tomar de ejemplo
void* armarSanguche(void *data) {
	
	//creo el nombre de la accion de la funcion 
	char *accion = "armar sanguche";
    
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	sem_wait(&mydata->semaforos_param.sem_armarSanguche1);
	sem_wait(&mydata->semaforos_param.sem_armarSanguche2);
    sem_wait(&mydata->semaforos_param.sem_armarSanguche3);
	
		int t= *((int *) &mydata->tiempo);
    escribir("¡Armando sanguches!", data);
	mydata->tiempo=t+1;
	
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
    
  
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	
	
	int t2= *((int *) &mydata->tiempo);
    escribir("¡Ganó!", data);
	mydata->tiempo=t2+1;
	
    printf("\n");
    printf("\t¡Equipo %d  %s \n" ,mydata->equipo_param, ",Terminó de armar sanguche! ");
   printf("\t¡Equipo %d  %s \n" ,mydata->equipo_param, ",Ganó!\n ");
	
	
	
    exit(1);
    printf("\n");
    pthread_exit(NULL);
}


void* levantarReceta(void *data){

struct parametro *pthread_data = data;
char aux[200];
		char delimitador[] = "|";
		
		FILE *f;
		f = fopen("receta.txt", "r");
		if(f== NULL){
			printf("No se ha podido cargar el fichero");
			exit(1);
		}
	
		int cont=0;
		while(cont<9){ //cantidad de lineas a leer
			fgets(aux,200,f);
			char *token = strtok(aux, delimitador);
			  if(token != NULL){
				  
				  int contInterno=0;
        while(token != NULL){
            
           if(contInterno==0){
		   strcpy(pthread_data->pasos_param[cont].accion, token);
		   }
		   else{
			  strcpy(pthread_data->pasos_param[cont].ingredientes[contInterno-1], token);
			 
		   
		   }
		  
			
            token = strtok(NULL, delimitador);
			contInterno++;
        }
    }
		
		//printf("%s", aux);
			cont++;
			
		}
		
	
	fclose(f);

}

void escribir(char *string, void *data){

char *nombreArchivo = "salida.txt";
    char *modo = "a+";// w es para sobrescribir, a+ es para añadir al existente
    FILE *archivo = fopen(nombreArchivo, modo);
    // Si por alguna razón el archivo no fue abierto, salimos inmediatamente
    if (archivo == NULL) {
        printf("Error abriendo archivo %s", nombreArchivo);
        
    }
    struct parametro *mydata = data;
   
   int t= *((int *) &mydata->tiempo);
    fprintf(archivo, "Equipo %d Acción %d ,%s", mydata->equipo_param , t , string);
    fprintf(archivo, "%s", "\n");
    // Al final, cerramos el archivo
    fclose(archivo);
    
}

void* ejecutarReceta(void *i) {
	
    int tiempo=0; //Variable que utilizo para determinar en que momento se ejecuta cada acción

	//variables semaforos
	sem_t sem_mezclar;
	sem_t sem_salar;
    sem_t sem_agregarCarne;
    sem_t sem_empanar;
    sem_t sem_cocinarMilanesa;
   
    sem_t sem_armarSanguche1;
    sem_t sem_armarSanguche2;
    sem_t sem_armarSanguche3;
	//crear variables semaforos aqui
	
	//variables hilos
	pthread_t p1; //cortar
	pthread_t p2; //mezclar
	pthread_t p3; //salar
  	pthread_t p4; //hornear
    pthread_t p5; //agregar carne a la mezcla
    pthread_t p6; //empanar milanesa
    pthread_t p7; //Cocinar mila
    pthread_t p8; //Corta la lechuga, tomate y demas
    pthread_t p9;//Armado de sanguche 
   
	//crear variables hilos aqui
	
	//numero del equipo (casteo el puntero a un int)
	int p = *((int *) i);
	
	printf("Ejecutando equipo %d \n", p);

	//reservo memoria para el struct
	struct parametro *pthread_data = malloc(sizeof(struct parametro));

	//seteo los valores al struct
	
	//seteo numero de grupo
	pthread_data->equipo_param = p;

    pthread_data->tiempo=0;
	//seteo semaforos
	pthread_data->semaforos_param.sem_mezclar = sem_mezclar;
	pthread_data->semaforos_param.sem_salar = sem_salar;
    pthread_data->semaforos_param.sem_agregarCarne = sem_agregarCarne;
    pthread_data->semaforos_param.sem_empanar = sem_empanar;
    pthread_data->semaforos_param.sem_cocinarMilanesa = sem_cocinarMilanesa;
    
    pthread_data->semaforos_param.sem_armarSanguche1 = sem_armarSanguche1;
    pthread_data->semaforos_param.sem_armarSanguche2 = sem_armarSanguche2;
    pthread_data->semaforos_param.sem_armarSanguche3 = sem_armarSanguche3;
	
	
	levantarReceta(pthread_data); //Levanta la receta del archivo .txt
	

	sem_init(&(pthread_data->semaforos_param.sem_mezclar),0,0);
	sem_init(&(pthread_data->semaforos_param.sem_salar),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_agregarCarne),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_empanar),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_cocinarMilanesa),0,0);

    sem_init(&(pthread_data->semaforos_param.sem_armarSanguche1),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_armarSanguche2),0,0);
    sem_init(&(pthread_data->semaforos_param.sem_armarSanguche3),0,0);
	//inicializar demas semaforos aqui


	//creo los hilos a todos les paso el struct creado (el mismo a todos los hilos) ya que todos comparten los semaforos 
    int rc;
    rc = pthread_create(&p1,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	
	 rc = pthread_create(&p2,                           //identificador unico
                           NULL,                          //atributos del thread
                               mezclar,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia
	
	 rc = pthread_create(&p3,                           //identificador unico
                           NULL,                          //atributos del thread
                               salar,             //funcion a ejecutar
                                pthread_data);   
	

    rc = pthread_create(&p4,                           //identificador unico
                           NULL,                          //atributos del thread
                               hornear,             //funcion a ejecutar
                                pthread_data); 

     rc = pthread_create(&p5,                           //identificador unico
                           NULL,                          //atributos del thread
                               agregarCarne,             //funcion a ejecutar
                                pthread_data); 

      rc = pthread_create(&p6,                           //identificador unico
                           NULL,                          //atributos del thread
                               empanar,             //funcion a ejecutar
                                pthread_data); 

       rc = pthread_create(&p7,                           //identificador unico
                           NULL,                          //atributos del thread
                               cocinarMilanesa,             //funcion a ejecutar
                                pthread_data); 
    rc = pthread_create(&p8,                           //identificador unico
                            NULL,                          //atributos del thread
                                cortarCebollaYdemas,             //funcion a ejecutar
                                pthread_data);                     //parametros de la funcion a ejecutar, pasado por referencia

       rc = pthread_create(&p9,                           //identificador unico
                            NULL,                          //atributos del thread
                                armarSanguche,             //funcion a ejecutar
                                pthread_data);  

	//crear demas hilos aqui
	
	
	//join de todos los hilos
	pthread_join (p1,NULL);
	pthread_join (p2,NULL);
	pthread_join (p3,NULL);
    pthread_join (p4,NULL);
    pthread_join (p5,NULL);
    pthread_join (p6,NULL);
    pthread_join (p7,NULL);
    pthread_join (p8,NULL);
    pthread_join (p9,NULL);
	//crear join de demas hilos


	//valido que el hilo se alla creado bien 
    if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     }

	 
	//destruccion de los semaforos 
	sem_destroy(&sem_mezclar);
	sem_destroy(&sem_salar);
    sem_destroy(&sem_agregarCarne);
    sem_destroy(&sem_empanar);
    sem_destroy(&sem_cocinarMilanesa);
    sem_destroy(&sem_armarSanguche1);
    sem_destroy(&sem_armarSanguche2);
    sem_destroy(&sem_armarSanguche3);
	//destruir demas semaforos 
	
	//salida del hilo
	 pthread_exit(NULL);
}


int main ()
{
	
	sem_init(&s_Horno,0,2);
	
	
	//creo los nombres de los equipos 
	int rc;
	int *equipoNombre1 =malloc(sizeof(*equipoNombre1));
	int *equipoNombre2 =malloc(sizeof(*equipoNombre2));
   int *equipoNombre3 =malloc(sizeof(*equipoNombre3));
    int *equipoNombre4 =malloc(sizeof(*equipoNombre4));
//faltan equipos
  
	*equipoNombre1 = 1;
	*equipoNombre2 = 2;
    *equipoNombre3 = 3;
    *equipoNombre4 = 4;

	//creo las variables los hilos de los equipos
	pthread_t equipo1; 
	pthread_t equipo2;
    pthread_t equipo3;
    pthread_t equipo4;
//faltan hilos
  
	//inicializo los hilos de los equipos
    rc = pthread_create(&equipo1,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre1); 

    rc = pthread_create(&equipo2,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre2);

     /**/
     rc = pthread_create(&equipo3,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre3);
   
     rc = pthread_create(&equipo4,                           //identificador unico
                            NULL,                          //atributos del thread
                                ejecutarReceta,             //funcion a ejecutar
                                equipoNombre4);
  //faltn inicializaciones


   if (rc){
       printf("Error:unable to create thread, %d \n", rc);
       exit(-1);
     } 

	//join de todos los hilos
	pthread_join (equipo1,NULL);
	pthread_join (equipo2,NULL);
    pthread_join (equipo3,NULL);
    pthread_join (equipo4,NULL);
//.. faltan joins

    sem_destroy(&s_Horno);
    pthread_exit(NULL);
}


//Para compilar:   gcc subwayArgento.c -o ejecutable -lpthread
//Para ejecutar:   ./ejecutable
