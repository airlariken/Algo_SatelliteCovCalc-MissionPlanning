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
//            cout<<target_num<<'\t';
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

//    cout<<"finish!"<<endl;
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

void SatelliteSchedulePlanning::_preprocessing(time_period limit)
{
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end() ; ++it1) {
        for (auto it2 = it1->begin() ; it2 != it1->end(); ++it2) {
            for (auto it3 = it2->begin(); it3 != it2->end(); ) {
                bool it3_erase_tag = 0;
            
                if (it3->second < limit.first) {
                    it3 = it2->erase(it3);
                    it3_erase_tag = 1;
                }
                if (it3->first >= limit.second) {
                    it3 = it2->erase(it3);
                    it3_erase_tag = 1;
                }
                if (it3->first < limit.first) {
                    it3->first = limit.first;
                }
                if (it3->second > limit.second) {
                    it3->second = limit.second;
                }
                if (it3_erase_tag == 0)
                    ++it3;
            }
        }
    }
    
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
    
    
    
    
//    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end(); ++it1) {
//        if (activated_sat[it1-every_satellite_cov_window.begin()] == false)//没被激活不算进去
//            continue;
//        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
//            if (all_targets_table[it2-it1->begin()].is_scheduled == 1)      //target的is_scheduled为1，则不会在访问该target任何信息
//                continue;
//            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
//                if (getDuration(*it3) > all_targets_table[it2-it1->begin()].observe_time) {
//                    int target_num = distance(it1->begin(), it2);
//                    cout<<"target_num"<<target_num<<"is scheduled!(sate_num:"<<distance(every_satellite_cov_window.begin(), it1)<<')'<<endl;
//                    all_targets_table[target_num].is_scheduled = 1;
//                    all_targets_table[target_num].scheduled_sate_num = distance(every_satellite_cov_window.begin(), it1);
//                    total_reward += all_targets_table[target_num]._reward;//加奖励！！！！！！
//                    all_targets_table[target_num].scheduled_time = *it3;
//                    scheduled_targets.push_back(all_targets_table[target_num]);
//
//                    int handling_time = satellite_handling_time[distance(every_satellite_cov_window.begin(), it1)];
//                    //如果handling_time时间段存在feasible_time_interval则删除该interval
//                    for(auto it = all_sate_feasible_time_interval.begin(); it != all_sate_feasible_time_interval.end(); ++it) {
//                        for (int start = it3->first + handling_time; start < it3->second + handling_time; ++start)
//                            it->SecondInsideDelete(start);
//                    }
//                }
//            }
//        }
//    }
    
    
    
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
                   bool is_erase = false;//后续有个函数要对it2的迭代器指向的内容进行erase，故要判断it2是否被erase决定其是否要执行++it2的操作
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
//                       if (all_targets_table[target_num].is_scheduled == 0 && t_duration >= satellite_handling_time[it1-all_sate_feasible_time_interval.begin()] + all_targets_table[target_num].observe_time) {
                       if (all_targets_table[target_num].is_scheduled == 0 && t_duration >=  all_targets_table[target_num].observe_time) {
                           //安排分配
                           cout<<"target"<<all_targets_table[target_num].target_name<<"is scheduled!(sate_num:"<<it1-all_sate_feasible_time_interval.begin()<<')'<<endl;
                           all_targets_table[target_num].is_scheduled = 1;
                           all_targets_table[target_num].scheduled_sate_num =(int)distance(all_sate_feasible_time_interval.begin(), it1);
                           total_reward += all_targets_table[target_num]._reward;//加奖励！！！！！！
                           all_targets_table[target_num].scheduled_time = t_p;
                           scheduled_targets.push_back(all_targets_table[target_num]);
                           
                           //判断贪心选择后卫星处理时间内是否还在time_feasible_interval中，如果在一并删除该time_piece
                           auto fi_stop = it2->find(t_p.first + all_targets_table[target_num].observe_time + satellite_handling_time[it1-all_sate_feasible_time_interval.begin()]);
                           if (fi_stop != it2->end())
                               is_erase = it1->eraseScheduledTimePiece(it2, it2->find(t_p.first), fi_stop);//删除该时间片
                           else
                               is_erase = it1->eraseScheduledTimePiece(it2, it2->find(t_p.first), it2->end());//删除该时间片
            
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
                   for (auto it3 = it2->begin(); it3 != it2->end(); ) {
                       bool is_erase_it3 = 0;
                       for (auto it = it3->second.target_num_table.begin(); it != it3->second.target_num_table.end(); ) {
                           auto fi = find_if(scheduled_targets.begin(), scheduled_targets.end(), findTarget_with_Target_name(all_targets_table[*it].target_name));
                           if (fi != scheduled_targets.end()) {            //找到了
            //                        cout<<"找到已经分配的target "<<all_targets_table[*it].target_name<<"并且删除"<<endl;
                               it3->second.conflict_cnt--;
                               it = it3->second.target_num_table.erase(it);
                               if (it3->second.conflict_cnt == -1) {    //意味着应该删掉该timepiece了
                                   it3 = it2->erase(it3);
                                   is_erase_it3 = 1;
                                   break;
                               }
                               continue;
                           }
                           ++it;
                       }
                       if (is_erase_it3 == 0)
                           ++it3;
                       
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
                       cout<<"target"<<all_targets_table[target_num].target_name<<"is scheduled!(sate_num:"<<it1-all_sate_feasible_time_interval.begin()<<')'<<endl;
                       all_targets_table[target_num].is_scheduled = 1;
                       all_targets_table[target_num].scheduled_sate_num =(int) distance(all_sate_feasible_time_interval.begin(), it1);
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


//整数规划
void SatelliteSchedulePlanning::integerAlgo(time_period limit)
{
    for (auto it1 = every_satellite_cov_window.begin(); it1 != every_satellite_cov_window.end() ; ++it1) {
        for (auto it2 = it1->begin() ; it2 != it1->end(); ++it2) {
            for (auto it3 = it2->begin(); it3 != it2->end(); ) {
                bool it3_erase_tag = 0;
            
                if (it3->second < limit.first) {
                    it3 = it2->erase(it3);
                    it3_erase_tag = 1;
                }
                if (it3->first >= limit.second) {
                    it3 = it2->erase(it3);
                    it3_erase_tag = 1;
                }
                if (it3->first < limit.first) {
                    it3->first = limit.first;
                }
                if (it3->second > limit.second) {
                    it3->second = limit.second;
                }
                if (it3_erase_tag == 0)
                    ++it3;
            }
        }
    }
    using namespace operations_research;
    using namespace sat;
    using namespace operations_research;
    // Create the mip solver with the SCIP backend.
    std::unique_ptr<MPSolver> solver(MPSolver::CreateSolver("SCIP"));
    
    SatParameters parameters;

    auto status = solver->SetNumThreads(16);
    using namespace absl;
    
//    solver->SetTimeLimit(100);
    solver->set_time_limit(1*60*1000);//1mins
    
    
    if (!solver) {
    LOG(WARNING) << "SCIP solver unavailable.";
    return;
    }

//    const double infinity = solver->infinity();
//    const int infinity = INT_MAX/4;
    const int infinity = 3600*24;
    
    // x and y are integer non-negative variables.
    vector<vector<vector<MPVariable*>>> Vec_Xijk;
    for (int i = 0; i < all_targets_table.size(); ++i) {
        vector<vector<MPVariable*>> t_v1;
        for (int j = 0; j < every_satellite_cov_window.size(); ++j) {
            vector<MPVariable*> t_v2;
            if (activated_sat[j] == true) {
                for (auto it3 = every_satellite_cov_window[j][i].begin(); it3 != every_satellite_cov_window[j][i].end(); ++it3) {
                    t_v2.push_back(solver->MakeIntVar(0, 1, "Xijk"));
                }
            }
            t_v1.push_back(t_v2);
        }
        Vec_Xijk.push_back(t_v1);
    }
    
    
    vector<MPVariable*> Vec_ti;
    for (auto it = all_targets_table.begin(); it != all_targets_table.end(); ++it) {
        int tar_num = distance(all_targets_table.begin(), it);
//        int Sbeg = infinity, Send = 0;
//        for (int i = 0; i < every_satellite_cov_window.size(); ++i) {
//            for (int k = 0; k < every_satellite_cov_window[i][tar_num].size(); ++k) {
//                if (every_satellite_cov_window[i][tar_num][k].first < Sbeg) {
//                    Sbeg = every_satellite_cov_window[i][tar_num][k].first;
//                }
//                if (every_satellite_cov_window[i][tar_num][k].second > Send) {
//                    Send = every_satellite_cov_window[i][tar_num][k].second;
//                }
//            }
//        }
        Vec_ti.push_back(solver->MakeIntVar(0, infinity, "ti"));
        //优化缩小解空间
//        cout<<tar_num<<" :"<<Sbeg<<'\t'<<Send<<endl;
//        if (Sbeg == infinity)
//            Vec_ti.push_back(solver->MakeIntVar(0, 1, "ti"));
//        else
//            Vec_ti.push_back(solver->MakeIntVar(Sbeg-1, Send+1, "ti"));
    }
//
    
    vector<vector<vector<MPVariable*>>> Vec_Fii_j;
    for (int i = 0; i < all_targets_table.size(); ++i) {
        vector<vector<MPVariable*>> t_v1;
        for (int i_ = 0; i_ < all_targets_table.size(); ++i_) {
            vector<MPVariable*> t_v;
            if(i != i_) {
                for (int j = 0; j < every_satellite_cov_window.size(); ++j) {
                    if (activated_sat[j] == true) {
                        if (every_satellite_cov_window[j][i].size() != 0 && every_satellite_cov_window[j][i_].size() != 0) {
                            //两个target会被同一个卫星服务的话引入变量Fii_j = 1表示第j个卫星侦查顺序为第i个目标在第i_目标前面
                            t_v.push_back(solver->MakeIntVar(0, 1, "Fii_"));
                        }
                        else
                            t_v.push_back(nullptr);
                    }
                    else
                        t_v.push_back(nullptr);
                }
            }
            t_v1.push_back(t_v);
        }
        Vec_Fii_j.push_back(t_v1);
    }
    //attention！ Fi_ij和Fii_j可以在同一个vec中表示，只是i_和i的取值交换就行



    LOG(INFO) << "Number of variables = " << solver->NumVariables();
    
    
    //MPConstraint
    // E(xijk) <= 1
    for (auto it1 = Vec_Xijk.begin(); it1 != Vec_Xijk.end(); ++it1) {
        MPConstraint* const c0 = solver->MakeRowConstraint(-1, 1, "c0");
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
//            if (activated_sat[distance(it1->begin(), it2)] == false)
////                continue;
//                cout<<"continue";
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                c0->SetCoefficient(*it3, 1);
            }
        }
    }
//
////    Di*Xijk <= Aj
//    MPConstraint* const c1 = solver->MakeRowConstraint(-infinity, 1, "c1");
//    for (auto it1 = Vec_Xijk.begin(); it1 != Vec_Xijk.end(); ++it1) {
//        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
//            if (activated_sat[distance(it1->begin(), it2)] == false)
//                continue;
//            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
//                int tar_num = distance(Vec_Xijk.begin(), it1);
//                c1->SetCoefficient(*it3, all_targets_table[tar_num].observe_time);
//            }
//        }
//    }
    
//    Sbeg <= ti <= Send - Di
    for (auto it = Vec_ti.begin(); it != Vec_ti.end(); ++it) {
        // Sbeg <= ti <= Send - Di
        //暂定！！！！！！！！！！！！！！！！！
        int Sbeg = 3600 * 24;//暂定！！！！！！！！！！！！！！！！！//暂定！！！！！！！！！！！！！！！！！
        //暂定！！！！！！！！！！！！！！！！！
        int Send = 0;//暂定！！！！！！！！！！！！！！！！！//暂定！！！！！！！！！！！！！！！！！//暂定！！！！！！！！！！！！！！！！！
        //暂定！！！！！！！！！！！！！！！！！
        int tar_num = distance(Vec_ti.begin(), it);
        for (int i = 0; i < every_satellite_cov_window.size(); ++i) {
            for (int k = 0; k < every_satellite_cov_window[i][tar_num].size(); ++k) {
                if (every_satellite_cov_window[i][tar_num][k].first < Sbeg) {
                    Sbeg = every_satellite_cov_window[i][tar_num][k].first;
                }
                if (every_satellite_cov_window[i][tar_num][k].second > Send) {
                    Send = every_satellite_cov_window[i][tar_num][k].second;
                }
            }
        }
        MPConstraint* c2 = NULL;
        if (Send - all_targets_table[tar_num].observe_time <= Sbeg)
            c2 = solver->MakeRowConstraint(0, 0, "c2");
        else
            c2 = solver->MakeRowConstraint(Sbeg, Send - all_targets_table[tar_num].observe_time, "c2");
        c2->SetCoefficient(*it, 1);
    }
    
    //    ti-BegijK*Xijk >= 0
        for (auto it1 = Vec_Xijk.begin(); it1 != Vec_Xijk.end(); ++it1) {
            for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
                if (activated_sat[distance(it1->begin(), it2)] == false)  continue;
                for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                    MPConstraint* const c3 = solver->MakeRowConstraint(0, infinity, "c3");
                    int tar_num = distance(Vec_Xijk.begin(), it1);//对应target_num
                    int sata_num = (int)distance(it1->begin(), it2);
                    int win_num = distance(it2->begin(), it3);
                    c3->SetCoefficient(*it3, -1*every_satellite_cov_window[sata_num][tar_num][win_num].first);
                    c3->SetCoefficient(Vec_ti[tar_num], 1);
                }
            }
        }
    
    //ti-(Endijk-Di)Xijk - infinity(1-xijk)<=0
    
    for (auto it1 = Vec_Xijk.begin(); it1 != Vec_Xijk.end(); ++it1) {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            if (activated_sat[distance(it1->begin(), it2)] == false)  continue;
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                MPConstraint* const c4 = solver->MakeRowConstraint(-infinity, infinity, "c4");
                int tar_num = distance(Vec_Xijk.begin(), it1);//对应target_num
                int sata_num = (int)distance(it1->begin(), it2);
                int win_num = distance(it2->begin(), it3);
                c4->SetCoefficient(Vec_ti[tar_num], 1);
                int Di = all_targets_table[tar_num].observe_time;
                int Endijk = every_satellite_cov_window[sata_num][tar_num][win_num].second;
                c4->SetCoefficient(*it3, infinity+Di-Endijk);
            }
        }
    }
