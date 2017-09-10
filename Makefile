cc=gcc
bin=httpd

src=$(shell ls *.c)
CGI=wwwroot/cgibin 
LDFLAGS=-lpthread

.PHONY:all
all:$(bin) cgi
$(bin):$(src)
	$(cc) -o $@ $^ $(LDFLAGS) 
	
.PHONY:cgi
cgi:
	for name in 'echo $(CGI)';\
	do\
		cd $$name;\
		make;\
		cd -;\
	done

.PHONY: clean
clean:
	rm -rf output $(bin)
	for name in 'echo $(CGI)';\
	do\
		cd $$name;\
		make clean;\
		cd -;\
	done


.PHONY:output
output:
	mkdir -p output/wwwroot/cgibin
	mkdir -p output/log
	mkdir -p output/conf
	cp conf/server.conf output/conf
	cp httpd output
	cp plugin/server_ctl.sh output
	cp wwwroot/imags ouput/wwwroot -a
	for name in 'echo $c(CGI)';\
	do\
		cd $$name;\
		make output;\
		cd -;\
	done
