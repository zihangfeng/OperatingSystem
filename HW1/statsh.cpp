// Zihang Feng
// Fall 2013 
// CS 385
// 665726671
// jfeng9

#include <iostream>
#include <string>
#include <cstdio>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <vector>

using namespace std;

struct node
{
	double system_time;
	double user_time;
	string *user_input;

	node(double new_user_time, double new_system_time, string *new_user_input)
	{
		system_time = new_system_time;
		user_time = new_user_time;
		user_input = new_user_input;
	}
};

// clone the process with the call fork
void call_of_fork(char **tokens, vector<node> &stats, string *userinput, int *a)
{
	pid_t pid = fork();
        if(pid < 0)// something is wrong
        {
                cout << "Fork Failure" << endl;
                exit(-1);
        }
        else if(pid == 0)// the child process
        {
        	execvp(tokens[0],tokens);
                cout << "Something weng wrong" << endl;
                exit(-2);
        }
        else
     		*a = pid;

}
int main()
{
	string * userinput = new string("...");
	vector<node> stats;
	char **tokens;
	int *pid_array;
	while(true)
	{
		userinput = new string();
		cout << "Type a command: ";
		getline(cin, *userinput);
		if(userinput->compare("exit") == 0)
		{
			break;
		}
		else if(userinput->compare("stats") == 0)
		{
			for(vector<node>::iterator i = stats.begin(); i != stats.end(); ++i)
			{
				cout << *(i->user_input) << "\tUser Time: " << i->user_time << "\tSystem Time: " << i->system_time <<  endl;
				
			}	
			continue;
		}
		char* copy_of_user_input = (char*)malloc(sizeof(char)*(userinput->length()+1));
		strcpy(copy_of_user_input, userinput->c_str());
		int num_of_commands = 0;

		if(strtok(copy_of_user_input, "|") != NULL)// count the commands by the bars
                {					   // and get the number of bars
			num_of_commands = 1;
                        while(strtok(NULL, "|") != NULL)
                                        num_of_commands++;
                }
		strcpy(copy_of_user_input, userinput->c_str());// make another same copy again
							       // because the previous has been devastated

		// separate all commands and the indiv_commands pointer of array points to each of them
		// but not store them
		char **indiv_commands = (char**)malloc(sizeof(num_of_commands + 1));
		indiv_commands[0] = strtok(copy_of_user_input,"|");
                if(indiv_commands[0] != NULL)
		{
			for(int p = 1; p < num_of_commands; p++)
				indiv_commands[p] = strtok(NULL, " ");	
		}		
		
		pid_array = (int*)malloc(sizeof(int)*num_of_commands);
		int num_of_tokens = 0;
		
		// now we store every command because of the strcpy method 
		for(int k = 0; k < num_of_commands; k++)
		{
			// the method of fork is called first before the next command
			char *command = (char*)malloc(sizeof(char)*(strlen(indiv_commands[k]) + 1));
			strcpy(command, indiv_commands[k]);// get the number of tokens
			if(strtok(command, " ") != NULL)
                	{
                       		num_of_tokens = 1;
                   		while(strtok(NULL, " ") != NULL)
                                num_of_tokens++;// get the number of commands
                	}
                        strcpy(command, indiv_commands[k]);// make the same copy due the previous has been destroyed
			tokens = (char**)malloc(sizeof(char*)*(num_of_tokens + 1));
			tokens[0] = strtok(command," ");
		 	if(tokens[0] != NULL)
                        {
                                for(int j = 1; j <= num_of_tokens; j++)
                                {
                                        tokens[j] = strtok(NULL, " ");
                                }
                        }                
                        call_of_fork(tokens, stats, userinput, &pid_array[k]);

		}
		// store all necessary values here
		for(int d = 0; d < num_of_commands; d++)
		{
			int status;
                	struct rusage usage;
                	wait4(pid_array[d],&status,0,&usage);// wait the children to come back
                	double num_of_seconds_in_user_time = usage.ru_utime.tv_sec + usage.ru_utime.tv_usec/1000000;
                	double num_of_seconds_in_system_time = usage.ru_stime.tv_sec + usage.ru_stime.tv_usec/1000000;
                	stats.push_back(node(num_of_seconds_in_system_time, num_of_seconds_in_user_time, userinput));
		}

		free(copy_of_user_input);
		free(tokens);
		}

	return 0;
}
