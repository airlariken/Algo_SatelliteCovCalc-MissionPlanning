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
    return;
}
void SatelliteSchedulePlanning::readAllSatCovWinFiles()
{
    for (int i = 0; i < SATELLITE_FILE_CNT; ++i) {
        readSatCovWinFile(i);
    }
}

void SatelliteSchedulePlanning::_preprocessing()
{
    //对于一个卫星
    //如果某一个观察窗口的长度小于该target所需观察时间，直接删除
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
        if (activated_sat[it1-every_satellite_cov_window.begin()] == false)//没被激活不算进去
            continue;
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                if (getDuration(*it3) < all_targets_table[it2-it1->begin()].observe_time) {
                    it3 = it2->erase(it3);
                    cout<<"erase:"<<it3->first<<'\t'<<it3->second<<endl;
                }
            }
        }
    }
    
 
    //将单个卫星的所有观测窗口合并，但要保留时间窗口的起点和终点，方便后续检测和处理
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
        feasibleTimeIntervals temp_TI;
        if (activated_sat[it1-every_satellite_cov_window.begin()] == false)//没被激活不算进去
            continue;
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
    
    //循环处理单覆盖情况
    InterativelyRemove();
    //对于卫星的可用时间段（内部可能包含许多重叠的目标观测时间窗口），不断的循环处理头尾，如果能分配则分配，直到无法直接分配为止。
    
    cout<<"finish"<<endl;
    return;
}
void SatelliteSchedulePlanning::InterativelyRemove()
{
    bool is_finished = 0;
    //处理好只有单目标的覆盖并且在该覆盖时间内能解决问题的情况，如果target观测完成，则将该target的is_scheduled置为1，以后不会在访问该target任何信息
    while (is_finished == 0) {
        is_finished = 1;
            for (auto it1 = all_sate_feasible_time_interval.begin(); it1 != all_sate_feasible_time_interval.end(); ++it1) {
               if (it1->TI.time_pieces.size() == 0)//如果该卫星没有被activated 则为空
                   continue;
               //找到冲突数为0的时间段，直接分配出去
               for (auto it2 = it1->feasibleTimeIntervals_table.begin(); it2 != it1->feasibleTimeIntervals_table.end(); ) {
                   auto t_it = it2->begin();//map.begin
                   bool is_erase = true;//后续有个函数要对it2的迭代器指向的内容进行erase，故要判断it2是否被erase决定其是否要执行++it2的操作
                   while (t_it != it2->end()) {
                       is_erase = false;
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
                           all_targets_table[target_num].scheduled_target_num =(int) distance(all_sate_feasible_time_interval.begin(), it1);
                           total_reward += all_targets_table[target_num]._reward;//加奖励！！！！！！
                           all_targets_table[target_num].scheduled_time = t_p;
                           scheduled_targets.push_back(all_targets_table[target_num]);
                           is_erase = it1->eraseScheduledTimePiece(it2, it2->find(t_p.first), it2->find(t_p.second));//删除该时间片
                           is_finished = 0;
                           break;
                       }
                   }
                   if (is_erase == false)
                       ++it2;
               }
            }

            cout<<"total reward :"<<total_reward<<endl;
            //把已分配的target从冲突计数中删除
            for (auto it1 = all_sate_feasible_time_interval.begin(); it1 != all_sate_feasible_time_interval.end(); ++it1){
               for (auto it2 = it1->feasibleTimeIntervals_table.begin(); it2 != it1->feasibleTimeIntervals_table.end(); ++it2) {
                   for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                       
                       for (auto it = it3->second.target_num_table.begin(); it != it3->second.target_num_table.end(); ) {
                           auto fi = find_if(scheduled_targets.begin(), scheduled_targets.end(), findTarget_with_Target_name(all_targets_table[*it].target_name));
                           if (fi != scheduled_targets.end()) {            //找到了
            //                        cout<<"找到已经分配的target "<<all_targets_table[*it].target_name<<"并且删除"<<endl;
                               it3->second.conflict_cnt--;
                               it = it3->second.target_num_table.erase(it);
                               continue;
                           }
                           ++it;
                       }
                   }
               }
            }
    }
    
}

