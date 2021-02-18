#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

//建立通用配置文件的数据结构
typedef struct{
    char ask[100];
    char setting[100];
    char flag ;//设立标志位，确认是yes or no
}SETTING;

SETTING st[150],st_1;//建立配置的结构数组
int index = 0;
int autoIndex = 1;
char token[20];
FILE *fw;
FILE *autoconf;

FILE *deleteLine(FILE *f)
{
    char c = '\0';
    c = fgetc(f);
    //printf("%c",c);
    fflush(stdin);
    while(c != '\n')
    {
        c = fgetc(f);
        //printf("%c",c);
    }
    return f;
}

FILE *storeInfo(FILE *fp)
{
                int j = 0;
                char input = '\0';
                char ch = '\0';
                ch = fgetc(fp);//吃掉第一个单引号
                while(ch != '\'')//得到判断语句
                {
                    st[index].ask[j++] = ch;
                    ch = fgetc(fp);
                }
                ch = fgetc(fp);//吃掉第二个单引号
                printf("%s",st[index].ask);

                while(ch == ' ')
                {
                    printf("%c",ch);
                    ch = fgetc(fp);//吃掉空格
                }

                int k = 0;
                while(ch != ' ')
                {
                    st[index].setting[k++] = ch;
                    ch = fgetc(fp);
                }
                printf("(%s)",st[index].setting);

                while(ch == ' ')
                {
                    printf("%c",ch);
                    ch = fgetc(fp);//吃掉空格
                }
                //得到判断标志
                printf("[%c]",ch);
                fflush(stdin);
                input = getchar();
                if(input == '\n')
                    st[index].flag = ch;
                else
                    st[index].flag = input;

                ch = fgetc(fp);//此时ch里赋值是换行
                //printf("%c",ch);

                if(st[index].flag=='n')
                {
                    fputs("# ",fw);
                    fputs(st[index].setting,fw);
                    fputs(" is not set\n",fw);

                    fputs("#undef ",autoconf);
                    fputs(st[index].setting,autoconf);
                    fputc('\n',autoconf);
                }
                else{
                    fputs(st[index].setting,fw);
                    fputs("#define  ",autoconf);
                    fputs(" = ",fw);
                    if(!strcmp(st[index].setting,"NR_FTAPE_BUFFERS"))
                    {
                        fputc(st[index].flag,fw);
                        fputs(st[index].setting,autoconf);
                        fputs(" (",autoconf);
                        fputc(st[index].flag,autoconf);
                        fputs(")\n",autoconf);
                    }
                    else{
                        fputs(st[index].setting,fw);
                        fputs(st[index].setting,autoconf);
                        fputs(" 1\n",autoconf);
                    }
                    fputc('\n',fw);
                }

                return fp;
}


