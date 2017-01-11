OUT = libaud.a
EXE = utest

SRCDIR = src/
INCDIR = inc/
OBJDIR = bin/
TSTDIR = tst/
DOCDIR = doc/
LIBDIR = lib/

# external sources
EXT = $(LIBDIR)catch.hpp

# source and object files for library archive
SRC = $(wildcard $(SRCDIR)*.c)
OBJ = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.o, $(SRC))

# source and object files for test executable
TSRC = $(wildcard $(TSTDIR)*.cpp)
TOBJ = $(patsubst $(TSTDIR)%.cpp, $(OBJDIR)%.opp, $(TSRC))

DEP  = $(patsubst $(SRCDIR)%.c, $(OBJDIR)%.d, $(SRC)) $(patsubst $(TSTDIR)%.cpp, $(OBJDIR)%.dpp, $(TSRC))

# C compiler flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror

# C++ compiler and linker flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror
LXXFLAGS = 

.PHONY: all clean destroy doc test

all: $(OUT)

clean:
	rm -rf $(OUT) $(EXE) $(OBJDIR)

destroy: clean
	rm -rf $(DOCDIR) $(LIBDIR)

doc: $(SRC) $(INC) | $(DOCDIR)
	doxygen

test: $(EXE)

# create archive 
$(OUT): $(OBJ)
	ar -cq $@ $(OBJ)

$(EXE): $(TOBJ) $(OUT)
	$(CXX) $(CXXFLAGS) $^ $(LXXFLAGS) -o $@

# .o file
$(OBJDIR)%.o: $(SRCDIR)%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# .opp file
$(OBJDIR)%.opp: $(TSTDIR)%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEP)

# .d file
$(OBJDIR)%.d: $(SRCDIR)%.c $(EXT) | $(OBJDIR)
	$(CC) -MM $< -MT $(subst .d,.o,$@) -MF $@

# .dpp file
$(OBJDIR)%.dpp: $(TSTDIR)%.cpp $(EXT) | $(OBJDIR)
	$(CXX) -MM $< -MT $(subst .dpp,.opp,$@) > $@

# folders
$(OBJDIR) $(DOCDIR) $(LIBDIR):
	mkdir $@

# external files
$(LIBDIR)catch.hpp: | $(LIBDIR)
	wget -q "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp" -O $@