//
//    //ti-ti_ >=.....
//    //两条约束合并
    for (int i = 0; i < Vec_Fii_j.size(); ++i) {
        vector<vector<MPVariable*>> t_v1;
        for (int i_ = 0; i_ < Vec_Fii_j[i].size(); ++i_) {
            vector<MPVariable*> t_v;
            if(i != i_) {
                for (int j = 0; j < Vec_Fii_j[i][i_].size(); ++j) {
                    if (activated_sat[j] == false)  continue;
                    int low_bound = all_targets_table[i_].observe_time - infinity;
                    MPConstraint* const c5= solver->MakeRowConstraint(low_bound, infinity, "c5");
                    c5->SetCoefficient(Vec_ti[i], 1);
                    c5->SetCoefficient(Vec_ti[i_], -1);
                    int t_c = - infinity - satellite_handling_time[j];
                    c5->SetCoefficient(Vec_Fii_j[i][i_][j], t_c);

                    low_bound = all_targets_table[i].observe_time - infinity;
                    MPConstraint* const c6= solver->MakeRowConstraint(low_bound, infinity, "c6");
                    c6->SetCoefficient(Vec_ti[i_], 1);
                    c6->SetCoefficient(Vec_ti[i], -1);
                    t_c = - infinity - satellite_handling_time[j];
                    c6->SetCoefficient(Vec_Fii_j[i_][i][j], t_c);
                }
            }
        }
    }
    
    

    for (int i = 0; i < Vec_Fii_j.size(); ++i) {
        for (int i_ = 0; i_ < Vec_Fii_j[i].size(); ++i_) {
            if(i != i_) {
                for (int j = 0; j < Vec_Fii_j[i][i_].size(); ++j) {
                    MPConstraint* const c7= solver->MakeRowConstraint(-infinity, 0, "c7");
                    c7->SetCoefficient(Vec_Fii_j[i][i_][j], 1);
                    c7->SetCoefficient(Vec_Fii_j[i_][i][j], 1);
//                    for (auto it1 = Vec_Xijk[i].begin(); it1 != Vec_Xijk[i].end(); ++it1) {
                        for (auto it2 = Vec_Xijk[i][j].begin(); it2 != Vec_Xijk[i][j].end(); ++it2) {
                            c7->SetCoefficient(*it2, -1);
                        }
//                    }
                    MPConstraint* const c8= solver->MakeRowConstraint(-infinity, 0, "c8");
                    c8->SetCoefficient(Vec_Fii_j[i][i_][j], 1);
                    c8->SetCoefficient(Vec_Fii_j[i_][i][j], 1);
//                    for (auto it1 = Vec_Xijk[i_].begin(); it1 != Vec_Xijk[i_].end(); ++it1) {
                        for (auto it2 = Vec_Xijk[i_][j].begin(); it2 != Vec_Xijk[i_][j].end(); ++it2) {
                            c8->SetCoefficient(*it2, -1);
                        }
//                    }

                    MPConstraint* const c9= solver->MakeRowConstraint(-1, infinity, "c9");
                    c9->SetCoefficient(Vec_Fii_j[i][i_][j], 1);
                    c9->SetCoefficient(Vec_Fii_j[i_][i][j], 1);

                    for (auto it2 = Vec_Xijk[i][j].begin(); it2 != Vec_Xijk[i][j].end(); ++it2) {
                        c9->SetCoefficient(*it2, -1);
                    }
                    for (auto it2 = Vec_Xijk[i_][j].begin(); it2 != Vec_Xijk[i_][j].end(); ++it2) {
                        c9->SetCoefficient(*it2, -1);
                    }


                    MPConstraint* const c10= solver->MakeRowConstraint(-infinity, 1, "c10");
                    for (int x = 0; x < every_satellite_cov_window.size(); ++x) {
                        if (activated_sat[x] == true) {
                            if(Vec_Fii_j[i][i_][x] != nullptr)
                                c10->SetCoefficient(Vec_Fii_j[i][i_][x], 1);
//                            else
//                                cout<<"kong指针"<<endl;
                            if(Vec_Fii_j[i_][i][x] != nullptr)
                                c10->SetCoefficient(Vec_Fii_j[i_][i][x], 1);
//                            else
//                                cout<<"kong指针"<<endl;
                        }
                    }
                }
            }
        }
    }


    LOG(INFO) << "Number of constraints = " << solver->NumConstraints();

    // Maximize
    MPObjective* const objective = solver->MutableObjective();
    for (auto it1 = Vec_Xijk.begin(); it1 != Vec_Xijk.end(); ++it1) {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                int tar_num = distance(Vec_Xijk.begin(), it1);
                objective->SetCoefficient(*it3, all_targets_table[tar_num]._reward);
            }
        }
    }
    
