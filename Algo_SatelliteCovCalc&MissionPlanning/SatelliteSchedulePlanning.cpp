//
//  SatelliteSchedulePlanning.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/5.
//

#include "SatelliteSchedulePlanning.hpp"

void SatelliteSchedulePlanning::readSatCovWinFile(const int &file_num)
{
    // @variable vector<vector<vector<time_period>>>类型变量 every_satellite_cov_window a[i][j][m]表示第i个卫星的对第j个target有若干个观察窗口(period)，而这是第m个观察窗口
    string path = "mid_res";
    char a= file_num + '0';
    path.push_back(a);  path.insert(path.length(), ".txt");
    ifstream fin(path);
    if  (fin.fail()){
        cerr<<"cant open mid_res file(file_num"<<file_num<<')'<<endl;
        exit(9);
    }
    
    //开始循环读取每个target以及每个target内的每个卫星覆盖数据
    
    while (!fin.eof()) {
        string s;
        getline(fin, s);
        if(*s.begin() == 't') {//意味着是target_num这一行
            
            string str_target_nums = s.substr(11);
            int target_num = atoi(str_target_nums.c_str());
            cout<<target_num<<'\t';
            for(int i = 0; i < SATELLITE_FILE_CNT; ++i) {
                vector<time_period> temp_vec;
                getline(fin, s);
                int pos = (int)s.find("size");
                if (pos == -1)
                    continue;
                string temp = s.substr(pos+4);
    //            cout<<temp<<endl;
                int size = atoi(temp.c_str());
                for (int j = 0; j < size; ++j) {
                    fin>>s;
                    int start_time_cnt = atoi(s.c_str());
                    fin>>s;
                    int end_time_cnt = atoi(s.c_str());
                    temp_vec.push_back(time_period(start_time_cnt, end_time_cnt));
                    getline(fin, s);
                }
                every_satellite_cov_window[i].push_back(temp_vec);
            }
        }
    }

    cout<<"finish!"<<endl;
    return;
}
void SatelliteSchedulePlanning::readTarInfoFile(const int &file_num)
{
//    for (int i = 0; i < TARGET_FILE_CNT; ++i) {
        string path = "/Users/chenziwei/Desktop/算法课设/Data/TargetInfo/target.txt";
        const char a = file_num + '0';   string t;  t+=a;
               path.insert(60, t.c_str());
        ifstream fin(path);
        if (fin.fail()) {
            cerr<<"fail to open TargetInfofile!"<<endl;
            exit(1);
        }
        while (!fin.eof()) {
            string t_name;
            double x, y;
            int obs_t, reward;
            fin>>t_name>>x>>y>>obs_t>>reward;
            if (x < 0) {            //统一到卫星经度表示
                x += 360;
            }
            all_targets_table.push_back(TargetScheduleInfo(t_name, EarthPos(x, y), obs_t, reward)); //这里跟原函数有些区别
        }
        fin.close();
//    }

    return;
}
void SatelliteSchedulePlanning::readAllSatCovWinFiles()
{
    for (int i = 0; i < SATELLITE_FILE_CNT; ++i) {
        readSatCovWinFile(i);
    }
}
void SatelliteSchedulePlanning::greedyAlgo()
{
    //贪心选择：按照收益/时间的大小进行排序，在可行时间段中按该顺序选择target进行观测，直到该区间无法再加入新的目标观测为止
}

void SatelliteSchedulePlanning::_preprocessing()
{
    //对于一个卫星
    //如果某一个观察窗口的长度小于该target所需观察时间，直接删除
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                if (getDuration(*it3) < getDuration(all_targets_table[it2-it1->begin()].scheduled_time))
                    it3 = it2->erase(it3);
            }
        }
    }
    
    //处理好只有单目标的覆盖并且在该覆盖时间内能解决问题的情况，如果target观测完成，则将该target的is_scheduled置为1，以后不会在访问该target任何信息
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            if (all_targets_table[it2-it1->begin()].is_scheduled == 1)      //target的is_scheduled为1，则不会在访问该target任何信息
                continue;
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                if (getDuration(*it3) >= getDuration(all_targets_table[it2-it1->begin()].scheduled_time) + satellite_handling_time[it1-every_satellite_cov_window.begin()]) {
                    TargetScheduleInfo cur = all_targets_table[it2-it1->begin()];
                    cur.is_scheduled = 1;
                    scheduled_targets.push_back(cur);
                }
            }
        }
    }
    
    //将单个卫星的所有观测窗口合并，但要保留时间窗口的起点和终点，方便后续检测和处理
   
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
        AllTimeIntervals tmp_sate_feasible_time_interval;//对一个卫星的所有可行时间片汇总
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            if (all_targets_table[it2-it1->begin()].is_scheduled == 1)      //target的is_scheduled为1，则不会在访问该target任何信息
                continue;
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                tmp_sate_feasible_time_interval.addTimeWindow(*it3);
            }
        }
        all_sate_feasible_time_interval.push_back(tmp_sate_feasible_time_interval);
    }

    //对于卫星的可用时间段（内部可能包含许多重叠的目标观测时间窗口），不断的循环处理头尾，如果能分配则分配，直到无法直接分配为止。
    
    for (auto it1 = all_sate_feasible_time_interval.begin(); it1 != all_sate_feasible_time_interval.end(); ++it1) {
        for (auto it2 = it1->time_interval_table.begin(); it2 != it1->time_interval_table.end(); ++it2) {
            _preprocessInterativlyRemove(*it2);
        }
    }
}
void SatelliteSchedulePlanning::_preprocessInterativlyRemove(TimeInterval &itrv)
{
//    bool front_can_interative = 1, back_can_interative = 1;
//    while (1) {
//        auto it = find_if(itrv.time_pieces.begin(), itrv.time_pieces.end(), time_piece_is_feasible_pred());             //找到第一个不为disabled的对象
//    }
}



