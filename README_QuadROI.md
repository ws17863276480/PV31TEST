# 四边形ROI选择器

## 功能概述

本项目已重构，新增了不规则四边形ROI选择功能，相比原来的矩形ROI选择，四边形ROI可以更精确地定义感兴趣区域。

## 主要特性

### 1. 交互式四边形选择
- 通过鼠标点击4个点来定义不规则四边形
- 实时预览选择过程
- 支持ESC键取消选择
- 自动计算外接矩形

### 2. 文件操作
- 保存四边形ROI到文件（`quad_roi.txt`）
- 从文件加载四边形ROI
- 兼容原有的矩形ROI文件格式

### 3. 几何计算
- 点是否在四边形内的检测（射线法）
- 四边形绘制和可视化
- 外接矩形计算

## 使用方法

### 编译项目
```bash
mkdir build
cd build
cmake ..
make
```

### 运行主程序
```bash
./point_laser
```

程序启动后会显示菜单：
1. 矩形ROI - 原有的矩形选择功能
2. 四边形ROI - 新的四边形选择功能
3. 相机标定 - 相机标定功能

### 四边形ROI选择步骤
1. 选择选项2（四边形ROI）
2. 在弹出的窗口中点击4个点来定义四边形
3. 按ESC键可以取消选择
4. 选择完成后会显示结果和保存选项
5. 可以点击图像测试点检测功能

### 运行测试程序
```bash
./test_quad_roi
```

测试程序功能：
- 加载已保存的四边形ROI文件
- 创建示例四边形ROI
- 演示点检测功能

## 文件格式

### 四边形ROI文件格式（quad_roi.txt）
```
QUAD_ROI
x1 y1
x2 y2
x3 y3
x4 y4
```

### 矩形ROI文件格式（rect_roi.txt）
```
x y width height
```

## API接口

### 核心函数

```cpp
// 四边形ROI选择
QuadROI selectQuadROIFromImage(const cv::Mat& image, const std::string& windowName = "选择四边形ROI");

// 保存四边形ROI
bool saveQuadROIToFile(const QuadROI& quadRoi, const std::string& filePath);

// 加载四边形ROI
bool loadQuadROIFromFile(QuadROI& quadRoi, const std::string& filePath);

// 绘制四边形ROI
void drawQuadROI(cv::Mat& image, const QuadROI& quadRoi, const cv::Scalar& color = cv::Scalar(0, 255, 0), int thickness = 2);

// 点检测
bool isPointInQuadROI(const cv::Point2f& point, const QuadROI& quadRoi);
```

### QuadROI结构体

```cpp
struct QuadROI {
    std::vector<cv::Point2f> points;  // 四个顶点
    cv::Rect boundingBox;             // 外接矩形
    
    bool isValid() const;             // 检查是否有效
    void updateBoundingBox();         // 更新外接矩形
};
```

## 技术实现

### 1. 鼠标交互
- 使用OpenCV的鼠标回调函数
- 实时显示选择过程和预览线
- 支持动态点添加和连线

### 2. 点检测算法
- 使用射线法（Ray Casting Algorithm）
- 从测试点向右发射射线
- 统计与四边形边界的交点数
- 奇数个交点表示点在内部

### 3. 文件格式兼容
- 通过文件头标识区分矩形和四边形ROI
- 向后兼容原有的矩形ROI文件

## 示例代码

```cpp
#include "roi_selector.h"

// 选择四边形ROI
Mat image = imread("test.jpg");
QuadROI quadRoi = selectQuadROIFromImage(image, "选择ROI");

if (quadRoi.isValid()) {
    // 保存ROI
    saveQuadROIToFile(quadRoi, "my_roi.txt");
    
    // 绘制ROI
    Mat result = image.clone();
    drawQuadROI(result, quadRoi, Scalar(0, 255, 0), 2);
    imshow("结果", result);
    
    // 测试点检测
    Point2f testPoint(100, 100);
    bool inside = isPointInQuadROI(testPoint, quadRoi);
    cout << "点(" << testPoint.x << "," << testPoint.y << ") " 
         << (inside ? "在四边形内" : "在四边形外") << endl;
}
```

## 注意事项

1. 四边形顶点按顺时针或逆时针顺序点击
2. 确保四边形不重叠（避免自交）
3. 文件保存路径需要写入权限
4. 图像窗口关闭后ROI选择会结束

## 更新日志

- 新增四边形ROI选择功能
- 重构ROI选择器模块
- 添加文件格式兼容性
- 实现点检测算法
- 创建测试程序 