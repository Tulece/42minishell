#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "minishell.h"

// Définition des types et structures nécessaires
typedef enum {
    COMMAND,
    PIPE,
    AND,
    OR
} NodeType;

typedef struct Node {
    NodeType type;
    char *value;
    struct Node *left;
    struct Node *right;
} Node;

// Définition du type de jeton
typedef struct Token {
    char *value;
    NodeType type;
    struct Token *next;
} Token;

// Fonction pour créer un nouveau nœud
Node *create_node(NodeType type, char *value) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->type = type;
    node->value = ft_strdup(value);
    node->left = NULL;
    node->right = NULL;
    return node;
}

// Fonction pour libérer la mémoire de l'arbre
void free_tree(Node *root) {
    if (root == NULL) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root->value);
    free(root);
}

// Fonction récursive pour construire l'arbre syntaxique
Node *parse_tokens(Token *tokens) {
    if (tokens == NULL) return NULL;

    // Trouver le premier opérateur
    Token *current = tokens, *previous = NULL;
    while (current != NULL) {
        if (current->type == PIPE || current->type == AND || current->type == OR) {
            break;
        }
        previous = current;
        current = current->next;
    }

    // Si aucun opérateur n'est trouvé, il s'agit d'une simple commande
    if (current == NULL) {
        return create_node(COMMAND, tokens->value);
    }

    // Créer le nœud pour l'opérateur
    Node *node = create_node(current->type, current->value);

    // Séparer la liste de jetons en deux parties
    Token *left_tokens = tokens;
    Token *right_tokens = current->next;

    // Important: exclure l'opérateur de la liste de jetons pour éviter la récursivité infinie
    if (previous != NULL) {
        previous->next = NULL; // Cela termine la liste de gauche au token précédant l'opérateur
    }

    // Construire les sous-arbres gauche et droit récursivement
    node->left = parse_tokens(left_tokens);
    node->right = parse_tokens(right_tokens);

    return node;
}

// Fonction de test pour afficher l'arbre syntaxique (à des fins de débogage)
void print_tree(Node *root) {
    if (root == NULL) return;
    print_tree(root->left);
    printf("%s\n", root->value);
    print_tree(root->right);
}

// int ast_contructor() {
//     // Supposons que vous ayez une liste de jetons générés par le lexer
//     Token *tokens = (Token *)malloc(sizeof(Token));
//     tokens->value = "ls";
//     tokens->type = COMMAND;
//     tokens->next = (Token *)malloc(sizeof(Token));
//     tokens->next->value = "|";
//     tokens->next->type = PIPE;
//     tokens->next->next = (Token *)malloc(sizeof(Token));
//     tokens->next->next->value = "grep";
//     tokens->next->next->type = COMMAND;
//     tokens->next->next->next = NULL;

//     // Construire l'arbre syntaxique
//     Node *root = parse_tokens(tokens);

//     // Afficher l'arbre syntaxique (à des fins de débogage)
//     print_tree(root);

//     // Libérer la mémoire utilisée par l'arbre et les jetons
//     free_tree(root);

//     return 0;
// }
