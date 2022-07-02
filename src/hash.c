#include "hash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACIDAD_MINIMA 3
#define FIN_DE_STRING '\0'
#define PORCENTAJE_DE_OCUPACION 75


typedef struct nodo {
	void *elemento;
	char *clave;
	struct nodo *siguiente;
} nodo_t;

struct hash {
	size_t capacidad;
	size_t ocupados;
	nodo_t **tabla;
};

hash_t *hash_crear(size_t capacidad)
{
	hash_t *hash = calloc(1, sizeof(hash_t));
	if (!hash)
		return NULL;

	if (capacidad < CAPACIDAD_MINIMA)
		capacidad = CAPACIDAD_MINIMA;

	nodo_t **tabla = calloc(capacidad, sizeof(nodo_t *));
	if (!tabla){
		hash_destruir(hash);
		return NULL;
	}

	hash->capacidad = capacidad;
	hash->tabla = tabla;
	
	return hash;
}

size_t funcion_hash(const char *clave)
{
	int suma = 0;
	
	for (int i = 0; clave[i] != FIN_DE_STRING; i++)
		suma += clave[i]*3;
		
	return (size_t)suma;
}

char *duplicar_clave(const char *s)
{
	if (!s)
		return NULL;

	char *p = malloc(strlen(s) + 1);
	if (!p)
		return NULL;

	strcpy(p, s);
	return p;
}

nodo_t *nodo_crear(const char *clave, void *elemento)
{
	if (!clave)
		return NULL;

	nodo_t *nodo = malloc(sizeof(nodo_t));
	if (!nodo)
		return NULL;
	
	char *duplicada = duplicar_clave(clave);
	nodo->clave = duplicada;
	nodo->elemento = elemento;
	nodo->siguiente = NULL;

	return nodo;
}

nodo_t *nodo_insertar(nodo_t *nodo, size_t *ocupados, void **anterior, const char *clave, void *elemento)
{
	if (!clave)
		return NULL;

	if (!nodo){
		nodo_t *nodo = nodo_crear(clave, elemento);
		if (!nodo)
			return NULL;
		if (anterior)
			*anterior = NULL;
		(*ocupados)++;
		return nodo;
	}

	nodo_t *nodo_actual = nodo;
	while (strcmp(nodo_actual->clave, clave) != 0 && nodo_actual->siguiente)
		nodo_actual = nodo_actual->siguiente;

	if (strcmp(nodo_actual->clave, clave) == 0){
		if (anterior)
			*anterior = nodo_actual->elemento;
		nodo_actual->elemento = elemento;	
	} else if (!nodo_actual->siguiente){
		nodo_t *nodo_nuevo = nodo_crear(clave, elemento);
		if (!nodo_nuevo)
			return NULL;
		nodo_actual->siguiente = nodo_nuevo;
		(*ocupados)++;
	}

	return nodo;
}

void swap_hashes(hash_t *hash1, hash_t *hash2)
{
	hash_t auxiliar;
	auxiliar = *hash1;
	*hash1 = *hash2;
	*hash2 = auxiliar;
}

bool necesita_rehash(hash_t *hash)
{
	int factor_de_carga = (int)((hash->ocupados*100)/(hash->capacidad));

	return (factor_de_carga >= PORCENTAJE_DE_OCUPACION);
}

hash_t *rehashear(hash_t *hash_original)
{
	if (!hash_original)
		return NULL;

	hash_t *hash_nuevo = hash_crear(hash_original->capacidad*2);
	if (!hash_nuevo)
		return NULL;

	nodo_t *nodo_actual;
	for (int i = 0; i < hash_original->capacidad; i++){
		nodo_actual = hash_original->tabla[i];
		while (nodo_actual){
			hash_nuevo = hash_insertar(hash_nuevo, nodo_actual->clave, nodo_actual->elemento, NULL);
			nodo_actual = nodo_actual->siguiente;	
		}
	}

	swap_hashes(hash_original, hash_nuevo);
	
	if (hash_original->ocupados == hash_nuevo->ocupados){
		hash_destruir(hash_nuevo);
		return hash_original;
	}

	return NULL;
}

