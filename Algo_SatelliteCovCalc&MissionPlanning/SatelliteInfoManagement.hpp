//
//  SatelliteInfoManagement.hpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#ifndef SatelliteInfoManagement_hpp
#define SatelliteInfoManagement_hpp

#include "Header.h"

struct EarthPos{
    float _x = -1;
    float _y = -1;
    EarthPos() = default;
    EarthPos(float x, float y):_x(x), _y(y){}
};

struct SatelliteCovArea{
    vector<EarthPos> polygo_pos;
    SatelliteCovArea() = default;
    
};
struct EarthTime{
    int _day = 0;
    int _hours = 0;
    int _minutes = 0;
    int _seconds = 0;
    string _date = " ";//年月不纳入计算默认认为不会改变
    EarthTime() = default;
    EarthTime(int day, int hour, int minute, int second, string date): _day(day), _hours(hour), _minutes(minute), _seconds(second), _date(date){}
    
    ostream &operator<<(ostream &out);
};

class SatelliteInfoManagement
{
    vector<SatelliteCovArea*> satellite_timetable;
    
    
public:
    void readSatInfoFile();
    EarthTime getTime(const int& cnt, const EarthTime& start_time) const;//传入该卫星的起始观察时间和第几秒观察反推出目前时间
    EarthTime getTime(string s) const;//传入该卫星的时间字符串返回一个Earthtime数据结构
    
    
    //test
    EarthTime satellite_1_starttime;
};
#endif /* SatelliteInfoManagement_hpp */
