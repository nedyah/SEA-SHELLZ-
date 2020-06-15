/**
 * @file
 *
 * Main file that contains our shell program. Designed 
 * to emulate a bash-like shell with built in support 
 * for various commands and allows for piping and redirection
 *
*/


#include <fcntl.h>
#include <locale.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include "debug.h"
#include "history.h"

#define ARG_MAX 4096

// issues: 
// -!ls 
// -& background processes 
// -jobs
// -Leak 
 





/**
 * Main function that will loop until user specifies an exit.
 * Designed to emulate a shell program that will call execvp to 
 * execute commands
 **/
int main(void)
{

    LOGP("Initializing shell\n");
    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    struct history_entry *head = NULL;

    bool ran = true;
    unsigned int count = 0;

    signal(SIGINT, sig_handler);
    signal(SIGCHLD, sig_jobs);
    
    while (true)
    {
	char *args[4097] = { 0 };
	int tokens = 0;	
	if (isatty(STDIN_FILENO))
    	{
		/*    Interactive mode; turn on the prompt */
    		print_prompt(ran, count);
	}
		
	char *line =  0;
	size_t line_sz = 0;
	if (getline(&line, &line_sz, stdin) == -1)
	{
		break;
	}
	if (line[0] == '!')
	{
		if (line[1] == '!')
		{
			if ((line = get_last(head)) == NULL)
			{
				free(line);
				continue;
			}
		}
		else if (isdigit(line[1]))
		{
			char *tok = next_token(&line, "!");

			if (atoi(tok) > count || atoi(tok) < (count - 100))
			{
				free(line);
				continue;
			}	
			else if ((line = get_by_count(head, tok)) == NULL)
			{
				free(line);
				continue;		
			}	
			
		}
		else if (isalpha(line[1]))
		{
			char *tok = next_token(&line, "!");

			if (strcmp((line = get_by_name(head, tok, count)), "Corndogs") == 0)
			{
				
				free(line);
				continue;
			}
						
		}	
	}
	char *oLine = strdup(line);

	struct history_entry *temp = (struct history_entry*) malloc(sizeof(struct history_entry));
	int start = tokens;
	char *next_tok = line;
	char *curr_tok;
	int i = 0;
	bool hidden = false;
	while ((curr_tok = next_token(&next_tok, " \t\r\n")) != NULL)
	{
//		LOG("%s \n", curr_tok);		
		if (tokens >= ARG_MAX - 1)
		{
			break;
		}
		if (*curr_tok == '|')
		{
			temp->cmds[i].stdout_pipe = true;
			
			temp->cmds[i].tokens = &args[start];
		
			temp->cmds[i].tokens[tokens++] = NULL;
			i++;
			temp->cmds[i].stdout_pipe = false;
			start = tokens;
		
		}
		else if (*curr_tok == '>')
		{
			temp->cmds[i].stdout_file = next_token(&next_tok, " \t\r\n");
			break;
		}
		else if (*curr_tok == '#')
		{
			if (args[start] == 0)
			{
				args[start] = curr_tok;	
			}
			break;		
		}
		else if (curr_tok[0] == '$')
		{
			char *str = next_token(&curr_tok, "$");
			char *variables = getenv(str);
			if (variables == NULL)
				break;
			while ((curr_tok = next_token(&variables, " \t\r\n")) != NULL)
			{
				args[tokens++] = curr_tok;
			}

		}	
		else 
		{
			if (*curr_tok == '&')
			{
			//	job[numjobs].id = numjobs + 1;
			//	job[numjobs++].tokens = strdup(oLine);	
				hidden = true; 
			}
			else
				args[tokens++] = curr_tok;
		}
	}
	if (start != tokens)
		args[tokens] = NULL;
	temp->cmds[i].tokens = &args[start];
	temp->cmds[i].stdout_pipe = false;

	if (args[start] == 0)
	{	
		free(temp);
		free(oLine);
		free(line);
		continue;	
	}
	if (strcmp(args[start], "setenv") == 0)
	{
		if (args[start + 1] == 0 || args[start + 2] == 0)
		{
			free(oLine);
			free(line);
			free(temp);
			continue;
		}
		else
		{
			if (setenv(args[start + 1], args[start + 2], 1))
			{
				free(oLine);
				free(line);
				free(temp);
				continue;	
			}

		}
	}

	temp->cmd_id = count;
	temp->command = strdup(oLine);
	temp->next = NULL;
	free(oLine);
	if (head == NULL)
	{
		head = temp;
	}	
	else
	{
		struct history_entry *iter = head;
		while (iter != NULL)
		{
			if (iter->next == NULL)
			{
				iter->next = temp;
					break;
			}
			iter = iter->next;
		}
	}

	if (count >= 100)
	{
		struct history_entry *delete = head;
		head = head->next;
		free(delete->command);
		free(delete);
	}

//	if (strcmp("jobs", args[start]) == 0)
//	{
//		int k = 0;
//		for (k = 0; k < 10; k++)
//		{
//			if (job[k].tokens != NULL)
//				printf("%s", job[k].tokens);
//			
//		}
//		free(line);
//		continue;
//	}
	
	if (strcmp("cd", args[start]) == 0)
	{
		if (args[start + 1] == NULL)
		{
			chdir(getenv("HOME"));
		}
		else if (chdir(args[start + 1]) != 0)
		{
			perror("");
		}
		free(line);
		count++;
		continue;
	}
	
	if (strcmp("exit", args[start]) == 0) 
	{
		while (head != NULL)
    		{
			struct history_entry *delete = head;
			char *killme = delete->command;
			head = head->next;
			free(killme);
			free(delete);
   		}		
		free(line);
		exit(EXIT_SUCCESS);
	}

	if (strcmp("history", args[start]) == 0)
	{
		print_history(head, count);
		count++;
		free(line);
		continue;		
	}

	pid_t child = fork();

	if(child == -1)
	{
		perror("fork");
	}
	else if (child == 0)
	{	
		execute_pipeline(temp, 0);	
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		exit(EXIT_FAILURE);
	}
	else
	{
		int status=0;
		if (hidden == false)
		{
			waitpid(child, &status, 0);
		}
		
		if (status != 0) 
		{
			ran = false;	
		}
		else
		{
			ran = true;
		}	
	}
	free(line);
	count++;
    }

    while (head != NULL)
    {
	struct history_entry *delete = head;
	char *killme = delete->command;
	head = head->next;
	free(killme);
	free(delete);
    }

    return 0;
}
