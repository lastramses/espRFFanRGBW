
#include "espRFFanGlobals.h"
#include <includes.h>
#include <LocalTime.h>

void printLocalTime(){
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
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[80];
  time (&rawtime);
  timeinfo = localtime (&rawtime);

  strftime (buffer,80," %d %B %Y %H:%M:%S ",timeinfo);
  stdOut(buffer);
  
}

uint8_t isTime(uint8_t day, uint8_t hour, uint8_t minute){
  time_t rawtime;
  struct tm * timeinfo;
  char buffer[3];
  time (&rawtime);
  timeinfo = localtime (&rawtime);

  uint8_t timeCheck=0x0;

  strftime (buffer,3,"%w",timeinfo);
  uint8_t dayOfWeek = 1<<(atoi(&buffer[0]));
  //stdOut("dayOfWeek="+String(dayOfWeek) + ", day=" + String(day));
  if ((day&dayOfWeek) > 0)
    timeCheck |= 0b0001;
  strftime (buffer,3,"%H",timeinfo);
  //stdOut("buffer="+String(buffer)+"atoi(&buffer[0])="+String(atoi(&buffer[0])) + ", hour=" + String(hour));
  if (atoi(&buffer[0])==hour)
    timeCheck |= 0b0010;
  strftime (buffer,3,"%M",timeinfo);
  //stdOut("buffer="+String(buffer)+"atoi(&buffer[0])="+String(atoi(&buffer[0])) + ", minute=" + String(minute));
  if (atoi(&buffer[0])==minute)
    timeCheck |= 0b0100;
  //stdOut("timeCheck="+String(timeCheck));
  if (timeCheck == 0b0111)
    return besTRUE;
  else
    return besFALSE;
}