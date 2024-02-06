
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "exec_line.h"
#include "optionals.h"

//OPTIONAL 1
void
op_heredoc(char *string, int i)
{
	char buf[MAX];
	char tmpbuf[MAX];
	char strhd[strlen(string) + 1];
	char *here = "HERE{";
	char *end = "}";
	int tmpbuf_pos;
	int ishd_pos;
	int strlinehd_pos;

	int fd;

	tmpbuf_pos = 0;

	ishd_pos = 0;

	// get HERE{
	for (int k = strlen(string) - 5; k < strlen(string); k++) {
		strhd[ishd_pos] = string[k];
		ishd_pos++;
	}
	strhd[ishd_pos] = '\0';

	// compare if strhd is HERE{
	if (strcmp(strhd, here) == 0) {
		// to clear HERE{ from string and do the correct processing
		string[i] = '\0';

		char *strhdline;

		while ((strhdline = fgets(buf, sizeof(buf), stdin))) {

			strlinehd_pos = 0;

			// if buf == a {  is the end  of the HERE DOCUMENT
			if (strcmp(buf, end) == 1) {
				tmpbuf[tmpbuf_pos] = '\0';
				break;
			}

			while (strlinehd_pos < strlen(strhdline)) {

				tmpbuf[tmpbuf_pos] = strhdline[strlinehd_pos];
				tmpbuf_pos++;
				strlinehd_pos++;
			}

			// clear buf and strhdline
			memset(buf, 0, MAX);
			memset(strhdline, 0, strlen(strhdline));

		}

		fd = open("hd", O_CREAT | O_WRONLY | O_TRUNC, 0777);
		write(fd, tmpbuf, tmpbuf_pos);
		stdin_red("hd");
	}

}

// OPTIONAL 2
int
cmp_strings(char *input_string, char *word_to_find, int length_w2f)
{
	int i;
	char bufw2f[MAX];
	char bufinstr[MAX];
	int are_equals;

	are_equals = 0;
	i = 0;
	while (i < length_w2f) {
		bufinstr[i] = input_string[i];
		bufw2f[i] = word_to_find[i];
		i++;
	}

	bufinstr[i] = '\0';
	bufw2f[i] = '\0';

	if (strcmp(bufinstr, bufw2f) == 0) {
		are_equals = 1;
		remove_word(input_string, i);
	}

	return are_equals;

}

// if uses_ifok == 1 && RESULT  == 0 -> EXECUTE
// if uses_ifok == 1 && RESULT  == 1 -> NO_EXECUTE
// if uses_ifnot == 1 && RESULT  == 1 -> EXECUTE
// if uses_ifnot == 1 && RESULT  == 0 -> NO_EXECUTE

int
check_result(int uses_ifok, int uses_ifnot)
{
	int can_execute;
	char *value;
	int intvalue;

	can_execute = 0;

	value = getenv("RESULT");
	intvalue = atoi(value);

	if (uses_ifok == 1 && intvalue == 0)
		can_execute = 0;

	if (uses_ifok == 1 && intvalue == 1)
		can_execute = 1;

	if (uses_ifnot == 1 && intvalue == 1)
		can_execute = 0;

	if (uses_ifnot == 1 && intvalue == 0)
		can_execute = 1;

	return can_execute;
}

void
remove_word(char *string, int pos_str)
{
	char tmpstr[MAX];

	int i, pos_tmpstr;

	i = pos_str;
	pos_tmpstr = 0;
	while (string[i] != '\0') {
		tmpstr[pos_tmpstr] = string[i];
		pos_tmpstr++;
		i++;
	}
	tmpstr[pos_tmpstr] = '\0';

	memset(string, 0, MAX);

	for (i = 0; i < strlen(tmpstr); i++) {
		string[i] = tmpstr[i];
	}
	string[strlen(tmpstr)] = '\0';
}
