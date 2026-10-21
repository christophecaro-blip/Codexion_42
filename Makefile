NAME = codexion

CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
INCLUDES = -Iincludes

SRC = src/main.c \
	src/parsing/parse_args.c \
	src/parsing/parse_utils.c \
	src/coder/coder_init.c \
	src/coder/routine.c \
	src/coder/actions.c \
	src/cleanup/cleanup.c \
	src/dongle/dongle_init.c \
	src/logger/log.c \
	src/monitor/monitoring.c \
	src/scheduler/heap.c \
	src/scheduler/fifo.c \
	src/scheduler/edf.c \
	src/scheduler/heap_init.c \
	src/scheduler/ready.c \
	src/utils/utils.c

OBJ_DIR = obj
OBJ = $(SRC:src/%.c=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
