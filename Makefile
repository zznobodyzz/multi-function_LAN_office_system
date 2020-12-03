CC=gcc
CFLAGS+=-pthread -lsqlite3
export CC
export CFLAGS
SUBDIR:=client/src server/src
.PHONY:all $(SUBDIR)
all:$(SUBDIR)
$(SUBDIR):
		make -C $@
