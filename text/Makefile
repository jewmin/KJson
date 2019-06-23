


TARGET=libKJson.a
LIBPATH=lib
CXXFLAGS= -g -Wall -static
CPP=g++

.PHONY : clean 

text : text.cpp $(TARGET) 
	@$(CPP) $^ $(TARGET) $(CXXFLAGS) -o $@
	@./$@
	@$(RM) $@

$(TARGET) : 
	@cp ../lib/$(TARGET) $(TARGET)

clean : 
	@$(RM) *.o
	@$(RM) *.a
	@$(RM) text