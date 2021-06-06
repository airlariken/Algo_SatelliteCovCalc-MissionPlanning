//
//  SatelliteInfoManagement.cpp
//  Algo_SatelliteCovCalc&MissionPlanning
//
//  Created by 陈梓玮 on 2021/6/2.
//

#include "SatelliteInfoManagement.hpp"

//全局函数
ostream& operator<<(ostream& co,const EarthTime& t) {
    co<<t._hours<<":"<<t._minutes<<":"<<t._seconds;
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
            for (int i = 0 ; i < 20; ++i) {//最后一个与第一个重复了草
                double pos_x, pos_y;
                fin>>pos_x>>pos_y;
//                cout<<pos_x<<'\t'<<pos_y<<endl;
                (*(temp.end()-1)).polygon_pos.push_back(EarthPos(pos_x, pos_y));
//                (*(temp.end()-1)).ini();
            }
            getline(fin, s);
            getline(fin, s);
        }
        
    //    cout<<"test size"<<satellite_timetable.size()<<endl;
        fin.close();
        all_satellite_timetable.push_back(temp);
        satellite_1_duration_time = (int)temp.size();
//        cout<<satellite_1_duration_time<<endl;
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
            double x, y;
            int obs_t, reward;
            fin>>t_name>>x>>y>>obs_t>>reward;
            if (x < 0) {            //统一到卫星经度表示
                x += 360;
            }
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

void SatelliteInfoManagement::coverCal(const int &file_num)//传入文件的index,index见所给文件名
{
    //  @variable   target_i_window 9个卫星对一个目标的观测结果，a[i][j] 为第i个卫星对第file_num文件的第j个目标的观测窗口，其中第一个参数为转换时间，第二个是从starttime开始经过的秒数
    //  @variable   target_i_window_of_all_sat 所有卫星时间窗口一起存，去重后就是并集 所有点在多边形内的时刻都会被push进去，a[i]没有实际意义
    //  @variable   _target_i_double_cov_windows_set  用于检测二重覆盖窗口的集合
    //  @variable   target_i_double_cov_windows    存储二重覆盖窗口
    string path = "mid_res";
    char a= file_num + '0';
    path.push_back(a);  path.insert(path.length(), ".txt");
    ofstream fout(path, ios::trunc);
    
    string path2 = "combine_cov_res";
    char a2= file_num + '0';
    path2.push_back(a2);  path2.insert(path2.length(), ".txt");
    ofstream fout2(path2, ios::trunc);
    
    string path3 = "double_cov_res";
    char a3 = file_num + '0';
    path3.push_back(a3);  path3.insert(path3.length(), ".txt");
    ofstream fout3(path3, ios::trunc);
    
    string path4 = "uncov_res";
    char a4= file_num + '0';
    path4.push_back(a4);  path4.insert(path4.length(), ".txt");
    ofstream fout4(path4, ios::trunc);
    fout4.close();

    
    for (int k = 0; k < all_target_table[file_num].size(); ++k) {
        vector<vector<pair<EarthTime, int>>> target_k_window;//存单个卫星
        vector<pair<EarthTime, int>> target_k_window_of_all_sat;//所有卫星一起存
        set<int> __AUX_target_k_double_cov_windows;//用于检测二重覆盖窗口的集合
        set<int> target_k_double_cov_windows;//存储二重覆盖窗口
        
        
        target_k_window_of_all_sat.reserve(10000);
        TargetInfo target1 = all_target_table[file_num][k];
        for (int i = 0; i < all_satellite_timetable.size(); ++i) {
            vector<pair<EarthTime, int>> temp_vec;
            for (int j = 0; j < all_satellite_timetable[i].size(); ++j) {
                if (all_satellite_timetable[i][j].isInside_polygon(target1._pos)) {
                    pair<EarthTime, int> pair_tmp(getTime(j, satellite_1_starttime), j);
                    temp_vec.push_back(pair_tmp);
                    target_k_window_of_all_sat.push_back(pair_tmp);
                    if (__AUX_target_k_double_cov_windows.count(j) == 0) {//如果set中没有j则insert
                        __AUX_target_k_double_cov_windows.insert(j);
                    }
                    else{                                                       //set中至少有一个说明是至少有两个卫星覆盖，Push进double_cov中
                        target_k_double_cov_windows.insert(j);
                    }
                }
            }
            target_k_window.push_back(temp_vec);
        }

        _combineCov(target_k_window_of_all_sat, fout2, k, file_num);//计算所有卫星并集覆盖（单覆盖）
        _saveEverySateObsWindow(target_k_window, fout, k);//分开计算每个卫星对目标的覆盖
        _doubleCov(target_k_double_cov_windows, fout3, k);//计算二重+覆盖窗口
    }

        
}

