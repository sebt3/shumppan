PRJ     := eflge
BLD_DBG := YES
RM      := rm -f
MKDIR   := mkdir -p
LEDITOR := kate
SOUND   := OPENAL


# set build and target directory based on plateform
BLDD    := build
COMMON  := common
PLAYER  := runtime
PACKER  := packer
SHMUPE  := shumpege
DATAFSHU:= data_shump.eet
DATASHU	:= data
TARGET  := ../target

PL_LDFLAGS=-lGL
ifneq "$(findstring DPANDORA,$(CFLAGS))" ""
	BLDD   = build.pand
	TARGET = ../target.pand
	COMMON+=egl
	PL_LDFLAGS=-lGLES_CM -lEGL -lX11
else 
  ifneq  "$(findstring DCAANOO,$(CFLAGS))" ""
	BLDD   = build.caan
	TARGET = ../target.caan
	COMMON+=egl
	PL_LDFLAGS=-lts -lglport -lopengles_lite
  else
	CFLAGS+=-Wno-unused-but-set-variable
  endif
endif
ifeq ($(BLD_DBG), YES)
	CFLAGS += -g -DDEBUG
else
	CFLAGS += -03
endif
ifeq ($(SOUND), OPENAL)
	CFLAGS += -DUSE_OPENAL
	LDFLAGS += -lopenal -lvorbisfile -logg -lmodplug
else
	LDFLAGS += -lSDL_mixer
endif

INC_DIR := $(COMMON) $(TARGET)/include
SOURCES := $(COMMON) $(PLAYER) $(PACKER) $(SHMUPE)

LIB_DIR := $(TARGET)/lib
BIN_DIR := $(TARGET)/bin

