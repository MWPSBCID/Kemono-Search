
KemonoSearch: main.cpp
	g++ -o KemonoSearch main.cpp -lcurlpp -lcurl


clean:
	rm KemonoSearch
	rm output.txt
	rm website.json