void SatelliteInfoManagement::_saveEverySateObsWindow(vector<vector<pair<EarthTime, int>>>& target_i_window,ofstream &fout, const int &target_num)
{
    //  @every_satellite_window_period 是9个卫星对一个目标的观测时间段窗口，a[i][j] 为第i个卫星对第file_num的target文件的第k个目标的观测窗口，其中第一个参数为起始时间，第二个是结束时间
    vector<vector<time_period>> every_satellite_window_period;
    int t_start_time;
    if (target_i_window.size() == 0){
        cerr<<"target_i_window is empty (target_num:"<<target_num<<')'<<endl;
        return;
    }
    for (int i = 0; i < target_i_window.size(); ++i) {
        bool start_time_tag = 0, end_time_tag = 1;
        vector<time_period> temp_vec;
        if (target_i_window[i].size() == 0) {//可能存在卫星观测窗口是空的情况
            every_satellite_window_period.push_back(vector<time_period>());
            continue;
        }
        
        for(int j = 0; j < target_i_window[i].size()-1; ++j) {
            //连续时间记录起始和结束
            if(target_i_window[i][j+1].second - target_i_window[i][j].second <= 3) {
                if (start_time_tag == 0 && end_time_tag == 1) {
                    t_start_time = target_i_window[i][j].second;
                    start_time_tag = 1;
                    end_time_tag = 0;
                }
            }
            else {
                //断开了，此处应该是结束点
                if (start_time_tag == 1 && end_time_tag == 0) {
                    start_time_tag = 0;
                    end_time_tag = 1;
                    temp_vec.push_back(time_period(t_start_time, target_i_window[i][j].second));
                }
            }
        }
        //把结尾处理了
        auto t_size = target_i_window[i].size();
        temp_vec.push_back(time_period(t_start_time, target_i_window[i][t_size-1].second));
        every_satellite_window_period.push_back(temp_vec);
    }

    if (fout.fail()) {
        cerr<<"fali to open mid_res"<<endl;
        exit(6);
    }
    fout<<"target_num:"<<target_num<<endl;
    for(int i = 0; i < every_satellite_window_period.size();++i){
        fout<<"satellite:"<<i<<"size"<<every_satellite_window_period[i].size()<<endl;
        for (int j = 0; j < every_satellite_window_period[i].size(); ++j) {
            if (every_satellite_window_period[i][j].first == 0)
                break;
        fout<<every_satellite_window_period[i][j].first<<'\t'<<every_satellite_window_period[i][j].second<<endl;
//            fout<<getTime(every_satellite_window_period[i][j].first, satellite_1_starttime)<<"      "<<getTime(every_satellite_window_period[i][j].second, satellite_1_starttime)<<endl;
        }
    }
}
void SatelliteInfoManagement::calAllTargetCoverage()
{
    for (int i = 0; i < TARGET_FILE_CNT; ++i) {
        coverCal(i);
    }
}

