//
//  SatelliteSchedulePlanning.hpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/5.
//

#ifndef SatelliteSchedulePlanning_hpp
#define SatelliteSchedulePlanning_hpp

#include "SatelliteInfoManagement.hpp"

//GenAlgo macro and include file
#define MUTATION_RATE 5 //概率倒数
#define GENERATIONS 100
#include <random>

// @this .h and related .cpp file is used for Q2 & Q3 problems' solutions

struct TargetScheduleInfo: public TargetInfo {
    bool is_scheduled = 0;
    int remaining_time = 0;
    int scheduled_sate_num = -1;
    time_period scheduled_time = time_period(-1,-1);
    
    TargetScheduleInfo(string name, const EarthPos &p, const int &obs_t, const int &reward):TargetInfo(name, p, obs_t, reward), remaining_time(obs_t){}
};

struct TimePieceInfo {                           //按秒存时间片。。。
    int conflict_cnt = 0;
    set<int> target_num_table;                  //记录该s内的target下标
    TimePieceInfo(const int &target_num){target_num_table.insert(target_num);}
    set<int> findDifference(const TimePieceInfo & TPI);
};

struct TimeInterval
{
    map<int, TimePieceInfo> time_pieces;//每个int都是切成片的时间段,第二个是标记该时间段的种类
    void insertTimeInterval(const time_period &p, const int& target_num);

};

struct FeasibleTimeIntervals
{
    TimeInterval TI;
    vector<map<int, TimePieceInfo>> feasibleTimeIntervals_table;
    void dividedInterval();
    void __aux_dividedInterval(map<int, TimePieceInfo>::iterator it);
    //该函数用于找到时间段冲突数为0的一个period

    bool SecondInsideDelete(const int &second);
    bool eraseScheduledTimePiece(vector<map<int, TimePieceInfo>>::iterator &it_m, map<int, TimePieceInfo>::iterator e_begin, map<int, TimePieceInfo>::iterator e_end);
    //输入的秒数如果在里面则删除且返回true，如果不在则不执行任何操作返回false
    
    static time_period findNextNoConflictTimePeriod(const map<int, TimePieceInfo> &m, map<int, TimePieceInfo>::iterator& it, int &target_num);
    
    //该函数用于找到该时间段中冲突数最小的一个period，并且返回该冲突的所有target_num，为后续做target选择做准备
    static time_period findSmallestConflictsTimePeriod(const map<int, TimePieceInfo> &m, set<int> &Vec_target_num);

    //从头开始截取一个time interval进行贪心分析
    static time_period getBeginTimePeriod(const map<int, TimePieceInfo> &m, set<int> &Vec_target_num);
   
};



class SatelliteSchedulePlanning
{
private:
    vector<FeasibleTimeIntervals> all_sate_feasible_time_interval;
    vector<int>satellite_handling_time;//题目新加的要求，卫星观察完后要加一个处理时间
    vector<pair<time_period, TargetInfo>> useless_timewindow;
    int total_reward = 0;
    vector<bool> activated_sat;
public:
//private:
    vector<vector<vector<time_period>>> every_satellite_cov_window;
    map<string, TargetInfo> targetname_to_info;//由目标名字到target数据结构的映射
    vector<TargetScheduleInfo> all_targets_table;//全部读到一个vector中方便处理
    vector<TargetScheduleInfo> scheduled_targets;//pair中int为target下标，与all_target_table所对应
    
    
public:
//constructor
    SatelliteSchedulePlanning(){ini_clear();}
    
// files' reading segment
    void readSatCovWinFile(const int &file_num = 0);
    void readTarInfoFile(const int &file_num);//几乎照抄satlliteinfomanagt类中读文件的函数过来
    void readAllSatCovWinFiles();
    
//aux function
    inline int getDuration(const time_period &p) const;
    inline void ini_clear();
    bool _isSingleTarget(const int & sate_num, const int &target_num, const int &cnt_seconds);  //判定该target在该时间窗口是否一直是单覆盖
    inline bool _secondIsInTimeperiod(const time_period &p, const int &cnt_seconds);//该秒cnt_seconds在时间段p内返回true
    
//Algo segment
    //Q2
    void _setActivatedSat(vector<bool> activated_sat = {1,1,1,1,1,1,1,1,1});
    void greedyAlgo();
    void heuristicAlgo();
    void integerAlgo(time_period limit = time_period(0, 3600*24));
    //预处理函数，非常重要，缩小解空间
    void _preprocessing(time_period limit = time_period(0, 3600*24), string path_name_add = "");
    void InterativelyRemove();//循环处理冲突数为0的时间窗口，看看能不能分配
//    void _combineTimePeriod();
    
    
    //Q3
    void DPAlgo();
    void GeneticAlgo();
    void readScheduledTarInfFile(int file_num);
    //test&output fun
    inline void outputResult();
    void outputGreedyAlgoResult();
    
    
    
    struct Gen
    {
        int fitness;
        vector<bool> code;
        Gen(int n):code(n,0){}
    };
    struct Genetic_Algo
    {
        vector<TargetScheduleInfo> GenAlgo_target_table;
        vector<Gen*> gen_table;
        mt19937 e;
        double threshod;
        int MaxMemroy = 600;
        int code_len;
    public:
        //constructor
        Genetic_Algo(const int& gen_num,const int& code_length, const vector<TargetScheduleInfo> &tf) : GenAlgo_target_table(tf), code_len(code_length){
            for (int i = 0; i < gen_num; ++i)
                gen_table.push_back(new Gen(code_length));
            
            int obs_time_sum = 0;
            for (auto it = GenAlgo_target_table.begin(); it != GenAlgo_target_table.end(); ++it)
                obs_time_sum += it->observe_time;
            threshod = (double)MaxMemroy/obs_time_sum;
            
        }
        void ini();             //ini every gens' codes
        void mainGeneticFun();  //Genetic Algo main entry function
//        void coding();        //unused in this situation
        double fx(Gen g);       //fitness evaluation function
        bool isFeasible(Gen g); //judging whether the code is feasible
        void evalFitness();
        void selection();
        void crossOver(Gen* g1, Gen* g2);
        void mutation(Gen* g);
    };
};







