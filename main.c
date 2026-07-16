#include <stdio.h>      // para printf, scanf, fopen, etc.
#include <stdlib.h>     // para atof (convertir texto a numero)
#include <string.h>     // para strcmp, strcspn (comparar y limpiar textos)
#include <time.h>       // para fecha y hora

// ============================================
// funciones basicas
// ============================================

// limpia el buffer como cuando le das refresh al teclado
void limpiarBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// saca la fecha y hora actual, como un sello de tiempo
void obtenerFechaHora(char *fechaHora) {
    time_t t;
    struct tm *tm_info;
    time(&t);
    tm_info = localtime(&t);
    strftime(fechaHora, 50, "%Y-%m-%d %H:%M:%S", tm_info);
}

// ============================================
// manejo de archivos donde se guarda todo
// ============================================

// checa si existe el archivo de la cuenta - como buscar una carpeta
int cuentaExiste(int numCuenta) {
    char nombreCompleto[150];
    FILE *archivo;
    
    sprintf(nombreCompleto, "cuenta%d.txt", numCuenta);  // crea el nombre del archivo
    archivo = fopen(nombreCompleto, "r");               // intenta abrirlo
    
    if (archivo == NULL) {
        return 0;  // no existe :(
    } else {
        fclose(archivo);
        return 1;  // si existe :)
    }
}

// lee los datos de la cuenta como abrir una ficha
int leerDatosCuenta(int numCuenta, char *titular, float *saldo, char *pin) {
    char nombreCompleto[150];
    FILE *archivo;
    char linea[200];
    
    sprintf(nombreCompleto, "cuenta%d.txt", numCuenta);
    archivo = fopen(nombreCompleto, "r");
    
    if (archivo == NULL) {
        return 0;  // no se pudo abrir
    }
    
    fgets(titular, 150, archivo);                       // lee el nombre
    titular[strcspn(titular, "\n")] = 0;               // quita el enter
    
    fgets(linea, 200, archivo);                        // lee el saldo
    *saldo = atof(linea);                             // lo convierte a numero
    
    fgets(pin, 10, archivo);                          // lee el pin
    pin[strcspn(pin, "\n")] = 0;                     // quita el enter
    
    fclose(archivo);
    return 1;  // lectura exitosa
}

// valida el pin 
int validarPIN(int numCuenta, char *pinInput) {
    char pin[10];
    float saldo;
    char titular[150];
    
    if (!leerDatosCuenta(numCuenta, titular, &saldo, pin)) {
        return 0;  // no se pudo leer la cuenta
    }
    return (strcmp(pinInput, pin) == 0);  // compara los pins
}

// registra cada movimiento - como un diario de transacciones
void registrarTransaccion(int numCuenta, char *tipo, float monto, char *descripcion) {
    char nombreArchivo[150];
    char fechaHora[50];
    FILE *archivo;
    
    sprintf(nombreArchivo, "transacciones_cuenta%d.txt", numCuenta);
    archivo = fopen(nombreArchivo, "a");  // "a" = agregar al final
    
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo de transacciones\n");
        return;
    }
    
    obtenerFechaHora(fechaHora);  // pone fecha y hora
    fprintf(archivo, "[%s] %s: $%.2f - %s\n", fechaHora, tipo, monto, descripcion);
    fclose(archivo);
}

// guarda el nuevo saldo - como actualizar el estado de cuenta
void actualizarSaldo(int numCuenta, float nuevoSaldo) {
    char nombreCompleto[150];
    char titular[150];
    char pin[10];
    float saldo;
    FILE *archivo;
    
    leerDatosCuenta(numCuenta, titular, &saldo, pin);  // lee lo que hay
    
    sprintf(nombreCompleto, "cuenta%d.txt", numCuenta);
    archivo = fopen(nombreCompleto, "w");  // "w" = sobreescribe todo
    fprintf(archivo, "%s\n", titular);     // guarda el nombre
    fprintf(archivo, "%.2f\n", nuevoSaldo); // guarda el nuevo saldo
    fprintf(archivo, "%s\n", pin);         // guarda el pin
    fclose(archivo);
}

// ============================================
// opcion 1. consultar saldo - solo ver, no tocar
// ============================================

