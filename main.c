#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int opt,numCuenta;
    float saldo1, saldo2;

    printf("Bienvenido al banco central UACJ\n");
    printf("Operacion a realizar\n");
    printf("1. Consultar saldo\n");
    printf("2. Hacer una transferencia bancaria\n");
    printf("Opcion a escoger --> ");
    scanf("%d", &opt);

    switch (opt)
    {
    case 1:
    {
        char titular[150];
        char saldoTexto[150];
        char pin[10];       // Tamaño ampliado para mayor seguridad
        char pinInput[10];  // Tamaño ampliado para mayor seguridad
        FILE *archivo;
        char nombreCompleto[150];

        printf("Escriba su numero de cuenta: ");
        scanf("%d", &numCuenta);

        sprintf(nombreCompleto, "cuenta%d.txt", numCuenta);

        archivo = fopen(nombreCompleto, "r");

        if (archivo == NULL)
        {
            printf("Error: No se encontro el archivo %s\n", nombreCompleto);
            break;
        }
        else
        {
            printf("Escriba su pin: ");
            scanf("%s", pinInput);
            printf("\n");

            // --- L�NEA 1: Titular ---
            fgets(titular, 150, archivo);
            titular[strcspn(titular, "\n")] = 0; // Se limpia el salto de linea invisible

            // --- L�NEA 2: Saldo ---
            fgets(saldoTexto, 150, archivo);
            saldo1 = atof(saldoTexto); // Se convierte el texto a numero decimal (float)

            // --- L�NEA 3: PIN del archivo ---
            fgets(pin, 10, archivo);
            pin[strcspn(pin, "\n")] = 0; // Se limpia el salto de linea invisible

            if (strcmp(pinInput, pin) == 0)
            {
                printf("Acceso concedido.\n\n");
                printf("El titular es: %s\n", titular);
                printf("El saldo de la cuenta es: $%.2f\n", saldo1);
            }
            else
            {
                printf("Error: PIN incorrecto. Acceso denegado.\n");
            }

            fclose(archivo);
        }

        break;
    }
    default:
        printf("Opcion no valida\n");
        break;
    }

    return 0;
}
