//
//  SatelliteInfoManagement.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#include "SatelliteInfoManagement.hpp"
//void SatelliteInfoManagement::saveSatData()
//{
//    ofstream fout("tmp", ios::binary|ios::trunc);
//    if (fout.fail()) {
//        cerr<<"fail to open tmp.bin file!"<<endl;
//        exit(3);
//    }
//    fout.write(reinterpret_cast<char *>(this), sizeof(*this));
//    fout.close();
//    
//    ofstream fout1("size.txt", ios::binary|ios::trunc);
//    if (fout1.fail()) {
//        cerr<<"fail to open tmp.bin file!"<<endl;
//        exit(5);
//    }
//    fout1<<sizeof(*this);
//    fout1.close();
////    ofstream fout("tmp", ios::binary|ios::trunc);
////    if (fout.fail()) {
////        cerr<<"fail to open tmp.bin file!"<<endl;
////        exit(3);
////    }
////    fout.write(reinterpret_cast<char *>(satellite_timetable.data()), satellite_timetable.size()*sizeof(satellite_timetable.front()));
////    fout.close();
////
////    ifstream f_bin_in("tmp", ios::binary);
////    if (f_bin_in.fail()) {
////        cerr<<"fail to open tmp.bin file!"<<endl;
////        exit(4);
////    }
////    vector<SatelliteCovArea*> temp;
////    temp.resize(satellite_timetable.size());
////    f_bin_in.read(reinterpret_cast<char *>(&temp[0]), satellite_timetable.size()*sizeof(satellite_timetable.front()));
////
////    f_bin_in.close();
//    
//    return;
//}


//void SatelliteInfoManagement::readSatData()
//{
//    ifstream fin("size.txt");
//    int t_size = 0;
//    fin>>t_size;
//    ifstream f_bin_in("tmp", ios::binary);
//    if (f_bin_in.fail()) {
//        cerr<<"fail to open tmp.bin file!"<<endl;
//        exit(4);
//    }
//    f_bin_in.read(reinterpret_cast<char *>(this), t_size);
//    
//    f_bin_in.close();
//    return;
//}
void SatelliteInfoManagement::readSatInfoFile()
{
    for (int i = 0; i < 9; ++i){
        string path = "/Users/chenziwei/Desktop/算法课设/Data/SatelliteInfo/SatCoverInfo_.txt";
        const char a = i+'0';   string t;  t+=a;
        path.insert(70, t.c_str());
        
        ifstream fin(path);
        if (fin.fail()) {
            cerr<<"fail to open SatelliteInfofile!"<<endl;
            exit(1);
        }
        string s;
        vector<SatelliteCovArea> temp;
        temp.reserve(87000);
        int first_read_tag = 1;
        while (!fin.eof()) {
            getline(fin, s);//时间行
            if (s.empty()) {
                continue;
            }
            getTime(s);
            if (first_read_tag == 1) {//初始化第一个卫星的起始时间
                first_read_tag = 0;
                satellite_1_starttime = getTime(s);
            }
            temp.push_back(SatelliteCovArea());
            for (int i = 0 ; i < 21; ++i) {
                float pos_x, pos_y;
                fin>>pos_x>>pos_y;
                (*(temp.end()-1)).polygon_pos.push_back(EarthPos(pos_x, pos_y));
                (*(temp.end()-1)).ini();
            }
            getline(fin, s);
        }
        
    //    cout<<"test size"<<satellite_timetable.size()<<endl;
        fin.close();
        all_satellite_timetable.push_back(temp);
    }
    return;
}

void SatelliteInfoManagement::readTarInfoFile()
{
    ifstream fin("/Users/chenziwei/Desktop/算法课设/Data/TargetInfo/target1.txt");
    if (fin.fail()) {
        cerr<<"fail to open TargetInfofile!"<<endl;
        exit(1);
    }
    while (!fin.eof()) {
        string t_name;
        float x, y;
        int obs_t, reward;
        fin>>t_name>>x>>y>>obs_t>>reward;
        target_table.push_back(new TargetInfo(t_name, EarthPos(x, y), obs_t, reward));
    }
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

void SatelliteInfoManagement::coverCal()
{
    vector<pair<EarthTime, int>> target1_window;
    TargetInfo target1 = *target_table[0];
    for (int i = 0; i < all_satellite_timetable.size(); ++i) {
        for (int j = 0; j < all_satellite_timetable[i].size(); ++j) {
            if (all_satellite_timetable[i][j].isInside(target1._pos)) {
                pair<EarthTime, int> pair_tmp(getTime(j, satellite_1_starttime), j);
                target1_window.push_back(pair_tmp);
            }
        }
    }
    
    //求并集
    sort(target1_window.begin(), target1_window.end(), tar_window_sort());
    auto ite = unique(target1_window.begin(), target1_window.end(), tar_window_unique());
    target1_window.erase(ite, target1_window.end());

    for(auto &i : target1_window)
        cout<<i.first._hours<<":"<<i.first._minutes<<":"<<i.first._seconds<<endl;
}

//SatelliteCovArea

void SatelliteCovArea::_findMaxMin(float &max_x, float &min_x, float &max_y, float &min_y) const
{
    max_x = min_x = polygon_pos[0]._x;
    max_y = min_y = polygon_pos[0]._y;
    for (int i = 1; i < polygon_pos.size(); ++i) {
        if(polygon_pos[i]._x > max_x)
            max_x = polygon_pos[i]._x;
        else if(polygon_pos[i]._x < min_x)
            min_x = polygon_pos[i]._x;
        if(polygon_pos[i]._y > max_y)
            max_y = polygon_pos[i]._y;
        else if(polygon_pos[i]._y < min_y)
            min_y = polygon_pos[i]._y;
    }
    return;
}
void SatelliteCovArea::_getCircle()
{
    
}
void SatelliteCovArea::_getBoundary()
{
    float xmax,xmin,ymax,ymin;
    _findMaxMin(xmax, xmin, ymax, ymin);
    //求出矩形
    rec_boundary = Rectangle(xmax,xmin, ymax, ymin);
    
    //求出圆形
    circle_center._x = (xmax + xmin) / 2;
    circle_center._y = (ymax + ymin) / 2;
    circle_radius =  xmax - circle_center._x;
}

//EarthTime
ostream &EarthTime::operator<<(ostream &out)
{
    out<<this->_date<<this->_day<<this->_hours<<this->_minutes<<this->_seconds<<endl;
    return out;
}

void SatelliteCovArea::ini()
{
    if (polygon_pos.size() == 0) {
        cerr<<"didnt ini polygon_pos vector"<<endl;
        exit(2);
    }
    _getBoundary();
//    getRadiusAndCenterPoint();}
}

bool SatelliteCovArea::isInside(const EarthPos &p) const
{
    float t = pow(p._x - this->circle_center._x, 2) + pow(p._y - this->circle_center._y, 2);
    if (t < pow(this->circle_radius, 2)) {
        return true;
    }
    return false;
}
