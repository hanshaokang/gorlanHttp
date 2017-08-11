#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void my_math(char *data_string)
{
    //char *myargv[3];
    //int i=0;
    //char *start=data_string;
    //while(*start)
    //{
    //    if(*start=='='){
    //        myargv[i++]=start+1;
    //    }
    //    if(*start='&')
    //    {
    //        *start='\0';
    //    }
    //    start++;
    //}
    //myargv[i]=0;

    //int data1=atoi(myargv[0]);
    //int data2=atoi(myargv[1]);
    //printf("<html><h1>data1: %d, data2: %d\n</h1><html>",data1,data2);

    printf("%s\n",data_string);



}

int main()
{
    char *method=getenv("METHOD");
    char *data_string=NULL;
    char buf[1024];
    if(method){
        if(strcasecmp(method,"GET")==0){
            //get
            strcpy(buf,getenv("QUERY_STRING"));
            data_string=buf;
            }else{
            //post
            char *content_len=getenv("CONTENT-LENTH");
            int cl=atoi(content_len);
            int i=0;
            char c;
            for(;i<cl;i++)
            {
                read(0,&c,1);
                buf[i]=c;
            }
            buf[i]=0;
            data_string=buf;
        }
    }
    if(data_string){
        my_math(data_string);
    }
    return 0;
}
