build: lindex linget

lindex: lindex.c

linget: linget.c

clean:
	-rm -f lindex lindex.exe
	-rm -f linget linget.exe
