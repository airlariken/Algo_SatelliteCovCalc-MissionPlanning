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
struct Rectangle {
    EarthPos left_up;
    EarthPos right_up;
    EarthPos left_down;
    EarthPos right_down;
    Rectangle(const float &max_x, const float &min_x, const float &max_y, const float &min_y)
    {
        left_up = EarthPos(min_x, max_y);
        right_up = EarthPos(max_x, max_y);
        left_down = EarthPos(min_x, min_y);
        right_down = EarthPos(max_x, min_y);
    }
    Rectangle() = default;
};

struct SatelliteCovArea{
    vector<EarthPos> polygon_pos;       //记录多边形的所有顶点
    EarthPos circle_center;
    float circle_radius;
    Rectangle rec_boundary;             //矩形边界
    
    //cstor
        SatelliteCovArea() = default;
    void ini();//circle和rec的初始化
    void findMaxMin(float &max_x, float &min_x, float &max_y, float &min_y) const;
    void getBoundary();
    
    bool isInside(const EarthPos &p) const;
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
struct TargetInfo{
    string target_name;
    EarthPos _pos;
    int observe_time = 0;
    int _reward = 0;
    
    TargetInfo(string name, const EarthPos &p, const int &obs_t, const int &reward):target_name(name), _pos(p), observe_time(obs_t), _reward(reward){}
};
class SatelliteInfoManagement
{
//private:
public:
    vector<SatelliteCovArea*> satellite_timetable;
    vector<TargetInfo*> target_table;
    
public:
    void readSatInfoFile();
    void readTarInfoFile();
    EarthTime getTime(const int& cnt, const EarthTime& start_time) const;//传入该卫星的起始观察时间和第几秒观察反推出目前时间
    EarthTime getTime(string s) const;//传入该卫星的时间字符串返回一个Earthtime数据结构
    
    //test
    EarthTime satellite_1_starttime;
};
#endif /* SatelliteInfoManagement_hpp */
