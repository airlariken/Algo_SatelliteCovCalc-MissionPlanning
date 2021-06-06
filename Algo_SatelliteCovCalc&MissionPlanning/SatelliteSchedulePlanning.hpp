//
//  SatelliteSchedulePlanning.hpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/5.
//

#ifndef SatelliteSchedulePlanning_hpp
#define SatelliteSchedulePlanning_hpp

#include "SatelliteInfoManagement.hpp"

struct TargetScheduleInfo: public TargetInfo {
    bool is_scheduled = 0;
    int remaining_time = 0;
    time_period scheduled_time;
    
    TargetScheduleInfo(string name, const EarthPos &p, const int &obs_t, const int &reward):TargetInfo(name, p, obs_t, reward), remaining_time(obs_t){}
};


class SatelliteSchedulePlanning
{
//private:
public:
    vector<vector<vector<time_period>>> every_satellite_cov_window;
    map<string, TargetInfo> targetname_to_info;//由目标名字到target数据结构的映射
    vector<TargetScheduleInfo> all_targets_table;//全部读到一个vector中方便处理
    
//    list<pair<int, TargetInfo*>> unscheduled_targets;//这个target指针指向all_targets_table中的元素，方便对其变量obs_time进行处理
    vector<TargetScheduleInfo> scheduled_targets;//pair中int为target下标，与all_target_table所对应
public:
    void readSatCovWinFile(const int &file_num = 0);
    void readTarInfoFile(const int &file_num);//几乎照抄satlliteinfomanagt类中读文件的函数过来
    void readAllSatCovWinFiles();
    void greedyAlgo();
    void ini_clear();
    SatelliteSchedulePlanning(){ini_clear();}

};
#endif /* SatelliteSchedulePlanning_hpp */
