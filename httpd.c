#include "httpd.h"

int startup(const char *ip,int port)
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        perror("socket");
        exit(1);
    }

    int opt=1;
    int reset=setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(reset<0)
    {
        perror("setsockopt");
        exit(4);
    }

    struct sockaddr_in local;
    local.sin_family=AF_INET;
    local.sin_port=htons(port);
    local.sin_addr.s_addr=inet_addr(ip);

    if(bind(sock,(struct sockaddr*)&local,sizeof(local))<0)
    {
        perror("bind");
        exit(2);
    }

    if(listen(sock,10)<0)
    {
        perror("listen");
        exit(3);
    }

    return sock;
}

static void echo_client(int nums)
{

}

static void clear_fdhead(int fd)
{
    char buf[SIZE];
    int ret=0;
    do{
        ret=get_line(fd,buf,sizeof(buf));
    }while(ret>0&&strcmp(buf,"\n")!=0);
}

void echo_www(int fd,char *path,int size)
{
    int new_fd=open(path,O_RDONLY);
    if(new_fd<0){
        return;
    }
    const char *status_line="HTTP/1.0 200 ok\r\n";
    const char *blank_line="\r\n";
    send(fd,status_line,strlen(status_line),0);
    send(fd,blank_line,strlen(blank_line),0);
    sendfile(fd,new_fd,NULL,size);


    close(fd);
}

static int get_line(int fd,char buf[],int len)
{
    char c='\0';
    int i=0;
    while(c!='\n'&& i<len-1){
        ssize_t s=recv(fd,&c,1,0);
        if(s>0)
        {
            //\ r \r\n \n -> \n
            if(c=='\r')
            {
                recv(fd,&c,1,MSG_PEEK);
                if(c=='\n')
                {
                    recv(fd,&c,1,0);
                }
                else{
                    c='\n';
                }
            }
            //c=='\n'

            buf[i++]=c;
        }
        else
            return -1;
    }
    buf[i]=0;
    return i;
}

static int exec_cgi(int sock,char *method,char *path,char *query_strin)
{
    int content_len=-1;
    char method_env[SIZE];
    char query_strin_env[SIZE];
    char content_len_env[SIZE/10];
    //1stly judge method..
    if(strcasecmp(method,"GET")==0)
    {
        clear_fdhead(sock);
    }
    else{
        //POST..
        char line[1024];
        int ret=-1;

        do{
            ret=get_line(sock,line,sizeof(line));
            if(ret>0&&strncmp(line,"Content-Length: ",16)==0)
            {
                content_len=atoi(&line[16]);
            }
            printf("%s",line);
        }while(ret!=1);
        if(content_len==-1)
        {
            echo_client(404);
            return 10;
        }
    }

    const char *echo_line="HTTP/1.0  200  OK\r\n";
    send(sock,echo_line,strlen(echo_line),0);
    const char *type_line="Content-Type:text/html;charset=ISO-8859-1\r\n";
    send(sock,type_line,strlen(type_line),0);
    const char *null_line="\r\n";
    send(sock,null_line,strlen(null_line),0);

    //path->exec

    int input[2];
    int output[2];
    if(pipe(input)<0)
    {
        echo_client(500);
        return 12;
    }
    if(pipe(output)<0)
    {
        echo_client(500);
        return 13;
    }
    pid_t id=fork();
    if(id<0){
        echo_client(500);
        return 11;
    }
    else if(id==0)
    {
        //child..

        close(input[1]);
        close(output[0]);
        dup2(input[0],0);
        dup2(output[1],1);
        sprintf(method_env,"METHOD=%s ",method);
        putenv(method_env);

        if(strcasecmp(method,"GET")==0)
        {
            //GET..
            sprintf(query_strin_env,"QUERY_STRING=%s ",query_strin);
            putenv(query_strin_env);
        }else{
            //POST..
            sprintf(content_len_env,"CONTENT-LENGTH=%d ",content_len);
            putenv(content_len_env);
        }

        execl(path,path,NULL);

        exit(1);
    }else{
        //father..

        close(input[0]);
        close(output[1]);
        if(strcasecmp(method,"POST")==0)
        {
            int i=0;
            char c='\0';
            //read content..write in child..
            for(;i<content_len;i++)
            {
                recv(sock,&c,1,0);
                write(input[1],&c,1);
            }
            while(read(output[0],&c,1)>0)
            {
                send(sock,&c,1,0);
            }
        }

        int ret=waitpid(id,NULL,0);
        close(input[1]);
        close(output[0]);
    }
    return 0;
}

void *handler_request(void *arg)
{
    int fd=(int)arg;
    char buf[SIZE];
    char method[SIZE/10];
    char url[SIZE];
    char *query_strin;
    int cgi=0;
    char path[SIZE];
    int nums=0;
    int ret=0;
    ret=-1;
    if(get_line(fd,buf,sizeof(buf))<=0){
        ret=-1;
        nums=404;
        goto end;
    }

    //GET /a/b/index.html http/1.0
    //or get
    int i=0;
    int j=0;
    while(i<sizeof(method)-1&&j <sizeof(buf)&& !isspace(buf[j]))
    {
        method[i]=buf[j];
        i++,j++;

    }
    method[i]=0;

           while(j<sizeof(buf)&& isspace(buf[j])){
                j++;
    }

    i=0;
    while(i<sizeof(url)&& j<sizeof(buf)&& !isspace(buf[j]))
    {
        url[i]=buf[j];
        i++,j++;
    }
    url[i]=0;
    //ignore difference in GET and get..
    if(strcasecmp(method,"GET")&& strcasecmp(method,"POST")){
        ret=-2;
        nums=404;
        goto end;
    }

    if(strcasecmp(method,"POST")==0){
        cgi=1;
    }
    else if(strcasecmp(method,"GET")==0)
    {
        query_strin=url;
        while(*query_strin!='\0'){
            query_strin++;
            if(*query_strin=='?'){
                *query_strin='\0';
                //url point s..query_strin point after ?..
                query_strin++;
                cgi=1;
                break;
            }

        }
    }

    //output to str..
    sprintf(path,"wwwroot%s",url);
    //so path is wwwroot/a/b?wd....
    //
    //deal with only /
    if(path[strlen(path)-1]=='/'){
        strcat(path,"index.html");
    }
    //wwwroot/a/b/c.html..
    //!man 2 stat,to find struct stat
    //it has inode\nlink\uid\gid\size\atime\stime\ctime etc..
    struct stat st;
    if(stat(path,&st)<0){
        ret=-3;
        nums=404;
        goto end;
    }else{
        if(S_ISDIR(st.st_mode)){
            //the end must not be /...
            strcat(path,"/index.html");
        }else if(st.st_mode& S_IXUSR||(st.st_mode&S_IXGRP)||(st.st_mode&S_IXOTH)){
            cgi=1;
        }else{}

        //method,path,cgi(get->query_strin)
        if(cgi){
            exec_cgi(fd,method,path,query_strin);
        }else{
            //return src..this method only have path,and it's not cgi...
            clear_fdhead(fd);
            echo_www(fd,path,st.st_size);
        }
    }
end:
    //nums is errno..
    echo_client(nums);
    close(fd);
    return (void *)ret;
}