void SatelliteSchedulePlanning::greedyAlgo()
{
    //贪心选择：按照收益/时间的大小进行排序，在可行时间段中按该顺序选择target进行观测，直到该区间无法再加入新的目标观测为止
    for (auto it1 = all_sate_feasible_time_interval.begin(); it1 != all_sate_feasible_time_interval.end(); ++it1) {
                   if (it1->TI.time_pieces.size() == 0)//如果该卫星没有被activated 则为空
                       continue;
                   //找到冲突数为0的时间段，直接分配出去
                   for (auto it2 = it1->feasibleTimeIntervals_table.begin(); it2 != it1->feasibleTimeIntervals_table.end(); ++it2) {
                       auto t_it = it2->begin();//map.begin
//                       bool is_erase = true;//后续有个函数要对it2的迭代器指向的内容进行erase，故要判断it2是否被erase决定其是否要执行++it2的操作
                       int temp_scheduling_target = -1;
                       while (t_it != it2->end()) {
//                           is_erase = false;
                           set<int> t_tars_num;
                           time_period t_p = feasibleTimeIntervals::getBeginTimePeriod(*it2, t_tars_num);
                           if (t_p.first == -1)
                               break;
                           //得到当前时间段可安排的卫星
                           vector<pair<TargetScheduleInfo, int>>t_reward_list;
                           for (auto it = t_tars_num.begin(); it != t_tars_num.end(); ++it) {
                               t_reward_list.push_back(pair<TargetScheduleInfo, int>(all_targets_table[*it], distance(t_tars_num.begin(), it)));
                           }
                           
                           //按照奖励比时间排序
                           sort(t_reward_list.begin(), t_reward_list.end(), sortMinReward());
                           bool is_scheduled = 0;
                           for (auto it = t_reward_list.begin(); it != t_reward_list.end(); ++it) {
//                               auto &a_trg_tbl = all_targets_table;//名字太长了我草，我想换个名字
                               if (getDuration(t_p) >= it->first.remaining_time + satellite_handling_time[distance(all_sate_feasible_time_interval.begin(), it1)]) {
                                   //分配！
                                   int target_num = it->second;
                                   cout<<"target_num"<<target_num<<"is scheduled!(sate_num:"<<it1-all_sate_feasible_time_interval.begin()<<')'<<endl;
                                   all_targets_table[target_num].is_scheduled = 1;
                                   all_targets_table[target_num].scheduled_target_num =(int) distance(all_sate_feasible_time_interval.begin(), it1);
                                   total_reward += all_targets_table[target_num]._reward;//加奖励！！！！！！
                                   if (all_targets_table[target_num].scheduled_time.first == -1) {
                                       all_targets_table[target_num].scheduled_time = t_p;
                                   }
                                   else
                                       all_targets_table[target_num].scheduled_time.second = t_p.second;
                                   
                                   scheduled_targets.push_back(all_targets_table[target_num]);
//                                   is_erase = it1->eraseScheduledTimePiece(it2, it2->find(t_p.first), it2->find(t_p.second));//删除该时间片
                                   is_scheduled = 1;
                                   break;
                               }
                               if (is_scheduled == 0) {         //意味着上面没有分配到，则暂时分配最有性价比的那个
                                   cout<<"this interval cant be scheduled at once!"<<endl;
                                   if (temp_scheduling_target != -1 && temp_scheduling_target != t_reward_list.begin()->second) {   //换了个性价比更高的卫星
                                       all_targets_table[temp_scheduling_target].remaining_time =  all_targets_table[temp_scheduling_target].observe_time;//将原卫星已分配时间则恢复回去
                                       all_targets_table[temp_scheduling_target].scheduled_time = time_period(-1,-1);
                                   }
                                   temp_scheduling_target = t_reward_list.begin()->second;
                                   all_targets_table[temp_scheduling_target].remaining_time -= getDuration(t_p);//分配该时间段给该目标，故该目标剩余观察时间减少
                               }
                           }

                           if (temp_scheduling_target != 1 && all_targets_table[temp_scheduling_target].is_scheduled == 0) {
                               all_targets_table[temp_scheduling_target].remaining_time =  all_targets_table[temp_scheduling_target].observe_time;//如果依旧未分配则恢复回去
                               all_targets_table[temp_scheduling_target].scheduled_time = time_period(-1,-1);
                           }
                       }
//                       if (is_erase == false)
//                           ++it2;
                   }
                }
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
