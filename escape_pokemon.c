#include "src/sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOMBRE 20
#define MAX_VERBO  20

int main(int argc, char *argv[])
{
	if (argc != 3)
		return -1;
	
	sala_t *sala = sala_crear_desde_archivos(argv[1], argv[2]);
	
	if (sala == NULL) {
		printf("Error al crear la sala de escape\n");
		return -1;
	}

	int cantidad_objetos = 0;
	

	char **string = sala_obtener_nombre_objetos(sala, &cantidad_objetos);

	printf("Objetos...\n");

	for (int i = 0; i < cantidad_objetos; i++){
		printf("%i: %s\n", i, string[i]);
	}
	free(string);

	printf("\nInteracciones...\n");

	int cantidad_interacciones = 4;

	char verbo[cantidad_interacciones][MAX_VERBO];
	strcpy(verbo[0], "examinar");
	strcpy(verbo[1], "abrir");
	strcpy(verbo[2], "usar");
	strcpy(verbo[3], "quemar");

	char primer_objeto[cantidad_interacciones][MAX_NOMBRE];
	strcpy(primer_objeto[0], "habitacion");
	strcpy(primer_objeto[1], "pokebola");
	strcpy(primer_objeto[2], "llave");
	strcpy(primer_objeto[3], "mesa");

	char segundo_objeto[cantidad_interacciones][MAX_NOMBRE];
	strcpy(segundo_objeto[0], "");
	strcpy(segundo_objeto[1], "");
	strcpy(segundo_objeto[2], "cajon");
	strcpy(segundo_objeto[3], "");
	
	bool es_valida;
	char *booleano;
	for (int i = 0; i < cantidad_interacciones; i++){
		es_valida = sala_es_interaccion_valida(sala, verbo[i], primer_objeto[i], segundo_objeto[i]);
		if(es_valida  == true){
			booleano = "Válida";
		}else{
			booleano = "Inválido";
		}
		printf("%s %s %s = %s\n", verbo[i], primer_objeto[i], segundo_objeto[i], booleano);
	}

	sala_destruir(sala);

	return 0;
}

//gcc ./escape_pokemon.c ./src/*.c ./src/*.h -o pokemon -std=c99 -Wall -Wconversion -Werror -lm
//valgrind ./pokemon ./ejemplo/objetos.txt ./ejemplo/interacciones.txt 