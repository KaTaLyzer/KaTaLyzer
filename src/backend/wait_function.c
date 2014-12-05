#include "wait_function.h"

void waiting()
{
    printf("Waiting...\n");
	struct tm *timetm;
	time_t timenow;
	for(;;){
                time( &timenow );
                timetm = localtime( &timenow );

                if(timetm->tm_sec != 0)
                {
                        //pockame az na novu minutu...
                        msleep(20);
                        fflush(stdout);
                }
                else break;
    }
}

//toto su funkcie, ktore potrebujem na spanie na 10 milisekund...
int __nsleep(const struct timespec *req, struct timespec *rem)
{
    struct timespec temp_rem;
    if(nanosleep(req,rem)==-1) __nsleep(rem,&temp_rem);
    else return 1;
    return 0;
}

int msleep(unsigned long milisec)
{
    struct timespec req={0},rem={0};
    time_t sec=(int)(milisec/1000);
    milisec=milisec-(sec*1000);
    req.tv_sec=sec;
    req.tv_nsec=milisec*1000000L;
    __nsleep(&req,&rem);
    return 1;
}
// potialto su funkcie, ktore potrebujem na spanie na 10 milisekund...

