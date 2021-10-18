#******************************************************************************#
#Paramètres

TARGETS       = chess

SILENT       ?= 1
DOTESTS      ?= 0
DEBUG        ?= 0

CC            = g++
EXT           = cpp

DEPDIR        = .d/
SRCDIR        = src/
OBJDIR        = obj/
TESTDIR       = tests/

PKGLIST       = gtkmm-3.0

CFLAGS        = -Wall -Wextra -std=c++17 -iquote $(SRCDIR) -DUSE_RESOURCES
CFLAGS_DBG    = -ggdb3 -O0
CFLAGS_RLS    = -O3

LDFLAGS       =
LDFLAGS_DBG   =
LDFLAGS_RLS   =

#******************************************************************************#
# Configuration

ifeq ($(OS), Windows_NT)
  RELEASEDIR  = windows/
  DEBUGDIR    = windows-debug/
else
  RELEASEDIR  = linux/
  DEBUGDIR    = linux-debug/
  LDFLAGS    += -lpthread
endif

ifeq ($(SILENT), 1)
  S = @
else
  S =
endif

ifeq ($(DEBUG), 1)
  CFLAGS     += $(CFLAGS_DBG)
  LDFLAGS    += $(LDFLAGS_DBG)
  BUILDDIR    = $(DEBUGDIR)
else
  CFLAGS     += $(CFLAGS_RLS)
  LDFLAGS    += $(LDFLAGS_RLS)
  BUILDDIR    = $(RELEASEDIR)
endif

ifdef PKGLIST
  ifeq (0, $(shell pkg-config --exists $(PKGLIST) && echo 1 || echo 0))
    $(error Missing pkg-config or invalid package in '$(PKGLIST)')
  endif
    CFLAGS   += $(shell pkg-config --cflags $(PKGLIST))
    LDFLAGS  += $(shell pkg-config --libs $(PKGLIST))
endif

OBJDIR       := $(BUILDDIR)$(OBJDIR)
DEPDIR       := $(BUILDDIR)$(DEPDIR)

DIRS          = $(shell tree -Ffi $(SRCDIR) | grep /$$) $(SRCDIR)
SRCS          = $(foreach dir, $(DIRS), $(wildcard $(dir)*.$(EXT)))

TESTS         = $(basename $(subst $(SRCDIR), , $(wildcard $(SRCDIR)$(TESTDIR)*.$(EXT))))

TARGETSOBJ    = $(foreach target, $(TARGETS) $(TESTS), $(OBJDIR)$(target).o)
OBJS          = $(filter-out $(TARGETSOBJ), $(SRCS:$(SRCDIR)%.$(EXT)=$(OBJDIR)%.o))
DEPS          = $(SRCS:$(SRCDIR)%.$(EXT)=$(DEPDIR)%.d)

DEPFLAGS      = -MT $@ -MMD -MF $(DEPDIR)$*.Td
POSTCOMPILE   = mv -f $(DEPDIR)$*.Td $(DEPDIR)$*.d && touch $@

$(shell mkdir -p $(BUILDDIR)$(TESTDIR) $(subst $(SRCDIR), $(DEPDIR), $(DIRS)) $(subst $(SRCDIR), $(OBJDIR), $(DIRS))>/dev/null)

TOBUILD       = $(TARGETS)
ifeq ($(DOTESTS), 1)
  TOBUILD    += $(TESTS)
endif
TOBUILD      := $(addprefix $(BUILDDIR), $(TOBUILD))

#******************************************************************************#
# Les règles

.PHONY: all clean info backup

all: $(TOBUILD)

info:
	@echo OS: $(OS) && echo
	@echo Targets: $(TARGETS) && echo
	@echo Sources: $(SRCS) && echo
	@echo Tagets Obj: $(TARGETSOBJ) && echo
	@echo Objets: $(OBJS) && echo
	@echo Tests: $(TESTS) && echo
	@echo Deps: $(DEPS) && echo
	@echo Dirs: $(DIRS) && echo

clean:
	@echo Cleanning everything...
	$(S)rm -rf *~ $(RELEASEDIR) $(DEBUGDIR)
	@echo Done

backup:
	@echo Backing up project
	@name=$$(basename `pwd`) && \
	cd .. && \
	fname=$$name.`date +"%Y%m%d-%H%M%S"`.tgz && \
	tar czvf $$name/$$fname $$name/Makefile $$name/$(SRCDIR) && \
	cd $$name && \
	echo -n "$$fname : " && \
	ls -lh $$fname


$(DEPDIR)%.d: ;
-include $(DEPS)

$(OBJDIR)%.o: $(SRCDIR)%.$(EXT)

$(OBJDIR)%.o: $(SRCDIR)%.$(EXT) $(DEPDIR)%.d
	@echo Compiling $*.$(EXT)...
	$(S)$(CC) -c $(SRCDIR)$*.$(EXT) -o $(OBJDIR)$*.o $(DEPFLAGS) $(CFLAGS)
	$(S)$(POSTCOMPILE)

$(TOBUILD): $(BUILDDIR)% : $(OBJDIR)%.o $(OBJS)
	@echo Building $@...
	$(S)$(CC) -o $@ $^ $(LDFLAGS)
	@echo Done

################################################################################
# Régles spécifiques au projet

.PHONY: resources

resources:
	@echo Writing resources.$(EXT)...
	@glib-compile-resources --target=$(SRCDIR)resources.$(EXT) --sourcedir=resources/ --generate-source resources.xml
