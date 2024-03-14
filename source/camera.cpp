#include "camera.h"


//initializing constructor for the camera
Camera::Camera(int index, int fps, int width, int height) {
    //create a new OpenCV VideoCapture and set its api preference to "video for linux 2" for parameter control
    camera = new cv::VideoCapture(index, cv::CAP_V4L2);
    
    //enable MJPEG compression for maximum framerate and resolution (using a four character code to describe the format)
    camera->set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

    //attempt to set resolution
    camera->set(cv::CAP_PROP_FRAME_WIDTH, width);
    camera->set(cv::CAP_PROP_FRAME_HEIGHT, height);

    //attempt to set framerate
    camera->set(cv::CAP_PROP_FPS, fps);

    if (!camera->isOpened())
        throw CameraException();
}

//read frame and save it in the frame variable
bool Camera::getFrame(cv::Mat &frame) {
    return camera->read(frame);
}

//release the VideoCapture and delete it
Camera::~Camera() {
    camera->release();
    delete camera;
}


//initialize the fields in the receiver
VideoReceiver::VideoReceiver(Camera &cam) : camera(cam), running(false) {}

//the main loop of the thread that reads frames and emits signals
void VideoReceiver::run() {
    //start the infinite loop
    running = true;
    while (running) {
        //read next frame
        bool ret = camera.getFrame(frame);
        //if reading succeeded
        if (ret) {
            //optionally resize - needs recompilation at the moment but could be easily expanded to have the ability to toggle resizing
            //cv::resize(frame, frame, cv::Size(WIDTH, HEIGHT), 0, 0, cv::INTER_CUBIC);
            //emit the signal with the new frame
            emit update_frame(frame);
        }
    }
}

//stop the main loop and wait for other threads to finish - necessary for not crashing on exit
void VideoReceiver::stop() {
    running = false;
    wait();
}


//constructor - initialize the main window
CameraWindow::CameraWindow(int index, int fps, int width, int height) {
    //register the Mat type for passing via signals and slots
    qRegisterMetaType<cv::Mat>("cv::Mat&");

    //set up UI elements
    frame.setLayout(&frame_layout);
    frame_layout.setContentsMargins(0, 0, 0, 0);
    frame_layout.addWidget(&video);

    setWindowTitle("Camera Window");
    this->setCentralWidget(&frame);
    this->setFixedSize(width, height);

    //create a new camera with given parameters
    camera = new Camera(index, fps, width, height);

    //create the threaded receiver object and pass it the camera for reference
    receiver = new VideoReceiver(*camera);

    //connect the frame update signal to updating the image in the main window
    QObject::connect(receiver, &VideoReceiver::update_frame,
                     this, &CameraWindow::setImage);

    //begin reading frames
    receiver->start();
}

//destructor - stop reading frames and delete objects
CameraWindow::~CameraWindow() {
    receiver->stop();
    delete camera;
    delete receiver;
}

//do stuff on keyboard clicks - in this case close app on pressing 'q'
void CameraWindow::keyReleaseEvent(QKeyEvent *event) {
    int key = event->key();
    if(key == Qt::Key_Q)
        close();
}

//the set image slot that takes the frame array data, converts it into an image structure that Qt understands
//then converts it to pixmap that can be shown in the main window and updates the view in the window
void CameraWindow::setImage(cv::Mat& f) {
    QImage image(f.data, f.cols, f.rows, f.cols*f.channels(), QImage::Format_BGR888);
    pixmap = QPixmap::fromImage(image);
    video.setPixmap(pixmap);
}


