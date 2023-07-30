#include <msh.h>
#include <msh_parse.h>
#include <unistd.h>
#include <msh_parse.c>
#include <sys/wait.h>
#include <sys/types.h> 
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <aio.h>
#include <stddef.h>



//if the background is 1, use background, else foreground 
//every time you exevept, you put the command using the put command 



//foreground pipeline
static struct msh_pipeline * foreground;


//background pipeline
static struct msh_pipeline * background[MSH_MAXBACKGROUND];


pid_t bckgrundprocesses[MSH_MAXBACKGROUND];
int bckgrundcount = 0;
int bckgroundpd = 0;
pid_t foregrundPid;
pid_t susPid;



void signal_handler(int signal_number, siginfo_t *info, void *context)
{
    (void) info;
    (void) context;

	if(foreground == NULL)
	{
		return;
	}

    //control C
    if(signal_number == SIGINT)
    {

		for(int i = 0; i < MSH_MAXARGS; i++)
		{
			struct msh_command * c = msh_pipeline_command(foreground, i);
			if(c != NULL)
			{
				struct cmd_data *data = (struct cmd_data *)msh_command_getdata(c);
				if (data != NULL) 
				{
					kill(data->pid, SIGTERM);
				}
			}
		}
		return;
	}

    //control Z
    if(signal_number == SIGTSTP)
    {

		for (int i = 0; i < MSH_MAXARGS; i++) 
		{
    		struct msh_command *c = msh_pipeline_command(foreground, i);
    		if (c != NULL) 
			{
        		struct cmd_data *data = (struct cmd_data *)msh_command_getdata(c);
        		if (data != NULL) 
				{
            		kill(data->pid, SIGCONT);
        		} 
   			}
		}
	}

}




//setting up the signal
void setup_signal(int signo, void (*sh)(int , siginfo_t *, void *))
{
    sigset_t masked;
    struct sigaction siginfo;

    sigemptyset(&masked);
    sigaddset(&masked, signo);
    siginfo = (struct sigaction) 
    {
        .sa_sigaction = sh,
        .sa_mask      = masked,
        .sa_flags     = SA_SIGINFO
    };

    if (sigaction(signo, &siginfo, NULL) == -1) 
    {
        perror("sigaction error");
        exit(EXIT_FAILURE);
    }
}



