NAME = philo
CC = cc
OBJS_DIR = obj
CFLAGS = -Wall -Wextra -Werror -g -pthread
LDFLAGS = -pthread

SRCS = main.c ft_atoi.c parser.c mutex.c philo.c  philo_output.c table.c
DEBUG_FLAGS = -g3 -O0
TSAN_FLAGS = -fsanitize=thread -g3 -O1
ASAN_FLAGS = -fsanitize=address -fsanitize=undefined -g3 -O1

debug: CFLAGS += $(DEBUG_FLAGS)
debug: re

tsan: CFLAGS += $(TSAN_FLAGS)
tsan: LDFLAGS += -fsanitize=thread
tsan: re

asan: CFLAGS += $(ASAN_FLAGS)
asan: LDFLAGS += -fsanitize=address -fsanitize=undefined
asan: re

OBJS = $(SRCS:%.c=$(OBJS_DIR)/%.o)

all: $(NAME)

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

$(NAME): $(OBJS)
	@$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all re fclean clean
