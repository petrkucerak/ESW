#include "list.hpp"
#include <cassert>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined (USE_MUTEX)
#  define rd_lock(lock) pthread_mutex_lock(lock)
#  define rd_unlock(lock) pthread_mutex_unlock(lock)
#  define wr_lock(lock) pthread_mutex_lock(lock)
#  define wr_unlock(lock) pthread_mutex_unlock(lock)
#elif defined (USE_RWLOCK)
#  define rd_lock(lock) // TODO
#  define rd_unlock(lock) // TODO
#  define wr_lock(lock) // TODO
#  define wr_unlock(lock) // TODO
#elif defined (USE_RCU)
#  define rd_lock(lock) // TODO
#  define rd_unlock(lock) // TODO
#  define wr_lock(lock) // TODO
#  define wr_unlock(lock) //TODO
#else
#  error "No lock type defined"
#endif

#ifdef DEBUG
#define debug_printf(...) printf(__VA_ARGS__)
#else
#define debug_printf(...)
#endif

unsigned calc_checksum(const char *str)
{
    unsigned sum = 0;
    while (*str)
        sum += *str++;
    return sum;
}

void esw_list_init(LIST_TYPE *list)
{
#if defined (USE_MUTEX)
    CHECK(pthread_mutex_init(&list->lock, NULL));
    list->head = NULL;
#elif defined (USE_RWLOCK)
    // TODO
#elif defined (USE_RCU)
    // TODO
#else
#error "No lock type defined"
#endif
}

void esw_list_push(LIST_TYPE *list, const char *const key, const char *const value)
{
    assert(list);
    assert(key);
    assert(value);

    debug_printf("Pushing %s: %s\n", key, value);

    esw_node_t *node = esw_list_create_node(key, value);
#if defined (USE_MUTEX) || defined (USE_RWLOCK)
    wr_lock(&list->lock);
    node->next = list->head;
    list->head = node;
    wr_unlock(&list->lock);
#elif defined (USE_RCU)
    // TODO
#endif
}

void esw_list_update(LIST_TYPE *list, const char *const key, const char *const value)
{
    assert(list);

    /* Replaces first occurrence in the list */
#if defined (USE_MUTEX) || defined (USE_RWLOCK)
    wr_lock(&list->lock);
    esw_node_t *current = list->head;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            char *new_value = strdup(value);
            debug_printf("Updating %s: %s -> %s\n", key, current->value, new_value);
            free(current->value);
            current->value = new_value;
            current->checksum = calc_checksum(new_value);
            break;
        }
        current = current->next;
    }
    wr_unlock(&list->lock);
#elif defined (USE_RCU)
    // TODO
#endif
}

bool esw_list_find(LIST_TYPE *list, const char *const key, char *value, const size_t max_len)
{
    bool found = false;
    assert(list);
    assert(key);

    rd_lock(&list->lock);
#if defined (USE_MUTEX) || defined (USE_RWLOCK)
    esw_node_t *current = list->head;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (strlen(current->value) < max_len) {
                strcpy(value, current->value);
                if (calc_checksum(value) != current->checksum)
                    errx(1, "%s:%d wrong checksum", __FILE__, __LINE__);
            } else {
                strncpy(value, current->value, max_len - 1);
                value[max_len - 1] = '\0';
                if (calc_checksum(value) != current->checksum)
                    errx(1, "%s:%d wrong checksum", __FILE__, __LINE__);
            }
            found = true;
            break;
        }
        current = current->next;
    }
#elif defined (USE_RCU)
    // TODO
#endif
    rd_unlock(&list->lock);

    return found;
}

esw_node_t *esw_list_create_node(const char *const key, const char *const value)
{
    esw_node_t *node = (esw_node_t *)calloc(1, sizeof(esw_node_t));
    node->key = strdup(key);
    node->value = strdup(value);
    node->checksum = calc_checksum(value);

    return node;
}

void esw_list_free_node(esw_node_t *node)
{
    free(node->key);
    free(node->value);
    free(node);
}

void esw_list_free_content(LIST_TYPE *list)
{
#if defined (USE_MUTEX) || defined (USE_RWLOCK)
    esw_node_t *current;
    esw_node_t *tmp;
    assert(list != NULL);
    current = list->head;
    while (current) {
        tmp = current;
        current = current->next;
        esw_list_free_node(tmp);
    }
#elif defined (USE_RCU)
    // TODO (not necessary)
#endif
}

void esw_list_free(LIST_TYPE *list)
{
    esw_list_free_content(list);
    free(list);
}
