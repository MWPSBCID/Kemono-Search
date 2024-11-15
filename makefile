SRCS := main.cpp parser.cpp
LIBS := -lcurlpp -lcurl
OBJS := $(SRCS:%.cpp=%.o)
DEPS := $(OBJS:%.o=%.d)

DEPFLAGS := -MMD -MP


KemonoSearch: $(OBJS)
	g++ -o $@ $^ $(LIBS)

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
	rm -f *.txt
