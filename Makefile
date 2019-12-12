#cross:=arm-linux-gnueabihf
#cross:=arm-himix100-linux

host:=
CC:=gcc
CPP:=g++

ifdef cross
	host=$(cross)
	CC=$(cross)-gcc
	CPP=$(cross)-g++
endif

ROOT=$(shell pwd)
LIBTOOL=$(shell which libtool)

#.o文件存放路径, 方便清理
OBJ = $(ROOT)/obj

INC += -I$(ROOT)/api
INC += -I$(ROOT)/src
INC += -I$(ROOT)/libs-src
INC += -I$(ROOT)/libs/include
INC += -I$(ROOT)/libs/include/freetype2

LIB += -L$(ROOT)/libs/lib
LIB += -lpthread -lm -lfreetype -lpng

obj += ${patsubst %.cpp,$(OBJ)/%-api-p.o,${notdir ${wildcard $(ROOT)/api/*.cpp}}}
obj += ${patsubst %.cpp,$(OBJ)/%-src-p.o,${notdir ${wildcard $(ROOT)/src/*.cpp}}}
obj += ${patsubst %.cpp,$(OBJ)/%-libs-p.o,${notdir ${wildcard $(ROOT)/libs-src/*.cpp}}}

obj += ${patsubst %.c,$(OBJ)/%-api-c.o,${notdir ${wildcard $(ROOT)/api/*.c}}}
obj += ${patsubst %.c,$(OBJ)/%-src-c.o,${notdir ${wildcard $(ROOT)/src/*.c}}}
obj += ${patsubst %.c,$(OBJ)/%-libs-c.o,${notdir ${wildcard $(ROOT)/libs-src/*.c}}}

%-api-p.o:../api/%.cpp
	$(CPP) -O3 -Wall -c $< $(INC) $(LIB) -o $@
%-src-p.o:../src/%.cpp
	$(CPP) -O3 -Wall -c $< $(INC) $(LIB) -o $@
%-libs-p.o:../libs-src/%.cpp
	$(CPP) -O3 -Wall -c $< $(INC) $(LIB) -o $@

%-api-c.o:../api/%.c
	$(CPP) -O3 -Wall -c $< $(INC) $(LIB) -o $@
%-src-c.o:../src/%.c
	$(CPP) -O3 -Wall -c $< $(INC) $(LIB) -o $@
%-libs-c.o:../libs-src/%.c
	$(CPP) -O3 -Wall -c $< $(INC) $(LIB) -o $@

app:$(obj)
	@$(CPP) -O3 -Wall -o $(ROOT)/$@ $(obj) $(INC) $(LIB)

libs:png freetype
	@echo "---------- all complete !! ----------"

freetype:
	@tar -xzf $(ROOT)/pkg/freetype-2.9.tar.gz -C $(ROOT)/libs && \
	cd $(ROOT)/libs/freetype-2.9 && \
	./configure --prefix=$(ROOT)/libs --host=$(host) --enable-shared --with-bzip2=no --with-zlib=no --with-harfbuzz=no --with-png=no && \
	make -j4 && \
	make install && \
	cd - && \
	rm -rf $(ROOT)/libs/freetype-2.9

png:
	@tar -xJf $(ROOT)/pkg/libpng-1.6.37.tar.xz -C $(ROOT)/libs && \
	cd $(ROOT)/libs/libpng-1.6.37 && \
	./configure --prefix=$(ROOT)/libs --host=$(host) && \
	make -j4 && \
	make install && \
	cd - && \
	rm -rf $(ROOT)/libs/libpng-1.6.37

# jpeg:
# 	@unzip $(ROOT)/pkg/jpegsr6.zip -d $(ROOT)/libs && \
# 	cd $(ROOT)/libs/jpeg-6b && \
# 	mkdir ../bin ../sbin ../include ../lib ../man/man1 -p && \
# 	sed -i "s/\r//" ./configure && \
# 	cp $(LIBTOOL) ./ && \
# 	./configure --prefix=$(ROOT)/libs --host=$(host) --enable-shared && \
# 	make -j4 && \
# 	make install && \
# 	cd - && \
# 	rm -rf $(ROOT)/libs/jpeg-6b

clean:
	@rm -rf ./app ./obj/*

cleanall:
	@rm -rf ./app ./obj/* ./libs/*
