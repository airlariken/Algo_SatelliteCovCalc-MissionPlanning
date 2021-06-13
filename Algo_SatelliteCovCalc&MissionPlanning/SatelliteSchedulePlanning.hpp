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

//enum time_piece_set
//{
//    FeasibleTimeInterval, DisabledTimeInterval, Back
//};
//enum time_window_set
//{
//
//}
struct TimePieceInfo{                           //按秒存时间片。。。
    int conflict_cnt = 0;
    set<int> target_num_table;                  //记录该s内的target下标
    TimePieceInfo(const int &target_num){target_num_table.insert(target_num);}
    set<int> find_difference(const TimePieceInfo & TPI) {   //给定这一秒和下一秒的时间片，下一秒的时间片增加的target就是该target开始的时间位置
        set<int> tmp_set;
        for (auto it1 = target_num_table.begin(); it1 != target_num_table.end(); ++it1) {
            if (TPI.target_num_table.find(*it1) == TPI.target_num_table.end()) {
                tmp_set.insert(*it1);
            }
        }
        for (auto it1 = TPI.target_num_table.begin(); it1 != TPI.target_num_table.end(); ++it1) {
            if (target_num_table.find(*it1) == TPI.target_num_table.end()) {
                tmp_set.insert(*it1);
            }
        }
        return tmp_set;
    }
};
struct TimeInterval
{
    map<int, TimePieceInfo> time_pieces;//每个int都是切成片的时间段,第二个是标记该时间段的种类
    void insertTimeInterval(const time_period &p, const int& target_num){
        for (int i = 0; i < p.second - p.first; ++i) {
            auto fi = time_pieces.find(p.first+i);
            if (fi != time_pieces.end()) {
                ++fi->second.conflict_cnt;
                fi->second.target_num_table.insert(target_num);
            }
            else
                time_pieces.insert(pair<int, TimePieceInfo>(p.first+i, TimePieceInfo(target_num)));
        }
    }

};
struct feasibleTimeIntervals
{
    TimeInterval TI;
    vector<map<int, TimePieceInfo>> feasibleTimeIntervals_table;
    void dividedInterval(){
        __aux_dividedInterval(TI.time_pieces.begin());
    }
    void __aux_dividedInterval(map<int, TimePieceInfo>::iterator it) {
        feasibleTimeIntervals_table.push_back(map<int, TimePieceInfo>());
        for (; it != TI.time_pieces.end(); ++it) {
            auto tempit = it;   ++tempit;//往前看一个
            if (tempit == TI.time_pieces.end()) {
                (--feasibleTimeIntervals_table.end())->insert(*it);
                return;                                          //递归出口
            }
            if (tempit->first == it->first+1) {             //意味着从这里断开了
                //就从这里开始拷贝数据,拷贝结束位置下一个是不为+1递增的地方
                (--feasibleTimeIntervals_table.end())->insert(*it);
            }
            else{
                //recursive call fun
                __aux_dividedInterval(++it);
                return;
            }
        }
    }
    
    static time_period findNextNoConflictTimePeriod(const map<int, TimePieceInfo> &m, map<int, TimePieceInfo>::iterator& it, int &target_num) {
        bool start_tag=0;
        time_period t_p;
        for (; it != m.end(); ++it) {
            if (start_tag == 0 && it->second.conflict_cnt == 0) {
                start_tag = 1;
                t_p.first = it->first;
                target_num = *(it->second.target_num_table.begin());
            }
            if (start_tag == 1 && it->second.conflict_cnt != 0) {
                t_p.second = it->first;
                return t_p;
            }
        }
        return time_period(-1,-1);
    }

    void eraseScheduledTimePiece(vector<map<int, TimePieceInfo>>::iterator &it_m, map<int, TimePieceInfo>::iterator e_begin, map<int, TimePieceInfo>::iterator e_end) {
        while(e_end != e_begin)
            e_begin = it_m->erase(e_begin);
        //由于删除时间片后可能将原有连续的时间片切成两半，故分割成两个
        map<int, TimePieceInfo> t_map;
        while (e_begin != it_m->end()) {
            t_map.insert(*e_begin);
            e_begin = it_m->erase(e_begin);
        }
        
//        for(auto it = feasibleTimeIntervals_table.begin(); it != feasibleTimeIntervals_table.end(); ++it) {//上面erase可能导致m为空，于是在feas里面将空的处理掉
            if (it_m->size() == 0)
                it_m = feasibleTimeIntervals_table.erase(it_m);
        
        feasibleTimeIntervals_table.push_back(t_map);
    }
};
class SatelliteSchedulePlanning
{
private:
    vector<feasibleTimeIntervals> all_sate_feasible_time_interval;
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
    
    void algoChoiceAndSatAct();
    void greedyAlgo(vector<bool> activated_sat = {1,1,1,1,1,1,1,1,1});
    void _preprocessing(vector<bool> activated_sat = {1,1,1,1,1,1,1,1,1});
    void _preprocessInterativlyRemove();
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






//pred
struct findTarget_with_Target_name {
    string name;
    findTarget_with_Target_name(string n):name(n){}
    bool operator()(const TargetScheduleInfo &info1){
        if (info1.target_name == name) {
            return true;
        }
        return false;
    }
    
};

#endif /* SatelliteSchedulePlanning_hpp */
