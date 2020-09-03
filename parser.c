#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	int size;
	char **items;
} tokenlist;

char *get_input(void);
tokenlist *get_tokens(char *input);

tokenlist *new_tokenlist(void);
void add_token(tokenlist *tokens, char *item);
void free_tokens(tokenlist *tokens);

int main()
{
	while (1) {
		printf("%s@%s: %s > ", getenv("USER"),getenv("MACHINE"),getenv("PWD"));

		/* input contains the whole command
		 * tokens contains substrings from input split by spaces
		 */

		char *input = get_input();
	//	printf("whole input: %s\n", input);

		tokenlist *tokens = get_tokens(input);
		char *fullCommand = "";
		for (int i = 0; i < tokens->size; i++) {

			//printf("token %d: (%s)\n", i, tokens->items[i]);
			if(tokens->items[i][0]=='$'){
				tokens->items[i]++;
				printf("COMMAND : %s\n", getenv(tokens->items[i]));
					tokens->items[i]--;//gives memory error if don't do this has to do with size
			}
			else if(tokens->items[i][0]=='~'){
				tokens->items[i]++;
				char *fullPath = (char *) malloc(strlen(getenv("HOME")) + strlen(tokens->items[i]) + 1);
				strcpy(fullPath,getenv("HOME"));
				strcat(fullPath,"/");
	   		strcat(fullPath,tokens->items[i]);
				printf("%s\n", fullPath);
				tokens->items[i]--; //gives memory error if don't do this has to do with size
				free(fullPath);
			}

			else if(tokens->items[i][0]=='-'){
				char *tempHolder = (char *) malloc(1 + strlen(fullCommand) + strlen(tokens->items[i]) );
				char *holder = (char *) malloc(1 + strlen(fullCommand));
				strcpy(holder, fullCommand);
				strcpy(tempHolder, holder);

				strcat(tempHolder, tokens->items[i]);
				fullCommand = tempHolder;
				printf("FULL COMMAND");
			}

			else{

						char delim[] = ":";
						char *pathString = getenv("PATH");
						printf("%s\n", getenv("PATH"));
						char *ptr = strtok(pathString , delim);

	while(ptr != NULL)
	{
		printf("'%s'\n", ptr);
		ptr = strtok(NULL, delim);
	}







//free(tempHolder);


					}
			}



		free(input);
		free_tokens(tokens);
	}

	return 0;
}

tokenlist *new_tokenlist(void)
{
	tokenlist *tokens = (tokenlist *) malloc(sizeof(tokenlist));
	tokens->size = 0;
	tokens->items = NULL;
	return tokens;
}

void add_token(tokenlist *tokens, char *item)
{
	int i = tokens->size;

	tokens->items = (char **) realloc(tokens->items, (i + 1) * sizeof(char *));
	tokens->items[i] = (char *) malloc(strlen(item) + 1);
	strcpy(tokens->items[i], item);

	tokens->size += 1;
}

char *get_input(void)
{
	char *buffer = NULL;
	int bufsize = 0;

	char line[5];
	while (fgets(line, 5, stdin) != NULL) {
		int addby = 0;
		char *newln = strchr(line, '\n');
		if (newln != NULL)
			addby = newln - line;
		else
			addby = 5 - 1;

		buffer = (char *) realloc(buffer, bufsize + addby);
		memcpy(&buffer[bufsize], line, addby);
		bufsize += addby;

		if (newln != NULL)
			break;
	}

	buffer = (char *) realloc(buffer, bufsize + 1);
	buffer[bufsize] = 0;

	return buffer;
}

tokenlist *get_tokens(char *input)
{
	char *buf = (char *) malloc(strlen(input) + 1);
	strcpy(buf, input);

	tokenlist *tokens = new_tokenlist();

	char *tok = strtok(buf, " ");
	while (tok != NULL) {

		add_token(tokens, tok);

		tok = strtok(NULL, " ");
	}

	free(buf);
	return tokens;
}

void free_tokens(tokenlist *tokens)
{
	for (int i = 0; i < tokens->size; i++)
		free(tokens->items[i]);

	free(tokens);
}
