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
#define EXITO 0

typedef struct jugador{
	bool escape_exitoso;
	hash_t *escenario;
	hash_t *inventario;
}jugador_t;

struct sala{
	hash_t *objetos;
	lista_t *interacciones;
	jugador_t* jugador;
};

struct vector_de_elementos{
	void **vec;
	size_t tamanio;
	size_t indice;
};


int comparador(void *elemento, void *contexto)
{
	struct objeto *objeto = (struct objeto *)elemento;
	return strcmp(objeto->nombre, (const char *)contexto);
}

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
	int recorrido = 0;
	
	while (elemento_leido == 1){

		objeto_auxiliar = objeto_crear_desde_string(string_objeto);
		if (objeto_auxiliar == NULL)
			return ERROR;

		if (!hash_insertar(sala->objetos, objeto_auxiliar->nombre, objeto_auxiliar, NULL)){
			free(objeto_auxiliar);
			return ERROR;
		}
		if (recorrido == 0){
			if (!hash_insertar(sala->jugador->escenario, objeto_auxiliar->nombre, objeto_auxiliar, NULL)){
				free(objeto_auxiliar);
				return ERROR;
			}
			recorrido++;
		}

		(*tamanio_objetos)++;

		elemento_leido = fscanf(arch_objetos, "%[^\n]\n", string_objeto);	
	}

	return EXITO;
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

	sala->objetos = hash_crear(14); //ver tamaño
	sala->interacciones = lista_crear();
	sala->jugador = malloc(sizeof(jugador_t));
	sala->jugador->escenario = hash_crear(14);
	sala->jugador->inventario = hash_crear(14);
	sala->jugador->escape_exitoso = false;

	if (!sala->objetos || !sala->interacciones || !sala->jugador || !sala->jugador->escenario || !sala->jugador->inventario){
		cerrar_archivos(arch_objetos, arch_interacciones);
		return NULL;
	}
		
	
	int tamanio_objetos = 0;
	
	if (crear_objetos(sala, &tamanio_objetos, arch_objetos) == ERROR){
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

bool agregar_clave_a_vector(const char *clave, void *valor, void *aux)
{
	struct vector_de_elementos *vector = aux;

	vector->vec[vector->indice] = (char*)clave;
	vector->indice++;

	return true;
}

char **obtener_nombres_objetos(hash_t *hash, int *cantidad)
{
	
	size_t cantidad_objetos = hash_cantidad(hash);
	char **string = malloc(hash_cantidad(hash) * sizeof(char*));

	if (string == NULL){
		if (cantidad != NULL)
			*cantidad = ERROR;	
		return NULL;
	}

	struct vector_de_elementos objetos;
	objetos.vec = (void*)string;
	objetos.tamanio = cantidad_objetos;
	objetos.indice = 0;

	hash_con_cada_clave(hash, agregar_clave_a_vector, &objetos);

	if (cantidad != NULL)
		*cantidad = (int)cantidad_objetos;
		
	return string;
}

/*
 * Devuelve un vector dinámico reservado con malloc que contiene los nombres de
 * todos los objetos existentes en la sala de escape.
 *
 * En la variable cantidad (si no es nula) se guarda el tamanio del vector de
 * nombres.
 *
 * El vector devuelto debe ser liberado con free.
 *
 * En caso de error devuelve NULL y pone cantidad en -1.
 */
char **sala_obtener_nombre_objetos(sala_t *sala, int *cantidad)
{	
	if (sala == NULL){
		if(cantidad != NULL)
			*cantidad = ERROR;	
		return NULL;
	}
	
	return obtener_nombres_objetos(sala->objetos, cantidad);
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
	if (sala == NULL){
		if(cantidad != NULL)
			*cantidad = ERROR;	
		return NULL;
	}
	
	return obtener_nombres_objetos(sala->jugador->escenario, cantidad);	
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
	if (sala == NULL){
		if(cantidad != NULL)
			*cantidad = ERROR;	
		return NULL;
	}

	return obtener_nombres_objetos(sala->jugador->inventario, cantidad);
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
	if (!sala || !nombre_objeto)
		return false;

	struct objeto *objeto = hash_obtener(sala->jugador->inventario, nombre_objeto);
	if (objeto){
		printf("Este objeto ya es encuentra en el inventario.\n");
		return false;
	}

	objeto = hash_obtener(sala->jugador->escenario, nombre_objeto);


	if (!objeto || !objeto->es_asible)
		return false;

	if (objeto && objeto->es_asible){
		hash_t *hash = hash_insertar(sala->jugador->inventario, nombre_objeto, objeto, NULL);
		if (hash){
			sala->jugador->inventario = hash;
			return true;
		}

	}
	return false;
	
}

/*
 * Obtiene la descripción de un objeto conocido o en posesión del usuario.
 *
 * Devuelve NULL en caso de error.
 */
char* sala_describir_objeto(sala_t* sala, const char *nombre_objeto)
{
	if (!sala || !nombre_objeto)
		return NULL;

	struct objeto* auxiliar = hash_obtener(sala->jugador->escenario, nombre_objeto);
	if (!auxiliar){
		auxiliar = hash_obtener(sala->jugador->inventario, nombre_objeto);

		if (!auxiliar)
			return NULL;
	}
		
	
	return auxiliar->descripcion;
}

/*
 * 
 * Elimina el objeto solicitado del inventario, del escenario 
 *
 */
bool objeto_eliminar(sala_t *sala, const char *objeto)
{
	if (hash_contiene(sala->jugador->inventario, objeto))
		hash_quitar(sala->jugador->inventario, objeto);
	if (hash_contiene(sala->jugador->escenario, objeto))
		hash_quitar(sala->jugador->escenario, objeto);
	if (hash_contiene(sala->objetos, objeto))
		free(hash_quitar(sala->objetos, objeto));
	

	if (hash_contiene(sala->jugador->inventario, objeto) || hash_contiene(sala->jugador->escenario, objeto) || hash_contiene(sala->objetos, objeto))
		return false;

	return true;
}

bool objeto_conocer(sala_t *sala, const char *nombre_objeto)
{
	if (hash_contiene(sala->jugador->inventario, nombre_objeto) || hash_contiene(sala->jugador->escenario, nombre_objeto))
		return false;
	
	if (hash_contiene(sala->objetos, nombre_objeto)){
		struct objeto *objeto = hash_obtener(sala->objetos, nombre_objeto);
		hash_insertar(sala->jugador->escenario, nombre_objeto, objeto, NULL);
		return true;
	}

	return false;
}

void ejecutar_interaccion(sala_t *sala, struct interaccion *interaccion, void (*mostrar_mensaje)(const char *mensaje, enum tipo_accion accion, void *aux), void *aux, int *ejecutadas)
{
	
	switch (interaccion->accion.tipo){
		
		case DESCUBRIR_OBJETO:
			if (objeto_conocer(sala, interaccion->accion.objeto))
				(*ejecutadas)++;
			else 
				return;
			
			break;

		case ELIMINAR_OBJETO:
			if (objeto_eliminar(sala, interaccion->accion.objeto))
				(*ejecutadas)++;
			else 
				return;
			
			break;
			
		case REEMPLAZAR_OBJETO:
			if (objeto_eliminar(sala, interaccion->objeto_parametro) && objeto_conocer(sala, interaccion->accion.objeto))
				(*ejecutadas)++;
			else 
				return;
				
			break;

		case MOSTRAR_MENSAJE:
			(*ejecutadas)++;			
			break;

		case ESCAPAR:
			sala->jugador->escape_exitoso = true;
			(*ejecutadas)++;
			break;
		case ACCION_INVALIDA:
			return;
			break;		
	}

	mostrar_mensaje(interaccion->accion.mensaje, interaccion->accion.tipo, aux);

}

bool interaccion_coincide(struct interaccion *interaccion, const char *verbo, const char *objeto1, const char *objeto2)
{
	return (strcmp(interaccion->objeto, objeto1) == 0 && strcmp(interaccion->verbo ,verbo) == 0 && strcmp(interaccion->objeto_parametro ,objeto2)== 0 );
}

bool objeto_es_conocido_o_vacio(sala_t *sala, const char *objeto)
{
	if (*objeto == 0)
		return true;
	
	if (hash_contiene(sala->jugador->inventario, objeto) || hash_contiene(sala->jugador->escenario, objeto))
		return true;
	
	return false;
}

int sala_ejecutar_interaccion(sala_t *sala, const char *verbo, const char *objeto1, const char *objeto2, void (*mostrar_mensaje)(const char *mensaje, enum tipo_accion accion, void *aux), void *aux)
{
	if (!sala || !verbo || !objeto1 || !aux)
		return ERROR;

	int ejecutadas = 0;

	if (!sala_es_interaccion_valida(sala, verbo, objeto1, objeto2))
		return ejecutadas;
		
	if (!objeto_es_conocido_o_vacio(sala, objeto1) || !objeto_es_conocido_o_vacio(sala, objeto2))
		return ejecutadas;

	lista_iterador_t *it = lista_iterador_crear(sala->interacciones);

	while(lista_iterador_tiene_siguiente(it)){

		struct interaccion *interaccion = lista_iterador_elemento_actual(it);
		
		if (interaccion_coincide(interaccion, verbo, objeto1, objeto2))
			ejecutar_interaccion(sala, interaccion, mostrar_mensaje, aux, &ejecutadas);			

		lista_iterador_avanzar(it);	
		
	}

	free(it);
	
	return ejecutadas;
}

int verificar_interaccion(void *actual, void *contexto)
{
	struct interaccion *aux1 = actual;
	struct interaccion *aux2 = contexto;

	if (strcmp(aux1->verbo, aux2->verbo) == 0 && strcmp(aux1->objeto, aux2->objeto) == 0 && strcmp(aux1->objeto_parametro, aux2->objeto_parametro) == 0)
		return EXITO;
	
	return ERROR;
}

bool sala_es_interaccion_valida(sala_t *sala, const char *verbo, const char *objeto1, const char *objeto2)
{
	if (sala == NULL || sala->interacciones == NULL || verbo == NULL || objeto1 == NULL || objeto2 == NULL)
		return false;
	
	struct interaccion buscado;
	
	strcpy(buscado.objeto, objeto1);
	strcpy(buscado.verbo, verbo);
	strcpy(buscado.objeto_parametro, objeto2);

	if (lista_buscar_elemento(sala->interacciones, verificar_interaccion, &buscado))
		return true;
	
	
	return false;
}

bool sala_escape_exitoso(sala_t *sala)
{
	if (!sala)
		return false;
	return sala->jugador->escape_exitoso;
}

void sala_destruir(sala_t *sala)
{

	hash_destruir(sala->jugador->escenario);
	hash_destruir(sala->jugador->inventario);
	lista_destruir_todo(sala->interacciones, free);
	hash_destruir_todo(sala->objetos, free);

	free(sala->jugador);
		
	free(sala);
}