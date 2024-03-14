#include "source/camera.h"

int main(int argc, char *argv[]) {

    //create Qt application
    QApplication app(argc, argv);

    //set the default program parameters: camera index, FPS, frame width, frame height
    int params[4] = { 0, 30, 1280, 720 };

    //get the first four program arguments and try to convert them to int
    for (int i = 1; i < argc; i++) {
        std::string buf(argv[i]);
        try {
            int v = std::stoi(buf);
            params[i-1] = v;
        }
        catch (const std::invalid_argument& e) {
            QMessageBox::critical(nullptr, "Error", "Invalid Argument", QMessageBox::Ok);
            return 1;
        }
    }

    //attempt to open the webcam view
    try {
        CameraWindow window(params[0], params[1], params[2], params[3]);
        window.show();
        return QApplication::exec();
    }
    //show error if failed
    catch (const CameraException& e) {
        std::cerr << e.what() << std::endl;
        QMessageBox::critical(nullptr, "Error", e.what(), QMessageBox::Ok);
        return 1;
    }
}
