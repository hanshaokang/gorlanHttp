cc=gcc
bin=httpd

src=$(shell ls *.c)

LDFLAGS=-lpthread

$(bin):$(src)
	$(cc) -o $@ $^ $(LDFLAGS) -D_DEBUG_ 

.PHONY: clean
clean:
	rm -f $(bin)

.PHONY:debug
debug:
	echo $(src)
