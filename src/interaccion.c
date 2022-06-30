#include "estructuras.h"
#include "interaccion.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define LECTURA_INTERACCIONES "%[^;];%[^;];%[^;];%c:%[^:]:%[^\n]\n"

enum tipo_accion definir_tipo_de_accion(char letra_accion){
	
	enum tipo_accion auxiliar;
	
	switch (letra_accion){
		case 'd':
			auxiliar = DESCUBRIR_OBJETO; 
			break;
	
		case 'r':
			auxiliar = REEMPLAZAR_OBJETO;
			break;
		
		case 'e':
			auxiliar = ELIMINAR_OBJETO;
			break;

		case 'm':
			auxiliar = MOSTRAR_MENSAJE;
			break;
		
		default:
			auxiliar = ACCION_INVALIDA;
			break;
	}
	return auxiliar;
}


struct interaccion *interaccion_crear_desde_string(const char *string)
{
	if (string == NULL)
		return NULL;

	struct interaccion *interaccion = malloc(sizeof(struct interaccion));
	if (interaccion == NULL)
		return NULL;
	
	char letra_accion;
	char objeto_2_aux[MAX_NOMBRE];
	char accion_objeto[MAX_NOMBRE];
	
	int elementos_leidos = sscanf(string, LECTURA_INTERACCIONES, interaccion->objeto, interaccion->verbo, objeto_2_aux, &letra_accion, accion_objeto, interaccion->accion.mensaje);

	if (elementos_leidos != 6){
		free(interaccion);
		return NULL;
	}
	
	if (strcmp(objeto_2_aux, "_") == 0)
		strcpy(objeto_2_aux, "");

	strcpy(interaccion->objeto_parametro, objeto_2_aux);

	if (strcmp(accion_objeto, "_") == 0)
		strcpy(accion_objeto, "");

	strcpy(interaccion->accion.objeto, accion_objeto);
	
	interaccion->accion.tipo = definir_tipo_de_accion(letra_accion);

	return interaccion;
}
