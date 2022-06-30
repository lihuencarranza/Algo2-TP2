#include "estructuras.h"
#include "sala.h"
#include "objeto.h"
#include "interaccion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lista.h"
#include "hash.h"

#define MAX_LINEA 1028
#define ERROR -1

struct sala{
	hash_t *objetos;
	lista_t *interacciones;
	bool escape_exitoso;
	hash_t *escenario;
	hash_t *inventario;
};

/*
 * Cierra los archivos recibidos
 */
void cerrar_archivos(FILE *arch_objetos, FILE *arch_interacciones)
{
	fclose(arch_objetos);
	fclose(arch_interacciones);
}

/*
 * Crea un string para guardar una linea del archivo de interacciones recibidos
 * 
 * Luego de leer, verifica que el elemento se haya leido y no esté vacío
 * 
 * Se creará una interaccion por cada linea bien leida del archivo
 * 
 * El caso de que un objeto no se pueda crear o el vector de punteros a punteros de interacciones
 * no pueda ampliarse, se devolverá error (-1)
 * 
 */
int crear_interacciones(sala_t *sala, int *tamanio_interacciones, FILE *arch_interacciones)
{
	char string_interaccion[MAX_LINEA];
	
	int elemento_leido = fscanf(arch_interacciones, "%[^\n]\n", string_interaccion);

	if (elemento_leido != 1)
		return ERROR;
	
	struct interaccion *interaccion_aux;

	while (elemento_leido == 1){

		interaccion_aux = interaccion_crear_desde_string(string_interaccion);
		if (interaccion_aux == NULL)
			return ERROR;			

			
		if(!lista_insertar(sala->interacciones, interaccion_aux)){
			free(interaccion_aux);
			return ERROR;
		}

		(*tamanio_interacciones)++;

		elemento_leido = fscanf(arch_interacciones, "%[^\n]\n", string_interaccion);
	}

	return 0;
}

/*
 * Crea un string para guardar una linea del archivo de objetos recibidos
 * 
 * Luego de leer, verifica que el elemento se haya leido y no esté vacío
 * 
 * Se creará un objeto por cada linea bien leida del archivo
 * 
 * El caso de que un objeto no se pueda crear o el vector de punteros a punteros de objetos
 * no pueda ampliarse, se devolverá error (-1)
 * 
 */
int crear_objetos(sala_t *sala, int *tamanio_objetos, FILE *arch_objetos)
{
	char string_objeto[MAX_LINEA];

	int elemento_leido = fscanf(arch_objetos, "%[^\n]\n", string_objeto);

	if (elemento_leido != 1)
		return ERROR;
		
	struct objeto *objeto_auxiliar;
	
	while (elemento_leido == 1){

		objeto_auxiliar = objeto_crear_desde_string(string_objeto);
		if (objeto_auxiliar == NULL)
			return ERROR;

		if(!hash_insertar(sala->objetos, objeto_auxiliar->nombre, objeto_auxiliar, NULL)){
			free(objeto_auxiliar);
			return ERROR;
		}

		(*tamanio_objetos)++;

		elemento_leido = fscanf(arch_objetos, "%[^\n]\n", string_objeto);	
	}

	return 0;
}

/* 
 * Verifica que los archivos recibidos hayan sido abiertos correctamente
 *
 * En el caso de que uno de los dos o ambos no se hayan podido abrir, devuelve -1.
 * 
 * Si ambos fueron abiertos correctamente devuelve 0
 * 
*/
int verificar_apertura_archivos(FILE *arch_objetos, FILE *arch_interacciones)
{
	if (arch_objetos == NULL && arch_interacciones == NULL)
		return ERROR;
	
	if (arch_objetos == NULL){
		fclose(arch_interacciones);
		return ERROR;
	}
	if (arch_interacciones == NULL){
		fclose(arch_objetos);
		return ERROR;
	}
	return 0;
}

