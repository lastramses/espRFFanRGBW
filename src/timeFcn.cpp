
#include "espGlobals.h"
#include <includes.h>
#include <timeFcn.h>
/*
 %a Abbreviated weekday name
 %A Full weekday name
 %b Abbreviated month name
 %B Full month name
 %c Date and time representation for your locale
 %d Day of month as a decimal number (01-31)
 %H Hour in 24-hour format (00-23)
 %I Hour in 12-hour format (01-12)
 %j Day of year as decimal number (001-366)
 %m Month as decimal number (01-12)
 %M Minute as decimal number (00-59)
 %p Current locale's A.M./P.M. indicator for 12-hour clock
 %S Second as decimal number (00-59)
 %U Week of year as decimal number,  Sunday as first day of week (00-51)
 %w Weekday as decimal number (0-6; Sunday is 0)
 %W Week of year as decimal number, Monday as first day of week (00-51)
 %x Date representation for current locale
 %X Time representation for current locale
 %y Year without century, as decimal number (00-99)
 %Y Year with century, as decimal number
 %z %Z Time-zone name or abbreviation, (no characters if time zone is unknown)
*/

long getTiNowRaw(){
  time_t tiRaw;
  time (&tiRaw);
  return tiRaw;
}

tm* convRawTiToLoc(time_t tiRaw){
  return localtime(&tiRaw);
}

String getTiStr(long tiRaw){
  struct tm* tiLocal = convRawTiToLoc(tiRaw);
  char buffer[80];
  strftime (buffer,80,"%d %B %Y %H:%M:%S",tiLocal);
  return String(buffer);
}

void printTiLocNow(){
  stdOut(getTiStr(getTiNowRaw()));
  /*
  time_t rawtime;
  struct tm * timeinfo;
  time (&rawtime);
  timeinfo = localtime (&rawtime);
  stdOut("tm_wday=" +String(timeinfo->tm_wday));
  stdOut("tm_hour=" +String(timeinfo->tm_hour));
  stdOut("tm_min=" +String(timeinfo->tm_min));
  stdOut("tm_isdst=" +String(timeinfo->tm_isdst));
  */
}

uint8_t isLocTi(uint8_t day, uint8_t hour, uint8_t minute){
  struct tm* tiNow = convRawTiToLoc(getTiNowRaw());

  uint8_t stTiChk=0x0;
  
  if ((day&(1<<tiNow->tm_wday))>0) //day is encoded as 0b0 Sat Fri ... Sun 
    stTiChk |= 0b0001;
  if (hour==tiNow->tm_hour)
    stTiChk |= 0b0010;
  if (minute==tiNow->tm_min)
    stTiChk |= 0b0100;
  
  if (stTiChk == 0b0111)
    return fsTRUE;
  else
    return fsFALSE;
}