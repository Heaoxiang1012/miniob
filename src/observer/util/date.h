#pragma once 

#include <stdio.h>
#include <string>
#include <sstream>
#include <stdint.h>

#include "rc.h"
#include "sql/parser/parse_defs.h"
#include "common/log/log.h"

inline bool is_leap_year(int year)
{
  return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}
inline bool is_valid_date(int year,int month,int day)
{
  int32_t date = year * 10000 + month * 100 + day;
  int32_t min_day = 1970 * 10000 + 1 * 100 + 1;
  int32_t max_day = 2038 * 10000 + 2 * 100;

  if(date < min_day || date > max_day){
    return false;
  }
  int max_month_day[] = {0,31,28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if(is_leap_year(year)){
    max_month_day[2] = 29;
  }

  if (month <= 0 || month > 12) return false;
  if (day <= 0 || day > max_month_day[month]) return false;

  return true;
}
inline RC string_to_date(const char *str,int32_t &date)
{
  int year = 0, month = 0, day = 0;
  int ret = sscanf(str, "%d-%d-%d", &year, &month, &day);
  // LOG_WARN("date : %d ,year: %d , month : %d , day : %d",date,year,month,day);
  if(ret!=3){
    return RC::INVALID_ARGUMENT;
  }

  if(!is_valid_date(year, month, day)){
    LOG_WARN("invaild date .");
    return RC::INVALID_ARGUMENT;
  }

  date = year * 10000 + month * 100 + day;

  // LOG_WARN("date : %d ,year: %d , month : %d , day :
  // %d",date,year,month,day);
  return RC::SUCCESS;
}
inline std::string need_zero(int t){
  if(t >= 10) 
    return std::to_string(t);
  else
    return "0" + std::to_string(t);
}

inline std::string date_to_string(int32_t date){
  std::string date_to_str = "";
  int year = date / 10000;
  int month = (date / 100) % 100;
  int day = date % 100;

  date_to_str =
      std::to_string(year) + '-' + need_zero(month) + '-' + need_zero(day);

  return date_to_str;
}
