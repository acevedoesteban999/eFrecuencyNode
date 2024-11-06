#include "eAuth.h"


User users[MAX_USERS];         // Declaración del array de usuarios
unsigned int count_users=0;

// Inicializa los usuarios con un usuario predeterminado (admin)
esp_err_t init_users() {
    esp_err_t ret = init_nvs();
    
    // Inicializar usuario admin
    strcpy(users[0].username, "admin");
    strcpy(users[0].password, "admin");
    users[0].is_authenticated = false; // Por defecto, no está autenticado
    users[0].session_token[0] = '\0';  // Sin token inicial
    count_users = 1;                   // Solo tenemos un usuario inicial

    return ret;
}

void save_user(){

}

// Genera un token de sesión aleatorio
void generate_session_token(char *token) {
    // Generar un token aleatorio simple
    snprintf(token, TOKEN_LEN, "%lu_%d", (unsigned long)time(NULL), rand());
}

// Autentica al usuario comparando el nombre de usuario y la contraseña
bool authenticate_user(const char *username, const char *password) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0) {
            generate_session_token(users[i].session_token); // Generar token
            users[i].is_authenticated = true; // Marcar como autenticado
            return true; // Autenticación exitosa
        }
    }
    return false; // Falló la autenticación
}

// Busca un usuario por su nombre de usuario
User* find_user_by_username(const char *username) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].username, username) == 0) {
            return &users[i]; // Devolver puntero al usuario encontrado
        }
    }
    return NULL; // Falló la búsqueda
}

// Obtiene un usuario por su token de sesión
User* get_user_by_session_token(const char *session_token) {
    for (int i = 0; i < count_users; i++) {
        if (strcmp(users[i].session_token, session_token) == 0) {
            return &users[i]; // Devolver puntero al usuario encontrado
        }
    }
    return NULL; // No se encontró el usuario
}

// Cierra sesión del usuario dado un token de sesión
bool logout_user(const char *session_token) {
    User *user = get_user_by_session_token(session_token);
    if (user) {
        user->is_authenticated = false; // Cerrar sesión
        user->session_token[0] = '\0';  // Limpiar el token
        return true; // Cierre de sesión exitoso
    }
    return false; // Falló el cierre de sesión
}
