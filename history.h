/*
 * @file 
 *
 * header file that contains the declarations for our structs 
 * used in the shell as well as the function prototypes 
*/


#ifndef _HISTORY_H_
#define _HISTORY_H_

#define HIST_MAX 100
#include <stdbool.h>
/**
 * struct containing tokens of commands 
 * and whether it is a pipe 
 * and whether it is piping in to a file
 **/
struct command_line {
	char **tokens;
	bool stdout_pipe;
	char *stdout_file;
};


/**
 * struct for background jobs
 **/
struct jobs {
	char *tokens;
	int id;

};



//struct jobs job[10]= { 0 };
//int numjobs = 0;



/**
 * struct that contains id of command and the 
 * command char pointer as well as the nested 
 * structure containg the tokenized commands 
 * and further information
 **/
struct history_entry {
    int cmd_id;
    char *command;
    
    struct command_line cmds[200];


    struct history_entry *next;
   
};



/**
 * Function prototypes
 **/
void print_history();
char* get_last(struct history_entry *head);
char* get_by_count(struct history_entry *head, char* count);
char* get_by_name(struct history_entry *head, char* name, int count);
char *next_token(char **str_ptr, const char *delim);
void execute_pipeline(struct history_entry *com, int count);
void sig_jobs(int signo);
void sig_handler(int signo);
void print_prompt(bool ran, int count);
//void killthe(pid_t child);
#endif
