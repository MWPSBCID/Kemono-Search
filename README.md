# Use
If run without arguments it will create an output.txt file and an output.json
file in the directory in which it is run.

## Command line arguments:

-t ... Shows time information

-u ... Gets usernames of posters

-d ... Shows debug information

-o [output.txt] ... Sets output file for regular output

-j [output.json] ... Sets output file for JSON output

-s [search term] ... Sets the search term and disables search term input

-f [filter term] ... Sets the filter term and disables filter term input

## JSON output

The JSON output file can be used with the index.html file in the tools
directory.

Note: The -u argument must be set for the JSON file to be outputed.

# Dependencies

- curl
- curlpp
- gcc

# Compilation

KemonoSearch is compiled with make using the provided makefile