void SatelliteInfoManagement::_combineCov(vector<pair<EarthTime, int>> target_i_window_of_all_sat, ofstream &fout, const int &target_num, const int &file_num)
{
    
    if (target_i_window_of_all_sat.size() == 0){
        cerr<<"target_i_window_of_all_sat is empty (target_num:"<<target_num<<')'<<endl;
        return;
    }
    
    //求并集
    sort(target_i_window_of_all_sat.begin(), target_i_window_of_all_sat.end(), tar_window_sort());
    auto ite = unique(target_i_window_of_all_sat.begin(), target_i_window_of_all_sat.end(), tar_window_unique());
    target_i_window_of_all_sat.erase(ite, target_i_window_of_all_sat.end());
    
    
    vector<time_period> all_satellite_window_period;//所有卫星并集的时间段求
    int t_start_time;
    bool start_time_tag = 0, end_time_tag = 1;
    for(int j = 0; j < target_i_window_of_all_sat.size()-1; ++j) {
        //连续时间记录起始和结束
        if(target_i_window_of_all_sat[j].second + 1 == target_i_window_of_all_sat[j+1].second) {
            if (start_time_tag == 0 && end_time_tag == 1) {
                t_start_time = target_i_window_of_all_sat[j].second;
                start_time_tag = 1;
                end_time_tag = 0;
            }
        }
        else {
            //断开了，此处应该是结束点
            if (start_time_tag == 1 && end_time_tag == 0) {
                start_time_tag = 0;
                end_time_tag = 1;
                all_satellite_window_period.push_back(time_period(t_start_time, target_i_window_of_all_sat[j].second));
            }
        }
    }
    //把结尾处理了
    auto t_size = target_i_window_of_all_sat.size();
    all_satellite_window_period.push_back(time_period(t_start_time, target_i_window_of_all_sat[t_size-1].second));

    if (fout.fail()) {
        cerr<<"fali to open mid_res"<<endl;
        exit(6);
    }
    fout<<"target_num:"<<target_num<<endl;
        
    for (int j = 0; j < all_satellite_window_period.size(); ++j) {
        fout<<getTime(all_satellite_window_period[j].first, satellite_1_starttime)<<"      "<<getTime(all_satellite_window_period[j].second, satellite_1_starttime)<<endl;
//        fout<<all_satellite_window_period[j].first<<'\t'<<all_satellite_window_period[j].second<<endl;
    }
//    mutex_uncover_cal = 1;//可以计算时间间隔，打开mutex
    //直接继续计算时间间隔
    _calUncoverSegement(all_satellite_window_period, file_num, target_num);
}

void SatelliteInfoManagement::_calUncoverSegement(const vector<time_period> &all_satellite_window_period, const int &file_num, const int &target_num)
{
    //由于现在改成在_combineCov函数中直接调用此函数，故无需在设置mutex
//    if (mutex_uncover_cal == 0) {
//        cerr<<"didnt cal fun(combineCov), unless you cal combineCov fun to cal window you will able to call this function(_calUncoverSegement)"<<endl;
//        exit(8);
////        return;
//    }
//    string path = "combine_cov_res";
//    char a = file_num + '0';
//    path.push_back(a);  path.insert(path.length(), ".txt");
//    ifstream fin(path);
//    if (fin.fail()) {
//        cerr<<"cant open combin_cov_res file"<<endl;
//        exit(8);
//    }
    
    vector<time_period> uncover_window_period;
    int start_time = 0;
    bool start_time_tag = 0;
    for (auto it = all_satellite_window_period.begin(); it != all_satellite_window_period.end(); ++it) {
        uncover_window_period.push_back(time_period(start_time, it->first));
        start_time_tag = 1;
        start_time = it->second;
    }
    if ((all_satellite_window_period.end()-1)->second != satellite_1_duration_time) {     //只要这个覆盖窗口最后没包括卫星的结束时刻，则说明要做结尾处理
        uncover_window_period.push_back(time_period((all_satellite_window_period.end()-1)->second, satellite_1_duration_time));
    }
    
    string path = "uncov_res";
    char a= file_num + '0';
    path.push_back(a);  path.insert(path.length(), ".txt");
    ofstream fout(path, ios::app);
    if (fout.fail()) {
        cerr<<"cant open uncov_res file"<<endl;
        exit(8);
    }
    
    //写文件的同时顺便找最大最小值
    int max_period = 0, min_period = INT_MAX, average_period = 0;;
    fout<<"target:"<<target_num<<endl;
    for (auto it = uncover_window_period.begin(); it != uncover_window_period.end(); ++it) {
        average_period += it->second- it->first;
        if (it->second- it->first > max_period)
            max_period = it->second- it->first;
        if (it->second- it->first < min_period) {
            min_period = it->second- it->first;
        }
        fout<<getTime(it->first, satellite_1_starttime)<<"      "<<getTime(it->second, satellite_1_starttime)<<endl;
    }
    
    fout<<"max_period"<<max_period<<'\t'<<"min_period"<<min_period<<'\t'<<"average_period"<<((float)average_period/uncover_window_period.size())<<endl;
    fout.close();

    
}

