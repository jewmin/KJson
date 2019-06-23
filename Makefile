


TARGET=libKJson.a
CPP=g++
OBJPATH=obj
LIBPATH=lib
SRCPATH=src
TSRCPATH=text

.PHONY : default all clean text

default : all
ifneq ($(LIBPATH), $(wildcard $(LIBPATH)))
	mkdir $(LIBPATH)
endif
ifneq ($(OBJPATH), $(wildcard $(OBJPATH)))
	mkdir $(OBJPATH)
endif
	@mv $(SRCPATH)/*.o $(OBJPATH)
	@mv $(SRCPATH)/*.a $(LIBPATH)
	
all : 
	@cd $(SRCPATH) && $(MAKE) $@
	
text :
	@cd $(TSRCPATH) && $(MAKE) $@

clean : 
	@cd $(SRCPATH) && $(MAKE) $@
	@cd $(LIBPATH) && $(RM) *.a
	@cd $(OBJPATH) && $(RM) *.o
	@cd $(TSRCPATH) && $(RM) *.a