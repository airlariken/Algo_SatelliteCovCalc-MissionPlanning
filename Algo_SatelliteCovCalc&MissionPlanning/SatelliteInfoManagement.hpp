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
    void _findMaxMin(float &max_x, float &min_x, float &max_y, float &min_y) const;
    void _getBoundary();
    void _getCircle();
    
    bool isInside_circle(const EarthPos &p) const;
    bool isInside_polygon(const EarthPos &p) const;
};
struct EarthTime{
    int _day = 0;
    int _hours = 0;
    int _minutes = 0;
    int _seconds = 0;
    string _date = " ";//年月不纳入计算默认认为不会改变
    EarthTime() = default;
    EarthTime(int day, int hour, int minute, int second, string date): _day(day), _hours(hour), _minutes(minute), _seconds(second), _date(date){}
    
//    ostream &operator<<(ostream &out);
    friend ostream& operator<<(ostream& co,const EarthTime& t);
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
private:
    typedef pair<EarthTime, EarthTime> time_period;
//private:
public:
//    vector<SatelliteCovArea> singal_satellite_timetable;
    vector<vector<SatelliteCovArea>> all_satellite_timetable;
    vector<TargetInfo*> target_table;
    
public:
    void readSatInfoFile();
    void readTarInfoFile();
//    void saveSatData();
//    void readSatData();
    EarthTime getTime(const int& cnt, const EarthTime& start_time) const;//传入该卫星的起始观察时间和第几秒观察反推出目前时间
    EarthTime getTime(string s) const;//传入该卫星的时间字符串返回一个Earthtime数据结构
    
    void coverCal();
    void _signalCov();
    void _doubleCov();
    //test
    EarthTime satellite_1_starttime;
};

struct tar_window_sort{
    bool operator()(const pair<EarthTime, int> &obj1,const pair<EarthTime, int> &obj2) const {
        if (obj1.second < obj2.second)
            return true;
        return false;
    }

};
struct tar_window_unique{
    bool operator()(const pair<EarthTime, int> &obj1,const pair<EarthTime, int> &obj2) const {
        if (obj1.second == obj2.second)
            return true;
        return false;
    }
};

struct two_vec
{
    EarthPos start_vec; //裁剪窗口边界向量起点
    EarthPos end_vec; //裁剪窗口边界向量终点
//    bool result(const EarthPos &point) {
//    return (end_vec._x - start_vec._x) * (point._y - start_vec._y) - (end_vec._y - start_vec._y) * (point._x - start_vec._x);
//    }
    int result() {
        float t = start_vec._x*end_vec._y - end_vec._x*start_vec._y;
        if (t > 0)
            return 1;
        else if (t < 0)
            return -1;
        else return 0;
    }
    two_vec(const EarthPos &p1, const EarthPos &p2, const EarthPos &p3, const EarthPos &p4) {
        start_vec = EarthPos(p2._x-p1._x, p2._y-p1._y);
        end_vec = EarthPos(p4._x-p3._x, p4._y-p3._y);
    }
    two_vec() = default;
};
 

#endif /* SatelliteInfoManagement_hpp */
