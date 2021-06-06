//
//  SatelliteSchedulePlanning.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/5.
//

#include "SatelliteSchedulePlanning.hpp"
void SatelliteSchedulePlanning::ini_clear()
{
    every_satellite_cov_window.clear();
    every_satellite_cov_window.resize(SATELLITE_FILE_CNT);
    targetname_to_info.clear();
    all_targets_table.clear();
    scheduled_targets.clear();
}


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
    for (int i = 0; i < SATELLITE_FILE_CNT; ++i){
        readSatCovWinFile(i);
    }
}
void SatelliteSchedulePlanning::greedyAlgo()
{
    
}