//  @function definition only inline function can be defined in .h file
//  @other function definition should be defined in .cpp file
//  @inline functions must be definded in .h file

inline set<int> TimePieceInfo::findDifference(const TimePieceInfo & TPI) {   //给定这一秒和下一秒的时间片，下一秒的时间片增加的target就是该target开始的时间位置
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

inline void TimeInterval::insertTimeInterval(const time_period &p, const int& target_num){
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


inline void SatelliteSchedulePlanning::ini_clear()
{
    every_satellite_cov_window.clear();
    every_satellite_cov_window.resize(SATELLITE_FILE_CNT);
    targetname_to_info.clear();
    all_targets_table.clear();
    scheduled_targets.clear();
    satellite_handling_time = vector<int>{30,30,30,35,35,35,25,25,25};//题目给出的处理时间
    total_reward = 0;
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

inline void SatelliteSchedulePlanning::outputResult()
{
    for (auto it = scheduled_targets.begin(); it != scheduled_targets.end(); ++it) {
        cout<<it->target_name<<'\t'<<it->scheduled_time.first<<'\t'<<it->scheduled_time.first<<"target_num:"<<it->scheduled_sate_num<<endl;
    }
}

inline void SatelliteSchedulePlanning::_setActivatedSat(vector<bool> t_activated_sat)
{
    activated_sat = t_activated_sat;
}

inline void FeasibleTimeIntervals::dividedInterval(){
    __aux_dividedInterval(TI.time_pieces.begin());
}
inline void FeasibleTimeIntervals::__aux_dividedInterval(map<int, TimePieceInfo>::iterator it) {
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


inline bool FeasibleTimeIntervals::eraseScheduledTimePiece(vector<map<int, TimePieceInfo>>::iterator &it_m, map<int, TimePieceInfo>::iterator e_begin, map<int, TimePieceInfo>::iterator e_end) {
    while(e_end != e_begin)
        e_begin = it_m->erase(e_begin);
    //由于删除时间片后可能将原有连续的时间片切成两半，故分割成两个
    map<int, TimePieceInfo> t_map;
    while (e_begin != it_m->end()) {
        t_map.insert(*e_begin);
        e_begin = it_m->erase(e_begin);
    }
    if (t_map.size() != 0) {            //捏吗，push_back会让vector的所有迭代器失效！！！
        long dis = distance(feasibleTimeIntervals_table.begin(), it_m);
        feasibleTimeIntervals_table.push_back(t_map);
        it_m = feasibleTimeIntervals_table.begin() + dis;
    }
    
    //上面erase可能导致m为空，于是在feas_table里面将空的处理掉，并且防止迭代器失效，所以传入参数是迭代器的引用
    if (it_m->size() == 0) {
        it_m = feasibleTimeIntervals_table.erase(it_m);
        return true;
    }
    return false;
}
inline bool FeasibleTimeIntervals::SecondInsideDelete(const int &second) {
    for (auto it = feasibleTimeIntervals_table.begin(); it != feasibleTimeIntervals_table.end(); ++it) {
        auto fi = it->find(second);
        if (fi != it->end()) {
            it->erase(fi);
            return true;
        }
    }
    return false;
}

inline time_period FeasibleTimeIntervals::findNextNoConflictTimePeriod(const map<int, TimePieceInfo> &m, map<int, TimePieceInfo>::iterator& it, int &target_num)
{         //第一个参数是map的引用，第二个是该map迭代器，第三个是卫星编号
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
inline time_period FeasibleTimeIntervals::findSmallestConflictsTimePeriod(const map<int, TimePieceInfo> &m, set<int> &Vec_target_num) {
    bool start_tag=0;
    time_period t_p;
    int t_minconflicts = INT_MAX;
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it->second.conflict_cnt < t_minconflicts) {
            start_tag = 1;
            t_p.first = it->first;
            Vec_target_num = it->second.target_num_table;
        }
        if (start_tag == 1 && it->second.conflict_cnt > t_minconflicts) {
            t_p.second = it->first;
//                cout<<"conflicts times:"<<t_minconflicts<<endl;
            return t_p;
        }
    }
    return time_period(-1,-1);
}
inline time_period FeasibleTimeIntervals::getBeginTimePeriod(const map<int, TimePieceInfo> &m, set<int> &Vec_target_num)
{
    time_period t_p;
    Vec_target_num.clear();
    if (m.size() == 0)
        return time_period(-1,-1);
    int t_conflicts = m.begin()->second.conflict_cnt;
    Vec_target_num.insert(m.begin()->second.target_num_table.begin(), m.begin()->second.target_num_table.end());
    t_p.first = m.begin()->first;
    for (auto it = m.begin(); it != m.end(); ++it) {
        if (it->second.conflict_cnt != t_conflicts) {
            t_p.second = it->first;
            cout<<"conflicts times:"<<t_conflicts<<endl;
            return t_p;
        }
    }
    t_p.second = m.end()->first;
    return t_p;
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
struct sortMinReward {
    bool operator()(const pair<TargetScheduleInfo, int> &inf1, const pair<TargetScheduleInfo, int> &inf2)const {
        return (float)inf1.first._reward/inf1.first.remaining_time < (float)inf2.first._reward/inf2.first.remaining_time;
    }
};

#endif /* SatelliteSchedulePlanning_hpp */