EXE_SHU := $(BIN_DIR)/$(SHMUPE)
EXE_PCK := $(BIN_DIR)/$(PRJ).pack
BLDDCMN := $(BLDD)/common
BLDDPCK := $(BLDD)/packer
BLDDRUN := $(BLDD)/runtime
BLDDSHU := $(BLDD)/shumpege
COUNTER := $(BLDD)/.cnt.sh
OBJCMN  := $(foreach dir, $(COMMON),   $(patsubst $(dir)/%.c,  $(BLDDCMN)/%.o,$(wildcard $(dir)/*.c))) 
OBJPCK  := $(foreach dir, $(PACKER),   $(patsubst $(dir)/%.c,  $(BLDDPCK)/%.o,$(wildcard $(dir)/*.c))) 
OBJRUN  := $(foreach dir, $(PLAYER),   $(patsubst $(dir)/%.c,  $(BLDDRUN)/%.o,$(wildcard $(dir)/*.c))) 
OBJSHU  := $(foreach dir, $(SHMUPE),   $(patsubst $(dir)/%.c,  $(BLDDSHU)/%.o,$(wildcard $(dir)/*.c))) 
SRCFILES:= $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.c) $(wildcard $(dir)/*.h)) $(foreach dir, $(DATASHU), $(wildcard $(dir)/*.txt) $(wildcard $(dir)/*.emap))
OBJS    := $(OBJCMN) $(OBJRUN) $(OBJSHU) $(OBJPCK)

JUNK    := $(shell find . -name '*~')

LDFLAGS += $(foreach dir, $(LIB_DIR), -L$(wildcard $(dir))) -lm  $(shell sdl-config --libs) -lSDL_ttf -lSDL_image $(shell pkg-config --libs eet eina)
CFLAGS  += $(shell pkg-config --cflags eet eina) $(shell sdl-config --cflags) -Wall
CFLAGS  += $(foreach dir, $(INC_DIR), -I$(wildcard $(dir)))
SHOW_BLD_CMN = printf "[%3d%%] \033[32mBuilding common file \033[1m%s\033[0m\n"  `$(COUNTER)`
SHOW_BLD_RUN = printf "[%3d%%] \033[32mBuilding runtime file \033[1m%s\033[0m\n"  `$(COUNTER)`
SHOW_BLD_PCK = printf "[%3d%%] \033[32mBuilding packer file \033[1m%s\033[0m\n"  `$(COUNTER)`
SHOW_BLD_SHU = printf "[%3d%%] \033[32mBuilding shumpege file \033[1m%s\033[0m\n"  `$(COUNTER)`
SHOW_GEN_MAP = printf "[%3d%%] \033[32mUpdating tilemap file \033[1m%s\033[0m\n"  `$(COUNTER)`
SHOW_LNK     = printf "[%3d%%] \033[34mLinking binary \033[34;1m%s\033[0m\n"  `$(COUNTER)`
SHOW_SUCESS  = printf "[%3d%%] \033[33;1mSuccessfully build target \033[0;33m%s\033[0m\n"  `$(COUNTER)`
SHOW_STARTED = printf "[%3s%%] \033[33;1mEditor started\033[0m\n"  "---"
SHOW_GAMESTT = printf "[%3s%%] \033[33;1mGame starting\033[0m\n"  "---"
SHOW_PACKING = printf "[%3s%%] \033[33;1mPacking the data \033[0;33m%s\033[0m\n"  "---"


all: $(BLDD) $(COUNTER) $(BLDDCMN) $(BLDDRUN) $(BLDDPCK) $(BLDDSHU) $(OBJ) $(EXE_SHU) $(EXE_PCK)
	@$(SHOW_SUCESS) all
runtest: $(COUNTER) $(EXE_SHU) $(DATAFSHU)
	@$(SHOW_GAMESTT)
	$(EXE_SHU) $(DATAFSHU)

edit:
	@$(LEDITOR) Makefile $(SRCFILES) >/dev/null 2>&1 &
	@$(SHOW_STARTED)

$(DATAFSHU): $(DATASHU)/* $(EXE_PCK)
	@$(SHOW_PACKING) $@
	@rm -f $(DATAFSHU)
	@LD_LIBRARY_PATH=/usr/local/lib $(EXE_PCK) $(DATAFSHU) $(DATASHU)
$(EXE_PCK):$(BLDDCMN) $(BLDDPCK) $(OBJCMN) $(OBJPCK)
	@$(SHOW_LNK) $@
	@$(CC) $(OBJCMN) $(OBJPCK) -o $(EXE_PCK) $(LDFLAGS) $(PL_LDFLAGS)

$(BLDDPCK)/%.o: $(PACKER)/%.c
	@$(SHOW_BLD_PCK) $<
	@$(CC) $(CFLAGS) -I$(PACKER) -c $< -o $@

$(EXE_SHU):$(BLDDCMN) $(BLDDRUN) $(BLDDSHU) $(OBJCMN) $(OBJRUN) $(OBJSHU)
	@$(SHOW_LNK) $@
	@$(CC) $(OBJCMN) $(OBJRUN) $(OBJSHU) -o $(EXE_SHU) $(LDFLAGS) $(PL_LDFLAGS)

$(BLDDSHU)/%.o: $(SHMUPE)/%.c common/*.h $(PLAYER)/*.h $(SHMUPE)/*.h
	@$(SHOW_BLD_SHU) $<
	@$(CC) $(CFLAGS) -I$(PLAYER) -I$(SHMUPE) -c $< -o $@

$(BLDDRUN)/%.o: $(PLAYER)/%.c common/*.h $(PLAYER)/*.h
	@$(SHOW_BLD_RUN) $<
	@$(CC) $(CFLAGS) -I$(PLAYER) -c $< -o $@

$(BLDDCMN)/%.o: common/%.c common/*.h
	@$(SHOW_BLD_CMN) $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(BLDDCMN)/%.o: egl/%.c egl/*.h
	@$(SHOW_BLD_CMN) $<
	@$(CC) $(CFLAGS) -DUSE_GLES1 -DUSE_EGL_SDL -c $< -o $@

$(DATASHU)/%.map: $(DATASHU)/%.tmx
	@$(SHOW_GEN_MAP) $@
	@./gentilemap.sh $< $@

$(COUNTER): Makefile $(SRCFILES) $(BLDD)
	@echo "#!/bin/sh">$@
	@echo 'expr 100 "*" `ls -1d $(BLDDCMN)/*.o $(BLDDPCK)/*.o $(BLDDSHU)/*.o $(BLDDRUN)/*.o $(EXE_PCK) $(EXE_SHU) 2>/dev/null|wc -l` "/" `echo $(OBJS) $(EXE_PCK) $(EXE_SHU)|wc -w`' >> $@
	@chmod 700 $@

$(BLDD):
	@$(MKDIR) $(BLDD)

$(BLDDRUN): $(BLDD)
	@$(MKDIR) $(BLDDRUN)

$(BLDDPCK): $(BLDD)
	@$(MKDIR) $(BLDDPCK)

$(BLDDSHU): $(BLDD)
	@$(MKDIR) $(BLDDSHU)

$(BLDDCMN): $(BLDD)
	@$(MKDIR) $(BLDDCMN)

clean:
	@$(RM) $(OBJS) $(EXE_ED) $(EXE_SHU) $(JUNK) $(BLDD)/.cnt.sh
	@$(RM) -r $(BLDD)

.PHONY:clean all editor player runtest edit
.PRECIOUS: $(OBJS)
.SUFFIXES: .o .c .h .S .cpp .eet
