all: libbyz.a

CC:=g++
C:=gcc
AR:=ar
RANLIB:=ranlib
MAKEDEPEND:=makedepend -Y
MAKE=gmake
INCS:=-I../gmp -I../sfs/include/sfslite -I../sfs -I../sfslite-1.2/crypt -I..sfslite-1.2/async -I..sfslite-1.2/arpc -I../sfslite-1.2/sfsmisc -I../sfslite-1.2/svc 

#[JPM, as per Rodrigo's advice]
#CPPFLAGS:= $(INCS)  -O3 -march=pentiumpro -fno-exceptions -DNDEBUG   -DRECOVERY
CPPFLAGS:= $(INCS) -g -Wall -DRECOVERY -std=c++11 
#CPPFLAGS:= $(INCS) -O9 -funroll-loops -ffast-math -malign-double -march=pentiumpro -fomit-frame-pointer -fno-exceptions -DNDEBUG 
#CPPFLAGS:= $(INCS) -O3 -march=pentiumpro -fomit-frame-pointer -fno-exceptions -DNDEBUG   -DRECOVERY
#CPPFLAGS:= $(INCS) -O3 -march=pentiumpro -fno-exceptions -DNDEBUG   -DRECOVERY -pg
#CPPFLAGS:= $(INCS) -O3 -march=pentiumpro -fno-exceptions -DNDEBUG   -DRECOVERY 

# [elwong: The flags we used for SOSP 2007]
#CPPFLAGS := $(INCS) -O3 -march=pentium4 -fno-exceptions -DRECOVERY

# [elwong: Allows building on newer versions of gcc]
CPPFLAGS += -fpermissive

CFLAGS:= $(CPPFLAGS)

SINCS:= -I../sfs  -I../sfs/crypt -I../sfs/async -I../sfs/arpc -I../sfs/sfsmisc -I../sfs/svc -I../sfs/include/sfs 

Principal.o: Principal.cc Principal.h
	$(CC) $(CPPFLAGS) $(SINCS) -o Principal.o -c Principal.cc
Node.o: Node.cc Node.h
	$(CC) $(CPPFLAGS) $(SINCS) -o Node.o -c Node.cc

%.o:: %.cc
	$(CC) $(CPPFLAGS) -o $@ -c $<

%.o:: %.c
	$(C) $(CFLAGS) -o $@ -c $<


C_FILES=\
Client.cc             Replica.cc            New_key.cc\
ITimer.cc             Principal.cc          Log_allocator.cc \
libbyz.cc             DSum.cc 		    Committed_locally.cc \
Message.cc            Order_request.cc      Reply.cc \
Digest.cc             Node.cc               Request.cc \
Checkpoint.cc         Local_commit.cc       Req_queue.cc \
Status.cc             Request_map.cc        Request_history.cc\
View_change.cc        New_view.cc           View_change_ack.cc \
Rep_info.cc           View_info.cc          NV_info.cc \
Stable_estimator.cc   Statistics.cc         Time.cc \
State.cc              Req_cache.cc	    Hate_primary.cc 


c_FILES= fail.c umac.c

H_FILES := $(C_FILES:%.C=%.H)

h_FILES := $(c_FILES:%.c=%.h)

OBJ_FILES:=$(C_FILES:%.cc=%.o) $(c_FILES:%.c=%.o)

objs: $(OBJ_FILES)

clean:
	-rm *.o
	-rm *~

clobber:
	-rm *.o
	-rm libbyz.a
	-rm *~

depend:
	$(MAKEDEPEND) $(INCS) $(SINCS) $(C_FILES)

libbyz.a: objs
	$(AR) rcv $@ $(OBJ_FILES) 
	$(RANLIB) $@

# DO NOT DELETE THIS LINE -- make depend depends on it.
