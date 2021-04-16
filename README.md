# 💻Trabajo Práctico Threads y Semáforos 💻
### Alumno Emanuel Elgani Sauer Esteban
**Básicamente este proyecto se puede resumir en que 4 equipos compiten por terminar primero un sanguche!! ,  pero a su vez no cuentan con el equipamiento individual!!, por lo tanto se tendrá que controlar el acceso a tales recursos compartidos!!**
## Comenzando 🤓
_Para iniciar este desafío, lo primero que me propuse a realizar es un pseudocódigo que me permita observar con distinto panorama el trabajo práctico, de esta manera tuve una gran facilidad para implementarlo, ya que al tener una idea concreta, la única dificultad que pude encontrar es la sintaxis de programación en C, por lo tanto tuve que investigar como llevar a cabo las funciones de escribir en un archivo, leer, etc._ 📝
Para poder realizar el psudocódigo el primer paso fue leer bien el problema y entender que acciones existen y a cuales habilitan. 📖

### Acciones
- Cortar 2 dientes de ajo y un puñado de perejil
  - Mezclar el ajo y perejil con 1 huevo
    - Poner sal a la mezcla
      - Agregar carne a la mezcla
        - Empanar milanesa
          - Cocinar Milanesa
            - Habilita una parte del armado del sanguche    
- Hornear panes
  - Habilita una parte de el armado del sanguche
- Cortar lechuga fresca, tomate, cebolla morada y pepino
  - Habilita una parte del armado del sanguche   

*A través de la lista anidada se puede determinar _que acciónes son las que habilitan a otras y cuales no dependen de ninguna_*

### Pseudocódigo ✏️ ✏️
Mediante el esquema planteado pude crear el siguiente pseudocódigo:
#### Semáforos globales:
- sem_Horno = 2
- mutexSalero = 1
- mutexSarten = 1

Como se observa he decidido utilizar un semáforo global para el horno, el cual es inicializado en 2 ya que como máximo pueden acceder 2 equipos. A su vez utilizo un mutex para el salero y otro para el sarten ya que solo pueden usarlo 1 equipo a la vez.
#### Semáforos propios del equipo:
- sem_mezclar = 0
- sem_salar  = 0
- sem_agregarCarne  = 0
- sem_empanar  = 0
- sem_cocinarMilanesa  = 0
- sem_armarSanguche1  = 0
- sem_armarSanguche2  = 0
- sem_armarSanguche3  = 0

#### Imagen que muestra como interactuan las funciones mediante los semáforos.

