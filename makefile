SRCS := main.cpp parser.cpp jsonExporter.cpp stringFuncs.cpp globalVariables.cpp userFunctions.cpp
LIBS := -lcurlpp -lcurl
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(OBJS:%.o=%.d)

DEPFLAGS := -MMD -MP


KemonoSearch: $(OBJS)
	g++ -o build/$@ $^ $(LIBS)
	rm -f $(DEPS)
	rm -f $(OBJS)

$(OBJS) : %.o : %.cpp
	g++ $(DEPFLAGS) -c $< $(LIBS)

-include $(DEPS)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f KemonoSearch
	rm -f output.txt
	rm -f website.json
	rm -f user.json
	rm -f output.json
	rm -f *.txt
	rm -r -f build/*
