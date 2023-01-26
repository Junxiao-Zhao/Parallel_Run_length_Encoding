/*
References:
All the discussions on Discord.
https://blog.csdn.net/cuhkljt/article/details/12656089 | c语言将一个string 数字转成 int 数字_IT熱血青年的博客-CSDN博客_c语言 string转int
https://blog.csdn.net/tong_xin2010/article/details/41895507 | 【转】结构体定义在头文件.h中和定义在.c中的不同考虑_tong_xin2010的博客-CSDN博客_头文件定义的结构体在c中不能用
http://c.biancheng.net/view/8628.html | pthread_join()函数：等待线程执行结束
https://blog.csdn.net/u013632755/article/details/115421267 | Unix C语言POSIX的线程创建、获取线程ID、汇合线程、分离线程、终止线程、线程的比较_lucky_白杨的博客-CSDN博客
https://blog.csdn.net/qq_36359022/article/details/78796784 | 线程池原理及C语言实现线程池_青城山小和尚的博客-CSDN博客_c语言线程池
https://www.cnblogs.com/XiaoXiaoShuai-/p/11855408.html | pthread_cond_broadcast & pthread_cond_signal - 机智的小小帅 - 博客园
https://blog.csdn.net/Mculover666/article/details/106646339 | C语言中getopt()函数的用法_Mculover666的博客-CSDN博客_getopt
https://stackoverflow.com/questions/9642732/parsing-command-line-arguments-in-c#:~:text=Parsing%20command-line%20arguments%20in%20C%201%20if%20the,defaults%20to%20comparing%20the%20files%20character%20by%20character. | Parsing command-line arguments in C - Stack Overflow
https://stackoverflow.com/questions/17490033/do-i-need-to-keep-a-file-open-after-calling-mmap-on-it | c - Do I need to keep a file open after calling mmap on it? - Stack Overflow
*/

#include "global.h"

int main(int argc, char **argv)
{
    // muti-threads
    if (getopt(argc, argv, "j:") != -1)
    {
        multi_threads(argc, argv, atoi(optarg));
    }

    // single-thread
    else
    {
        unsigned char *string = (unsigned char *)malloc(1024 * 1024 * 1024); // alloc 1G
        int pos;

        pos = single_thread(string, argc, argv);
        write(1, string, pos + 1);

        free(string);
    }

    return 0;
}