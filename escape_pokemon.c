#include "src/sala.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NOMBRE 20
#define MAX_VERBO  20
#define MAX_INPUT 1024
#define ERROR -1


void imprimir_archivo(FILE *fptr)
{
	char c;
	c = (char)fgetc(fptr);
	while (c != EOF){
		printf("%c", c);
		c = (char)fgetc(fptr);
	}
}

void mostrar_mensaje(const char *mensaje, enum tipo_accion accion, void *aux)
{
	printf("%s\n", mensaje);
}

void comandos()
{
	printf("╔═════════════════════╗\n");
	printf("║    INTERACCIONES    ║\n");
	printf("╠═════════════════════╣\n");
	printf("║ agarrar             ║\n");
	printf("║ descibrir           ║\n");
	printf("║ examinar            ║\n");
	printf("║ abrir               ║\n");
	printf("║ ayuda               ║\n");
	printf("║ salir               ║\n");
	printf("╚═════════════════════╝\n");
}

void ayuda()
{
	printf("║ El objetivo de este juego es escapar de la sala. Para hacer  ║\n");
	printf("║ esto, deberás utilizar los objetos que estén en el escenario ║\n");
	printf("║ e inventario. Se te solicitará una interacción junto a un    ║\n");
	printf("║ objeto y un segundo objeto con el siguiente formato:         ║\n");	
	printf("║ interaccion primer_objeto segundo_objeto                     ║\n");	
	printf("║ En el caso de no usar un segundo objeto, dejar en blanco     ║\n");	
	printf("║ Adicionalmente están los comandos 'ayuda' para recordar los  ║\n");
	printf("║ comandos, y 'salir' para terminar la partida y salir del     ║\n");
	printf("║ juego.                                                       ║\n");
}

void instrucciones()
{
	printf("╔══════════════════════════════════════════════════════════════╗\n");
	printf("║                        PARTIDA NUEVA                         ║\n");
	printf("╠══════════════════════════════════════════════════════════════╣\n");
	ayuda();
	printf("║ Al iniciar, conocerás un solo objeto, te invito a            ║\n");
	printf("║ 'examinarlo' (guiño...guiño...).                             ║\n");
	printf("╚══════════════════════════════════════════════════════════════╝\n");
	comandos();
	printf("╔════════════════════╗\n");
	printf("║ ENTER PARA INICIAR ║\n");
	printf("╚════════════════════╝\n");
	
}

void estado_de_juego(sala_t *sala)
{
	int cantidad_conocidos = 0;
	int cantidad_inventario = 0;
	char **conocidos = sala_obtener_nombre_objetos_conocidos(sala, &cantidad_conocidos);
	char **inventario = sala_obtener_nombre_objetos_poseidos(sala, &cantidad_inventario);

	printf("╔═════════════════════╗\n");
	printf("║      CONOCIDOS      ║\n");
	printf("╚═════════════════════╝\n");

	for (int i = 0; i < cantidad_conocidos; i++){
		printf("    %s\n", conocidos[i]);
	}
	printf("\n");
	printf("╔═════════════════════╗\n");
	printf("║       POSEIDOS      ║\n");
	printf("╚═════════════════════╝\n");

	for (int i = 0; i < cantidad_inventario; i++){
		printf("    %s\n", inventario[i]);
	}
	printf("\n\n");

	free(conocidos);
	free(inventario);
	

}

void registrar_input(char *input, char *verbo, char *objeto1, char *objeto2)
{	
	printf("Ingrese una interacion:\n");
	fgets(input, MAX_INPUT, stdin);
	sscanf(input, "%s %s %s", verbo, objeto1, objeto2);
	for (int i = 0; verbo[i]; i++){
		verbo[i] = (char)tolower(verbo[i]);
	}
	for (int j = 0; objeto1[j]; j++){
		objeto1[j] = (char)tolower(objeto1[j]);
	}
	for (int k = 0; objeto1[k]; k++){
		objeto2[k] = (char)tolower(objeto2[k]);
	}
	
}

