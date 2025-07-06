/* Example: Single linked list
 *
 * A basic single linked list implementation for demonstation purposes.
 *
 * Compile: gcc -o list list.c
 * Execute: ./list
 *
 * Sven Reissmann (sven.reissmann@rz.hs-fulda.de)
 */
#include <stdlib.h>
#include <stdio.h>

/* Datastructure for a very basic single linked list
 */
struct list {
  int data;
  struct list *next;
};

/* Append a single element to the list
 *
 * Parameters:
 *    *head: Pointer to the beginning of the list
 *     data: The data-value of the element to add
 *   return: Pointer to the beginning of the list
 */
struct list* append_element (struct list *head, int data) {
  struct list *tmp = head;

  // Create the new element
  struct list *new_el = (struct list *) malloc (sizeof (struct list));
  if (new_el == NULL) {
    exit (EXIT_FAILURE);
  }
  new_el->data = data;
  new_el->next = NULL;

  // If list is currently empty, just return new element (as new head)
  if (head == NULL) {
    return new_el;
  }

  // If list is not empty, iterate the list until reaching the last
  // element, and append the new element there
  while (tmp->next != NULL) {
    tmp = tmp->next;
  }
  tmp->next = new_el;

  // Return the head of the list
  return head;
}

/* Remove a single element from the list
 *
 * Parameters:
 *    *head: Pointer to the first element of the list
 *     data: The data-value of the element to delete
 *   return: Pointer to the beginning of the list
 */
struct list* delete_element (struct list *head, int data) {
  struct list *tmp = head;
  struct list *prev = NULL;
 
  // Find the list element to delete, as well as the previous one
  while (tmp != NULL) {
    if (tmp->data == data)
      break;
    prev = tmp;
    tmp = tmp->next;
  }

  // Did not find the element to delete
  if (tmp == NULL) {
    return head;
  }
 
  // The element we want to delete is the first one
  if (tmp == head) {
    tmp = tmp->next;
    free (head);
    return tmp;
  }

  // The element we want to delete is a middle one
  prev->next = tmp->next;
  free (tmp);
  return head;
}

/* Find and return a list element by its data value
 *
 * Parameters:
 *    *head: Pointer to the first element of the list
 *     data: The data-value of the element to find
 *   return: Pointer to the found element, or NULL (nil)
 */
struct list* find (struct list *head, int data) {
  struct list *tmp = head;

  while (tmp != NULL) {
    if (tmp->data == data)
      break;
    tmp = tmp->next;
  }

  return tmp;
}

/* Free the whole list.
 *
 * Parameters:
 *    *head: Pointer to the first element of the list
 */
void destroy_list (struct list *head) {
  struct list *tmp;

  while (head != NULL) {
    tmp = head;
    head = head->next;
    free (tmp);
  }
}

/* Print the list to the screen (stdout)
 *
 * Parameters:
 *    *head: Pointer to the first element of the list
 */
void output (struct list *head) {
  struct list *tmp = head;

  while (tmp != NULL) {
    printf ("%d ", tmp->data);
    tmp = tmp->next;
  }
  printf ("\n");
}


/* Usage example for the list implemented above
 */
int main (int argc, char **argv) {
  struct list *head = NULL;
  struct list *el = NULL;

  head = append_element (head, 5);
  head = append_element (head, 3);
  head = append_element (head, 8);
  head = append_element (head, 1);

  output (head);

  el = find (head, 2);
  if (el) {
    printf ("Found: %d\n", el->data);
  } else {
    printf ("Not found!\n");
  }

  head = delete_element (head, 5);
  head = delete_element (head, 8);

  output (head);

  destroy_list (head);
  exit (EXIT_SUCCESS);
}