sala_t *sala_crear_desde_archivos(const char *objetos, const char *interacciones)
{
	FILE *arch_objetos = fopen(objetos, "r");
	FILE *arch_interacciones = fopen(interacciones, "r");

	if (verificar_apertura_archivos(arch_objetos, arch_interacciones) == -1)
		return NULL;

	sala_t *sala = calloc(1, sizeof(sala_t));	
	if (sala == NULL){
		printf("No se reservó memoria para sala\n");
		cerrar_archivos(arch_objetos, arch_interacciones);
		return NULL;
	} 

	sala->objetos = hash_crear(10); //ver tamaño
	sala->interacciones = lista_crear();
	sala->escenario = hash_crear(10);
	sala->inventario = hash_crear(10);
	sala->escape_exitoso = false;

	if (!sala->objetos || !sala->interacciones || !sala->escenario || !sala->inventario){
		cerrar_archivos(arch_objetos, arch_interacciones);
		return NULL;
	}
		
	
	int tamanio_objetos = 0;
	
	if (crear_objetos(sala, &tamanio_objetos, arch_objetos) == -1){
		sala_destruir(sala);
		cerrar_archivos(arch_objetos, arch_interacciones);
		return NULL;
	}
		
	int tamanio_interacciones = 0;

	if (crear_interacciones(sala, &tamanio_interacciones, arch_interacciones) == -1){
		sala_destruir(sala);
		cerrar_archivos(arch_objetos, arch_interacciones);
		return NULL;
	}
		
	cerrar_archivos(arch_objetos, arch_interacciones);
	
	return sala;
}

struct vector_con_indice{
	void **vec;
	size_t tamanio;
	size_t indice;
};

bool agregar_clave_a_vector(const char *clave, void *valor, void *aux)
{
	struct vector_con_indice *mi_vector = aux;

	mi_vector->vec[mi_vector->indice] = (char*)clave;
	mi_vector->indice++;

	return true;
}

char **sala_obtener_nombre_objetos(sala_t *sala, int *cantidad)
{
	if (sala == NULL){
		if(cantidad != NULL)
			*cantidad = ERROR;	
		return NULL;
	}

	size_t cantidad_objetos = hash_cantidad(sala->objetos);
	char **string = malloc(hash_cantidad(sala->objetos) * sizeof(char*));

	if (string == NULL){
		if (cantidad != NULL)
			*cantidad = ERROR;	
		return NULL;
	}

	struct vector_con_indice mi_vector;
	mi_vector.vec = (void*)string;
	mi_vector.tamanio = cantidad_objetos;
	mi_vector.indice = 0;

	hash_con_cada_clave(sala->objetos, agregar_clave_a_vector, &mi_vector);

	if (cantidad != NULL)
		*cantidad = (int)cantidad_objetos;
		
	return string;

}





/*
 * Devuelve un vector dinámico reservado con malloc que contiene los nombres de
 * todos los objetos actualmente conocidos por el jugador en la sala de escape.
 * No incluye los objetos poseidos por el jugador.
 *
 * En la variable cantidad (si no es nula) se guarda el tamanio del vector de
 * nombres.
 *
 * El vector devuelto debe ser liberado con free.
 *
 * En caso de error devuelve NULL y pone cantidad en -1.
 */
char **sala_obtener_nombre_objetos_conocidos(sala_t *sala, int *cantidad)
{
	return NULL;
}

/*
 * Devuelve un vector dinámico reservado con malloc que contiene los nombres de
 * todos los objetos actualmente en posesión del jugador.
 *
 * En la variable cantidad (si no es nula) se guarda el tamanio del vector de
 * nombres.
 *
 * El vector devuelto debe ser liberado con free.
 *
 * En caso de error devuelve NULL y pone cantidad en -1.
 */
char **sala_obtener_nombre_objetos_poseidos(sala_t *sala, int *cantidad)
{
	return NULL;
}

/*
 * Hace que un objeto conocido y asible pase a estar en posesión del jugador.
 *
 * Devuelve true si pudo agarrar el objeto o false en caso de error (por ejemplo
 * el objeto no existe o existe pero no es asible o si dicho objeto ya está en
 * posesión del jugador).
 */
