#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
    int fd;
    char status;
    /*1、判断参数*/
    if (argc < 3)
    {
        printf("Usage: %s </dev/xxx> -w <on | off>\n",argv[0]);
        printf("       %s </dev/xxx> -r\n",argv[0]);
        return -1;
    }
    /*2、打开文件*/
    fd = open(argv[1],O_RDWR);
    if(fd == -1)
    {
        printf("can not open file %s\n",argv[1]);
        return -1;
    }

    /*3、写文件*/
    if(0 == strcmp(argv[2],"-w") && argc==4)
    {
        if(0 == strcmp(argv[3],"on"))
        {
            status = 1;
            write(fd,&status,1);
        }
        if((0 == strcmp(argv[3],"off"))) 
        {
            status = 0;
            write(fd,&status,1);
        }
    }
    else if (0 == strcmp(argv[2],"-r") && argc==3)
    {
        read(fd,&status,1);
        printf("the device <%s> current state is %s\n",argv[1],status ? "ON" : "OFF");
    }    
    else
    {
        printf("config error!!!\n");
        return -1;
    }
    close(fd);
    
    return 0;
}
