SRCDIR = ./src
OBJDIR = ./obj
BINDIR = bin
SODIR = ./so
INCDIR = ./include
DBDIR = ../docker/public-html/driver
DB_INCDIR = /usr/include/mysql
TEMPLATEDIR = ../docker/etc/fleropp



CXX=g++
CXXWARN=-Wall -Wextra -Wshadow -Wpedantic -Wconversion -Werror\
 			-Wtrampolines -Wvla -Wlogical-op -Wduplicated-cond\
  			-Wduplicated-branches -Wformat-signedness -Wswitch-default -Wswitch-enum -Wno-stringop-overflow
CXXFLAGS=-std=c++20 $(CXXWARN) -I $(INCDIR) -fPIC -DFPM_BUILD
LDLIBS=-lpthread -lfcgi -lfcgi++ -lboost_filesystem -lboost_system -lboost_program_options -lcgicc -lspdlog -lfmt 
LDFLAGS=-rdynamic
BOOSTFLAGS=-DBOOST_BIND_GLOBAL_PLACEHOLDERS
DB_LDLIBS=-lsoci_core -lsoci_mysql -lmysqlclient

BIN=fleropp_fpm
SO=libfleropp.so
SRC=$(wildcard $(SRCDIR)/*.cpp)
OBJ=$(addprefix $(OBJDIR)/,$(notdir $(SRC:.cpp=.o)))
GCH=$(INCDIR)/HTMLLiterals.hpp
MYSQL_DB_SRC=$(SRCDIR)/db_driver/mysql.cpp
MYSQL_DB_SO=db_driver_mysql.so

# https://stackoverflow.com/questions/1079832/how-can-i-configure-my-makefile-for-debug-and-release-builds

#
# Debug build settings
#
DBGDIR = debug
DBGEXEDIR = $(DBGDIR)/$(BINDIR)
DBGEXE = $(DBGEXEDIR)/$(BIN)
DBGOBJDIR = $(DBGDIR)/$(OBJDIR)

DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJ))
DBGSO = $(DBGDIR)/$(SODIR)/$(SO)
DBGSODIR = $(DBGDIR)/$(SODIR)/$(SODIR)
DBGCFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXEDIR = $(RELDIR)/$(BINDIR)
RELEXE = $(RELEXEDIR)/$(BIN)
RELOBJDIR = $(RELDIR)/$(OBJDIR)

RELOBJS = $(addprefix $(RELDIR)/, $(OBJ))
RELSODIR = $(RELDIR)/$(SODIR)
RELSO = $(RELDIR)/$(SODIR)/$(SO)
RELCFLAGS = -O3 -DNDEBUG

ifeq ($(PREFIX),)
	PREFIX := /usr
endif

##### Release #####
.PHONY : all
all: $(RELEXEDIR) $(RELOBJDIR) $(RELSODIR) compile link gch shared dbdriver

$(RELEXEDIR):
	mkdir -p $(RELEXEDIR)

$(RELOBJDIR):
	mkdir -p $(RELOBJDIR)

$(RELSODIR):
	mkdir -p $(RELSODIR)

.PHONY : compile
compile: $(RELOBJS)

.PHONY : $(RELBJDIR)/%.o
$(RELOBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(RELCFLAGS) $(BOOSTFLAGS) $< -o $@

.PHONY : link
link: compile
	$(CXX) $(RELCFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $(RELEXE) $(RELOBJS) $(LDLIBS)

.PHONY : shared
shared: compile
	$(CXX) $(RELCFLAGS) $(CXXFLAGS) -shared -o $(RELSO) $(RELOBJS) $(LDLIBS)

.PHONY : dbdriver
dbdriver:
	$(CXX) $(RELCFLAGS) $(CXXFLAGS) -I $(DB_INCDIR) -shared -o $(DBDIR)/$(MYSQL_DB_SO) $(MYSQL_DB_SRC) $(LDLIBS) $(DB_LDLIBS)

.PHONY : gch
gch: $(GCH)
	$(CXX) $(CXXFLAGS) $(RELCFLAGS)  $<

##### Debug #####
.PHONY : dbg
dbg: $(DBGEXEDIR) $(DBGOBJDIR) $(DBGSODIR) dbgcompile dbglink gch dbgshared dbgdbdriver
# dbggch
$(DBGEXEDIR):
	mkdir -p $(DBGEXEDIR)

$(DBGOBJDIR):
	mkdir -p $(DBGOBJDIR)

$(DBGSODIR):
	mkdir -p $(DBGSODIR)

.PHONY : dbgcompile
dbgcompile: $(DBGOBJS)

.PHONY : $(DBGOBJDIR)/%.o
$(DBGOBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $(DBGCFLAGS) $(BOOSTFLAGS) $< -o $@

.PHONY : dbglink
dbglink: dbgcompile
	$(CXX) $(DBGCFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $(DBGEXE) $(DBGOBJS) $(LDLIBS)

.PHONY : dbgshared
dbgshared: dbgcompile
	$(CXX) $(DBGCFLAGS) $(CXXFLAGS) -shared -o $(DBGSO) $(DBGOBJS) $(LDLIBS)

.PHONY : dbgdbdriver
dbgdbdriver:
	$(CXX) $(DBGCFLAGS) $(CXXFLAGS) -I $(DB_INCDIR) -shared -o $(DBDIR)/$(MYSQL_DB_SO) $(MYSQL_DB_SRC) $(LDLIBS) $(DB_LDLIBS)

.PHONY : clean
clean:
	find -type f -name "$(BIN)" -delete
	find -type f -name "*.o" -delete
	find -type f -name "*~" -delete

install: 
	install -d $(DESTDIR)$(PREFIX)/lib/fleropp/drivers
	install -d $(DESTDIR)$(PREFIX)/include/fleropp
	install -d $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)/etc/fleropp
	install -m 755 $(RELSO) $(DESTDIR)$(PREFIX)/lib/fleropp
	install -m 755 $(DBDIR)/$(MYSQL_DB_SO) $(DESTDIR)$(PREFIX)/lib/fleropp/drivers
	cp -rn $(INCDIR)/* $(DESTDIR)$(PREFIX)/include/fleropp
	chmod -R 644 $(DESTDIR)$(PREFIX)/include/fleropp/*
	install -m 755 $(RELEXE) $(DESTDIR)$(PREFIX)/bin
	cp -n $(TEMPLATEDIR)/*.template $(DESTDIR)/etc/fleropp 
	chmod 644 $(DESTDIR)/etc/fleropp/*.template

