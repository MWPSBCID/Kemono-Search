
KemonoSearch: main.cpp
	g++ -o KemonoSearch main.cpp -lcurlpp -lcurl


clean:
	rm -f KemonoSearch
	rm -f output.txt
	rm -f website.json
	rm -f *.txt
