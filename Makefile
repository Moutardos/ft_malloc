# VARS

HOSTTYPE	?=	$(shell uname -m)_$(shell uname -s)
NAME 		=	libft_malloc_$(HOSTTYPE).so
LINKNAME	=	libft_malloc.so

LIBS_DIR	=	libs
LIBFT		=	$(LIBS_DIR)/42_libft
LIBFT_A		=	$(LIBFT)/libft.a

SRC_DIR 	:=	src

SRCS_ENGINE :=	\
				memory_zones.c \
				memory_block.c

SRCS_MALLOC	:=	\
				malloc.c \
				free.c \
				realloc.c

SRCS		:=	\
				${addprefix engine/, $(SRCS_ENGINE)} \
				${addprefix malloc/, $(SRCS_MALLOC)}

OBJ_DIR		:=	obj
OBJS		:=	${addprefix $(OBJ_DIR)/, $(SRCS:.c=.o)}

SRCS 		:=	${addprefix $(SRC_DIR)/, $(SRCS)}  


INCLUDE_DIR	:=	include
BIN_DIR		:=	bin

CC			:= gcc
CFLAGS		:= -Wall -Wextra -MMD -MP -fPIC -I$(INCLUDE_DIR) -I$(LIBFT)/include

# RULES
all: $(BIN_DIR)/$(NAME)

$(LIBFT_A):
	$(MAKE) -C $(LIBFT)

$(BIN_DIR)/$(NAME): $(LIBFT_A) $(OBJS)
	mkdir -p ${@D}
	$(CC) -shared -o $@ $(OBJS) $(LIBFT_A)
	ln -sf $(BIN_DIR)/$(NAME) $(LINKNAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p ${@D}
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT) clean

fclean: clean
	rm -rf $(BIN_DIR)
	rm -f $(LINKNAME)
	$(MAKE) -C $(LIBFT) fclean

re: fclean all

.PHONY: all clean fclean re
