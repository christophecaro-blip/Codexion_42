/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:13 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:54:14 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	swap(t_heap_entry *a, t_heap_entry *b)
{
	t_heap_entry	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static void	heap_up(t_heap *heap, long index)
{
	while (index > 0 && heap->array[(index - 1) / 2].key
		> heap->array[index].key)
	{
		swap(&heap->array[index], &heap->array[(index - 1) / 2]);
		index = (index - 1) / 2;
	}
}

void	insert(t_heap *heap, long key, t_coder *coder)
{
	t_heap_entry	entry;
	long			index;

	entry.key = key;
	entry.coder = coder;
	heap->array[heap->size] = entry;
	index = heap->size;
	heap->size++;
	heap_up(heap, index);
}
