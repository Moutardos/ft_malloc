HOSTTYPE ?= $(shell uname -m)_$(shell uname -s)
NAME = libft_malloc_$(HOSTTYPE).so
LINKNAME = libft_malloc.so

CC = gcc
CFLAGS = -Wall -Wextra -Werror

SRC_DIR = src/
SRC_FILES = malloc.c free.c realloc.c
SRCS = $(addprefix $(SRC_DIR), $(SRC_FILES))
OBJ_DIR = obj/
OBJS = $(addprefix $(OBJ_DIR), $(SRC_FILES:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) -shared -o $(NAME) $(OBJ)
	# Create or update the symbolic link
	ln -sf $(NAME) $(LINKNAME)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME) $(LINKNAME)

re: fclean all

.PHONY: all clean fclean re
