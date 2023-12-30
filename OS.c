#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


void loop();
char *read_line();
char **tokenize(char *line);
int token_recognize(char **tokens);
int handle_input_redirection(char *file);
int handle_output_redirection(char *file);
void closefile(int fd);
void handle_pipe_operation(char **cmd1, char **cmd2);


int main() {

    loop();

    return 0;
}

void loop(){
    char *line;
    char **tokens;
    int end_flag = 0;
    
    do{
        line = read_line();
        tokens = tokenize(line);
        end_flag = token_recognize(tokens);
        
    } while (!end_flag);
    free(line);
    free(tokens);
}

char *read_line(){
    char *l = NULL;
    size_t bufsize = 0;
    printf("s1103847> ");
    getline(&l,&bufsize,stdin);
    //scanf("%[^\n]%*c", l);
    return l;
}

char **tokenize(char *line){
    int bufsize = 64;
    int position = 0;
    char **tokens = malloc(bufsize*sizeof(char*));
    char *token;

    token = strtok(line, " \t\r\n\a");
    while (token) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += 64;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "Allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, " \t\r\n\a");
    }
    tokens[position] = NULL;
    return tokens;
}

int handle_input_redirection(char *file) {
    //int fd = open(file, O_RDONLY);
    
    /*if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    if (dup2(fd, STDIN_FILENO) == -1) {
        perror("dup2");
        exit(EXIT_FAILURE);
    }*/


}

int handle_output_redirection(char *file) {

    /*int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);//write in 

    int fd_file =open("/dev/tty",O_RDWR,0666);//open on terminal
    
    dup2(fd,STDOUT_FILENO);

    dup2(fd_file,STDOUT_FILENO);
    
    close(fd_file);*/

    /*if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        exit(EXIT_FAILURE);
    }

    if (close(fd) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }*/
    //return fd;
}

void closefile(int fd){
    close(fd);
}

void handle_pipe_operation(char **cmd1, char **cmd2) {
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid1 = fork();
    if (pid1 == 0) {
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        execvp(cmd1[0], cmd1);
        perror("execvp cmd1");
        exit(EXIT_FAILURE);
    }

    pid_t pid2 = fork();
    if (pid2 == 0) {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        execvp(cmd2[0], cmd2);
        perror("execvp cmd2");
        exit(EXIT_FAILURE);
    }

    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void execute_command(char **tokens) {
    pid_t pid = fork();
    if (pid == 0) {
        execvp(tokens[0], tokens);
        perror("execvp");
        exit(EXIT_FAILURE);
    }
    wait(NULL);
}


int token_recognize(char **tokens) {
    int fd;
    if (tokens[0] == NULL) {
        return 0;
    }

    if (strcmp(tokens[0], "exit") == 0) {
        printf("Process end\n");
        return 1;
    }

    if (strcmp(tokens[0], "help") == 0) {
        printf("funtion can use : <,>,|\n");
        return 0;
    }
    else {
        char *file = NULL;
        for (int i = 0; tokens[i] != NULL; i++) {
            if (strcmp(tokens[i], "<") == 0) {
                file = tokens[i + 1];
                tokens[i] = NULL;
                int fd = open(file, O_RDONLY);
                dup2(fd,STDIN_FILENO);
                close(fd);
                execlp("cat","cat",NULL);
                //handle_input_redirection(file);
                //fd=handle_input_redirection(file);
            } 
            else if (strcmp(tokens[i], ">") == 0) {
                file = tokens[i + 1];
                tokens[i] = NULL;
                //handle_output_redirection(file);
                //fd=handle_output_redirection(file);
                int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0666);//write in 

                int fd_file =open("/dev/tty",O_RDWR,0666);//open on terminal
    
                dup2(fd,STDOUT_FILENO);
                execute_command(tokens);
                dup2(fd_file,STDOUT_FILENO);
                
                closefile(fd);
                closefile(fd_file);
                return 0;
    
            } 
            else if (strcmp(tokens[i], "|") == 0) {
                tokens[i] = NULL;
                char **cmd1 = tokens;
                char **cmd2 = &tokens[i + 1];
                handle_pipe_operation(cmd1, cmd2);
            }
        }
        execute_command(tokens);
        return 0;
    }
    
}