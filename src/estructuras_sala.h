#ifndef ESTRUCTURAS_SALA_H_
#define ESTRUCTURAS_SALA_H_

#include <stdbool.h>
#include "sala.h"
#include "lista.h"
#include "hash.h"

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

#endif // ESTRUCTURAS_SALA_H_