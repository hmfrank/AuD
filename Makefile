# main program executable
EXE = aud

OBJDIR = bin/
DOXDIR = doc/
LIBDIR = lib/
SRCDIR = src/
TSTDIR = tst/

INC = $(wildcard $(SRCDIR)*.h)
SRC = $(wildcard $(SRCDIR)*.c) $(wildcard $(TSTDIR)*.cpp)
OBJ = $(filter %.o, $(SRC:$(SRCDIR)%.c=$(OBJDIR)%.o)) \
      $(filter %.opp, $(SRC:$(TSTDIR)%.cpp=$(OBJDIR)%.opp))
DEP = $(filter %.d, $(SRC:$(SRCDIR)%.c=$(OBJDIR)%.d)) \
      $(filter %.dpp, $(SRC:$(TSTDIR)%.cpp=$(OBJDIR)%.dpp))

# C compiler and linker flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror
LDFLAGS =

# C++ compiler and linker flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror
LXXFLAGS =

.PHONY: all clean destroy


all: $(EXE)

clean:
	rm -rf $(EXE) $(OBJDIR)

destroy: clean
	rm -rf $(LIBDIR) $(DOCDIR)


# create documentation
doc: $(SRC) $(INC) | $(DOCDIR)
        doxygen

# link object files
$(EXE): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ $(LXXFLAGS) -o $@

# .o file
$(OBJDIR)%.o: $(SRCDIR)%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

# .opp file
$(OBJDIR)%.opp: $(TSTDIR)%.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

-include $(DEP)

# .d file
$(OBJDIR)%.d: $(SRCDIR)%.c | $(OBJDIR)
	$(CC) -MM $< -MT $(subst .d,.o,$@) -MF $@

# .dpp file
$(OBJDIR)%.dpp: $(TSTDIR)%.cpp | $(OBJDIR)
	$(CXX) -MM -MG $< -MT $(subst .dpp,.opp,$@) | sed 's/ /\\\n/g' | sed 's/^..\//src\/..\//g' | sed 's/.*\/\.\.\///g' > $@ # regex magic

# folders
$(OBJDIR) $(LIBDIR) $(DOCDIR):
	mkdir $@

# catch single header file
$(LIBDIR)catch.hpp: | $(LIBDIR)
	wget "https://raw.githubusercontent.com/philsquared/Catch/master/single_include/catch.hpp" -O $@
