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

#ifndef __AMIWS_linked_str_stack_H
#define __AMIWS_linked_str_stack_H

#include <stddef.h>

typedef struct LinkedStrStackNode_ {
    char *str;
    size_t len;

    struct LinkedStrStackNode_ *prev;
} LinkedStrStackNode;

typedef struct LinkedStrStack_ {
    LinkedStrStackNode *top;
} LinkedStrStack;


LinkedStrStack *linked_str_stack_create();

void linked_str_stack_destroy(LinkedStrStack *stack);

void linked_str_stack_push(LinkedStrStack *stack, char *str, size_t len);

size_t linked_str_stack_peek(LinkedStrStack *stack, char **str);

void linked_str_stack_pull(LinkedStrStack *stack);

#endif
