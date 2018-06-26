#include <opencv2\opencv.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "opencv2/core/core.hpp"

#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;

class ColletorMat
{
private:
    int indexFrame;
    bool found;
    Mat frame;

public:

    ColletorMat(int index, bool found, Mat frame)
    {
        this->indexFrame = index;
        this->found = found;
        this->frame = frame;
    }

    ~ColletorMat()
    {

    }

    // settors
    void set_indexFrame(int index)
    {
        this->indexFrame = index;
    }

    void set_found(bool found)
    {
        this->found = found;
    }

    void set_frame(Mat frame)
    {
        this->frame = frame;
    }

    // accessors
    int get_indexFrame()
    {
        return this->indexFrame;
    }

    bool get_found()
    {
        return this->found;
    }

    Mat get_frame()
    {
        return this->frame;
    }

};

void matwrite(ofstream& fs, const Mat& mat, int index, bool checking)
{


    // Data Object
    int indexFrame = index;
    bool found = checking;
    fs.write((char*)&indexFrame, sizeof(int));    // indexFrame
    fs.write((char*)&found, sizeof(bool));    // bool checking

    // Header
    int type = mat.type();
    int channels = mat.channels();
    fs.write((char*)&mat.rows, sizeof(int));    // rows
    fs.write((char*)&mat.cols, sizeof(int));    // cols
    fs.write((char*)&type, sizeof(int));        // type
    fs.write((char*)&channels, sizeof(int));    // channels

    // Data
    if (mat.isContinuous())
    {
        fs.write(mat.ptr<char>(0), (mat.dataend - mat.datastart));
    }
    else
    {
        int rowsz = CV_ELEM_SIZE(type) * mat.cols;
        for (int r = 0; r < mat.rows; ++r)
        {
            fs.write(mat.ptr<char>(r), rowsz);
        }
    }
}



ColletorMat matread(ifstream& fs)
{
    // Data Object
    int indexFrame;
    bool found;
    fs.read((char*)&indexFrame, sizeof(int));     //
    fs.read((char*)&found, sizeof(bool));         //

    // Header
    int rows, cols, type, channels;
    fs.read((char*)&rows, sizeof(int));         // rows
    fs.read((char*)&cols, sizeof(int));         // cols
    fs.read((char*)&type, sizeof(int));         // type
    fs.read((char*)&channels, sizeof(int));     // channels

    // Data
    Mat mat(rows, cols, type);
    fs.read((char*)mat.data, CV_ELEM_SIZE(type) * rows * cols);

    ColletorMat ojbectMat(indexFrame, found, mat);
    return ojbectMat;
}

int main()
{
    // Save the random generated data
    {
        Mat image1, image2, image3;
        image1 = imread("C:\\opencvVid\\data_seq\\Human3\\0001.jpg");
        image2 = imread("C:\\opencvVid\\data_seq\\Human3\\0002.jpg");
        image3 = imread("C:\\opencvVid\\data_seq\\Human3\\0003.jpg");

        if (image1.empty() || image2.empty() || image3.empty()) {
            std::cout << "error: image not readed from file\n";
            return(0);
        }

        imshow("M1",image1);
        imshow("M2",image2);
        imshow("M3",image3);

        (char)cvWaitKey(0);

        ofstream fs("azdoudYoussef.bin", fstream::binary);
        matwrite(fs, image1, 100, true);
        matwrite(fs, image2, 200, true);
        matwrite(fs, image3, 300, true);
        fs.close();

        double tic = double(getTickCount());
        ifstream loadFs("azdoudYoussef.bin", ios::binary);

        if(!loadFs.is_open()){
            cout << "error while opening the binary file" << endl;
        }

        ColletorMat lcolletorMat1 = matread(loadFs);
        ColletorMat lcolletorMat2 = matread(loadFs);
        ColletorMat lcolletorMat3 = matread(loadFs);

        cout << "frames loaded up " << endl;

        vector<ColletorMat> setFrames;
        setFrames.push_back(lcolletorMat1);
        setFrames.push_back(lcolletorMat2);
        setFrames.push_back(lcolletorMat3);

        imshow("1", lcolletorMat1.get_frame());
        imshow("2", lcolletorMat2.get_frame());
        imshow("3", lcolletorMat3.get_frame());
        (char)cvWaitKey(0);

        cout << "indexFrame" <<lcolletorMat1.get_indexFrame() << "found" << lcolletorMat1.get_found();
        double toc = (double(getTickCount()) - tic) * 1000. / getTickFrequency();
        cout << "Using Raw: " << toc << endl;
        loadFs.close();

    }

    return 0;
}
