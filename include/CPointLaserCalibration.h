#include<opencv2/opencv.hpp>
#include<iostream>
#include<unistd.h>
#include<fstream>
#include<string>
#include<chrono>
#include"spdlog/spdlog.h"
#include"spdlog/sinks/daily_file_sink.h"

#define INVALID_VALUE 9999

class CPointLaserCalibration
{
public:
    CPointLaserCalibration();

    enum TCalibrationPlateDist
    {
        NEAREST_DIST_50 = 1,
        MIDDLE_DIST_75,
        FARTHEST_DIST_100
    };

    enum TCameraSelfCheckState
    {
        STATE_ERROR = 0,
        STATE_CORRECT,
        EMPTY_IMAGE
    };

    struct TPoint
    {
        float x;
        float y;
        TPoint() : x(0.0f), y(0.0f){}

        TPoint(float x_, float y_) : x(x_), y(y_){}
    };

    struct TDtofOffset
    {
        float angle;
        float dist;
        TDtofOffset() : angle(0.0f), dist(0.0f){}

        TDtofOffset(float angle_, float dist_) : angle(angle_), dist(dist_){}
    };

    struct TCameraParameter
    {
        cv::Mat camera_matrix;              // 内参矩阵
        cv::Mat rotat_matrix;               // 旋转矩阵
        cv::Mat trans_vector;               // 平移向量
    };

    struct TImagePackage
    {
        cv::Mat image_front;
        cv::Mat image_back;
    };

    struct TDtofCalibrationResult
    {
        float offset_angle;            // 点激光的偏置角度(不是弧度)
        float offset_dist;             // 点激光的偏置距离
        float pitch;                   // 点激光的上翘角度
        bool  offset_valid_flag;       // 偏置角度和偏置距离的有效性，如果为false说明两次标定的误差过大
        bool  pitch_valid_flag;        // 上翘角度的有效性，如果为false则角度超出要求范围
    };

    
    void getCameraParameter();

    void initLog();

    TCameraSelfCheckState cameraSelfCheck(const cv::Mat& image);

    void findOuterRectangle(const cv::Mat& image, const TCameraParameter& camera_parameter, cv::Point3f& out_rect_center_world);

    void findOuterRectangleTest(const cv::Mat& image);

    void findPointLaser(const cv::Mat& image, const TCameraParameter& camera_parameter, cv::Point3f& point_laser_world);

    void pixel2world(const cv::Point& pixel_point, const TCameraParameter& camera_parameter, cv::Point3f& world_point);

    void calculateDtofOffset(const cv::Point3f& laser_point_near, const cv::Point3f& laser_point_far, int dist_type, TDtofOffset& dtof_offset);

    void calculateDtofPitch(const cv::Point3f& laser_point, float& pitch_angular);

    void calcuPointLaserAverage(const std::deque<cv::Point3f>& point_laser_deque, cv::Point3f& average_value);

    bool isvalidDtofOffset(const TDtofOffset& dtof_offset, const TDtofOffset& verify_dtof_offset);

    bool isvalidCoordinate(cv::Point3f& world_point);

    bool isvalidDtofPitch(float dtof_pitch);

    bool getInitPlateCenterState();

    void setPlateDist(TCalibrationPlateDist DIST);

    std::pair<float, float> getDtofOffset();

    void findPointLaserTest(const cv::Mat image);

    void manualCalculateDtofOffset(const cv::Mat& image);

    TDtofCalibrationResult automaticCalibrationDtof(const TImagePackage& image_package_50, const TImagePackage& image_package_75);

private:
    cv::Rect    m_out_rect;                     // 四个黑色矩形的外接矩形
    cv::Point   m_out_rect_center_pixel;        // 四个黑色矩形的外接矩形中心的像素坐标
    cv::Point3f m_out_rect_center_world;        // 四个黑色矩形的外接矩形中心的世界坐标
    cv::Point   m_point_laser_pixel;            // 点激光在像素坐标系中的中心点
    cv::Point3f m_point_laser_world;            // 点激光中心点在世界坐标系中的坐标

    cv::Mat          m_dist_coeffs;             // 畸变系数
    TCameraParameter m_camera_para_50;          // 50cm处标定板使用的相机参数
    TCameraParameter m_camera_para_75;
    TCameraParameter m_camera_para_100;

    bool        m_is_init_plate_center;         // 为1是首次记录标定靶中心坐标，供后续相机自检使用；为0是正常使用时计算中心坐标
    int         jig_config_number;              // 和点激光治具编号对应
    int         m_init_plate_dist;              // 初始化治具时，要保存每块标定板的中心点坐标，此参数是待标定的板子距离
    float       m_plate_dist;                   // 正常使用时，需要检测的标定板距离
    int         m_current_plate_dist_flag;      // 代表当前使用的哪块标定板

    cv::Point3f m_near_laser_point;             // 点激光在三块标定板上的世界坐标位置
    cv::Point3f m_middle_laser_point;
    cv::Point3f m_far_laser_point;
    int         m_first_plat_dist;

    TDtofOffset m_Dtof_offset;
    float       m_Dtof_pitch;
    bool        m_calculate_Dtof_offset;
    bool        m_first_excute;

    bool        m_save_image;
    int         m_point_num;

    float                           m_Dtof_height;
    std::shared_ptr<spdlog::logger> m_logger;
    std::deque<cv::Point3f>         m_rect_center_deque;
    cv::Point3f                     m_plate_center_point_100;
    TCalibrationPlateDist           m_manual_calib_dist;
    std::chrono::time_point<std::chrono::system_clock> m_first_finish_time;
};

