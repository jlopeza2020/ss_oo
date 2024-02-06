#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "exec_line.h"
#include "exec_commands.h"
#include "exec_command.h"
#include "optionals.h"
#include <err.h>
#include <errno.h>

// compare argument with EXIT, and if so, it ends the program
int
equal_strings(char *string1, char *string2)
{
	int i = 0;
	int equals = 0;

	while ((string1[i] == string2[i]) && (string1[i] != '\0')
	       && (string2[i] != '\0'))
		i++;
	if ((string1[i] == string2[i]) && (string1[i] == '\0')
	    && (string2[i] == '\0'))
		exit(0);
	return equals;
}

// obtain the number of commands that are going to be executed
int
get_positions(char *cmd, char **wrds_exec)
{
	char *token;
	char *rest = cmd;
	int i = 0;

	while ((token = strtok_r(rest, "|", &rest))) {
		wrds_exec[i] = token;
		i++;
	}
	wrds_exec[i] = NULL;

	return i;
}

void
process_output(char *string, int bg, int times_executed)
{

	char **tmparr;
	int total_pos;
	int i, j, pipe_flag;
	char redirec[MAX];
	char standarin[MAX];
	char *strswcmds;
	char *stdin_bg = "/dev/null";

	// OPTIONAL 2
	char *ifok = "ifok";
	char *ifnot = "ifnot";

	//if ifok == 1 means the programs uses_ifok
	int uses_ifok;

	//if ifnot == 1 means the programs uses_ifnot
	int uses_ifnot;

	int df_pos;

	int strswcmds_size;
	int pos_strwcmds;
	int h_red;
	int appear_equal;
	int can_execute;

	int no_heredoc;

	pipe_flag = 0;

	i = 0;			// increases position of the string 
	h_red = 0;
	appear_equal = 0;

	//used for correct sustitutions
	can_execute = 0;
	df_pos = 0;
	no_heredoc = 0;

	if (times_executed > 0 && string[0] == 'i' && string[1] == 'f') {
		// OPTIONAL 2 (ifok, ifnot ...)
		uses_ifok = cmp_strings(string, ifok, strlen(ifok));
		uses_ifnot = cmp_strings(string, ifnot, strlen(ifnot));
		can_execute = check_result(uses_ifok, uses_ifnot);
	}

	cd_builtin(string);

	while (string[i] != '\0' && can_execute == 0) {

		if (string[i] == '=') {
			//check if there is any space between the =
			if ((string[--i] != ' ') && (string[i = i + 2] != ' '))
				appear_equal = 1;
			else {
				fprintf(stderr, "incorrect equal \n");
				can_execute = 1;
			}
		}

		if (string[i] == '$' && df_pos == 0) {
			can_execute = sustitutions(string);
			df_pos++;
		}

		if (string[i] == '|') {
			pipe_flag = 1;
		}

		if (string[i] == '<' && h_red == 0) {
			no_heredoc = 1;
			h_red = 1;
			strswcmds_size = i;
			i++;

			if (string[i] == ' ')
				i++;

			for (j = 0;
			     string[i] != '\0' && string[i] != ' '
			     && string[i] != '|' && string[i] != '>'; j++) {
				standarin[j] = string[i];
				i++;
			}
			standarin[j] = '\0';
			stdin_red(standarin);

		} else if (string[i] == '<' && h_red == 1) {
			no_heredoc = 1;
			i++;

			if (string[i] == ' ')
				i++;

			for (j = 0;
			     string[i] != '\0' && string[i] != ' '
			     && string[i] != '|' && string[i] != '>'; j++) {
				standarin[j] = string[i];
				i++;
			}
			standarin[j] = '\0';
			stdin_red(standarin);
		}
		// if finds a >, that will be the file for the output (cut it)
		if (string[i] == '>' && h_red == 0) {
			no_heredoc = 1;
			h_red = 1;
			strswcmds_size = i;
			i++;

			if (string[i] == ' ')
				i++;

			for (j = 0;
			     string[i] != '\0' && string[i] != ' '
			     && string[i] != '|' && string[i] != '<'; j++) {
				redirec[j] = string[i];
				i++;
			}
			redirec[j] = '\0';
			stdout_red(redirec);
		} else if (string[i] == '>' && h_red == 1) {
			no_heredoc = 1;
			i++;

			if (string[i] == ' ')
				i++;

			for (j = 0;
			     string[i] != '\0' && string[i] != ' '
			     && string[i] != '|' && string[i] != '<'; j++) {
				redirec[j] = string[i];
				i++;
			}
			redirec[j] = '\0';
			stdout_red(redirec);
		}
		// OPTIONAL 1: here document
		if (string[i] == 'H' && no_heredoc == 0)
			op_heredoc(string, i);

		i++;
	}

	if (bg == 1 && h_red == 0 && can_execute == 0)
		stdin_red(stdin_bg);

	// means that the string has a redirection
	if (h_red == 1 && can_execute == 0) {
		// string with the commands but omits the redirections 
		strswcmds = malloc(sizeof(char) * (strswcmds_size + 1));

		for (pos_strwcmds = 0; pos_strwcmds < strswcmds_size;
		     pos_strwcmds++) {
			strswcmds[pos_strwcmds] = string[pos_strwcmds];
		}
		strswcmds[strswcmds_size] = '\0';
	}
	//See if execute one command or more commands with or no redirections
	if (appear_equal == 1 && can_execute == 0)
		set_value(string);
	else if (can_execute == 0) {

		if (pipe_flag == 0) {
			if (h_red == 1) {
				process_command(strswcmds, bg);
				free(strswcmds);
			} else
				process_command(string, bg);

		} else {
			tmparr = malloc((MAX + 1) * sizeof(char *));
			if (h_red == 1) {
				total_pos = get_positions(strswcmds, tmparr);
				process_commands(total_pos, tmparr, bg);
				free(strswcmds);

			} else {
				total_pos = get_positions(string, tmparr);
				process_commands(total_pos, tmparr, bg);
			}
			free(tmparr);
		}
	}
}

