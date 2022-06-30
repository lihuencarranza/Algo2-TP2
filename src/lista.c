#include "lista.h"
#include <stddef.h>
#include <stdlib.h>

lista_t *lista_crear()
{
	return calloc(1,sizeof(lista_t));
}

lista_t *lista_insertar(lista_t *lista, void *elemento)
{
	if(!lista)
		return NULL;
	
	nodo_t *nodo = malloc(sizeof(nodo_t));

	if (!nodo)
		return NULL;

	nodo->elemento = elemento;
	nodo->siguiente = NULL;	
	
	if (!lista->nodo_inicio){
		lista->nodo_inicio = nodo;
	} else {
		lista->nodo_fin->siguiente = nodo;
	}
		
	lista->nodo_fin = nodo;

	lista->cantidad++;

	return lista;
}

lista_t *lista_insertar_en_posicion(lista_t *lista, void *elemento,
				    size_t posicion)
{	
	if (!lista) 
		return NULL;

	if (posicion >= lista->cantidad) 
		return lista_insertar(lista, elemento);
	
	nodo_t* a_agregar = calloc( 1, sizeof(nodo_t));
	if (!a_agregar)
		return NULL;

	a_agregar->elemento = elemento;

	if (!lista->nodo_inicio)
		lista->nodo_inicio = a_agregar;

	nodo_t* nodo_anterior = lista->nodo_inicio;

	if (posicion == 0){
		a_agregar->siguiente = lista->nodo_inicio;
		lista->nodo_inicio = a_agregar;
	} else {
		
		for (int i = 0; i < posicion-1; i++){
			nodo_anterior = nodo_anterior->siguiente;
		}

		a_agregar->siguiente = nodo_anterior->siguiente;
		nodo_anterior->siguiente =  a_agregar;
	}

	lista->cantidad++;

	return lista;
	
}

void *lista_quitar(lista_t *lista)
{	
	if (lista_vacia(lista))
		return NULL;

	void *elemento = lista->nodo_fin->elemento;

	if (lista->cantidad == 1){
		free(lista->nodo_fin);
		lista->nodo_inicio = lista->nodo_fin = NULL;
		lista->cantidad--;
		return elemento;
	}	
	
	nodo_t *aux = lista->nodo_inicio; 

	for(int i = 0; i < lista->cantidad-2; i++){
		aux = aux->siguiente;
	}
	
	lista->nodo_fin = aux;
	free(lista->nodo_fin->siguiente);
	aux->siguiente = NULL;

	lista->cantidad--;
	return elemento;
}

void *lista_desencolar(lista_t* lista){
	
	if (lista_vacia(lista))
		return NULL;
	
	nodo_t* nodo_aux = lista->nodo_inicio;
	void *elemento = lista->nodo_inicio->elemento;

	lista->nodo_inicio = nodo_aux->siguiente;
	free(nodo_aux);
	lista->cantidad--;

	return elemento;
}

void *lista_quitar_de_posicion(lista_t *lista, size_t posicion)
{
	if (lista_vacia(lista))
		return NULL;

	if (posicion >= lista->cantidad-1)
		return lista_quitar(lista);

	if (posicion == 0 && lista->cantidad > 1)
		return lista_desencolar(lista);

	nodo_t *aux = lista->nodo_inicio; 
	void *elemento = aux->elemento;
	
	if (posicion == 0){
		lista->nodo_inicio = lista->nodo_inicio->siguiente;
		free(aux);
	} else{

		for(int i = 0; i < posicion-1; i++)
			aux = aux->siguiente;

		nodo_t *a_eliminar = aux->siguiente;
		elemento = a_eliminar->elemento;
		aux->siguiente = aux->siguiente->siguiente;	
		free(a_eliminar);
		
	}
	
	lista->cantidad--;

	return elemento;
}

void *lista_elemento_en_posicion(lista_t *lista, size_t posicion)
{
	if (lista_vacia(lista) || (posicion >= lista->cantidad)) 
		return NULL;

	nodo_t* aux = lista->nodo_inicio;
	for(int i = 0; i < posicion; i++)
		aux = aux->siguiente;
	return aux->elemento;
	
}

void *lista_buscar_elemento(lista_t *lista, int (*comparador)(void *, void *),
			    void *contexto)
{
	if (lista_vacia(lista) || !comparador)
		return NULL;

	nodo_t *aux = lista->nodo_inicio;

	for (int i = 0; i < lista->cantidad; i++){
		if (comparador(aux->elemento, contexto) == 0)
			return aux->elemento;
		aux = aux->siguiente;
	}
	return NULL;
}

void *lista_primero(lista_t *lista)
{
	if (lista_vacia(lista))
		return NULL;
	return lista->nodo_inicio->elemento;
}

void *lista_ultimo(lista_t *lista)
{
	if (lista_vacia(lista))
		return NULL;

	return lista->nodo_fin->elemento;
}

bool lista_vacia(lista_t *lista)
{
	return (!lista || lista->cantidad == 0);
}

size_t lista_tamanio(lista_t *lista)
{
	if (!lista)
		return 0;
	return lista->cantidad;
}

void lista_destruir(lista_t *lista)
{	
	lista_destruir_todo(lista, NULL);
}

void lista_destruir_todo(lista_t *lista, void (*funcion)(void *))
{
	if (!lista)
		return;
		
	while(lista->nodo_inicio){
		nodo_t *aux = lista->nodo_inicio;
		lista->nodo_inicio = aux->siguiente;
		if (funcion != NULL)
			funcion(aux->elemento);
		free(aux);
	}

	free(lista);
}

lista_iterador_t *lista_iterador_crear(lista_t *lista)
{
	if (!lista) 
		return NULL;

	lista_iterador_t* iterador = calloc( 1, sizeof(lista_iterador_t));
	
	if (!iterador) 
		return NULL;
	
	iterador->lista = lista;

	if (!lista_vacia(lista))
		iterador->corriente = lista->nodo_inicio;
	
	return iterador;
}

bool lista_iterador_tiene_siguiente(lista_iterador_t *iterador)
{
	if (!iterador)
		return false;

	return (iterador->corriente != NULL);
}

bool lista_iterador_avanzar(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->corriente)
		return false;

	iterador->corriente = iterador->corriente->siguiente;
	return iterador->corriente;
}

void *lista_iterador_elemento_actual(lista_iterador_t *iterador)
{
	if (!iterador || !iterador->corriente)
		return NULL;

	return iterador->corriente->elemento;
}

void lista_iterador_destruir(lista_iterador_t *iterador)
{
	free(iterador);
}


/**
 * Iterador interno. Recorre la lista e invoca la funcion con cada elemento de
 * la misma como primer parámetro. Dicha función puede devolver true si se deben
 * seguir recorriendo elementos o false si se debe dejar de iterar elementos.
 *
 * El puntero contexto se pasa como segundo argumento a la función del usuario.
 *
 * La función devuelve la cantidad de elementos iterados o 0 en caso de error
 * (errores de memoria, función o lista NULL, etc).
 *
 */
size_t lista_con_cada_elemento(lista_t *lista, bool (*funcion)(void *, void *),
			       void *contexto)
{	
	if (!lista || !funcion)
		return 0;

	size_t contador = 0;

	bool continuar = true;

	nodo_t* actual = lista->nodo_inicio;

	while (contador < lista->cantidad && continuar){
		continuar = funcion(actual->elemento, contexto);
		actual = actual->siguiente;
		contador++;
	}

	return contador;
	
}