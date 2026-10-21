/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_init.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:54:20 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:54:21 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

t_heap	*initialize_heap(int capacity)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->array = malloc(sizeof(t_heap_entry) * capacity);
	if (!heap->array)
	{
		free(heap);
		return (NULL);
	}
	heap->size = 0;
	heap->capacity = capacity;
	return (heap);
}

static void	heap_down(t_heap *heap, long index)
{
	long	left;
	long	right;
	long	smallest;

	while (1)
	{
		left = 2 * index + 1;
		right = 2 * index + 2;
		smallest = index;
		if (left < heap->size && heap->array[left].key
			< heap->array[smallest].key)
			smallest = left;
		if (right < heap->size && heap->array[right].key
			< heap->array[smallest].key)
			smallest = right;
		if (smallest == index)
			break ;
		swap(&heap->array[index], &heap->array[smallest]);
		index = smallest;
	}
}

int	extract_min(t_heap *heap, t_heap_entry *out)
{
	if (heap->size == 0)
		return (0);
	*out = heap->array[0];
	heap->size--;
	heap->array[0] = heap->array[heap->size];
	heap_down(heap, 0);
	return (1);
}
