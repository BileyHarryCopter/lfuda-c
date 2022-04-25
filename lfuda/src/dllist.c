#include "error.h"
#include "memutil.h"

#include "dllist.h"

#include <assert.h>
#include <stddef.h>

struct dl_node_s {
  struct dl_node_s *next, *prev;
  void *data;
};

dl_node_t dl_node_init(void *data) {
  struct dl_node_s *node = calloc_checked(1, sizeof(struct dl_node_s));
  node->data = data;
  return node;
}

void dl_node_free(dl_node_t node_) {
  free(node_);
}

void *dl_node_get_data(dl_node_t node_) {
  struct dl_node_s *node = (struct dl_node_s *)node_;
  assert(node);
  return node->data;
}

void dl_node_set_data(dl_node_t node_, void *data) {
  struct dl_node_s *node = (struct dl_node_s *)node_;
  assert(node);
  node->data = data;
}

struct dl_list_s {
  struct dl_node_s *head, *tail;
  size_t len;
};

dl_list_t dl_list_init() {
  struct dl_list_s *list = calloc_checked(1, sizeof(struct dl_list_s));
  return list;
}

int dl_list_is_empty(dl_list_t list_) {
  struct dl_list_s *list = (struct dl_list_s *)list_;
  assert(list);
  return !(list->len);
}

void dl_list_push_front(dl_list_t list_, dl_node_t node_) {
  struct dl_list_s *list = (struct dl_list_s *)list_;
  struct dl_node_s *node = (struct dl_node_s *)node_;

  assert(list);
  assert(node);

  // List is empty
  if (list->head == NULL) {
    list->head = list->tail = node;
    node->next = node->prev = NULL;
  } else {
    node->next = list->head;
    list->head->prev = node;
    node->prev = NULL;
    list->head = node;
  }

  list->len++;
}

void dl_list_push_back(dl_list_t list_, dl_node_t node_) {
  struct dl_list_s *list = (struct dl_list_s *)list_;
  struct dl_node_s *node = (struct dl_node_s *)node_;

  assert(list);
  assert(node);

  // List is empty
  if (list->head == NULL) {
    list->head = list->tail = node;
    node->next = node->prev = NULL;
  } else {
    node->prev = list->tail;
    list->tail->next = node;
    list->tail = node;
    node->next = NULL;
  }

  list->len++;
}

void dl_list_insert_after(dl_list_t list_, dl_node_t node_, dl_node_t toinsert_) {
  struct dl_list_s *list = (struct dl_list_s *)list_;
  struct dl_node_s *node = (struct dl_node_s *)node_;
  struct dl_node_s *toinsert = (struct dl_node_s *)toinsert_;

  assert(list);
  assert(node);
  assert(toinsert);

  // Pushing to back
  if (node == list->tail) {
    dl_list_push_back(list_, toinsert_);
    return;
  }

  toinsert->prev = node;
  toinsert->next = node->next;
  node->next = toinsert;

  list->len++;
}

dl_node_t dl_list_pop_front(dl_list_t list_) {
  struct dl_list_s *list = (struct dl_list_s *)list_;
  struct dl_node_s *node;

  assert(list);

  node = list->head;
  if (!node) {
    ERROR("Trying to pop an element from an empty list\n");
  }

  if (list->head == list->tail) {
    list->head = list->tail = 0;
  } else {
    list->head = list->head->next;
    node->next = NULL;
  }

  list->len--;
  return node;
}

dl_node_t dl_list_pop_back(dl_list_t list_) {
  struct dl_list_s *list = (struct dl_list_s *)list_;
  struct dl_node_s *node;

  assert(list);

  node = list->tail;
  if (!node) {
    ERROR("Trying to pop an element from an empty list\n");
  }

  if (list->head == list->tail) {
    list->head = list->tail = 0;
  } else {
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  }

  list->len--;
  return node;
}

dl_node_t dl_list_remove(dl_list_t list_, dl_node_t node_) {
  struct dl_list_s *list = (struct dl_list_s *)list_;
  struct dl_node_s *node = (struct dl_node_s *)node_;

  assert(list);
  assert(node);

  if (node == list->head) {
    return dl_list_pop_front(list_);
  } else if (node == list->tail) {
    return dl_list_pop_back(list_);
  }

  node->prev->next = node->next;
  node->next->prev = node->prev;
  node->next = node->prev = NULL;

  list->len--;
  return node;
}

void dl_list_free(dl_list_t list_, void (*data_free)(void *)) {
  struct dl_list_s *list = (struct dl_list_s *)list_;

  assert(list);

  struct dl_node_s *curr = list->head, *prev = NULL;
  while (curr) {
    prev = curr;
    if (data_free) {
      data_free(curr->data);
    }
    curr = curr->next;
    free(prev);
  }

  free(list);
}