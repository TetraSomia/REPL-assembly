NAME	=	repl-asm

CC	=	gcc

INC	=	./inc/

CFLAGS	+=	-I$(INC)	\
		-Wall -Wextra	\
		-g

LDFLAGS	+=

SRC_ROOT	=	./src/

SRC_TEST	=	./src/test/

SRC_CMD		=	./src/cmd/

SRC_INST	=	./src/inst/

SRCS	=	$(SRC_ROOT)main.c	\
		$(SRC_ROOT)utils.c	\
		$(SRC_ROOT)assembly.c	\
		$(SRC_TEST)assembly.c	\
		$(SRC_ROOT)init.c	\
		$(SRC_ROOT)repl.c	\
		$(SRC_CMD)help.c	\
		$(SRC_INST)add.c	\
		$(SRC_INST)remove.c	\
		$(SRC_INST)commit.c

OBJS	=	$(SRCS:.c=.o)

RM	=	rm -f

all:		$(NAME)

$(NAME):	$(OBJS)
		@$(CC) $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
		@$(RM) $(OBJS)

fclean:		clean
		@$(RM) $(NAME)

re:		fclean all

.PHONY:		all clean fclean re
