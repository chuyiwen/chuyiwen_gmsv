0031:npc_lua:000000000:2:14=48041e4b:16=47d21313:002E:Makefile:05215:1:14=48041e4b:16=47d21313:INCFLAGS=-I. -I../include
FLTFLAGS="-s 1331200"   
export FLTFLAGS

#CFLAGS=-w -O3 -g $(INCFLAGS)
#CHOST="i686-pc-linux-gnu"
#CFLAGS=-march=pentium4 -w -O3 -pipe -mmmx -msse -msse2 $(INCFLAGS)
#CXXFLAGS="-march=pentium4 -O3 -pipe -fomit-frame-pointer"
CFLAGS=-w -O3 -pipe $(INCFLAGS)

PROG=libnpc_lua.a

SRC=npc_lua.c npc_lua_ex.c npc_lua_item.c npc_lua_nlg.c npc_lua_nl.c npc_lua_obj.c npc_lua_char.c npc_lua_battle.c

OBJ=$(SRC:.c=.o)

ifeq (0,$(MAKELEVEL))
CC=gcc
RM=rm -f
AR=ar cr
MV=mv
RANLIB=ranlib
SED=sed
SHELL=/bin/sh
endif

all: $(PROG)
$(PROG): $(OBJ)
	$(RM) $(PROG)
	$(AR) $(PROG) $(OBJ)
	$(RANLIB) $(PROG)
	
depend:
	$(MV) Makefile Makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(CC) $(INCFLAGS) -M $(SRC) >> Makefile 

clean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~

distclean:
	$(RM) $(PROG)
	$(RM) $(OBJ)
	$(RM) *~
	$(MV) Makefile Makefile.bak
	$(SED) -ne '1,/^# DO NOT DELETE THIS LINE/p' Makefile.bak>Makefile
	$(RM) Makefile.bak

# DO NOT DELETE THIS LINE
npc_lua.o: npc_lua.c ../include/npc_lua.h ../include/npc_lua_interface.h \
  ../include/version.h ../include/sasql.h /usr/include/stdio.h \
  /usr/include/_ansi.h /usr/include/newlib.h /usr/include/sys/config.h \
  /usr/include/machine/ieeefp.h /usr/include/cygwin/config.h \
  /usr/lib/gcc/i686-pc-cygwin/3.4.4/include/stddef.h \
  /usr/lib/gcc/i686-pc-cygwin/3.4.4/include/stdarg.h \
  /usr/include/sys/reent.h /usr/include/_ansi.h /usr/include/sys/_types.h \
  /usr/include/sys/lock.h /usr/include/sys/types.h \
  /usr/include/machine/_types.h /usr/include/machine/types.h \
  /usr/include/sys/features.h /usr/include/cygwin/types.h \
  /usr/include/sys/sysmacros.h /usr/include/stdint.h \
  /usr/include/endian.h /usr/include/sys/stdio.h /usr/include/sys/cdefs.h \
  /usr/include/stdlib.h /usr/include/machine/stdlib.h \
  /usr/include/alloca.h /usr/include/cygwin/stdlib.h \
  /usr/include/cygwin/wait.h /usr/include/netinet/in.h \
  /usr/include/cygwin/in.h /usr/include/cygwin/socket.h \
  /usr/include/asm/socket.h /usr/include/cygwin/if.h \
  /usr/include/sys/socket.h /usr/include/features.h \
  /usr/include/sys/time.h /usr/include/cygwin/sys_time.h \
  /usr/include/sys/select.h /usr/include/time.h \
  /usr/include/machine/time.h /usr/include/cygwin/time.h \
  /usr/include/signal.h /usr/include/sys/signal.h \
  /usr/include/cygwin/signal.h /usr/include/cygwin/sockios.h \
  /usr/include/sys/uio.h /usr/include/asm/byteorder.h \
  /usr/include/arpa/inet.h /usr/include/netdb.h /usr/include/unistd.h \
  /usr/include/sys/unistd.h /usr/include/getopt.h /usr/include/strings.h \
  /usr/include/string.h /usr/include/sys/string.h /usr/include/errno.h \
  /usr/include/sys/errno.h ../include/correct_bug.h ../include/common.h \
  ../include/../lua/lua.h ../include/../lua/luaconf.h \
  /usr/lib/gcc/i686-pc-cygwin/3.4.4/include/limits.h \
  /usr/lib/gcc/i686-pc-cygwin/3.4.4/include/syslimits.h \
  /usr/include/limits.h ../include/../lua/lauxlib.h \
  ../include/../lua/lua.h /usr/include/assert.h ../include/util.h \
  ../include/char.h ../include/char_base.h ../include/skill.h \
  ../include/util.h ../include/title.h ../include/addressbook.h \
  ../include/net.h /usr/include/pthread.h /usr/include/sched.h \
  ../include/link.h ../include/char_data.h ../include/item.h \
  ../include/char.h ../include/char_base.h ../include/char_data.h \
  ../include/anim_tbl.h ../include/object.h ../include/battle.h \
  ../include/npcutil.h ../include/item.h ../include/readmap.h
npc_lua_ex.o: npc_lua_ex.c ../include/npc_lua.h \
  ../include/npc_lua_interface.h ../include/version.h ../include/sasql.h \
  /usr/include/stdio.h /usr/include/_ansi.h /usr/include/newlib.h \
  /usr/include/sys/config.h /usr/include/machine/ieeefp.h \
  /usr/include/cygwin/config.h \
  /usr/lib/gcc/i686-pc-cygwin/3.4.4/include/stddef.h \
  /usr/lib/gcc/i686-pc-cygwin/3.4.4/include/stdarg.h \
  /usr/include/sys/reent.h /usr/include/_ansi.h /usr/include/sys/_types.h \
  /usr/include/sys/lock.h /usr/include/sys/types.h \
  /usr/include/machine/_types.h /usr/include/machine/types.h \
  /usr/include/sys/features.h /usr/include/cygwin/types.h \
  /usr/include/sys/sysmacros.h /usr/include/stdint.h \
  /usr/include/endian.h /usr/include/sys/st