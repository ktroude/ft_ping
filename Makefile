# Configuration
NAME    := ft_ping
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror
INC     := -Iinclude

SRC_DIR := src
OBJ_DIR := obj

SRC     := $(wildcard $(SRC_DIR)/*.c)
OBJ     := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(INC) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
