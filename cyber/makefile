default: cyber

.PHONY: default format

CFILES= \
	track.c\
	tourney.c\
	gang.c\
	server.c\
	cons.c\
	mail.c\
	utils.c\
	combat.c\
	major.c\
	fileio.c\
	memory.c\
	help.c\
	jack.c\
	move.c\
	ipc.c\
	command.c\
	edit.c\
	subs.c\
	run.c\
	display.c\
	win.c\
	fakebbs.c

HFILES= include/header.h \
		include/globals.h \
		include/programs.h \
		include/proto.h \
		include/world.h \
		include/vended.h \
		include/ranks.h \
		include/systypes.h \
		include/muicyber.h \
		include/fakebbs.h

#CFLAGS= @\bbsv6\muicyber\src\makdll.cfg -I..\.. -I..\INCLUDE -I..\..\SRC
CFLAGS = -g -Iinclude -I. -std=c99

ifeq ($(CC),clang)
	CFLAGS += -ferror-limit=1
else
	-std=gnu89
endif

LDFLAGS = -lc -lm --entry=main

#cyber: $(OFILES) $
	#$(LD) -o $@ $^ $(LDFLAGS)
cyber: $(CFILES) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(CFILES) -lm

actkey: actkey.c
	$(CC) $(CFLAGS) -o $@ $^
wgkey: wgkey.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OFILES) cyber actkey wgkey

# cyber: \
# 	$(OFILES) \
# 	#\bbsv6\dlib\galimp.lib \bbsv6\dlib\gsblimp.lib \
# 	\run286\bc4\lib\phapi.lib \bbsv6\dlib\galmsg.def
# 	ltdll cyber

$(OFILES): $(HFILES)

format:
	astyle --options=astyle.options include/*.h *c