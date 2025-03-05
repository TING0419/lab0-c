#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *head =
        (struct list_head *) malloc(sizeof(struct list_head));

    if (!head)
        return NULL;

    head->next = head;
    head->prev = head;
    return head;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    if (list_empty(head)) {
        free(head);
        return;
    }
    struct list_head *node, *safe;
    list_for_each_safe (node, safe, head) {
        element_t *current = list_entry(node, element_t, list);
        q_release_element(current);
    }
    free(head);
}

bool q_insert_head(struct list_head *head, char *s)
{
    if (!head || !s)


        return false;

    element_t *element = malloc(sizeof(element_t));

    if (!element)

        return false;

    int s_len = strlen(s);

    element->value = (char *) malloc((s_len + 1) * sizeof(char));

    if (!element->value) {
        free(element);

        return false;
    }

    strncpy(element->value, s, (s_len + 1));

    list_add(&element->list, head);

    return true;
}


/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    element_t *new_node = (element_t *) malloc(sizeof(element_t));
    if (!new_node) {
        return false;
    }
    list_add_tail(&new_node->list, head);
    new_node->value = strdup(s);
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;

    element_t *tmp = list_entry(head->next, element_t, list);

    if (sp && bufsize > 0) {
        size_t len = strlen(tmp->value);

        if (len < bufsize) {
            memcpy(sp, tmp->value, len + 1);
        } else {
            memcpy(sp, tmp->value, bufsize - 1);
            sp[bufsize - 1] = '\0';
        }
    }

    list_del(&tmp->list);
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head)) {
        return NULL;
    }
    return q_remove_head(head->prev->prev, sp, bufsize);
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;

    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    // https://leetcode.com/problems/delete-the-middle-node-of-a-linked-list/
    if (!head || list_empty(head)) {
        return false;
    }

    struct list_head *slow = head->next, *fast = head->next;
    while (fast != head && fast->next != head) {
        slow = slow->next;
        fast = fast->next->next;
    }

    list_del(slow);
    element_t *element = list_entry(slow, element_t, list);
    q_release_element(element);
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    // https://leetcode.com/problems/remove-duplicates-from-sorted-list-ii/
    if (!head || list_empty(head) || list_is_singular(head))
        return false;
    element_t *node = NULL, *safe = NULL, *dup_tail = NULL;
    list_for_each_entry_safe (node, safe, head, list) {
        if (&safe->list != head && !strcmp(node->value, safe->value)) {
            list_del(&node->list);
            q_release_element(node);
            dup_tail = safe;
        } else {
            if (dup_tail) {
                list_del(&dup_tail->list);
                q_release_element(dup_tail);
            }
            dup_tail = NULL;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *first = head->next, *second = head->next->next;

    while (second != head && first != head) {
        list_move(second, first->prev);
        first = first->next;
        second = first->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head) || list_is_singular(head)) {
        return;
    }

    struct list_head *node = NULL, *safe = NULL;
    list_for_each_safe (node, safe, head) {
        list_move(node, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
    if (!head || list_empty(head) || list_is_singular(head) || k == 1)
        return;
    int len = q_size(head), cnt = 0;
    struct list_head *node = NULL, *safe = NULL, *reverse_head = head;

    if (len < k)
        return;
    list_for_each_safe (node, safe, head) {
        list_move(node, reverse_head);
        if (++cnt == k) {
            if ((len -= k) < k)
                return;
            cnt = 0;
            reverse_head = safe->prev;
        }
    }
}

/* Merge two sorted lists into one, using a dummy head node */
static struct list_head *merge_sorted_lists(struct list_head *left,
                                            struct list_head *right,
                                            bool descend)
{
    LIST_HEAD(dummy);
    struct list_head *tail = &dummy;

    while (left && right) {
        const char *lval = list_entry(left, element_t, list)->value;
        const char *rval = list_entry(right, element_t, list)->value;
        if ((!descend && strcmp(lval, rval) <= 0) ||
            (descend && strcmp(lval, rval) >= 0)) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }
    tail->next = left ? left : right;
    return dummy.next;
}

/* Recursively perform merge sort on a singly-linked list */
static struct list_head *merge_sort_list(struct list_head *head, bool descend)
{
    if (!head || !head->next)
        return head;

    struct list_head *slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    struct list_head *second = slow->next;
    slow->next = NULL;

    head = merge_sort_list(head, descend);
    second = merge_sort_list(second, descend);
    return merge_sorted_lists(head, second, descend);
}

/* Sort elements of queue in ascending/descending order using merge sort */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    // Break circularity to form a simple singly linked list.
    head->prev->next = NULL;

    // Sort the list starting from the first data node.
    head->next = merge_sort_list(head->next, descend);

    // Rebuild the doubly linked list pointers and restore circularity.
    struct list_head *cur = head;
    while (cur->next) {
        cur->next->prev = cur;
        cur = cur->next;
    }
    cur->next = head;
    head->prev = cur;
}


int purge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))  // If the list is empty, return 0
        return 0;

    if (list_is_singular(head))  // If the list has only one element, return 1
        return 1;

    int cnt = 1;  // Keep one element by default
    struct list_head *node, *safe;
    struct list_head *peak =
        head->prev;  // The reference point for comparison is the last element

    // Traverse the list in reverse order (starting from the tail)
    for (node = head->prev; node != head; node = safe) {
        safe = node->prev;  // Store the next node for the next iteration
        const char *s1 = list_entry(peak, element_t, list)
                             ->value;  // Value of the peak element
        const char *s2 = list_entry(node, element_t, list)
                             ->value;  // Value of the current node

        // If the elements satisfy the condition for sorting (based on the
        // 'descend' flag), remove the node and free the memory
        if ((!descend && strcmp(s1, s2) <= 0) ||
            (descend && strcmp(s1, s2) >= 0)) {
            list_del(node);  // Remove the node from the list
            q_release_element(list_entry(
                node, element_t, list));  // Free the memory for the element
        } else {
            peak = node;  // Update the peak node to the current one
            cnt++;        // Increment the count of retained elements
        }
    }

    return cnt;  // Return the number of elements that remain in the list
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return purge(head, 0);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return purge(head, 1);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head))
        return 0;
    int size = 0;
    struct list_head *merge_queue = NULL, *iter = head->next;
    while (iter != head) {
        queue_contex_t *curr_queue = list_entry(iter, queue_contex_t, chain);
        size += curr_queue->size;
        curr_queue->q->prev->next = NULL;
        merge_queue =
            merge_sorted_lists(merge_queue, curr_queue->q->next, descend);
        iter = iter->next;
        INIT_LIST_HEAD(curr_queue->q);
    }
    LIST_HEAD(dummy_head);
    dummy_head.next = merge_queue;
    struct list_head *node = NULL, *safe = NULL;
    for (node = &dummy_head, safe = dummy_head.next; safe->next;
         node = safe, safe = node->next) {
        safe->prev = node;
    }
    safe->next = &dummy_head;
    dummy_head.prev = safe;
    list_splice(&dummy_head, list_first_entry(head, queue_contex_t, chain)->q);
    return size;
}
