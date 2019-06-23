


TARGET=libKJson.a
CPP=g++
STATICLIB=ar -r
CXXFLAGS=-c -g -Wall 
SOURCES=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, %.o, $(SOURCES))

LIBPATH=lib
OBJPATH=obj

.PHONY : all clean

all : $(OBJS)
	@$(STATICLIB) $(TARGET) $(OBJS)
$(OBJS) : $(SOURCES)
	@$(CPP) $*.cpp $(CXXFLAGS)
	 
clean : 
	@$(RM) *.o
	@$(RM) *.a