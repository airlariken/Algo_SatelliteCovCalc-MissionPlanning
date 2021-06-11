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
void SatelliteSchedulePlanning::greedyAlgo(vector<bool> activated_sat)
{
    //贪心选择：按照收益/时间的大小进行排序，在可行时间段中按该顺序选择target进行观测，直到该区间无法再加入新的目标观测为止
}

void SatelliteSchedulePlanning::_preprocessing(vector<bool> activated_sat)
{
    //对于一个卫星
    //如果某一个观察窗口的长度小于该target所需观察时间，直接删除
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
        if (activated_sat[it1-every_satellite_cov_window.begin()] == false)//没被激活不算进去
            continue;
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                if (getDuration(*it3) < getDuration(all_targets_table[it2-it1->begin()].scheduled_time)){
                    it3 = it2->erase(it3);
                    cout<<"erase:"<<it3->first<<'\t'<<it3->second<<endl;
                }
            }
        }
    }
    

    
    
    //将单个卫星的所有观测窗口合并，但要保留时间窗口的起点和终点，方便后续检测和处理
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
        feasibleTimeIntervals temp_TI;
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            if (all_targets_table[it2-it1->begin()].is_scheduled == 1)      //target的is_scheduled为1，则不会在访问该target任何信息
                continue;
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                temp_TI.TI.insertTimeInterval(*it3, (int)(it2-it1->begin()));
            }
        }
        all_sate_feasible_time_interval.push_back(temp_TI);
        (--all_sate_feasible_time_interval.end())->dividedInterval();
    }
    
    //处理好只有单目标的覆盖并且在该覆盖时间内能解决问题的情况，如果target观测完成，则将该target的is_scheduled置为1，以后不会在访问该target任何信息
    int total_reward = 0;
    for (auto it1 = all_sate_feasible_time_interval.begin(); it1 != all_sate_feasible_time_interval.end(); ++it1) {
        if (it1->TI.time_pieces.size() == 0)//如果该卫星没有被activated 则为空
            continue;
        //找到冲突数为0的时间段，直接分配出去
        for (auto it2 = it1->feasibleTimeIntervals_table.begin(); it2 != it1->feasibleTimeIntervals_table.end(); ++it2) {
            auto t_it = it2->begin();//map.begin
            while (t_it != it2->end()) {
                int target_num = -1;
                time_period t_p = feasibleTimeIntervals::findNextNoConflictTimePeriod(*it2, t_it, target_num);
                if (t_p.first == -1) {
                    break;
                }
                int t_duration = getDuration(t_p);
//                cout<<"target_num: "<<target_num<<"duration: "<<t_duration<<endl;
                //未分配且冲突数为0的直接分配出去
                if (all_targets_table[target_num].is_scheduled == 0 && t_duration >= satellite_handling_time[it1-all_sate_feasible_time_interval.begin()] + all_targets_table[target_num].observe_time) {
                    //安排分配
                    cout<<"target_num"<<target_num<<"is scheduled!(sate_num:"<<it1-all_sate_feasible_time_interval.begin()<<')'<<endl;
                    all_targets_table[target_num].is_scheduled = 1;
                    total_reward += all_targets_table[target_num]._reward;//加奖励！！！！！！
                    all_targets_table[target_num].scheduled_time = t_p;
                    break;
                }
                
            }

        }
    }
    cout<<"total reward :"<<total_reward<<endl;
//    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
//        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
//            if (all_targets_table[it2-it1->begin()].is_scheduled == 1)      //target的is_scheduled为1，则不会在访问该target任何信息
//                continue;
//            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
//                if (getDuration(*it3) >= getDuration(all_targets_table[it2-it1->begin()].scheduled_time) + satellite_handling_time[it1-every_satellite_cov_window.begin()]) {
//                    TargetScheduleInfo &cur = all_targets_table[it2-it1->begin()];
//                    cur.is_scheduled = 1;
//                    scheduled_targets.push_back(cur);
//                    cout<<"target "<<cur.target_name<<"is scheduled!"<<endl;
//                    break;//这个循环内全是该target的时间窗口，如果进入if说明已经被分配，直接跳出该循环。
//                }
//            }
//        }
//    }
    
    
    //对于卫星的可用时间段（内部可能包含许多重叠的目标观测时间窗口），不断的循环处理头尾，如果能分配则分配，直到无法直接分配为止。
    
    cout<<"finish"<<endl;
    return;
}
void SatelliteSchedulePlanning::_preprocessInterativlyRemove()
{

}



void SatelliteSchedulePlanning::algoChoiceAndSatAct()
{
//    vector<int> activated_sat;//可用卫星
//    char code = '3';
//    _preprocessing()
//    switch (code) {
//        case '1':
//            <#statements#>
//            break;
//
//        default:
//            break;
//    }
}
