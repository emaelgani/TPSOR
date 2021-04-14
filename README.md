# 💻Trabajo Práctico Threads y Semáforos 💻
### Alumno Emanuel Elgani Sauer Esteban
**Básicamente este proyecto se puede resumir en que 4 equipos compiten por terminar primero un sanguche!! 😮,  pero a su vez no cuentan con el equipamiento individual!!, por lo tanto se tendrá que controlar el acceso a tales recursos compartidos!! 😱**
## Comenzando 🤓
_Para iniciar este desafío, lo primero que me propuse a realizar es un pseudocódigo que me permita observar con distinto panorama el trabajo práctico, de esta manera tuve una gran facilidad para implementarlo, ya que al tener una idea concreta, la única dificultad que pude encontrar es la sitaxis de programación en C, por lo tanto tuve que investigar como escribir en un archivo, leer, etc._ 📝
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

![Muestra](https://user-images.githubusercontent.com/65500254/114777131-afa60a80-9d49-11eb-8f39-abb2d7181271.png)

### Siguiente paso
_Una vez que tuve el diagrama realizado correctamente con una idea concreta y bien definida de lo que debo hacer con respecto a la problemática, el siguiente paso fue abrir el archivo.c para analizar su sintaxis. Este paso no me costó mucho trabajo ya que básicamente el archivo contenía un ejemplo y a través de los comentarios me indicaba que tenía tareas tenía que realizar y cuales faltaban. Por lo tanto seguí la lógica que estaba planteada con respecto a la siguiente función:_
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

##### Pequeño problema encontrado ❓ ❔
_El primer problema que tuve es realizar la acción de levantar la receta desde un archivo.txt, estuve tratando de implementarla pero no podía separar la acción de los ingredientes, por lo cual mi idea fue implementar en el archivo receta.txt el siguiente algoritmo: acción|parámetro|parámetro|... , de esta manera 
