/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkaczmar <jkaczmar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 18:17:37 by jkaczmar          #+#    #+#             */
/*   Updated: 2022/09/14 19:09:51 by jkaczmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Steps to make this work
//Intialize variables 
//we need a lastfd flag to check for the pipe
//Then we need array of two ints for pipe itself
//we are looping through all the argumetns passed by the user till it's smaller or equal
//Then we there is somehting in a index and if it's different than "|" and ";" that means we have an argument
//So we can increment the arguement count
//If that's not the case but we already have argument count bigger than 0
//We move to another if statement where we check if the current positon in argv is a pipe if it is we change the flag to 1
//if not we change it to 0
//Next we go to the execution part we check if our current argument isn't a cd
//if it is we check if there are at least two arguments to it and execute it
//If it's not cd we go to normal executing 
//We check for pipe flag if pipe_flag is true we use pipe(pipe_fd);
//After this we fork and save pid
//If pid is negative we throw an error
//If it's 0 we are in the child
//in the child we check if the last_fd flag is different than -1 and we dup2(lastfd, 0)
//Next up we check if our pipe flag is equal to 0 and if it is we dup2(pipe_fd[1],1)
//Next up we just execute the command with execve
//in the parent process we wait
//and we close(pipe_fd[1])
//if last_fd_flag is different than -1 we close it 
//now if our pipe_flag is 1
//we assign last_fd = pipe_fd[0]
//if not we assign -1 to it 

int main(int argc, char **argv, char **env)
{
    if(argc == 1)
    {
        return 1;
    }
    int pipefd[2];
    int pid;
    int pip = 0;
    int lastfd = -1;
    int nbarg = 0;
    int i = 1;
    while(i <= argc)
    {
        if(argv[i] && strcmp(argv[i], "|") && strcmp(argv[i], ";"))
        {
            nbarg++;
            //Check if there are any commands
        }else if(nbarg){
            if(argv[i])
            {
                if(strcmp(argv[i], "|") == 0)
                {
                    pip = 1;
                }else{
                    pip = 0;
                }
            }
                argv[i] = NULL;
            //Execute command
            if(strcmp(argv[i - nbarg], "cd") == 0)
            {
                if(nbarg == 2)
                {
                    if(chdir(argv[i - nbarg + 1]) == -1)
                    {
                        write(2, "Error" , 6);
                    }
                }else{
                    write(2, "Wrong number of arguments", 20);
                }
            }else{
                if(pip == 1)
                {
                    if(pipe(pipefd) == -1)
                    {
                        write(2, "Error", 6);
                        exit(1);
                    }
                }
                    if((pid = fork()) == -1)
                    {
                        write(2, "Error", 6);
                        exit(1);
                    }
                    if(pid == 0)
                    {
                        if(lastfd != -1 && (dup2(lastfd, 0)) == -1)
                        {
                            write(2, "Error", 6);
                            exit(1);
                        }
                        if(pip != 0 && (dup2(pipefd[1], 1) == -1))
                        {
                            write(2, "Error", 6);
                            exit(1);
                        }
                        if(execve(argv[i - nbarg], &argv[i - nbarg], env) == -1)
                        {
                            write(2, "Error" ,6);
                        }
                    }else{
                        waitpid(-1, 0, 0);
                        close(pipefd[1]);
                        if(lastfd != -1)
                        {
                            close(lastfd);
                        }
                        if(pip)
                        {
                            lastfd = pipefd[0];
                        }else{
                            lastfd = -1;
                        }   
                }
               nbarg = 0;
            }
        }
        pip = 0;
        i++;
    }
}