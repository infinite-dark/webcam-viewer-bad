#ifndef QT_WEBCAM_VIEW
#define QT_WEBCAM_VIEW

#include <QApplication>
#include <QtGui>
#include <QtWidgets>

#include <opencv4/opencv2/opencv.hpp>

#include <iostream>
#include <cmath>
#include <string>

#endif //QT_WEBCAM_VIEW

#define WIDTH 1280
#define HEIGHT 720
#define FPS 30

//register the OpenCV array type in Qt for passing via signals and slots
Q_DECLARE_METATYPE(cv::Mat)

//exception for failing to open the camera
class CameraException : public std::exception {
public:
    const char* what() const noexcept override {
        return "Opening Camera Failed";
    }
};

//minimal camera abstraction class for basic setup and reading frames
class Camera {
public:
    explicit Camera(int index, int fps=FPS, int width=WIDTH, int height=HEIGHT);
    bool getFrame(cv::Mat &frame);
    ~Camera();
private:
    cv::VideoCapture* camera{};
};

//thread class for reading frames in a loop and sending update signals to the main window
class VideoReceiver : public QThread {
    //register as QObject - for setting up signals and slots
Q_OBJECT
public:
    explicit VideoReceiver(Camera &cam);
    void run() override;
    void stop();

    //the image update signal that also carries the frame data
signals:
    void update_frame(cv::Mat& frame);
private:
    Camera& camera;
    cv::Mat frame;
    bool running;
};

//the main application window
class CameraWindow : public QMainWindow {
    //register as QObject - for setting up signals and slots
Q_OBJECT
public:
    explicit CameraWindow(int index, int fps=FPS, int width=WIDTH, int height=HEIGHT);
    ~CameraWindow() override;

    //the image update slot that receives a frame, converts it to an image and updates it in the window
private slots:
    void setImage(cv::Mat& f);
private:
    void keyReleaseEvent(QKeyEvent *event) override;
    //void closeEvent(QCloseEvent *event) override;

    QWidget frame;
    QVBoxLayout frame_layout;

    Camera* camera;
    VideoReceiver* receiver;
    QLabel video;
    QPixmap pixmap;
};