void msh_execute(struct msh_pipeline *p) 
{

    int inputfd = 0;
    pid_t pid;
    int j;
    int outfd;
    int stat;
    int pipefd[2];

    if(p->background != 1)
	{
		foreground = p;

	}
	else if (p->background==1) 
	{
		for(int i = 0; i < MSH_MAXBACKGROUND; i++)
		{
			if(background[i] == NULL)
			{
				background[i] = p;
				break;
			}
		}
	}




    for (int i = 0; i < p->i; i++) 
    {
  
        struct msh_command *command = p->c[i];
    
        char *cmd = command->args[0];


        if (strcmp(cmd, "cd") == 0) 
        {
            char *dir = command->args[1];
            if (dir == NULL || strcmp(dir, "~") == 0) 
            {
                if (chdir(getenv("HOME")) == -1) 
                {
                    perror("chdir error");
                }
            } 
            else 
            {
                if (dir[0] == '~') 
                {
                    char *dir_casa = getenv("HOME");
                    char *nueva_dir = malloc(strlen(dir_casa) + strlen(dir) + 1);
                    strcpy(nueva_dir, dir_casa);
                    strcat(nueva_dir, &dir[1]);
                    dir = nueva_dir;
                }
               
                if (chdir(dir) == -1) 
                {
                    perror("chdir error");
                }
                if (dir != command->args[1]) 
                {
                    free(dir);
                }
            }
            continue;
        }


        if (strcmp(cmd, "exit") == 0) 
        {
            exit(EXIT_SUCCESS);
        }


        if (strcmp(cmd, "fg") == 0) 
        {
            if (susPid != 0) 
            {
                kill(susPid, SIGCONT);
                waitpid(susPid, NULL, 0);
                susPid = 0;
            }
            continue;
        }


        if (strcmp(cmd, "bg") == 0) 
        {
            if (susPid != 0) 
            {
                kill(susPid, SIGCONT);
                susPid = 0;
            }
        }


        if (pipe(pipefd) == -1) 
        {
            perror("pipe error");
            exit(EXIT_FAILURE);
        }

   
        int statDeredirecion = 0; 
        int redirrecion; 
        char *paginaDeredirrecion = NULL;
        for (int j = 0; j < command->count; j++) 
        {
     
            if (strcmp(command->args[j], "1>") == 0) 
            {
                statDeredirecion = 1; 
                redirrecion = 1; 
                paginaDeredirrecion = command->args[j + 1];
                command->args[j] = NULL;
                command->args[j + 1] = NULL;
                command->count -= 2;
                break;
            }
    
            else if (strcmp(command->args[j], "2>") == 0) 
            {
                statDeredirecion = 1; 
                redirrecion = 2; 
                paginaDeredirrecion = command->args[j + 1];
                command->args[j] = NULL;
                command->args[j + 1] = NULL;
                command->count -= 2;
                break;
            }
        
            else if (strcmp(command->args[j], "1>>") == 0) 
            {
                statDeredirecion = 2; 
                redirrecion = 1; 
                paginaDeredirrecion = command->args[j + 1];
                command->args[j] = NULL;
                command->args[j + 1] = NULL;
                command->count -= 2;
                break;
            }
       
            else if (strcmp(command->args[j], "2>>") == 0) 
            {
                statDeredirecion = 2; 
                redirrecion = 2; 
                paginaDeredirrecion = command->args[j + 1];
                command->args[j] = NULL;
                command->args[j + 1] = NULL;
                command->count -= 2;
                break;
            }
        }

     
        if ((pid = fork()) == -1) 
        {
            perror("fork error");
            exit(EXIT_FAILURE);
        } 
 
        else if (pid == 0) 
        {

            if (i != 0) 
            {
                if (dup2(inputfd, STDIN_FILENO) == -1) 
                {
                    perror("dup2 error");
                    exit(EXIT_FAILURE);
                }
            }


            if (i != p->i - 1) 
            {
                if (dup2(pipefd[1], STDOUT_FILENO) == -1) 
                {
                    perror("dup2 error");
                    exit(EXIT_FAILURE);
                }
            }

  
            if (statDeredirecion == 1) 
            { 
                if (redirrecion == 1) 
                { 
                    if ((outfd = open(paginaDeredirrecion, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) 
                    {
                        perror("open error");
                        exit(EXIT_FAILURE);
                    }

                    if (dup2(outfd, STDOUT_FILENO) == -1) 
                    {
                        perror("dup2 error");
                        exit(EXIT_FAILURE);
                    }
                }
                else if (redirrecion == 2) 
                {
                    if ((outfd = open(paginaDeredirrecion, O_WRONLY | O_CREAT | O_TRUNC, 0666)) == -1) 
                    {
                        perror("open error");
                    }

                    if (dup2(outfd, STDERR_FILENO) == -1) 
                    {
                        perror("dup2 error");
                        exit(EXIT_FAILURE);
                    }
                }
                close(outfd);
            }
            else if (statDeredirecion == 2) 
            { 
                if (redirrecion == 1) 
                { 
                    if ((outfd = open(paginaDeredirrecion, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1) 
                    {
                        perror("open error");
                        exit(EXIT_FAILURE);
                    }

                    if (dup2(outfd, STDOUT_FILENO) == -1) 
                    {
                        perror("dup2 error");
                        exit(EXIT_FAILURE);
                    }
                }
                else if (redirrecion == 2) 
                { 
                    if ((outfd = open(paginaDeredirrecion, O_WRONLY | O_CREAT | O_APPEND, 0666)) == -1) 
                    {
                        perror("open error");
                        exit(EXIT_FAILURE);
                    }

                    if (dup2(outfd, STDERR_FILENO) == -1) 
                    {
                        perror("dup2 error");
                        exit(EXIT_FAILURE);
                    }
                }
                close(outfd);
            }

       
            close(pipefd[0]);
            close(pipefd[1]);

            if (i != 0) 
            {
                close(inputfd);
            }

      
            if (execvp(cmd, command->args) == -1) 
            {
                perror("execvp error");
                exit(EXIT_FAILURE);
            }

            pid_t *pointer  = calloc(sizeof(pid_t),1);
			*pointer = pid;
			msh_command_putdata(command, (void*)pointer, free);
        } 
        else 
        {

            if (inputfd != STDIN_FILENO) 
            {
                close(inputfd);
            }

            inputfd = pipefd[0];

     
            close(pipefd[1]);
        
     
            if (command->isLast && !p->background) 
            {
                foregrundPid = pid;
                waitpid(pid, &stat, WUNTRACED);
            }

       
            else 
            {
                if (bckgrundcount < MSH_MAXBACKGROUND) 
                {
                    bckgrundprocesses[bckgrundcount] = pid;
                    bckgrundcount++;
                } 
            }
        }
    }

   
    int i = 0;
    while (i < bckgrundcount) 
    {
        if (waitpid(bckgrundprocesses[i], &stat, WNOHANG) != 0) 
        {
            for (j = i; j < bckgrundcount - 1; j++) 
            {
                bckgrundprocesses[j] = bckgrundprocesses[j + 1];
            }
            bckgrundcount--;
        } 
        else 
        {
            i++;
        }
    } 

   
    if (susPid != 0) 
    {
        waitpid(susPid, NULL, 0);
    }

    foregrundPid = 0;
    msh_pipeline_free(p);
    return;
}



void msh_init(void)
{
    setup_signal(SIGINT, signal_handler);
    setup_signal(SIGTSTP, signal_handler);
    return;
}


