# Project 2: Command Line Shell

Writer: Hayden Miller
University of San Francisco

## ABOUT 
This project emulates a UNIX shell similar to Bash. It will do execute basic command 
line functionality, and will allow piping and redirection. It also allows for several
built in Bash in commands like cd, setenv, and history

## INCLUDED FILES 
-history.h - header file with all struct/prototype definitions

-history.c - file with all functions that our driver function calls

-shell.c   - driver file that contains main

To compile

```bash
make 
./crash
```

## TESTING 

To execute, use `make test`. To pull in updated test cases, run `make testupdate`. You can also run a specific test case instead of all
at once 

```
# Run all

make test

# Run a specific test case:

make test run=20
```
