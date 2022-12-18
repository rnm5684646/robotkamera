#include <opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include <opencv2/core/utility.hpp>
#include<iostream>
#include<ctime>


using namespace cv;
using namespace std;


Mat org, imgResize, img, imgGRAY, imgBlur, mask, imgThreshold, imgContours;
double odchylka;
int bmin = 0, gmin = 0, rmin = 0;
int bmax = 52, gmax = 35, rmax = 36;


double contourDistance, prevDistance = 150, plocha;
int contourIndex;


double findContourDistance(const Mat& image, const vector<Point>& contour)
{
	Moments contourMoments = moments(contour);
	Point contourCenter(contourMoments.m10 / contourMoments.m00, contourMoments.m01 / contourMoments.m00);
	circle(image, contourCenter, 5, Scalar(0, 55, 55), FILLED);
	Point imageCenter(image.cols / 2, image.rows / 2);
	circle(image, imageCenter, 5, Scalar(0, 0, 125), FILLED);
	return norm(imageCenter - contourCenter); 
	
}

double findDevitation(const Mat& image, const vector<Point>& contour)
{
	Moments contourMoments = moments(contour);
	Point contourCenter(contourMoments.m10 / contourMoments.m00, contourMoments.m01 / contourMoments.m00);
	Point imageCenter(image.cols / 2, image.rows / 2);
	return imageCenter.x - contourCenter.x;

}
int main() {

	VideoCapture cap(0);
	namedWindow("Trackbars",	WINDOW_NORMAL);
	 createTrackbar("Blue Min", "Trackbars", &bmin, 255);
	 createTrackbar("Blue Max", "Trackbars", &bmax, 255);
	 createTrackbar("Green Min", "Trackbars", &gmin, 255);
	 createTrackbar("Green Max", "Trackbars", &gmax, 255);
	 createTrackbar("Red Min", "Trackbars", &rmin, 255);
	 createTrackbar("Red Max", "Trackbars", &rmax, 255);
	
	while (true)
	{	
		clock_t start = clock(); //m��en� �asu 

		cap.read(org);			// p�e�ten� webc
		resize(org, img, Size(), 0.5, 0.5); // zmen�en�

		Scalar lower(bmin, gmin, rmin);// limity pro masku
		Scalar upper(bmax, gmax, rmax);

		inRange(img, lower, upper, imgBlur); //maska
		GaussianBlur(imgBlur, mask, Size(3, 3), 0);

		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(mask, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);
		cout << "po�et kontur: " << contours.size()<< endl;
		if (contours.size() > 0) {
			for (int i = 0; i < contours.size(); i++)
			{
			contourDistance = findContourDistance(img, contours[i]);
			
			if (contourDistance < prevDistance)
				{
					prevDistance = contourDistance;
					contourIndex = i;
				}

			}
			drawContours(img, contours, contourIndex, Scalar(255, 255, 0), 2);
			prevDistance = 10000;
			odchylka = findDevitation(img, contours[contourIndex]);
			cout << "Odchylka: " << odchylka << endl;
		}
		//Moments m = moments(mask, true);

		imshow("org", img);
		
		imshow("maska", mask);

		waitKey(1);
		clock_t stop = clock();
		double cas = (stop - start) / static_cast<double>(CLOCKS_PER_SEC);
		cout << "fps: " << 1/cas  << endl;
	}
	return 0;
}