bool sala_agarrar_objeto(sala_t *sala, const char *nombre_objeto)
{
	return false;
}

/*
 * Obtiene la descripción de un objeto conocido o en posesión del usuario.
 *
 * Devuelve NULL en caso de error.
 */
char* sala_describir_objeto(sala_t* sala, const char *nombre_objeto)
{
	return NULL;
}

int ejecutar_interaccion(sala_t *sala, struct interaccion *interaccion, void (*mostrar_mensaje)(const char *mensaje, enum tipo_accion accion, void *aux), void *aux)
{
	switch (interaccion->accion.tipo){
		case ACCION_INVALIDA:
			break;
		case DESCUBRIR_OBJETO:
			/*if (conozco(interaccion->objeto) && conozco(interaccion->objeto_parametro)){
				hash_insertar(sala->escenario, interaccion->accion.objeto, objeto_de_la_sala, NULL); //PENSAR EL NULL
				mostrar_mensaje(interaccion->accion.mensaje, MOSTRAR_MENSAJE, aux);
				return 1;
			}*/
			break;
		case REEMPLAZAR_OBJETO:
			break;
		case ELIMINAR_OBJETO:
			break;
		case MOSTRAR_MENSAJE:
		//si puedo usar los objetos
			mostrar_mensaje(interaccion->accion.mensaje, MOSTRAR_MENSAJE, aux);
			break;
		case ESCAPAR:
			break;
				
		default:
			break;
	}



	return ERROR;
}

/*
 * Ejecuta una o mas interacciones en la sala de escape. Cuando una interacción
 * tenga un mensaje para mostrar por pantalla, se invocará la función
 * mostrar_mensaje (si no es NULL) con el mensaje a mostrar, el tipo de acción que representa el
 * mensaje y un puntero auxiliar del usuario.
 *
 * Devuelve la cantidad de interacciones que pudo ejecutar o 0 en caso de error.
 *
 */
int sala_ejecutar_interaccion(sala_t *sala, const char *verbo, const char *objeto1, const char *objeto2, void (*mostrar_mensaje)(const char *mensaje, enum tipo_accion accion, void *aux), void *aux)
{
	int ejecutadas = 0;	
	/*lista_iterador_t *it = lista_iterador_crear(sala->interacciones);

	for (;lista_iterador_tiene_siguiente(it); lista_iterador_avanzar(it)){
		struct interaccion *interaccion = lista_iterador_elemento_actual(it);
		
		if (interaccion_coincide(interaccion, verbo, objeto1, objeto2))
			ejecutadas += ejecutar_interaccion(sala, interaccion, mostrar_mensaje, aux);
		
	}*/
	
	return ejecutadas;
}






bool sala_es_interaccion_valida(sala_t *sala, const char *verbo, const char *objeto1, const char *objeto2)
{
	if (sala == NULL || sala->interacciones == NULL || verbo == NULL || objeto1 == NULL || objeto2 == NULL)
		return NULL;
	
	/*int contador = 0;

	for (int i = 0; i < sala->cantidad_interacciones; i++){
		if (strcmp(sala->interacciones[i]->verbo, verbo) == 0)
			contador++;

		if (strcmp(sala->interacciones[i]->objeto, objeto1) == 0)
			contador++;

		if (strcmp(sala->interacciones[i]->objeto_parametro, objeto2) == 0)
			contador++;

		if (contador == 3)
			return true;
		
		contador = 0;
	}*/

	return false;
}


/*
 * Devuelve true si se pudo escapar de la sala. False en caso contrario o si no existe la sala.
 */
bool sala_escape_exitoso(sala_t *sala)
{
	return false;
}



void sala_destruir(sala_t *sala)
{
	lista_destruir_todo(sala->interacciones, free);
	hash_destruir_todo(sala->objetos, free);
	hash_destruir(sala->escenario);
	hash_destruir(sala->inventario);
		
	free(sala);
}