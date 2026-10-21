/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ccaro <ccaro@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/07 09:56:21 by ccaro             #+#    #+#             */
/*   Updated: 2026/10/07 09:57:50 by ccaro            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>
# include <stdbool.h>
# include <time.h>

typedef enum e_scheduler
{
	SCHEDULER_FIFO,
	SCHEDULER_EDF
}	t_scheduler;

typedef enum e_coder_state
{
	CODER_WAITING,
	CODER_COMPILING,
	CODER_DEBUGING,
	CODER_REFACTORING
}	t_coder_state;

typedef struct s_config		t_config;
typedef struct s_dongle		t_dongle;
typedef struct s_simulation	t_simulation;
typedef struct s_coder		t_coder;
typedef struct s_heap_entry	t_heap_entry;
typedef struct s_heap		t_heap;

struct s_config
{
	long			number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	long			number_of_compiles_required;
	long			dongle_cooldown;
	t_scheduler		scheduler;
	struct timespec	start_time;
};

struct s_dongle
{
	pthread_mutex_t	mutex;
	bool			is_taken;
	long			available_at;
	t_heap			*wait_queue;
	bool			favor_even;
};

struct s_simulation
{
	const t_config	*config;
	t_coder			*coders;
	pthread_mutex_t	log_mutex;
	pthread_mutex_t	sched_mutex;
	pthread_cond_t	sched_cond;
	int				stop;
	int				started_count;
};

struct s_coder
{
	t_coder_state	state;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	long			id;
	long			curr_nb_compil;
	long			burnout_at;
	t_simulation	*sim;
};

struct s_heap_entry
{
	long	key;
	t_coder	*coder;
};

struct s_heap
{
	t_heap_entry	*array;
	int				size;
	int				capacity;
};

int				take_dongle(t_coder *coder);
void			release_dongle(t_coder *coder);
void			lock_dongles(t_coder *coder);
void			unlock_dongles(t_coder *coder);
int				dongle_available(t_dongle *dongle, long now);
int				is_ready(t_coder *coder);
int				parse_args(int argc, char **argv, t_config *config);
int				parse_positive_long(const char *str, long *out);
t_coder			*initialize_table(t_simulation	*sim, t_dongle	*dongle);
t_dongle		*initialize_dongle_table(const t_config	*config);
void			destroy_dongle_table(t_dongle *table, int n);
void			cleanup_simulation(t_simulation *sim, t_dongle *dongles,
					t_coder *coders, pthread_t *threads);
void			*coder_routine(void *arg);
pthread_t		*create_threads(const t_config	*config, t_coder	*table);
struct timespec	ms_to_deadline(struct timespec start, long ms);
long			get_timestamp(struct timespec start);
void			log_event(t_coder	*coder, const char	*message);
int				extract_min(t_heap *heap, t_heap_entry *out);
void			swap(t_heap_entry *a, t_heap_entry *b);
void			insert(t_heap *heap, long key, t_coder *coder);
t_heap			*initialize_heap(int capacity);
void			destroy_heap(t_heap *heap);
void			register_wait(t_coder *coder);
long			fifo_key(t_coder *coder, t_dongle *dongle);
long			edf_key(t_coder *coder, t_dongle *dongle);
void			*monitor_routine(void *arg);
#endif
