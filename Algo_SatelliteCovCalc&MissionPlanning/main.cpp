//
//  main.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//
#include <deque>
#include "SatelliteSchedulePlanning.hpp"
#include <sstream>
int main(int argc, const char * argv[]) {
    
//    cout<<SatelliteCovArea::transformedX(1, 80)<<endl;
//    cout<<1 * cos(M_PI/180 * 80);
//    //第一问部分
    SatelliteInfoManagement m;

//    string time_start_limit = "2021/1/1 11:00:00";
//    string time_stop_limit = "2021/1/1 12:30:00";
//    string time_start = "2021/1/1 0:00:00";
//////
//////
//    ifstream fin("/Users/chenziwei/C++/Algo_SatelliteCovCalc&MissionPlanning/Algo_SatelliteCovCalc&MissionPlanning/test.txt");
//    if (fin.fail()) {
//        exit(10);
//    }
//    for (int i = 0; i < 10; ++i) {
//        SatelliteSchedulePlanning p;
//        getline(fin, time_start_limit);
//        getline(fin, time_stop_limit);
//        time_period limit(m.getTime(m.getTime(time_start_limit), m.getTime(time_start)), m.getTime(m.getTime(time_stop_limit), m.getTime(time_start)));
//        p.readSatCovWinFile(i);
//        p.readTarInfoFile(i);
//        string t;   getline(fin, t);
//        stringstream ss;
//        ss<<t;
//        vector<bool> t_act_sat(9,0);
//        while (!ss.eof()) {
//            int a;
//            ss>>a;
//            t_act_sat[a] = 1;
//        }
//        p._setActivatedSat(t_act_sat);
//        string a;   char c = i+'0';     a+=c;
////        p._preprocessing(limit, a);
//        if (i==8) {
////            p.integerAlgo(limit);
//            p._preprocessing(limit);
////            p.greedyAlgo();
////            p.heuristicAlgo();
//        }
//    }
   

//    m.readSatInfoFile();
//    m.readTarInfoFile();
//    m.saveTarName(0);

//    m.coverCal(0);
//    m.coverCal(9);
//    m.calAllTargetCoverage();
    
    
    //第二问部分
    SatelliteSchedulePlanning p;
    
    p.readSatCovWinFile(0);
    p.readTarInfoFile(0);
    p.readScheduledTarInfFile(0);//读了文件就不要调用第二题的算法再算一次了！！！！！！


    
//    p._setActivatedSat(vector<bool>{1,0,0,0,0,0,1,0,0});
//    p._setActivatedSat(vector<bool>{1,0,0,0,0,0,1,0,0});
//    p._preprocessing(time_period(39600, 45000));
//    p._preprocessing();
    
//    p.greedyAlgo();//可以做预处理
    p.DPAlgo();//第三题基于第二题
    p.GeneticAlgo();
    
    
//    p.integerAlgo(time_period(39600,45000));
//    p.integerAlgo(time_period(39600,54000));
//    p.integerAlgo();
//    p.outputResult();
    return 0;
}
