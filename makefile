SRCS := main.cpp parser.cpp jsonExporter.cpp stringFuncs.cpp globalVariables.cpp userFunctions.cpp
LIBS := -lcurlpp -lcurl
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(OBJS:%.o=%.d)

DEPFLAGS := -MMD -MP


KemonoSearch: $(OBJS)
	mkdir -p build
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
	rm -f output.txt
	rm -r -f build/*