void consultarSaldo() {
    int numCuenta;
    char pinInput[10];
    char titular[150];
    float saldo;
    char pin[10];
    
    printf("\n--- CONSULTAR SALDO ---\n");
    printf("Escriba su numero de cuenta: ");
    scanf("%d", &numCuenta);  // pide el numero
    limpiarBuffer();
    
    if (!cuentaExiste(numCuenta)) {  // verifica si existe
        printf("Error: La cuenta no existe.\n");
        return;
    }
    
    printf("Escriba su PIN: ");
    scanf("%s", pinInput);  // pide el pin
    limpiarBuffer();
    
    if (!validarPIN(numCuenta, pinInput)) {  // valida el pin
        printf("Error: PIN incorrecto. Acceso denegado.\n");
        return;
    }
    
    leerDatosCuenta(numCuenta, titular, &saldo, pin);  // lee los datos
    
    printf("\n--- DATOS DE LA CUENTA ---\n");
    printf("Titular: %s\n", titular);     // muestra el nombre
    printf("Saldo disponible: $%.2f\n", saldo);  // muestra el saldo
    
    registrarTransaccion(numCuenta, "CONSULTA", 0.0, "Consulta de saldo");  // registra
}

// ============================================
// 2. transferencia - mover dinero entre cuentas
// ============================================

void realizarTransferencia() {
    int cuentaOrigen, cuentaDestino;
    float monto;
    char pinInput[10];
    char titularOrigen[150], titularDestino[150];
    float saldoOrigen, saldoDestino;
    char pinOrigen[10], pinDestino[10];
    
    printf("\n--- TRANSFERENCIA BANCARIA ---\n");
    printf("Cuenta de origen: ");
    scanf("%d", &cuentaOrigen);  // de donde sale el dinero
    limpiarBuffer();
    
    if (!cuentaExiste(cuentaOrigen)) {
        printf("Error: La cuenta de origen no existe.\n");
        return;
    }
    
    printf("Cuenta de destino: ");
    scanf("%d", &cuentaDestino);  // a donde va el dinero
    limpiarBuffer();
    
    if (!cuentaExiste(cuentaDestino)) {
        printf("Error: La cuenta de destino no existe.\n");
        return;
    }
    
    if (cuentaOrigen == cuentaDestino) {  // no se puede transferir a si mismo
        printf("Error: No puedes transferir a la misma cuenta.\n");
        return;
    }
    
    printf("Monto a transferir: $");
    scanf("%f", &monto);  // cuanto dinero
    limpiarBuffer();
    
    if (monto <= 0) {
        printf("Error: El monto debe ser mayor a cero.\n");
        return;
    }
    
    printf("Ingrese su PIN: ");
    scanf("%s", pinInput);  // pide pin para autorizar
    limpiarBuffer();
    
    if (!validarPIN(cuentaOrigen, pinInput)) {
        printf("Error: PIN incorrecto. Operacion cancelada.\n");
        return;
    }
    
    leerDatosCuenta(cuentaOrigen, titularOrigen, &saldoOrigen, pinOrigen);
    leerDatosCuenta(cuentaDestino, titularDestino, &saldoDestino, pinDestino);
    
    if (saldoOrigen < monto) {  // verifica que haya dinero suficiente
        printf("Error: Saldo insuficiente. Saldo disponible: $%.2f\n", saldoOrigen);
        return;
    }
    
    // hace la transferencia
    saldoOrigen -= monto;   // resta de la cuenta origen
    saldoDestino += monto;  // suma a la cuenta destino
    
    actualizarSaldo(cuentaOrigen, saldoOrigen);    // guarda el cambio en origen
    actualizarSaldo(cuentaDestino, saldoDestino);  // guarda el cambio en destino
    
    // registra en ambas cuentas
    char descripcion[200];
    sprintf(descripcion, "Transferencia a cuenta %d", cuentaDestino);
    registrarTransaccion(cuentaOrigen, "TRANSFERENCIA ENVIO", monto, descripcion);
    
    sprintf(descripcion, "Transferencia desde cuenta %d", cuentaOrigen);
    registrarTransaccion(cuentaDestino, "TRANSFERENCIA RECIBO", monto, descripcion);
    
    printf("\n--- TRANSFERENCIA EXITOSA ---\n");
    printf("Se transfirieron $%.2f de la cuenta %d a la cuenta %d\n", monto, cuentaOrigen, cuentaDestino);
    printf("Nuevo saldo de la cuenta %d: $%.2f\n", cuentaOrigen, saldoOrigen);
    printf("Nuevo saldo de la cuenta %d: $%.2f\n", cuentaDestino, saldoDestino);
}