//    objective->SetCoefficient(y, 1);
    objective->SetMaximization();
//
    const MPSolver::ResultStatus result_status = solver->Solve();
    // Check that the problem has an optimal solution.
    if (result_status != MPSolver::OPTIMAL) {
//        LOG(FATAL) << "The problem does not have an optimal solution!";
        cout<<"The problem does not have an optimal solution!"<<endl;
    }
//
    LOG(INFO) << "Solution:";
    LOG(INFO) << "Objective value = " << objective->Value();

    
    for (auto it1 = Vec_Xijk.begin(); it1 != Vec_Xijk.end(); ++it1) {
        for (auto it2 = it1->begin(); it2 != it1->end(); ++it2) {
            for (auto it3 = it2->begin(); it3 != it2->end(); ++it3) {
                if ((*it3)->solution_value() == 1) {
                    int tar_num = distance(Vec_Xijk.begin(), it1);//对应target_num
                    int sata_num = (int)distance(it1->begin(), it2);
                    int win_num = distance(it2->begin(), it3);
                    cout<<"tar_num:"<<tar_num<<'\t'<<"sata_num"<<sata_num<<endl;
                }
//                LOG(INFO) << "variables= " << (*it3)->solution_value()<<'\t';
            }
            
        }
        
    }
    
    for (auto it = Vec_ti.begin(); it != Vec_ti.end(); ++it) {
        cout<<"t"<<distance(Vec_ti.begin(), it)<<":"<<(*it)->solution_value()<<endl;
    }
//    LOG(INFO) << "x = " << x->solution_value();
//    LOG(INFO) << "y = " << y->solution_value();
//
    LOG(INFO) << "\nAdvanced usage:";
    LOG(INFO) << "Problem solved in " << solver->wall_time() << " milliseconds";
    LOG(INFO) << "Problem solved in " << solver->iterations() << " iterations";
    LOG(INFO) << "Problem solved in " << solver->nodes()
            << " branch-and-bound nodes";
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
