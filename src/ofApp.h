#pragma once

#include "ofMain.h"
#include "ofxHistoryPlot.h"
#include "ofxCvHaarFinder.h"
#include "ofxPlotter.h"
#include "ofxGPlot.h"
#include <thread>
#include "ofxOpenCv.h"
#include <fftw3.h>
//#include "ofOpenCv.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void exit();

	void keyPressed(int key);
	void reboundBox();
	void boundingBoxNoEyes();
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofVideoGrabber web_cam;
	char pixel_array[921600];

	float red_avg;
	float green_avg;
	float blue_avg;

	void HrCalc(vector<float> values);

	void FftFfreq(int size, float sample_spacing);

	vector<float> StandardizeVector(vector<float> values);

	float VecMean(vector<float> values);

	float VecMeanEndRange(vector<float> values, int num_last_vals);

	float VecStdDev(vector<float> values, float mean);

	vector<float> RemoveOutliers(vector<float> values, float k_value);

	ofxHistoryPlot * plot;

	vector <float> webcam_values;
	vector <float> all_green_values;
	vector <float> window_green_values;
	vector <float> std_values;

	vector <float> fft_freq;
	vector <float> out_values;
	vector <float> valid_index;
	vector <float> valid_power;
	vector <float> valid_freqs;
	vector <float> heart_rates;

	float heart_rate;

	int values_count = 0;

	void GreenValBounding(int x_pos, int y_pos, int width, int height);

	ofImage face;
	ofxCvHaarFinder finder;



	int frame_counter = 0;
	int last_10_inx = 0;

	int box_x = 240;
	int box_y = 120;
	int box_w = 160;
	int box_h = 240;

	// These are for tracking without eyes
	int forehead_x = 240;
	int forehead_y = 120;
	int forehead_w = 160;
	int forehead_h = 240;

	int low_face_x = 240;
	int low_face_y = 120;
	int low_face_w = 160;
	int low_face_h = 240;

	int eyes_h;

	bool check_eyes_removed = false;

	ofColor pointColor;
	bool drawLines;
	int circleResolution;
	ofxGPlot graph;
	vector<ofxGPoint> points;
};
