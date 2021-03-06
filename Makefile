all: dot3enuminterfaces.exe dot3getprofile.exe dot3reconnect.exe dot3setprofile.exe dot3setprofileeapuserdata.exe

clean:
	rm -f dot3enuminterfaces.exe dot3getprofile.exe dot3reconnect.exe dot3setprofile.exe dot3setprofileeapuserdata.exe libdot3api.a

%.exe: %.c libdot3api.a dot3api.h
	i686-w64-mingw32-gcc -W -Wall -O2 -L. $< -ldot3api -lole32 -leappcfg -o $@

lib%.a: %.def
	i686-w64-mingw32-dlltool -d $< -k -l $@