// ============================================
// 3. cambiar pin - cambiar la contrasena
// ============================================

void cambiarPIN() {
    int numCuenta;
    char pinActual[10], pinNuevo[10], confirmarPIN[10];
    char titular[150];
    float saldo;
    char pin[10];
    
    printf("\n--- CAMBIAR PIN ---\n");
    printf("Escriba su numero de cuenta: ");
    scanf("%d", &numCuenta);
    limpiarBuffer();
    
    if (!cuentaExiste(numCuenta)) {
        printf("Error: La cuenta no existe.\n");
        return;
    }
    
    printf("Ingrese su PIN actual: ");
    scanf("%s", pinActual);  // pide el pin viejo
    limpiarBuffer();
    
    if (!validarPIN(numCuenta, pinActual)) {  // verifica que sea correcto
        printf("Error: PIN actual incorrecto.\n");
        return;
    }
    
    printf("Ingrese su nuevo PIN (4 digitos): ");
    scanf("%s", pinNuevo);  // pide el nuevo pin
    limpiarBuffer();
    
    if (strlen(pinNuevo) != 4) {  // verifica que tenga 4 digitos
        printf("Error: El PIN debe tener exactamente 4 digitos.\n");
        return;
    }
    
    printf("Confirme su nuevo PIN: ");
    scanf("%s", confirmarPIN);  // pide confirmacion
    limpiarBuffer();
    
    if (strcmp(pinNuevo, confirmarPIN) != 0) {  // verifica que coincidan
        printf("Error: Los PIN no coinciden.\n");
        return;
    }
    
    leerDatosCuenta(numCuenta, titular, &saldo, pin);  // lee datos actuales
    
    // guarda con el nuevo pin
    char nombreCompleto[150];
    FILE *archivo;
    sprintf(nombreCompleto, "cuenta%d.txt", numCuenta);
    archivo = fopen(nombreCompleto, "w");
    fprintf(archivo, "%s\n", titular);
    fprintf(archivo, "%.2f\n", saldo);
    fprintf(archivo, "%s\n", pinNuevo);  // aqui guarda el pin nuevo
    fclose(archivo);
    
    registrarTransaccion(numCuenta, "CAMBIO PIN", 0.0, "Cambio de PIN");
    
    printf("\n--- PIN CAMBIADO EXITOSAMENTE ---\n");
    printf("Su PIN ha sido actualizado.\n");
}

// ============================================
// 4. deposito - meter dinero a la cuenta
// ============================================

void realizarDeposito() {
    int numCuenta;
    float monto;
    char titular[150];
    float saldo;
    char pin[10];
    
    printf("\n--- REALIZAR DEPOSITO ---\n");
    printf("Escriba su numero de cuenta: ");
    scanf("%d", &numCuenta);  // pide la cuenta
    limpiarBuffer();
    
    if (!cuentaExiste(numCuenta)) {
        printf("Error: La cuenta no existe.\n");
        return;
    }
    
    printf("Monto a depositar: $");
    scanf("%f", &monto);  // pide el monto
    limpiarBuffer();
    
    if (monto <= 0) {
        printf("Error: El monto debe ser mayor a cero.\n");
        return;
    }
    
    leerDatosCuenta(numCuenta, titular, &saldo, pin);  // lee el saldo actual
    
    saldo += monto;  // suma el deposito
    
    actualizarSaldo(numCuenta, saldo);  // guarda el nuevo saldo
    
    registrarTransaccion(numCuenta, "DEPOSITO", monto, "Deposito en efectivo");
    
    printf("\n--- DEPOSITO EXITOSO ---\n");
    printf("Se depositaron $%.2f a la cuenta %d\n", monto, numCuenta);
    printf("Nuevo saldo: $%.2f\n", saldo);
}