![Muestra](https://user-images.githubusercontent.com/65500254/114901740-79b96280-9deb-11eb-8301-182cf0a56a80.png)


### Siguiente paso
_Una vez que tuve el diagrama realizado correctamente con una idea concreta y bien definida de lo que debo hacer con respecto a la problemática, el siguiente paso fue abrir el archivo.c para analizar su sintaxis. Este paso no me costó mucho trabajo ya que básicamente el archivo contenía un ejemplo y a través de los comentarios me indicaba que tareas tenía que realizar y cuales faltaban. Por lo tanto seguí la lógica que estaba planteada con respecto a la siguiente función:_
```
//funcion para tomar de ejemplo
void* cortar(void *data) {
	//creo el nombre de la accion de la funcion 
	char *accion = "cortar";
	//creo el puntero para pasarle la referencia de memoria (data) del struct pasado por parametro (la cual es un puntero). 
	struct parametro *mydata = data;
	//llamo a la funcion imprimir le paso el struct y la accion de la funcion
	imprimirAccion(mydata,accion);
	//uso sleep para simular que que pasa tiempo
	usleep( 20000 );
	//doy la señal a la siguiente accion (cortar me habilita mezclar)
    sem_post(&mydata->semaforos_param.sem_mezclar);
	
    pthread_exit(NULL);
}

```
_Tal función define la estructura de todas las funciones del programa, por lo tanto el siguiente paso que realicé fue crear la funcion mezclar(), la cual habilita a poner sal y así sucesivamente._ **Este proceso lo lleve acabo sin ningún problema, ya que tenía la estructura en mente de lo que debía realizar**.

##### Pequeño problema encontrado 😕
_El primer problema que econtre fue realizar la acción de levantar la receta desde un archivo.txt, estuve tratando de implementarla pero no podía separar la acción de los ingredientes, por lo cual mi idea fue implementar en el archivo receta.txt la siguiente lógica: acción|ingrediente|ingrediente|... , por lo tanto el archivo txt me quedó de la siguiente manera:_ 
```
cortar|ajo|perejil
mezclar|ajo|perejil|huevo
salar mezcla|ajo|perejil|huevo
hornear pan|masa
agregar carne a la mezcla|ajo|perejil|huevo|carne
empanar milanesa|pan rallado
cocinar milanesa|aceite
cortar otros ingredientes|lechuga fresca|tomate|cebolla morada|pepino
armar sanguche|milanesa|lechuga fresca, tomate, cebolla morada, pepino|panes
```
_Tal archivo tiene como nombre receta.txt, el cual es levantado mediante la siguiente función_

```
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
			cont++;
	}
	fclose(f);
}

```
_A través de la función levantarReceta() se leen todas las lineas del archivo, y por cada línea en la primer interación (iteración 0) se determina que hace referencia a la acción, sino(si es mayor que 0) se hace referencia a los parámetros (ingredientes). De esta manera pude lograr levantar la receta desde un archivo .txt_

#### Segundo problema encontrado
_El segundo problema con el cual me encontré fue como realizar la acción para escribir en un archivo .txt los pasos que va realizando cada equipo. Para poder llevar a cabo este proceso declare una función llamada escribir(), la cual recibe como parámetro la cadena que se va a querer escribir en el archivo y la información de cada equipo, esta información la recibe para poder determinar que número de equipo se está ejecutando y en que tiempo se realizó la acción. De esta manera en el archivo de salida se obtendrá la siguiente secuencia:_
- Equpo 1 Acción 1 ¡Cortando Ajo y Perejil!
- Equpo 2 Acción 1 ¡Cortando Ajo y Perejil!
- Equpo 1 Acción 2 ¡Horneando Panes!

_Tal acción es llamada en el momento de la ejecución de cada paso de la receta. Del la mismo modo, la variable que controla el tiempo en el que se ejecuta cada acción es incrementada en 1, a su vez las funciónes que utilizan variables de recursos compartidos informarán en que momento de dejó de utilizar tal recurso_

_Se puede observar mediante la siguiente función:_
```
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
```
_De esta manera se puede observar que una vez que se realiza la acción de cocinarMilanesa, **esta obtiene el tiempo en el que se ejecuta la acción, luego escribe en el archivo de salida y por último incrementa el tiempo** A su vez realiza el proceso de escribir en la consola la información de lo que se está realizando **y luego de pasar el tiempo de espera se realiza el mismo procedimiento de obtener el tiempo, luego escribir en el archivo e incrementar el tiempo**._
_Este proceso lo realice de tal manera ya que se puede obtener una información detallada y no confusa de lo que sucede en cada instante de tiempo y a su vez detectar que acción ejecuta cada equipo, informando cuando se dejan de utilizar los recursos compartidos, los cuales son: Sarten, Horno (2 equipos a la vez) y el Salero._

#### Finalización 🏆
_Una vez que son ejecutadas las acciónes de cocinarMilanesa(), cortarCebollaYdemas(), armarSanguche(), estas habilitan a tres semáforos distintos, los cuales estan relacionados con la acción de armar el sanguche final(). La acción que arma el sanguche depende de los tres semáforos, una vez que se habilitan, se puede armar el sanguche y a su vez informar qué equipo ganó._

### Conclusión
_Si bien me parecio un trabajo bastante desafiante, por mi parte lo pude llevar a cabo de una manera eficiente ya que el primer paso que realicé fue armar el pseudocódigo, interpretar correctamente el problema y tener una idea concisa de lo que debia resolver. Luego fue interpretar el código que se ha proporcionado, lo cual no me resulto muy complicado ya que implementaba el primer paso, del cual yo debia seguir el orden. Una vez que implementé las demas acciones y corroboré de que todo funcionara correctamente, el siguiente paso fue determinar como levantar la receta (que secuencia utilizar para separar la acción de los ingredientes) y luego escribir los pasos en el archivo .txt. Tuve algunos inconvenientes con estas acciones ya que nunca he utilizado el lenguaje de programación en C._
_En fin, no me ha parecido muy conflictivo ya que interpreté el problema, realicé una sincronización de los hilos y semáforos de tal forma que pude concretar el deafío teniendo otro panorama general del problema._ 