void SatelliteInfoManagement::_doubleCov(set<int> &target_k_double_cov_windows, ofstream &fout, const int &target_num)
{

    if (target_k_double_cov_windows.size() == 0){
        cerr<<"target_k_double_cov_windows is empty (target_num:"<<target_num<<')'<<endl;
        return;
    }
    vector<time_period> all_satellite_window_period;//所有卫星并集的时间段求
    int t_start_time;
    bool start_time_tag = 0, end_time_tag = 1;
            
    for(auto it = target_k_double_cov_windows.begin(); it != target_k_double_cov_windows.end(); ++it) {
        //连续时间记录起始和结束
        if (++it == target_k_double_cov_windows.end())//如果到了倒数第一个元素则直接break做结尾处理
            break;
        --it;
        if(*it + 1 == *(++it)) {
            --it;
            if (start_time_tag == 0 && end_time_tag == 1) {
                t_start_time = *it;
                start_time_tag = 1;
                end_time_tag = 0;
            }
        }
        else {
            --it;
            //断开了，此处应该是结束点
            if (start_time_tag == 1 && end_time_tag == 0) {
                start_time_tag = 0;
                end_time_tag = 1;
                all_satellite_window_period.push_back(time_period(t_start_time, *it));
            }
        }
    }
    //把结尾处理了
    all_satellite_window_period.push_back(time_period(t_start_time, *(--target_k_double_cov_windows.end())));
    
    if (fout.fail()) {
        cerr<<"fali to open double_cov file"<<endl;
        exit(7);
    }
    fout<<"target_num:"<<target_num<<endl;
        
    for (int j = 0; j < all_satellite_window_period.size(); ++j) {
        fout<<getTime(all_satellite_window_period[j].first, satellite_1_starttime)<<"      "<<getTime(all_satellite_window_period[j].second, satellite_1_starttime)<<endl;
//        fout<<all_satellite_window_period[j].first<<'\t'<<all_satellite_window_period[j].second<<endl;
    }

}




//SatelliteCovArea

//void SatelliteCovArea::_findMaxMin(float &max_x, float &min_x, float &max_y, float &min_y) const
//{
//    max_x = min_x = polygon_pos[0]._x;
//    max_y = min_y = polygon_pos[0]._y;
//    for (int i = 1; i < polygon_pos.size(); ++i) {
//        if(polygon_pos[i]._x > max_x)
//            max_x = polygon_pos[i]._x;
//        else if(polygon_pos[i]._x < min_x)
//            min_x = polygon_pos[i]._x;
//        if(polygon_pos[i]._y > max_y)
//            max_y = polygon_pos[i]._y;
//        else if(polygon_pos[i]._y < min_y)
//            min_y = polygon_pos[i]._y;
//    }
//    return;
//}
//void SatelliteCovArea::_getCircle()
//{
//    //??
//}
//void SatelliteCovArea::_getBoundary()
//{
//    float xmax,xmin,ymax,ymin;
//    _findMaxMin(xmax, xmin, ymax, ymin);
    //求出矩形
//    rec_boundary = Rectangle(xmax,xmin, ymax, ymin);
    
    //求出圆形
//    circle_center._x = (xmax + xmin) / 2;
//    circle_center._y = (ymax + ymin) / 2;
//    circle_radius =  xmax - circle_center._x;
//}



//
//void SatelliteCovArea::ini()
//{
//    if (polygon_pos.size() == 0) {
//        cerr<<"didnt ini polygon_pos vector"<<endl;
//        exit(2);
//    }
//    _getBoundary();
//}

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
//
//bool SatelliteCovArea::isInside_circle(const EarthPos &p) const
//{
//    float t = pow(p._x - this->circle_center._x, 2) + pow(p._y - this->circle_center._y, 2);
//    if (t < pow(this->circle_radius, 2)) {
//        return true;
//    }
//    return false;
//}
