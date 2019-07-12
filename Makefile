NAME	=	repl-asm

CC	=	gcc

INC	=	./inc/

CFLAGS	+=	-I$(INC)	\
		-Wall -Wextra	\
		-g

LDFLAGS	+=	-lreadline

SRC_ROOT	=	./src/

SRC_CMD		=	./src/cmd/

SRC_INST	=	./src/inst/

SRC_CTX		=	./src/ctx/

SRC_UTILS	=	./src/utils/

SRCS	=	$(SRC_ROOT)main.c	\
		$(SRC_ROOT)init.c	\
		$(SRC_ROOT)repl.c	\
		$(SRC_UTILS)error.c	\
		$(SRC_UTILS)string.c	\
		$(SRC_UTILS)getters.c	\
		$(SRC_UTILS)unit_finder.c \
		$(SRC_UTILS)reg_names.c	\
		$(SRC_UTILS)print.c	\
		$(SRC_UTILS)deref.c	\
		$(SRC_INST)add.c	\
		$(SRC_INST)edit.c	\
		$(SRC_INST)remove.c	\
		$(SRC_INST)commit.c	\
		$(SRC_INST)assembly.c	\
		$(SRC_INST)nasm_error.c	\
		$(SRC_INST)dump.c	\
		$(SRC_INST)find.c	\
		$(SRC_INST)breakpoint.c	\
		$(SRC_INST)label.c	\
		$(SRC_CMD)help.c	\
		$(SRC_CMD)dump.c	\
		$(SRC_CMD)run.c		\
		$(SRC_CMD)cont.c	\
		$(SRC_CMD)inst.c	\
		$(SRC_CMD)disas.c	\
		$(SRC_CMD)label.c	\
		$(SRC_CMD)breakpoint.c	\
		$(SRC_CMD)print.c	\
		$(SRC_CMD)set.c		\
		$(SRC_CMD)stack.c	\
		$(SRC_CTX)switch.c	\
		$(SRC_CTX)sig_handler.c

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
