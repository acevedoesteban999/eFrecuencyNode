#pragma once
#include <stdbool.h>
#include "esp_err.h"
#include "string.h"
#include <time.h>
#include "eStore.h"

#define MAX_USERS 10         // Máximo número de usuarios permitidos
#define USERNAME_LEN 20      // Longitud máxima para nombres de usuario
#define PASSWORD_LEN 20      // Longitud máxima para contraseñas
#define TOKEN_LEN 64         // Longitud máxima para tokens de sesión

// Estructura para almacenar información de un usuario
typedef struct {
    char username[USERNAME_LEN];      // Nombre de usuario
    char password[PASSWORD_LEN];      // Contraseña
    char session_token[TOKEN_LEN];    // Token de sesión
    bool is_authenticated;             // Estado de autenticación
} User;

extern User users[MAX_USERS];         // Declaración del array de usuarios
extern unsigned int count_users;      // Declaración del contador de usuarios

// Inicializa los usuarios
esp_err_t init_users(void);

// Genera un token de sesión aleatorio
void generate_session_token(char *token);

// Autentica al usuario
bool authenticate_user(const char *username, const char *password);

// Busca un usuario por su nombre de usuario
User* find_user_by_username(const char *username);

// Obtiene un usuario por su token de sesión
User* get_user_by_session_token(const char *session_token);

// Cierra sesión del usuario
bool logout_user(const char *session_token);

