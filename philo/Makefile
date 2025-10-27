NAME = philo
CC = cc
OBJS_DIR = obj
CFLAGS = -Wall -Wextra -Werror -g -pthread

SRCS = main.c ft_atoi.c parser.c 

OBJS = $(SRCS:%.c=$(OBJS_DIR)/%.o)

all: $(NAME)

$(OBJS_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ -c $<

$(NAME): $(OBJS)
	@$(CC) $(OBJS) -o $(NAME) -pthread

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all re fclean clean