EXEDIR := scripts
OBJDIR := bin
SRCDIR := src
INCDIR := inc
MAKEDIR := bin
LIBFILE := $(OBJDIR)/libStatObj.a

CXX := $(shell root-config --cxx)
EXTRA_WARNINGS := -Wcast-align -Wcast-qual -Wdisabled-optimization -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -Winit-self -Winvalid-pch -Wlong-long -Wmissing-format-attribute -Wmissing-include-dirs -Wmissing-noreturn -Wpacked -Wpointer-arith -Wredundant-decls -Wstack-protector -Wswitch-default -Wswitch-enum -Wundef -Wunused -Wvariadic-macros -Wwrite-strings -Wabi -Wctor-dtor-privacy -Wnon-virtual-dtor #-Wstrict-null-sentinel -Wsign-promo -Wsign-compare #-Wunsafe-loop-optimizations -Wfloat-equal -Wsign-conversion -Wunreachable-code
CXXFLAGS := -isystem $(shell root-config --incdir) -Wall -Wextra -pedantic -Werror -Wshadow -Woverloaded-virtual -Wold-style-cast $(EXTRA_WARNINGS) $(shell root-config --cflags) -O2 -I $(INCDIR)
LD := $(shell root-config --ld)
LDFLAGS := $(shell root-config --ldflags)
LDLIBS := $(shell root-config --libs) -lMinuit

EXECUTABLES := $(addsuffix .exe, $(notdir $(basename $(wildcard $(SRCDIR)/*.cxx))))
OBJECTS := $(addprefix $(OBJDIR)/, $(addsuffix .o, $(notdir $(basename $(wildcard $(SRCDIR)/*.cpp)))))

vpath %.cpp $(SRCDIR)
vpath %.cxx $(SRCDIR)
vpath %.hpp $(INCDIR)
vpath %.o $(OBJDIR)
vpath %.exe $(EXEDIR)
vpath %.d $(MAKEDIR)

all: $(EXECUTABLES)

-include $(addsuffix .d,$(addprefix $(MAKEDIR)/,$(notdir $(basename $(wildcard $(SRCDIR)/*.cpp)))))
-include $(addsuffix .d,$(addprefix $(MAKEDIR)/,$(notdir $(basename $(wildcard $(SRCDIR)/*.cxx)))))

$(LIBFILE): $(OBJECTS)

$(MAKEDIR)/%.d: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -MM -MG -MF $@ $< 
	sed -i'' 's#$*.o#$(OBJDIR)/$*.o $(MAKEDIR)/$*.d#g' $@

$(MAKEDIR)/%.d: $(SRCDIR)/%.cxx
	$(CXX) $(CXXFLAGS) -MM -MG -MF $@ $< 
	sed -i'' 's#$*.o#$(OBJDIR)/$*.o $(MAKEDIR)/$*.d#g' $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/%.o: $(SRCDIR)/%.cxx
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OBJDIR)/%.a:
	ar rcsv $@ $^

%.exe: $(OBJDIR)/%.o $(LIBFILE)
	$(LD) $(LDFLAGS) -o $(EXEDIR)/$@ $^ $(LDLIBS)

.PHONY: clean

clean:
	-rm -rf $(EXEDIR)/*.exe $(OBJDIR)/*.o $(OBJDIR)/*.a $(MAKEDIR)/*.d *.exe *.out
	./scripts/remove_backups.sh