// with this function the user wants to change std int (keyboard) for what it 
// is inside of a file 
void
stdin_red(char *string)
{
	char *string_ptr;
	int fd;

	string_ptr = string;	// pointer to the string
	fd = open(string_ptr, O_RDONLY);	// asign the output to the file
	close(READ_END);	// close std_output 
	dup(fd);
}

// with this function the user can do the output redirection to a file
void
stdout_red(char *string)
{
	char *string_ptr;

	string_ptr = string;	// pointer to string 
	close(WRITE_END);	// Close std_output                
	open(string_ptr, O_CREAT | O_WRONLY, 0777);	// Designate file output
}

void
cd_builtin(char *string)
{
	int cd_pos;
	char path[MAX];

	int path_pos;

	path_pos = 0;

	if (string[0] == 'c' && string[1] == 'd') {
		cd_pos = 2;
		if (string[cd_pos] == ' ')
			cd_pos++;
		if (string[cd_pos] == '\n' || string[cd_pos] == '\0')
			chdir(getenv("HOME"));
		else {
			while (string[cd_pos] != ' ' && string[cd_pos] != '\0') {
				path[path_pos] = string[cd_pos];
				path_pos++;
				cd_pos++;
			}
			path[path_pos] = '\0';
			chdir(path);
		}
	}
}

void
set_value(char *string)
{
	int i, j;
	char name[MAX];
	char value[MAX];

	i = 0;
	while (string[i] != '\0') {
		for (j = 0; string[i] != '='; j++) {
			name[j] = string[i];
			i++;
		}
		name[j] = '\0';
		if (string[i] == '=')
			i++;

		for (j = 0; string[i] != '\0'; j++) {
			value[j] = string[i];
			i++;
		}
		value[j] = '\0';
		i++;
	}

	if (setenv(name, value, 0) != 0)
		fprintf(stderr, "error no set env");

}

int
sustitutions(char *string)
{
	char tmpstring[MAX];

	int str_pos, tmpstr_pos, pos, get_length;
	int can_execute;

	tmpstr_pos = 0;
	str_pos = 0;
	pos = 0;
	can_execute = 0;

	while (string[str_pos] != '\0') {
		if (string[str_pos] == '$') {
			char var_sust[MAX];
			char *sustitution;

			str_pos++;

			//store the $word in a char for the sustitution 
			for (pos = 0;
			     string[str_pos] != '\0' && string[str_pos] != ' '
			     && string[str_pos] != '|' && string[str_pos] != '>'
			     && string[str_pos] != '<'
			     && string[str_pos] != '$'; pos++) {
				var_sust[pos] = string[str_pos];
				str_pos++;
			}
			var_sust[pos++] = '\0';

			sustitution = getenv(var_sust);
			if (sustitution != NULL) {
				get_length = strlen(sustitution);

				for (pos = 0; pos < get_length; pos++) {
					tmpstring[tmpstr_pos] =
					    sustitution[pos];
					tmpstr_pos++;
				}
			} else {
				fprintf(stderr, "error %s does not exist\n",
					var_sust);
				can_execute = 1;
				return can_execute;
			}
		}
		tmpstring[tmpstr_pos] = string[str_pos];
		str_pos++;
		tmpstr_pos++;
	}
	tmpstring[tmpstr_pos] = '\0';

	// clear string for correct processing 
	memset(string, 0, MAX);
	pos = 0;
	while (tmpstring[pos] != '\0') {
		string[pos] = tmpstring[pos];
		pos++;
	}
	string[pos] = '\0';

	return can_execute;
}
