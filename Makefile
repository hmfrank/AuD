ARC = libaud.a
TST = utest

INCDIR = inc/
SRCDIR = src/

INC = $(wildcard $(INCDIR)*.h)
SRC = $(wildcard $(SRCDIR)*.c)

# phony targets
.PHONY: all clean destroy doc test

all: $(ARC)


clean:
	rm -rf $(ARC) $(TST)
	$(MAKE) -f MakefileAuD clean
	$(MAKE) -f MakefileTest clean

destroy: clean
	$(MAKE) -f MakefileAuD destroy
	$(MAKE) -f MakefileTest destroy

doc: $(SRC) $(INC) Doxyfile | $(DOCDIR)
	doxygen

test: $(TST)

# create library archive
$(ARC):
	$(MAKE) -f MakefileAuD all

# create unit test executable
$(TST):
	$(MAKE) -f MakefileTest all

# folders
$(DOCDIR):
	mkdir $@