hash_t *hash_insertar(hash_t *hash, const char *clave, void *elemento, void **anterior)
{
	if (!hash || !clave)
		return NULL;

	if (necesita_rehash(hash)){
		if (!rehashear(hash))
			return NULL;
	}

	size_t posicion = funcion_hash(clave) % hash->capacidad;

	nodo_t *cadena = nodo_insertar(hash->tabla[posicion], &hash->ocupados, anterior, clave, elemento);
	if (!cadena)
		return NULL;

	hash->tabla[posicion] = cadena;

	if (hash_contiene(hash, clave))
		return hash;
	
	return NULL;
}

nodo_t *nodo_quitar(nodo_t *nodo, const char *clave, void **elemento)
{
	nodo_t *nodo_actual = nodo;
	nodo_t *nodo_anterior = NULL;

	while (strcmp(nodo_actual->clave, clave) != 0 && nodo_actual){
		nodo_anterior = nodo_actual->siguiente;
		nodo_actual = nodo_actual->siguiente;
	}
	
	*elemento = nodo_actual->elemento;

	if (nodo_anterior)
		nodo_anterior->siguiente = nodo_actual->siguiente;
	else 
		nodo = nodo_actual->siguiente;
	
	free(nodo_actual->clave);
	free(nodo_actual);

	return nodo;
}

void *hash_quitar(hash_t *hash, const char *clave)
{
	if (!hash || !clave || hash->ocupados == 0 || !hash_contiene(hash, clave))
		return NULL;
	
	size_t posicion = funcion_hash(clave) % hash->capacidad;
	
	nodo_t *nodo = hash->tabla[posicion];
	if (!nodo)
		return NULL;
	
	void *elemento = NULL;

	hash->tabla[posicion] = nodo_quitar(nodo, clave, &elemento);

	if (!hash_obtener(hash, clave)){
		hash->ocupados--;
		return elemento;
	}
			
	return NULL;
}

void *hash_obtener(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return NULL;

	size_t posicion = funcion_hash(clave) % hash->capacidad;
	nodo_t *nodo_actual = hash->tabla[posicion];
	void *elemento = NULL;
	while (nodo_actual){
		if (strcmp(clave, nodo_actual->clave) == 0)
			elemento = nodo_actual->elemento;
		nodo_actual = nodo_actual->siguiente;
	}

	return elemento;
}

bool hash_contiene(hash_t *hash, const char *clave)
{
	if (!hash || !clave)
		return false;
	
	size_t posicion = funcion_hash(clave) % hash->capacidad;
	
	nodo_t *nodo_actual = hash->tabla[posicion];
	
	while (nodo_actual){
		if (strcmp(clave, nodo_actual->clave) == 0)
			return true;
		nodo_actual = nodo_actual->siguiente;
	}
	return false;
}

size_t hash_cantidad(hash_t *hash)
{
	if (!hash)
		return 0;
	return hash->ocupados;
}

void hash_destruir(hash_t *hash)
{
	hash_destruir_todo(hash, NULL);	
}

void hash_destruir_todo(hash_t *hash, void (*destructor)(void *))
{
	if (!hash)
		return;
	
	nodo_t *nodo_actual;
	nodo_t *nodo_anterior = NULL;
	
	for (int i = 0; i < hash->capacidad; i++){
		nodo_actual = hash->tabla[i];
		
		while (nodo_actual){
			if (destructor)
				destructor(nodo_actual->elemento);
			nodo_anterior = nodo_actual;
			nodo_actual = nodo_actual->siguiente;
			free(nodo_anterior->clave);
			free(nodo_anterior);
		}
		free(nodo_actual);
	}
	
	free(hash->tabla);
	free(hash);	
}

size_t hash_con_cada_clave(hash_t *hash, bool (*f)(const char *clave, void *valor, void *aux), void *aux)
{
	if (!hash || !f)
		return 0;

	size_t recorridos = 0;
	nodo_t *nodo_actual;

	for (int i = 0; i < hash->capacidad; i++){
		nodo_actual = hash->tabla[i];
		while (nodo_actual){
			recorridos++;
			if (!f(nodo_actual->clave, nodo_actual->elemento, aux))
				return recorridos;
			nodo_actual = nodo_actual->siguiente;
		}
	}

	return recorridos;
}