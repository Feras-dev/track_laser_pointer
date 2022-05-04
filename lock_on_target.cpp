/**
 * @file lock_on_target.cpp
 * @author Feras Alshehri (falshehri@mail.csuchico.edu)
 * @brief a program to detect the foreground target (laser spot) and lock on target by
 * overlaying a crosshair marker on the center of the detected spot.
 * @version 0.1
 * @date 2022-03-01
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <experimental/filesystem>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#define INTENSITY_THRESHOLD 253
#define LINE_THICKNESS 1
#define LINE_LENGTH_PX 80
#define BGR_WHITE (Scalar(255, 255, 255))
#define WIN_TITLE_1 "source frame"
#define WIN_TITLE_2 "gray frame"
#define WIN_SIZE_SCALE_DOWN 6
#define SYSTEM_ERROR (-1)
#define ESCAPE_KEY (27)

using namespace std;
using namespace cv;
namespace fs = std::experimental::filesystem;

// file global
static string out_filepath = "";
static string folder_path;
static string file_under_processing;

/**
 * @brief main loop to run during runtime and control the state machine.
 *
 */
static void mainLoop(string file_path) {
    char                 winInput;  // holds user input while window is in-focus
    Mat                  frame_src; // source frame
    vector<unsigned int> tmp;       // holds y coordinates in each row with
    vector<unsigned int> avg_x;     // holds center in each col
    vector<unsigned int> avg_y;     // holds center in each row
    unsigned int         x_com;
    unsigned int         y_com;
    int                  sum;

    // load image into Mat
    frame_src = imread(file_path, IMREAD_GRAYSCALE);

    // Ensure laoded frame is not empty
    if (frame_src.empty()) { exit(SYSTEM_ERROR); }

    // create a window to display our video
    namedWindow(WIN_TITLE_1);

    // get target's center coordinates
    // scan each pixel in each row to find the target and store the mean in the
    // corresponding vector

    // find center along the x-axis
    for (unsigned int y = 0; y < frame_src.rows; y++) {
        for (unsigned int x = 0; x < frame_src.cols; x++) {
            auto intensity = (unsigned int)frame_src.at<uchar>(y, x);
            if (intensity > INTENSITY_THRESHOLD) {
                tmp.push_back(x);
                // cout << intensity << " @ " << x << "," << y << endl;
            }
        }
        // row scan complete
        // find center in this row
        int sum = 0;
        for (int k = 0; k < tmp.size(); ++k) {
            sum += tmp[k];
        }
        // append center of this row in y vector
        if (sum / tmp.size() > 0) {
            avg_y.push_back(sum / tmp.size());
            // cout << "row avg = " << sum / tmp.size() << endl;
        }
        // and clean up before next row
        tmp.clear();
    }

    // find center along the y-axis
    for (unsigned int y = 0; y < frame_src.cols; y++) {
        for (unsigned int x = 0; x < frame_src.rows; x++) {
            auto intensity = (unsigned int)frame_src.at<uchar>(x, y);
            if (intensity > INTENSITY_THRESHOLD) {
                tmp.push_back(x);
                // cout << intensity << " @ " << x << "," << y << endl;
            }
        }
        // row scan complete
        // find center in this col
        int sum = 0;
        for (int k = 0; k < tmp.size(); ++k) {
            sum += tmp[k];
        }
        // append center of this col in x vector
        if (sum / tmp.size() > 0) {
            avg_x.push_back(sum / tmp.size());
            // cout << "col avg = " << sum / tmp.size() << endl;
        }
        // and clean up before next col
        tmp.clear();
    }

    // find the overall center of mass
    sum = 0;
    for (int k = 0; k < avg_x.size(); ++k) {
        sum += avg_x[k];
    }
    if (avg_x.size() > 0) { y_com = sum / avg_x.size(); }

    sum = 0;
    for (int k = 0; k < avg_y.size(); ++k) {
        sum += avg_y[k];
    }
    if (avg_y.size() > 0) { x_com = sum / avg_y.size(); }

    cout << "overall x =" << x_com << endl;
    cout << "overall y =" << y_com << endl;

    // draw crosshair marker on target's center
    Point p_x1(x_com + LINE_LENGTH_PX, y_com);
    Point p_x2(x_com - LINE_LENGTH_PX, y_com);
    Point p_y1(x_com, y_com + LINE_LENGTH_PX);
    Point p_y2(x_com, y_com - LINE_LENGTH_PX);

    line(frame_src, p_x1, p_x2, BGR_WHITE, LINE_THICKNESS, LINE_AA);
    line(frame_src, p_y1, p_y2, BGR_WHITE, LINE_THICKNESS, LINE_AA);

    // display source and processed frame
    // imshow(WIN_TITLE_1, frame_src);

    // waitKey(0);

    // write out the frame to disk as pgm file
    out_filepath = folder_path;
    out_filepath += "target_locked_frames/";
    out_filepath += file_under_processing;
    cout << "writing img to: " << out_filepath << endl;

    imwrite(out_filepath, frame_src);

    return;
}

/**
 * @brief entry point of program.
 *
 * @param argc count of cmdl args.
 * @param argv list of strings, each representing a cmdl arg.
 * @return int returns 0 upon successful execution.
 */
int main(int argc, char **argv) {
    // define command line args
    CommandLineParser parser(
        argc, argv,
        "{@folder_path      | | path to folder containing frames to process (*.pgm).}"
        "{help             h|false|show help message}");

    parser.printMessage();
    cout << "The program will process each frame in the folder path given, and "
            "output it "
            "as a new frame (*.pgm) under a new directory\n";
    cout << "Press 'ESC' at any moment to exit program.\n";

    folder_path = parser.get<string>("@folder_path");

    // process all .pgm files under the given folder path, limited to direct children
    // only
    for (auto &f : fs::directory_iterator(folder_path)) {
        if ((f.path().extension() == ".pgm") || (f.path().extension() == ".PGM")) {
            cout << " >> processing " << f.path() << "\n";
            file_under_processing = f.path().filename();
            mainLoop(f.path());
        } else {
            cout << " >> skipping file " << f.path() << "\n";
        }
    }

    destroyWindow(WIN_TITLE_1);

    cout << " >> Exiting!" << endl;

    return 0;
}