//
//  SatelliteInfoManagement.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#include "SatelliteInfoManagement.hpp"
void SatelliteInfoManagement::readSatInfoFile()
{
    ifstream fin("/Users/chenziwei/Desktop/算法课设/Data/SatelliteInfo/SatCoverInfo_0.txt");
    if (fin.fail()) {
        cerr<<"fail to open SatelliteInfofile!"<<endl;
        exit(1);
    }
    string s;

//    cout<<s.substr(0,8);
//    int time_second = 0, time_minute = 0, time_hour = 0;
    while (!fin.eof()) {
        getline(fin, s);//时间行
        if (s.empty()) {
            continue;
        }
        getTime(s);
//        while (s.substr(0,8) != "2022/1/1") {
        satellite_timetable.push_back(new SatelliteCovArea);
        for (int i = 0 ; i < 21; ++i) {
//            getline(fin, s);
            float pos_x, pos_y;
            fin>>pos_x>>pos_y;
            (*(satellite_timetable.end()-1))->polygo_pos.push_back(EarthPos(pos_x, pos_y));
        }
        getline(fin, s);
    }
    
    cout<<"test size"<<endl;
    fin.close();
    return;
}

EarthTime SatelliteInfoManagement::getTime(const int& cnt, const EarthTime& start_time) const
{
    int t_d = start_time._day, t_h = start_time._hours, t_m = start_time._minutes, t_s = start_time._seconds;
    t_s += cnt;
    t_h += t_s / 3600;
    if (t_h >= 24) {
        t_d += t_h/24;  //计算日
        t_h %= 24;      //计算小时
    }
    t_s %= 3600;
    t_m += t_s/60;  //计算分钟
    t_s %= 60;      //计算秒
    EarthTime t(t_d, t_h, t_m, t_s, start_time._date);
    
    return t;
}
ostream &EarthTime::operator<<(ostream &out)
{
    out<<this->_date<<this->_day<<this->_hours<<this->_minutes<<this->_seconds<<endl;
    return out;
}

EarthTime SatelliteInfoManagement::getTime(string s) const //传入该卫星的时间字符串返回一个Earthtime数据结构
{
//    auto fi = s.find("/");              //好像就是个int类型返回值
//    string t = s.substr(0, fi);
//    s = s.substr(fi);
//
//    fi = s.find("/");
//    string t = s.substr(0, fi);
//    s = s.substr(fi);
    int flag = 0, cnt = 0;//切分时间，将2021/1/1 x:xx:xx切分为 2022/1/ 和1 x:xx:xx:xx
    while (flag != 2) {
        if(s[cnt] == '/')
            flag++;
        cnt++;
    }
    string temp_time = s.substr(0, cnt);
    s = s.substr(cnt);
//    cout<<temp_time<<endl;
//    cout<<s<<endl;
    
    int t_d, t_h, t_m, t_s;
    auto fi = s.find(" ");
    string temp = s.substr(0,fi);
    t_d = atoi(s.c_str());
    s = s.substr(fi+1);
    //剩余字符串为x xx:xx::xx
    cnt = 0;
    while (s[cnt] != ':') {         //找到第一个: 因为小时可能是1位也可能是2位
        cnt++;
    }
    temp = s.substr(0, cnt);
    //剩余字符串为 xx:xx:xx
    t_h = atoi(temp.c_str());

    s = s.substr(cnt+1);
    //剩余字符串为 xx:xx
    t_m = atoi(s.substr(0,2).c_str());
    s = s.substr(3);
    //剩余字符串为 xx
    t_s = atoi(s.substr().c_str());
    return EarthTime(t_d, t_h, t_m, t_s, temp_time);
}
