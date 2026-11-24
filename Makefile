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
				memory_block.c \
				memory.c
SRCS_MALLOC	:=	\
				malloc.c \
				free.c \
				realloc.c

SRCS_VIEW	:=	\
				show_alloc_mem.c

SRCS		:=	\
				${addprefix engine/, $(SRCS_ENGINE)} \
				${addprefix malloc/, $(SRCS_MALLOC)} \
				${addprefix view/, $(SRCS_VIEW)}

OBJ_DIR		:=	obj
OBJS		:=	${addprefix $(OBJ_DIR)/, $(SRCS:.c=.o)}

SRCS 		:=	${addprefix $(SRC_DIR)/, $(SRCS)}  

TEST		:= malloc_test
TEST_DIR	:= tests

TEST_SRCS	:= main.c
TEST_SRCS	:= ${addprefix $(TEST_DIR)/, $(TEST_SRCS)}

INCLUDE_DIR	:=	include
BIN_DIR		:=	bin

CC			:= gcc
CFLAGS		:= -Wall -Wextra -g3 -gdwarf-4 -MMD -MP -fPIC
IFLAGS 		:= -I$(INCLUDE_DIR) -I$(LIBFT)/include

# RULES
all: $(BIN_DIR)/$(NAME)

test: $(BIN_DIR)/$(TEST)

$(BIN_DIR)/$(TEST): $(BIN_DIR)/$(NAME)
	$(CC) -g3 -gdwarf-4 $(IFLAGS) $(TEST_SRCS) -o $@ -L$(BIN_DIR) -lft_malloc -Wl,-rpath,$(BIN_DIR)
$(LIBFT_A):
	$(MAKE) -C $(LIBFT)

$(BIN_DIR)/$(NAME): $(LIBFT_A) $(OBJS)
	mkdir -p ${@D}
	$(CC) -shared -o $@ $(OBJS) $(LIBFT_A)
	ln -sf $(NAME) $(BIN_DIR)/$(LINKNAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p ${@D}
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@


clean:
	rm -rf $(OBJ_DIR)
	$(MAKE) -C $(LIBFT) clean

fclean: clean
	rm -rf $(BIN_DIR)
	$(MAKE) -C $(LIBFT) fclean

re: fclean all

.PHONY: all clean fclean re
