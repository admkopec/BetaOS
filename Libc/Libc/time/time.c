//
//  time.c
//  BetaOS
//
//  Created by Adam Kopeć on 11/30/15.
//  Copyright © 2015-2017 Adam Kopeć. All rights reserved.
//

#include <time.h>
#include <i386/pio.h>

#define CURRENT_CENTURY 21

unsigned char second;
unsigned char minute;
unsigned char hour;
unsigned char day;
unsigned char month;
unsigned int year;
char* pmam;
char* monthl;
char* dayofweekshort;
char* dayofweeklong;

static int century_register = 0x00;

void read_rtc(void);
enum {
    cmos_address = 0x70,
    cmos_data    = 0x71
};

int get_update_in_progress_flag(void);
unsigned char get_RTC_register(int reg);


//absolute_time_t absolute_UNIX = 0; // 0 - 1 January 1970
//absolute_time_t absolute_Beta = 0;
time_t absolute_UNIX = 0; // 0 - 1 January 1970
time_t absolute_Beta = 0;
//
#define Is_leap_year(year)  (((1970+year)>0)&&!((1970+year)%4)&&(((1970+year)%100)||!((1970+year)%400)))
#define Seconds_in_a_day    86400
#define Seconds_in_an_hour   3600
#define Seconds_in_a_minute    60
//
static const uint8_t monthDays[]={31,28,31,30,31,30,31,31,30,31,30,31};
//

time_t time(time_t * value) {
    read_rtc();
    absolute_UNIX = (year - 1970) * (Seconds_in_a_day * 365);
    for (uint32_t i = 1971; i < year; i++) {
        if (Is_leap_year(i)) {
            absolute_UNIX += Seconds_in_a_day;
        }
    }
    for (uint32_t i = 0; i < month-1; i++) {
        if ((i == 1) && Is_leap_year(year)) {
            absolute_UNIX += Seconds_in_a_day * 29;
        } else {
            absolute_UNIX += Seconds_in_a_day * monthDays[i];
        }
    }
    absolute_UNIX += day    * Seconds_in_a_day;
    absolute_UNIX += hour   * Seconds_in_an_hour;
    absolute_UNIX += minute * Seconds_in_a_minute;
    absolute_UNIX += second;
    if (value != NULL) {
        *value = absolute_UNIX;
    }
    return absolute_UNIX;
}

int get_update_in_progress_flag() {
    outb(cmos_address, 0x0A);
    return (inb(cmos_data) & 0x80);
}

unsigned char get_RTC_register(int reg) {
    outb(cmos_address, reg);
    return inb(cmos_data);
}

void read_rtc() {
    unsigned char century=21;
    unsigned char last_second;
    unsigned char last_minute;
    unsigned char last_hour;
    unsigned char last_day;
    unsigned char last_month;
    unsigned char last_year;
    unsigned char last_century;
    unsigned char registerB;


    while (get_update_in_progress_flag());
    second = get_RTC_register(0x00);
    minute = get_RTC_register(0x02);
    hour = get_RTC_register(0x04);
    day = get_RTC_register(0x07);
    month = get_RTC_register(0x08);
    year = get_RTC_register(0x09);
    if(century_register != 0) {
        century = get_RTC_register(century_register);
    }

    do {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
        last_century = century;

        while (get_update_in_progress_flag());
        second = get_RTC_register(0x00);
        minute = get_RTC_register(0x02);
        hour = get_RTC_register(0x04);
        day = get_RTC_register(0x07);
        month = get_RTC_register(0x08);
        year = get_RTC_register(0x09);
        if(century_register != 0) {
            century = get_RTC_register(century_register);
        }
    } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
            (last_day != day) || (last_month != month) || (last_year != year) ||
            (last_century != century) );

    registerB = get_RTC_register(0x0B);


    if (!(registerB & 0x04)) {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
        if(century_register != 0) {
            century = (century & 0x0F) + ((century / 16) * 10);
        }
    }


    if (!(registerB & 0x02) && (hour & 0x80)) {
        hour = ((hour & 0x7F) + 12) % 24;
    }


    if(century_register != 0) {
        year += century * 100;
    } else {
        year += (CURRENT_CENTURY-1) * 100;
    }
}

struct tm *gmtime_r(const time_t * timer __unused, struct tm * ptm) {
    ptm->tm_hour = hour;
    ptm->tm_min  = minute;
    ptm->tm_sec  = second;
    ptm->tm_mday = day;
    ptm->tm_mon  = month;
    ptm->tm_year = year - 1900;
    int y = year;
    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    y -= month < 3;
    int dayofweek = (y + y/4 - y/100 + y/400 + t[month-1] + day) % 7;
    ptm->tm_wday = dayofweek;
    
    return ptm;
}

struct tm *gmtime(const time_t * timer) {
    struct tm time;
    return gmtime_r(timer, &time);
}
//
//void translateMonth() {
//    switch (month) {
//        case 1:
//            monthl="January";
//            break;
//        case 2:
//            monthl="February";
//            break;
//        case 3:
//            monthl="March";
//            break;
//        case 4:
//            monthl="April";
//            break;
//        case 5:
//            monthl="May";
//            break;
//        case 6:
//            monthl="June";
//            break;
//        case 7:
//            monthl="July";
//            break;
//        case 8:
//            monthl="August";
//            break;
//        case 9:
//            monthl="September";
//            break;
//        case 10:
//            monthl="October";
//            break;
//        case 11:
//            monthl="November";
//            break;
//        case 12:
//            monthl="December";
//            break;
//
//        default:
//            break;
//    }
//}
//
//void translateDay() {
//    int y = year;
//    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
//    y -= month < 3;
//    int dayofweek = (y + y/4 - y/100 + y/400 + t[month-1] + day) % 7;
//
//    switch (dayofweek) {
//        case 0:
//            dayofweeklong = "Sunday";
//            dayofweekshort = "Sun";
//            break;
//        case 1:
//            dayofweeklong = "Monday";
//            dayofweekshort = "Mon";
//            break;
//        case 2:
//            dayofweeklong = "Tuesday";
//            dayofweekshort = "Tue";
//            break;
//        case 3:
//            dayofweeklong = "Wednesday";
//            dayofweekshort = "Wed";
//            break;
//        case 4:
//            dayofweeklong = "Thursday";
//            dayofweekshort = "Thu";
//            break;
//        case 5:
//            dayofweeklong = "Friday";
//            dayofweekshort = "Fri";
//            break;
//        case 6:
//            dayofweeklong = "Saturday";
//            dayofweekshort = "Sat";
//            break;
//        default:
//            break;
//    }
//}
//void translateHour() {
//    if (hour<12 && hour>0) {
//        pmam="AM";
//    } else if (hour>12 && hour<24) {
//        hour-=12;
//        pmam="PM";
//    } else if (hour==12) {
//        pmam="PM";
//    } else if (hour==0 || hour==24) {
//        hour=12;
//        pmam="AM";
//    }
//}
//
//void gettime() {
//    read_rtc();
//    translateHour();
//    translateDay();
//    translateMonth();
//}