// ============================================
// 5. crear cuenta - dar de alta nuevo cliente
// ============================================

void crearCuenta() {
    int numCuenta;
    char titular[150];
    float saldoInicial;
    char pin[10];
    char nombreCompleto[150];
    FILE *archivo;
    
    printf("\n--- DAR DE ALTA NUEVA CUENTA ---\n");
    printf("Escriba el numero para la nueva cuenta: ");
    scanf("%d", &numCuenta);  // pide el numero de cuenta
    limpiarBuffer();
    
    if (cuentaExiste(numCuenta)) {
        printf("Error: La cuenta ya existe. Intente con otro numero.\n");
        return;
    }
    
    printf("Nombre del titular: ");
    fgets(titular, 150, stdin);  // lee el nombre completo
    titular[strcspn(titular, "\n")] = 0;  // quita el enter
    
    printf("Monto de deposito inicial: $");
    scanf("%f", &saldoInicial);  // pide el saldo inicial
    limpiarBuffer();
    
    if (saldoInicial < 0) {
        printf("Error: El saldo inicial no puede ser negativo.\n");
        return;
    }
    
    printf("Cree su PIN (4 digitos): ");
    scanf("%s", pin);  // pide el nuevo pin
    limpiarBuffer();
    
    if (strlen(pin) != 4) {
        printf("Error: El PIN debe tener exactamente 4 digitos.\n");
        return;
    }
    
    // crea el archivo de la cuenta nueva
    sprintf(nombreCompleto, "cuenta%d.txt", numCuenta);
    archivo = fopen(nombreCompleto, "w");
    fprintf(archivo, "%s\n", titular);
    fprintf(archivo, "%.2f\n", saldoInicial);
    fprintf(archivo, "%s\n", pin);
    fclose(archivo);
    
    // registra el primer movimiento
    registrarTransaccion(numCuenta, "APERTURA", saldoInicial, "Apertura de cuenta");
    
    printf("\n--- CUENTA CREADA EXITOSAMENTE ---\n");
    printf("Bienvenido %s al Banco Central UACJ\n", titular);
    printf("Su numero de cuenta es: %d\n", numCuenta);
}

// ============================================
// programa principal 
// ============================================

int main() {
    int opt;  // variable para guardar la opcion del usuario
    
    printf("Sistema de Transacciones Bancarias UACJ\n");
    
    // ++++++++++++++++++++ menu principal ++++++++++++++++++++++++++++++++++++++++++++++
    do {  // bucle que se repite hasta que el usuario elija salir
        printf("----------------------------------------\n");
        printf("   BIENVENIDO AL BANCO CENTRAL UACJ    \n");
        printf("----------------------------------------\n");
        printf("1. Consultar saldo\n");        // opcion 1
        printf("2. Realizar transferencia bancaria\n");  // opcion 2
        printf("3. Cambiar PIN\n");            // opcion 3
        printf("4. Realizar deposito\n");      // opcion 4
        printf("5. Dar de alta nueva cuenta\n"); // opcion 5
        printf("6. Salir\n");                  // opcion 6
        printf("----------------------------------------\n");
        printf("Opcion a escoger --> ");
        scanf("%d", &opt);  // lee lo que elige el usuario
        limpiarBuffer();
        
        // ++++++++++++++++++++ switch - el que decide que hacer ++++++++++++++++++++++++++
        switch (opt) {
            case 1:
                consultarSaldo();  // va a consultar saldo
                break;
            case 2:
                realizarTransferencia();  // va a transferir
                break;
            case 3:
                cambiarPIN();  // va a cambiar pin
                break;
            case 4:
                realizarDeposito();  // va a depositar
                break;
            case 5:
                crearCuenta(); // va a crear cuenta
                break;
            case 6:
                printf("\nGracias por usar el sistema ATM del Banco Central UACJ\n");
                printf("Sesion finalizada exitosamente.\n");  // se despide
                break;
            default:
                printf("Opcion no valida. Por favor, seleccione una opcion del 1 al 6.\n");
                break;
        }
    } while (opt != 6);  // repite hasta que el usuario elija 6 (salir)
    
    return 0;  // fin del programa
}
