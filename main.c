/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jkaczmar <jkaczmar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/14 18:17:37 by jkaczmar          #+#    #+#             */
/*   Updated: 2022/09/14 18:51:10 by jkaczmar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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