void ayuda_extra()
{
	printf("┌──────────────────────────────────────────────────────────────┐\n");
	printf("│                         AYUDA EXTRA                          │\n");
	printf("├──────────────────────────────────────────────────────────────┤\n");
	printf("│ Para poder realizar una acción debés ingresar la interacción │\n");
	printf("│ de la siguiente manera:                                      │\n");
	printf("│                verbo objeto objeto2                          │\n");
	printf("│ A tener en cuenta: se separa las palabras con espacios. El   │\n");
	printf("│ segundo objeto es opcional. Para utilizar dos objetos a la   │\n");
	printf("│ vez, el orden es (verbo) (objeto que vas a utilizar) (obje-  │\n");
	printf("│ to con el que vas a usar el objeto uno). Ejemplo:            │\n");
	printf("│                abrir llave puerta                            │\n");
	printf("└──────────────────────────────────────────────────────────────┘\n");
}

bool objeto_en_escenario(sala_t *sala, char objeto[MAX_NOMBRE])
{
	int cant_conocidos = 0;

	char **conocidos = sala_obtener_nombre_objetos_conocidos(sala, &cant_conocidos);
	
	for (int i = 0; i < cant_conocidos; i++){
		if (strcmp(conocidos[i], objeto) == 0){
			free(conocidos);
			return true;
		}
			
	}
	free(conocidos);
	return false;

}

bool objeto_en_inventario(sala_t *sala, char objeto[MAX_NOMBRE])
{
	int cant_inventario = 0;
	char **inventario = sala_obtener_nombre_objetos_poseidos(sala, &cant_inventario);

	for (int i = 0; i < cant_inventario; i++){
		if (strcmp(inventario[i], objeto) == 0){
			free(inventario);
			return true;
		}
			
	}
	free(inventario);
	return false;
}

int main(int argc, char *argv[])
{
	if (argc != 3)
		return ERROR;

	system("clear");
	FILE *fptr = fopen("pantalla_inicio.txt", "r");
	
	imprimir_archivo(fptr);
	fclose(fptr);
	
	char c;
	scanf("%c", &c);

	if ( 'q' == c || 'Q'== c)
		return 0;
	
	sala_t *sala = sala_crear_desde_archivos(argv[1], argv[2]);
	
	if (!sala){
		printf("Error al crear la sala de escape\n");
		return ERROR;
	}
	system("clear");

	instrucciones();
	scanf("%c", &c);


	system("clear");
	while (!sala_escape_exitoso(sala)){
		
		comandos();
		estado_de_juego(sala);

		char input[MAX_INPUT];
		char verbo[MAX_VERBO];
		strcpy(verbo, "");
		char objeto1[MAX_NOMBRE];
		strcpy(objeto1, "");
		char objeto2[MAX_NOMBRE];
		strcpy(objeto2, "");

		registrar_input(input, verbo, objeto1, objeto2);

		if (strcmp(verbo, "ayuda") == 0){
			printf("╔══════════════════════════════════════════════════════════════╗\n");
			ayuda();
			printf("╚══════════════════════════════════════════════════════════════╝\n");
			printf("Si seguis sin entender como utilizar los comandos ingresá 'ayuda-extra'.\n");
			registrar_input(input, verbo, objeto1, objeto2);
		
		} else if(strcmp(verbo, "ayuda-extra")==0){
			system("clear");
			ayuda_extra(); 
		}else if (strcmp(verbo, "salir") == 0 && strcmp(objeto1, "") == 0){
			system("clear");
			fptr = fopen("pantalla_salida.txt", "r");
			imprimir_archivo(fptr);
			fclose(fptr);
			sala_destruir(sala);
			return 0;
		}else{
			system("clear");
		
			if (!objeto_en_escenario(sala, objeto1) && !objeto_en_inventario(sala, objeto1)){
				printf("Ese objeto no está en el escenario\n");
			}else if (strcmp(verbo, "agarrar") == 0){
				bool agarrado = sala_agarrar_objeto(sala, objeto1);
				if (agarrado)
					printf("El objeto se añadió a tu inventario\n");
				else
					printf("No se pudo agarrar %s.\n", objeto1);
			} else if (strcmp(verbo, "describir") == 0){
				printf("%s\n", sala_describir_objeto(sala, objeto1));
			} else if (sala_es_interaccion_valida(sala, verbo, objeto1, objeto2)){
				int cantidad = sala_ejecutar_interaccion(sala, verbo, objeto1, objeto2, mostrar_mensaje, NULL);
				if (cantidad == 0)
					printf("Hubo un error al ejecutar la interacción\n");
			} else {
				printf("La interacción es inválida\n");
			}
		}

	}
	
	system("clear");

	fptr = fopen("pantalla_final.txt", "r");
	
	imprimir_archivo(fptr);
	fclose(fptr);

	sala_destruir(sala);

	return 0;
}