//
//  SatelliteInfoManagement.hpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#ifndef SatelliteInfoManagement_hpp
#define SatelliteInfoManagement_hpp

#include "Header.h"
#include "AllKindOfStruct.h"
//文件个数宏定义
#define SATELLITE_FILE_CNT 9
#define TARGET_FILE_CNT 9

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

class SatelliteInfoManagement
{
private:
    typedef pair<EarthTime, EarthTime> time_period;
//private:
public:
//    vector<SatelliteCovArea> singal_satellite_timetable;
    vector<vector<SatelliteCovArea>> all_satellite_timetable;
//    vector<TargetInfo> target_table;
    vector<vector<TargetInfo>> all_target_table;
    
public:
    void readSatInfoFile();
    void readTarInfoFile();
//    void saveSatData();
//    void readSatData();
    inline EarthTime getTime(const int& cnt, const EarthTime& start_time) const;//传入该卫星的起始观察时间和第几秒观察反推出目前时间
    inline EarthTime getTime(string s) const;//传入该卫星的时间字符串返回一个Earthtime数据结构
    inline int getTime(EarthTime t, EarthTime start);//传入一个时间和开始时间返回经过的秒数
    
    void coverCal(const int &file_num = 1);
    void calAllTargetCoverage();
    void _saveEverySateObsWindow(vector<vector<pair<EarthTime, int>>>& target_i_window, ofstream &fout, const int &target_num);
    void _signalCov(vector<pair<EarthTime, int>> target_i_window_of_all_sat, ofstream &fout, const int &target_num);
    void _doubleCov(const int &file_num = 1);
    void _combine_coverage();//求所有卫星对单个target时间窗口的并集
    //test
    EarthTime satellite_1_starttime;
};




#endif /* SatelliteInfoManagement_hpp */
