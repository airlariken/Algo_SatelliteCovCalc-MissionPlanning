//
//  AllKindOfStruct.h
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/4.
//

#ifndef AllKindOfStruct_h
#define AllKindOfStruct_h
#include "Header.h"

struct EarthPos{
    double _x = -1;
    double _y = -1;
    EarthPos() = default;
    EarthPos(double x, double y):_x(x), _y(y){}
};
//struct Rectangle {
//    EarthPos left_up;
//    EarthPos right_up;
//    EarthPos left_down;
//    EarthPos right_down;
//    Rectangle(const float &max_x, const float &min_x, const float &max_y, const float &min_y)
//    {
//        left_up = EarthPos(min_x, max_y);
//        right_up = EarthPos(max_x, max_y);
//        left_down = EarthPos(min_x, min_y);
//        right_down = EarthPos(max_x, min_y);
//    }
//    Rectangle() = default;
//};


struct EarthTime{
    int _day = 0;
    int _hours = 0;
    int _minutes = 0;
    int _seconds = 0;
    string _date = " ";//年月不纳入计算默认认为不会改变
    EarthTime() = default;
    EarthTime(int day, int hour, int minute, int second, string date): _day(day), _hours(hour), _minutes(minute), _seconds(second), _date(date){}
    
//    bool operator==(const EarthTime& t){
//        if (this->_date == t._date && this->_hours == t._hours && this->_minutes == t._minutes && this->_seconds == t._seconds) {
//            return true;
//        }
//        return false;
//    }
    
    friend ostream& operator<<(ostream& co,const EarthTime& t);
};

typedef pair<int, int> time_period;

struct TargetInfo{
    string target_name;
    EarthPos _pos;
    int observe_time = 0;
    int _reward = 0;
    
    TargetInfo(string name, const EarthPos &p, const int &obs_t, const int &reward):target_name(name), _pos(p), observe_time(obs_t), _reward(reward){}
};


//函数对象
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

struct two_vec//两个向量
{
    EarthPos start_vec; //裁剪窗口边界向量起点
    EarthPos end_vec; //裁剪窗口边界向量终点
    //result函数返回叉乘方向
    int result() {
        double t = start_vec._x*end_vec._y - end_vec._x*start_vec._y;
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
 

#endif /* AllKindOfStruct_h */
