PROG	= ft_nm

SRCS 	= main.c
OBJS 	= ${SRCS:.c=.o}
MAIN	= main.c

HEADER	= -Iinclude

CC 		= gcc
CFLAGS 	= -Wall -Wextra -Werror -g

.c.o:		%.o : %.c
					@gcc ${CFLAGS} ${HEADER} -c $< -o $(<:.c=.o)

all: 		${PROG}

${PROG}:	${OBJS}
					@echo "\033[33m----Compiling lib----"
					@make re -C ./Libft
					@make re -C ./printf
					@$(CC) ${OBJS} -LLibft -lft -Lprintf -lprintf -o ${PROG}
					@echo "\033[32mFT_NM Compiled!\n"

clean:
					@make clean -C ./Libft
					@rm -f ${OBJS} ${OBJS_B}

fclean: 	clean
					@make fclean -C ./Libft
					@rm -f $(NAME)
					@rm -f ${PROG}
					@echo "\n\033[31mDeleting EVERYTHING!\n"

re:			fclean all

.PHONY: all clean fclean re