int main()
{
    FILE *fp;
    fp = fopen("config.in","r");
    fw = fopen("Linux.config","w+");
    autoconf = fopen("autoconf.h","w+");

    fflush(stdin);
    char ch = '\0';
    char prech = '\0';//得到上一行

    fputs("#\n",fw);
    fputs("# Automatically generated make config: don't edit\n",fw);
    fputs("#\n",fw);

    fputs("/*\n",autoconf);//写utoconf.h的开始注释
    fputs(" * Automatically generated make config: don't edit\n",autoconf);
    fputs("*/\n",autoconf);

    int fi = 0;//判断标志位

    ch = fgetc(fp);//取文件的第一个字符
    prech = ch;
    while(ch!=EOF){
        //处理注释
        while(ch == '#'){
            fp = deleteLine(fp);//返回换行所在的指针
            ch = fgetc(fp);//得到换行后的第一个字符
        }
        while(ch == ' ')
            ch = fgetc(fp);//吃掉空格
        //处理空白行
        while(ch=='"'||ch=='['||ch==']'||ch=='-'||ch=='.'||ch=='$'||ch=='_'||ch=='='||ch=='\t')
            ch = fgetc(fp);
        while(ch == '\n')
        {
            ch = fgetc(fp);
        }

        //是字母的处理
        while(isalpha(ch))
        {
            prech = ch;
            int i = 0;
            while(ch != ' '&&ch!='\n')//得到单词关键字
            {
                token[i++] = ch;
                ch = fgetc(fp);
            }
            token[i] = '\0';
            if(!strcmp(token,"fi"))
                continue;
            while(ch == ' ')
                ch = fgetc(fp);//吃掉空格

            if(!strcmp(token,"bool")||!strcmp(token,"int"))//判断是选择配置语句
            {
                fp = storeInfo(fp);//存储信息

                //memset(st[index].setting,0x00,sizeof(char)*100);
                //memset(st[index].ask,0x00,sizeof(char)*100);
                //memset(st[index].flag,0x00,sizeof(char));
            }

            if(!strcmp(token,"if"))
            {
                index--;
                if(strcmp(st[index].setting,"CONFIG_SCSI")==0)
                {
                    if(st[index].flag == 'n')
                    {
                        while(ch!=':')
                            ch = fgetc(fp);
                        while(ch != 'e')
                        {
                            if(ch == '*'||ch == ':')
                            {
                                while(ch != '\n')
                                {
                                    printf("%c",ch);
                                    ch = fgetc(fp);//ch 换行
                                }
                                printf("\n");
                                ch = fgetc(fp);
                            }
                        }
                        while(ch != 'f'){//吃掉else的部分
                            while(ch != '\n')
                                ch = fgetc(fp);
                            ch = fgetc(fp);
                        }
                    }
                    else{
                        while(ch!='e'){//吃掉if部分
                            while(ch!='\n')
                                ch = fgetc(fp);
                            ch = fgetc(fp);
                        }
                        while(ch!= '\n')
                            ch = fgetc(fp);
                    }
                }
                //处理ETHERCARDS的部分
                else if(strcmp(st[index].setting,"CONFIG_ETHERCARDS")==0)
                {
                    if(st[index].flag=='n')
                    {
                        fi = 2;
                        while(ch!=':')
                            ch = fgetc(fp);
                        while(ch != 'e')
                        {
                            if(ch == '*'||ch == ':')
                            {
                                while(ch != '\n')
                                {
                                    printf("%c",ch);
                                    ch = fgetc(fp);//ch 换行
                                }
                                printf("\n");
                                ch = fgetc(fp);
                            }
                        }
                        while(ch!='f'&&fi!=0)
                        {
                            while(ch!='\n')
                                ch = fgetc(fp);
                            ch = fgetc(fp);
                            if(ch=='f')
                            {
                                fi--;
                                ch = fgetc(fp);
                            }
                        }
                    }
                    else
                    {
                        while(ch!='e'){//吃掉if部分
                            while(ch!='\n')
                                ch = fgetc(fp);
                            ch = fgetc(fp);
                        }
                        while(ch!= '\n')
                            ch = fgetc(fp);
                    }
                }
                //处理else ethercards分支下的if
                else if(strcmp(st[index].setting,"CONFIG_SLIP")==0)
                {
                    if(st[index].flag=='y')
                    {
                        while(ch!='\n')
                            ch = fgetc(fp);
                    }
                    else{
                        while(ch!='f')
                            ch = fgetc(fp);
                    }
                }
                //处理CONFIG_PSMOUSE
                else if(strcmp(st[index].setting,"CONFIG_PSMOUSE")==0)
                {
                    if(st[index].flag=='y')
                    {
                        while(ch!='\n')
                            ch = fgetc(fp);
                    }
                    else{
                        while(ch!='f')
                            ch = fgetc(fp);
                    }
                }
                //处理CONFIG_FTAPE
                else if(strcmp(st[index].setting,"CONFIG_FTAPE")==0)
                {
                    if(st[index].flag=='y')
                    {
                        while(ch!='\n')
                            ch = fgetc(fp);
                    }
                    else{
                        while(ch!='f')
                        {
                            while(ch!='\n')
                                ch = fgetc(fp);
                            ch = fgetc(fp);
                        }
                    }

                }
                else if(strcmp(st[index].setting,"CONFIG_PROFILE")==0)
                {
                    int s=0;
                    while(strcmp(st[s].setting,"CONFIG_SCSI")!=0)
                        ++s;

                    if(st[s].flag=='y')
                    {
                        while(ch!='\n')
                            ch=fgetc(fp);
                    }
                    else{
                        while(ch!='f')
                        {
                            while(ch!='\n')
                                ch = fgetc(fp);
                            ch=fgetc(fp);
                        }
                    }
                    /*int p=0;
                    while(strcmp(st[p].setting,"CONFIG_SOUND")!=0)
                        p++;
                    if(st[s].flag=='y')
                        printf("exec touch .makesound\n");
                    else
                        printf("exec rm -f .makesound\n");*/
                }
            }
            memset(token,0x00,sizeof(char)*20);
            index++;

            ch = fgetc(fp);
        }
        //做输出处理
        int num= 0;//计算*的个数，如果小于2的话就输出成空白的注释
        while(ch == '*')
        {num++;
            if(autoIndex==1)
                fputs("\n/",autoconf);
            else
                fputc(' ',autoconf);
            while(ch != '\n')
            {
                if(ch=='*')//写文件操作
                    fputc('#',fw);
                else
                    fputc(ch,fw);

                fputc(ch,autoconf);
                printf("%c",ch);
                ch = fgetc(fp);
            }
            fputc('\n',fw);
            printf("\n");

            ch = fgetc(fp);
            if(ch=='*')
            {
                autoIndex = 0;
                fputc('\n',autoconf);
            }
            else{
                autoIndex = 1;
                //fputs("/\n",autoconf);
                if(num==1)
                    fputs("\n */\n",autoconf);
                else
                    fputs("/\n",autoconf);
            }
        }
        while(ch == ':')
        {
            prech= ':';
            while(ch != '\n')
            {

                printf("%c",ch);
                ch = fgetc(fp);
            }
            printf("\n");
            ch = fgetc(fp);
        }
    }
    int p=0;
    while(strcmp(st[p].setting,"CONFIG_SOUND")!=0)
        p++;
    if(st[p].flag=='y')
        printf("\nexec touch .makesound\n");
    else
        printf("\nexec rm -f .makesound\n");
    printf("\nthe linux kernel is now hopefully configured for your setup.\n");
    printf("Check the top-level Makefile for additional configuration,\n");
    printf("and do a 'make dep ; make clean' if you want to be sure all\n");
    printf("the files are correctly re-made\n");

    fclose(fp);
    fclose(fw);
    fclose(autoconf);
    return 0;
}
