HOSTTYPE ?= $(shell uname -m)_$(shell uname -s)
NAME = libft_malloc_$(HOSTTYPE).so
LINKNAME = libft_malloc.so

LIBS_DIR = libs
LIBFT = $(LIBS_DIR)/42_libft
LIBFT_A = $(LIBFT)/libft.a

SRC_DIR = src
SRC_FILES = malloc.c free.c realloc.c

OBJ_DIR = obj
OBJS = $(addprefix $(OBJ_DIR)/, $(SRC_FILES:.c=.o))

INCLUDE_DIR = include
BIN_DIR = bin

CC = gcc
CFLAGS = -Wall -Wextra -MMD -MP -I$(INCLUDE_DIR)

all: $(BIN_DIR)/$(NAME)

$(LIBFT_A):
	$(MAKE) -C $(LIBFT)

$(BIN_DIR)/$(NAME): $(LIBFT_A) $(OBJS) | $(BIN_DIR)
	$(CC) -shared -o $@ $(OBJS) $(LIBFT_A)
	ln -sf $(BIN_DIR)/$(NAME) $(LINKNAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR) $(BIN_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT) clean

fclean: clean
	rm -rf $(BIN_DIR)
	rm -f $(LINKNAME)
	$(MAKE) -C $(LIBFT) fclean

re: fclean all

.PHONY: all clean fclean re
