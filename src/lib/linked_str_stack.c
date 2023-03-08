/**
 * amiws -- Library with functions for read/create AMI packets
 * Copyright (C) 2017, Stas Kobzar <staskobzar@modulis.ca>
 *
 * This file is part of amiws.
 *
 * amiws is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * amiws is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with amiws.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <malloc.h>
#include <assert.h>

#include "linked_str_stack.h"

LinkedStrStackNode *linked_str_stack_node_create(char *str, size_t len, LinkedStrStackNode* prev)
{
  LinkedStrStackNode *node = (LinkedStrStackNode *)malloc(sizeof(LinkedStrStackNode));
  assert ( node != NULL && "Failed to allocate memory for linked stack node");

  node->str = str;
  node->len = len;
  node->prev = prev;

  return node;
}

void linked_str_stack_node_destroy(LinkedStrStackNode* node)
{
  if (node == NULL)
    return;

  if(node->str)
    free(node->str);

  free(node);
  node = NULL;
}

LinkedStrStack *linked_str_stack_create()
{
  LinkedStrStack *stack = (LinkedStrStack *)malloc(sizeof(LinkedStrStack));
  assert (stack != NULL && "Failed to allocate memory for linked stack");

  stack->top = NULL;
  return stack;
}

void linked_str_stack_destroy(LinkedStrStack *stack)
{
  if(stack == NULL)
    return;

  LinkedStrStackNode *prev, *current = stack->top;
  while(current != NULL) {
    prev = current->prev;
    linked_str_stack_node_destroy(current);
    current = prev;
  }

  free(stack);
  stack = NULL;
}

void linked_str_stack_push(LinkedStrStack *stack, char *str, size_t len)
{
  stack->top = linked_str_stack_node_create(str, len, stack->top);
}

size_t linked_str_stack_peek(LinkedStrStack *stack, char **str)
{
  if(stack->top == NULL) {
    *str = NULL;
    return 0;
  }
  *str = stack->top->str;
  return stack->top->len;
}

void linked_str_stack_pull(LinkedStrStack *stack)
{
  if(stack->top == NULL)
    return;
  LinkedStrStackNode *prev = stack->top->prev;
  linked_str_stack_node_destroy(stack->top);
  stack->top = prev;
}
