//
//  SatelliteInfoManagement.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#include "SatelliteInfoManagement.hpp"

//全局函数
ostream& operator<<(ostream& co,const EarthTime& t) {
    co<<t._day<<' '<<t._hours<<":"<<t._minutes<<":"<<t._seconds;
    return co;
}

//SatelliteInfoManagement类

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
    for (int i = 0; i < SATELLITE_FILE_CNT; ++i){
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
    for (int i = 0; i < TARGET_FILE_CNT; ++i) {
        string path = "/Users/chenziwei/Desktop/算法课设/Data/TargetInfo/target.txt";
        const char a = i + '1';   string t;  t+=a;
               path.insert(60, t.c_str());
        ifstream fin(path);
        if (fin.fail()) {
            cerr<<"fail to open TargetInfofile!"<<endl;
            exit(1);
        }
        vector<TargetInfo> tmp_vec;
        while (!fin.eof()) {
            string t_name;
            float x, y;
            int obs_t, reward;
            fin>>t_name>>x>>y>>obs_t>>reward;
            tmp_vec.push_back(TargetInfo(t_name, EarthPos(x, y), obs_t, reward));
        }
        all_target_table.push_back(tmp_vec);
        fin.close();
    }

    return;
}

int SatelliteInfoManagement::getTime(EarthTime t, EarthTime start)
{
    int sum = 0;
    sum += (t._day - start._day) * 3600*24;
    sum += (t._hours - start._hours) * 3600;
    sum += (t._minutes - start._minutes) * 60;
    sum += t._seconds - start._seconds;
    return sum;
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

    int flag = 0, cnt = 0;//切分时间，将2021/1/1 x:xx:xx切分为 2022/1/ 和1 x:xx:xx:xx
    while (flag != 2) {
        if(s[cnt] == '/')
            flag++;
        cnt++;
    }
    string temp_time = s.substr(0, cnt);
    s = s.substr(cnt);

    
    int t_d, t_h, t_m, t_s;
    auto fi = s.find(" ");
    string temp = s.substr(0,fi);
    t_d = atoi(s.c_str());
    s = s.substr(fi+1);
    //剩余字符串为x xx:xx::xx
    cnt = 0;
    while (s[cnt] != ':') {         //找到第一个: 因为小时位可能是1位也可能是2位
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

void SatelliteInfoManagement::coverCal(const int &file_num)
{
    //  @target_i_window 9个卫星对一个目标的观测结果，a[i][j] 为第i个卫星对第file_num文件的第j个目标的观测窗口，其中第一个参数为转换时间，第二个是从starttime开始经过的秒数
    string path = "mid_res";
    char a= file_num + '0';
    path.push_back(a);  path.insert(path.length(), ".txt");
    ofstream fout(path, ios::trunc);
    
    string path2 = "singal_cov_res";
    char a2= file_num + '0';
    path2.push_back(a2);  path2.insert(path2.length(), ".txt");
    ofstream fout2(path2, ios::trunc);
    
    for (int k = 0; k < all_target_table[file_num].size(); ++k) {
        vector<vector<pair<EarthTime, int>>> target_i_window;//存单个文星
        vector<pair<EarthTime, int>> target_i_window_of_all_sat;//所有卫星一起存
        target_i_window_of_all_sat.reserve(10000);
        TargetInfo target1 = all_target_table[file_num][k];
        for (int i = 0; i < all_satellite_timetable.size(); ++i) {
            vector<pair<EarthTime, int>> temp_vec;
            for (int j = 0; j < all_satellite_timetable[i].size(); ++j) {
                if (all_satellite_timetable[i][j].isInside_polygon(target1._pos)) {
                    pair<EarthTime, int> pair_tmp(getTime(j, satellite_1_starttime), j);
                    temp_vec.push_back(pair_tmp);
                    target_i_window_of_all_sat.push_back(pair_tmp);
                }
            }
            target_i_window.push_back(temp_vec);
        }

        _signalCov(target_i_window_of_all_sat, fout2, k);//计算所有卫星并集覆盖（单覆盖）
        _saveEverySateObsWindow(target_i_window, fout, k);//分开计算每个卫星对目标的覆盖
    }

        
}

void SatelliteInfoManagement::_saveEverySateObsWindow(vector<vector<pair<EarthTime, int>>>& target_i_window,ofstream &fout, const int &target_num)
{
//  @every_satellite_window_period 是9个卫星对一个目标的观测时间段窗口，a[i][j] 为第i个卫星对第file_num的target文件的第k个目标的观测窗口，其中第一个参数为起始时间，第二个是结束时间
vector<vector<time_period>> every_satellite_window_period;
EarthTime t_start_time;
    for (int i = 0; i < target_i_window.size(); ++i) {
        bool start_time_tag = 0, end_time_tag = 1;
        vector<time_period> temp_vec;
        if (target_i_window[i].size() == 0) {//可能存在卫星观测窗口是空的情况
            every_satellite_window_period.push_back(vector<time_period>());
            continue;
        }
        
        for(int j = 0; j < target_i_window[i].size()-1; ++j) {
            //连续时间记录起始和结束
            if(target_i_window[i][j].second + 1 == target_i_window[i][j+1].second) {
                if (start_time_tag == 0 && end_time_tag == 1) {
                    t_start_time = EarthTime(target_i_window[i][j].first);
                    start_time_tag = 1;
                    end_time_tag = 0;
                }
            }
            else {
                //断开了，此处应该是结束点
                if (start_time_tag == 1 && end_time_tag == 0) {
                    start_time_tag = 0;
                    end_time_tag = 1;
                    temp_vec.push_back(time_period(t_start_time, target_i_window[i][j].first));
                }
            }
        }
        //把结尾处理了
        auto t_size = target_i_window[i].size();
        temp_vec.push_back(time_period(t_start_time, target_i_window[i][t_size-1].first));
        every_satellite_window_period.push_back(temp_vec);
    }

    if (fout.fail()) {
        cerr<<"fali to open mid_res"<<endl;
        exit(6);
    }
    fout<<"target_num:"<<target_num<<endl;
    for(int i = 0; i < every_satellite_window_period.size();++i){
        fout<<"satellite:"<<i<<endl;
        for (int j = 0; j < every_satellite_window_period[i].size(); ++j) {
            if (every_satellite_window_period[i][j].first == EarthTime())
                break;
            fout<<every_satellite_window_period[i][j].first<<'\t'<<every_satellite_window_period[i][j].second<<endl;
        }
    }
}
void SatelliteInfoManagement::calAllTargetCoverage()
{
    for (int i = 0; i < TARGET_FILE_CNT; ++i) {
        coverCal(i);
    }
}

void SatelliteInfoManagement::_signalCov(vector<pair<EarthTime, int>> target_i_window_of_all_sat, ofstream &fout, const int &target_num)
{
    //求并集
            sort(target_i_window_of_all_sat.begin(), target_i_window_of_all_sat.end(), tar_window_sort());
            auto ite = unique(target_i_window_of_all_sat.begin(), target_i_window_of_all_sat.end(), tar_window_unique());
            target_i_window_of_all_sat.erase(ite, target_i_window_of_all_sat.end());
            
            
            vector<time_period> all_satellite_window_period;//所有卫星并集的时间段求
            EarthTime t_start_time;
            bool start_time_tag = 0, end_time_tag = 1;
                    
            for(int j = 0; j < target_i_window_of_all_sat.size()-1; ++j) {
                //连续时间记录起始和结束
                if(target_i_window_of_all_sat[j].second + 1 == target_i_window_of_all_sat[j+1].second) {
                    if (start_time_tag == 0 && end_time_tag == 1) {
                        t_start_time = EarthTime(target_i_window_of_all_sat[j].first);
                        start_time_tag = 1;
                        end_time_tag = 0;
                    }
                }
                else {
                    //断开了，此处应该是结束点
                    if (start_time_tag == 1 && end_time_tag == 0) {
                        start_time_tag = 0;
                        end_time_tag = 1;
                        all_satellite_window_period.push_back(time_period(t_start_time, target_i_window_of_all_sat[j].first));
                    }
                }
            }
            //把结尾处理了
            auto t_size = target_i_window_of_all_sat.size();
            all_satellite_window_period.push_back(time_period(t_start_time, target_i_window_of_all_sat[t_size-1].first));

            if (fout.fail()) {
                cerr<<"fali to open mid_res"<<endl;
                exit(6);
            }
            fout<<"target_num:"<<target_num<<endl;
                
            for (int j = 0; j < all_satellite_window_period.size(); ++j) {
    //            if (all_satellite_window_period[j].first == EarthTime())
    //                break;
                fout<<all_satellite_window_period[j].first<<'\t'<<all_satellite_window_period[j].second<<endl;
            }
    
}
void SatelliteInfoManagement::_doubleCov(const int &file_num)
{
    
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
    //??
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


void SatelliteCovArea::ini()
{
    if (polygon_pos.size() == 0) {
        cerr<<"didnt ini polygon_pos vector"<<endl;
        exit(2);
    }
    _getBoundary();
}

bool SatelliteCovArea::isInside_polygon(const EarthPos &p) const
{
    int x;
    int result;//-1和+1表示方向
    bool first_tag = 1;
    if (polygon_pos.size() == 0) {
        exit(5);
    }
    for (int i = 0; i < polygon_pos.size(); ++i) {
        two_vec t_two_vec;
        if (i == polygon_pos.size()-1) {
            t_two_vec = two_vec(polygon_pos[i], p, polygon_pos[0], p);
        }
        t_two_vec = two_vec(polygon_pos[i], p, polygon_pos[i+1], p);
        if (first_tag == 1) {   //第一次给result初始化
            result = t_two_vec.result();
            first_tag = 0;
        }
        else{           //叉乘方向不同说明点在外
            x = t_two_vec.result();
            if (result * x < 0) {
                return false;
            }
        }
    }
    return true;
}

bool SatelliteCovArea::isInside_circle(const EarthPos &p) const
{
    float t = pow(p._x - this->circle_center._x, 2) + pow(p._y - this->circle_center._y, 2);
    if (t < pow(this->circle_radius, 2)) {
        return true;
    }
    return false;
}
