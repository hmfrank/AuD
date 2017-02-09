ARC = libaud.a
TST = utest

# folders
OBJDIR = bin/
DOCDIR = doc/
INCDIR = inc/
LIBDIR = lib/
SRCDIR = src/
TSTDIR = tst/

# external sources and include paths
EXT = $(LIBDIR)catch.hpp
INCPATHS = $(LIBDIR)

# source and object files
INC = $(wildcard $(INCDIR)*.h)
SRC = $(wildcard $(SRCDIR)*.c)
OBJ = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(SRC))

TSRC = $(wildcard $(TSTDIR)*.cpp)
TOBJ = $(patsubst $(TSTDIR)%.cpp, $(OBJDIR)%.opp, $(TSRC))

DEP = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.d, $(patsubst $(TSTDIR)%.cpp, $(OBJDIR)%.dpp, $(TSRC) $(SRC)))

# C compiler flags
CC = gcc
CFLAGS = -std=c99 -mbmi -Wall -Wextra -Werror

# C++ compiler and linker flags
CXX = g++
CXXFLAGS = -std=c++11 $(patsubst %, -I %, $(INCPATHS)) -Wall -Wextra
LXXFLAGS = -lm

# phony targets
.PHONY: all clean destroy doc test

all: $(ARC)

clean:
	rm -rf $(ARC) $(TST) $(OBJDIR)

destroy: clean
	rm -rf $(LIBDIR) $(DOCDIR)

doc: $(SRC) $(INC) Doxyfile | $(DOCDIR)
	doxygen

test: $(TST)

# create archive
$(ARC): $(OBJ)
	ar -cq $@ $(OBJ)

# link test
$(TST): $(TOBJ) $(ARC)
	$(CXX) $(CXXFLAGS) $^ $(LXXFLAGS) -o $@

# .o file
$(OBJDIR)%.o: $(SRCDIR)%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# .opp file
$(OBJDIR)%.opp: $(TSTDIR)%.cpp $(EXT) | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEP)

# .d file
$(OBJDIR)%.d: $(SRCDIR)%.c | $(OBJDIR)
	$(CC) -MM $< -MT $(subst .d,.o,$@) -MF $@

# .dpp file
$(OBJDIR)%.dpp: $(TSTDIR)%.cpp | $(OBJDIR)
	$(CXX) -MM $< -MT $(subst .dpp,.opp,$@) > $@

# folders
$(DOCDIR) $(OBJDIR) $(LIBDIR):
	mkdir $@

# download external sources
$(LIBDIR)catch.hpp: | $(LIBDIR)
	wget -q "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp" -O $@
