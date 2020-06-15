/**
 * @file 
 *
 * file contains all the definitions of the functions listed in history.h
 * including: print_history 
 * 	      get_by_count
 * 	      get_by_name
 * 	      get_last 
 * 	      next_token
 * */
#include "history.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "debug.h"
#include <sys/wait.h>

/**
 * function will print history of commands entered 
 * in to the shell given the head 
 *
 * @Param head
 * @Param count 
 **/
void print_history(struct history_entry *head, int count)
{
	struct history_entry *temp = head;
	while (temp != NULL)
	{
			printf("%d %s", temp->cmd_id, temp->command);
			temp = temp->next;
	}
}

/**
 * Function will print prompt using getLogin,
 * gethostname and appropriate emoji.
 *
 * @Param ran 
 * @Param count
 *
 **/
void print_prompt(bool ran, int count)
{
	
	
	char buffer[20];
	gethostname(buffer, sizeof(buffer));
	if (ran == true)
	{
		char martini[] = { 0xF0, 0x9F, 0x8D, 0xB8, '\0' };
		printf("%s", martini);
	}
	else
	{
		char angry[] = { 0xF0, 0x9F, 0x98, 0xA1, '\0' };
		printf("%s", angry);
	}
	char cwd[20];
	printf("[%i]-[(%s)@(%s):", count, getlogin(), buffer);
	getcwd(cwd, 20);
	if (strcmp(cwd, getenv("HOME")) == 0)
	{
		printf("~ ]$ ");
	}
	else
	{
		//printf("%s \n", cwd);
		printf("- %s ]$ ", cwd);
	}
	fflush(stdout);

}/**
 * Function to catch whenever user 
 * presses ctrl + c
 *
 * @Param signo
 **/
void sig_handler(int signo)
{
	printf("\n");
}

/**
 * Function to 
 * @Param signo 
 **/
void sig_jobs(int signo)
{
	pid_t child;
	int status;

	while ((child = waitpid(-1, &status, WNOHANG)) != -1)
	{
		//	killthe(child);
		break;
	}
}

/**
 * Function will receive struct with commands inside to execute 
 * along with the count of pipes with count of pipes
 *
 * @Param com
 * @Param count 
 **/
void execute_pipeline(struct history_entry *com, int count)
{
	int fd[2];
	int ret;
	if (pipe(fd) == -1)
	{
		perror("pipe");
		return;
	}

	if (com->cmds[count].stdout_pipe == false)
	{
		if (com->cmds[count].stdout_file == NULL)
		{
			ret = execvp(com->cmds[count].tokens[0], com->cmds[count].tokens);
		}
		else
		{
			int f = open(com->cmds[count].stdout_file, O_WRONLY | O_TRUNC | O_CREAT, 0664);
			dup2(f, STDOUT_FILENO);
			ret = execvp(com->cmds[count].tokens[0], com->cmds[count].tokens);
		}
	}
	pid_t pid = fork();

	if (pid == 0)
	{
		close(fd[0]);
		dup2(fd[1], STDOUT_FILENO);
		execvp(com->cmds[count].tokens[0], com->cmds[count].tokens);
		close(fd[0]);
	}
	else
	{
		close(fd[1]);
		dup2(fd[0], STDIN_FILENO);
		close(fd[1]);
		execute_pipeline(com, count + 1);
	}
	close(fd[0]);
	close(fd[1]);
	if (ret == -1)
	{
		perror("execvp");
		return;
	}
}

/**Function to remove a job from the job array
 *
 * @Param child 
 **/
//void killthe(pid_t child)
//{
//	int i;
//	for (i = 0; i < 10; i++)
//	{
//		if (jobs[i].command != NULL)
//		{
//			if (child = jobs[i].id)
//			{
//				free(job[i].tokens);
//			}
//		}
//	}
//}
/**
 * Function that will return a char pointer depending 
 * on the count given
 *
 * @Param head
 * @Param count
 * @return char pointer
 **/
char* get_by_count(struct history_entry *head, char* count)
{
	struct history_entry *temp = head;

	while (temp != NULL)
	{
		if (temp->cmd_id == atoi(count))
		{
			return strdup(temp->command);
		}
		temp = temp->next;
	}
	return NULL;
}
/**
 * function will get the most recent command executed by 
 * the shell
 *
 * @Param head
 * @return char pointer
 **/
char* get_last(struct history_entry *head)
{
	struct history_entry *temp = head;

	while (temp != NULL)
	{
		if (temp->next == NULL)
		{
			return strdup(temp->command);
		}
		temp = temp->next;
	}
	return NULL;
}
/**
 * Function that will get a command if it has the same token
 * or letter. 
 *
 * @Param head
 * @Param name 
 * @return char array 
 **/
char* get_by_name(struct history_entry *head, char* name, int count)
{
	struct history_entry *temp = head;
	char *final = NULL;


//	name[strlen(name) - 1] = '\0';
	while (temp != NULL)
	{
//		char *string = strdup(temp->command);
		//if (strstr(string, name) != NULL && strstr(string, name) == string)
		
		if (strncmp(temp->command, name, strlen(name) - 1) == 0)
		{
			LOG("%s %s \n", temp->command, name);
			final = temp->command;
		}
		temp = temp->next;
	//	free(string);
	}
	if (final == NULL)
	{
		char *notNull = "Corndogs";
		final = notNull;
	}
	return strdup(final); 
}

/**
 * Function to get the next char pointer
 *
 * @Param str_ptr
 * @Param delim
 * @return char pointer to next token
 **/
char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    /* Zero length token. We must be finished. */
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    /* Take note of the start of the current token. We'll return it later. */
    char *current_ptr = *str_ptr + tok_start;

    /* Shift pointer forward (to the end of the current token) */
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        /* If the end of the current token is also the end of the string, we
         * must be at the last token. */
        *str_ptr = NULL;
    } else {
        /* Replace the matching delimiter with a NUL character to terminate the
         * token string. */
        **str_ptr = '\0';

        /* Shift forward one character over the newly-placed NUL so that
         * next_pointer now points at the first character of the next token. */
        (*str_ptr)++;
    }

    return current_ptr;
}
