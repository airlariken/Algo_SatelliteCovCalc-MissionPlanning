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

enum time_piece_set
{
    FeasibleTimeInterval, DisabledTimeInterval, Back
};
//enum time_window_set
//{
//
//}

struct TimeInterval
{
    map<int, time_piece_set>time_pieces;//每个int都是切成片的时间段,第二个是标记该时间段的种类
    bool isInsideInterval(const time_period &p){
        if (p.first >= time_pieces.begin()->first || p.second <= (--time_pieces.end())->first) {
            time_pieces.insert(pair<int, time_piece_set>(p.first, FeasibleTimeInterval));
            time_pieces.insert(pair<int, time_piece_set>(p.second, FeasibleTimeInterval));
            if (p.second == (--time_pieces.end())->first) {     //如果p.second是当前interval的最后一个，则置为back,并把原来的back置为feasible
                (--(--time_pieces.end()))->second = FeasibleTimeInterval;
                (--time_pieces.end())->second = Back;
            }
            return true;
        }
        return false;
    }
    void outputIntervals(){
        for (auto it = time_pieces.begin(); it != time_pieces.end(); ++it) {
            cout<<it->first<<'\t'<<it->second<<endl;
        }
    }
};
struct AllTimeIntervals         //多个时间段拼在一起
{
    vector<TimeInterval> time_interval_table;
    inline void addTimeWindow(const time_period &p);
};
class SatelliteSchedulePlanning
{
private:
    vector<AllTimeIntervals> all_sate_feasible_time_interval;
    vector<int>satellite_handling_time;
public:
    vector<vector<vector<time_period>>> every_satellite_cov_window;
    map<string, TargetInfo> targetname_to_info;//由目标名字到target数据结构的映射
    vector<TargetScheduleInfo> all_targets_table;//全部读到一个vector中方便处理
    
    vector<TargetScheduleInfo> scheduled_targets;//pair中int为target下标，与all_target_table所对应
    
    
public:
    void readSatCovWinFile(const int &file_num = 0);
    void readTarInfoFile(const int &file_num);//几乎照抄satlliteinfomanagt类中读文件的函数过来
    void readAllSatCovWinFiles();
    inline int getDuration(const time_period &p) const;
    inline void ini_clear();
    SatelliteSchedulePlanning(){ini_clear();}

    
    
    bool _isSingleTarget(const int & sate_num, const int &target_num, const int &cnt_seconds);  //判定该target在该时间窗口是否一直是单覆盖
    inline bool _secondIsInTimeperiod(const time_period &p, const int &cnt_seconds);
    
    void greedyAlgo();
    void _preprocessing();
    void _preprocessInterativlyRemove(TimeInterval & itrv);
//    void _combineTimePeriod();
};

//inline functions must be definded in .h file
inline void SatelliteSchedulePlanning::ini_clear()
{
    every_satellite_cov_window.clear();
    every_satellite_cov_window.resize(SATELLITE_FILE_CNT);
    targetname_to_info.clear();
    all_targets_table.clear();
    scheduled_targets.clear();
    satellite_handling_time = vector<int>{30,30,30,35,35,35,25,25,25};//题目给出的处理时间
}


inline int SatelliteSchedulePlanning::getDuration(const time_period &p) const
{
    return p.second - p.first;
}

inline bool SatelliteSchedulePlanning::_isSingleTarget(const int & sate_num, const int &target_num, const int &cnt_seconds)
{
    for (auto it = every_satellite_cov_window[sate_num][target_num].begin(); it != every_satellite_cov_window[sate_num][target_num].end(); ++it) {
        if (_secondIsInTimeperiod(*it, cnt_seconds)) {
            return true;
        }
    }
    return 1;
}

inline bool SatelliteSchedulePlanning::_secondIsInTimeperiod(const time_period &p, const int &cnt_seconds)
{
    return (cnt_seconds < p.second) && (cnt_seconds > p.first);
}




inline void AllTimeIntervals::addTimeWindow(const time_period &p)
{
    for (auto it = time_interval_table.begin(); it != time_interval_table.end(); ++it) {
        if (it->isInsideInterval(p)) {
            break;
        }
    }
}






//pred
struct time_piece_is_feasible_pred{
    bool operator()(const pair<int, time_piece_set> &piece){
        return piece.second == FeasibleTimeInterval;
    }
};
#endif /* SatelliteSchedulePlanning_hpp */
