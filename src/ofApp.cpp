#include "ofApp.h"
#include <list> 
#include <vector> 
#include <iostream> 
#include <thread>
#include <fftw3.h>
#include "ofxOpenCv.h"
//#include "objdetect.hpp"

// Toggle these settings
bool kRemove_first_two_seconds = true;
bool kRemove_eyes = true;
float forehead_porportion = .25;
float lower_face_porportion = .52;

int kCam_height = 480;
int kCam_width = 640;
float kCam_stretch = 1.5;

// Remove Eye Toggles
float kFace_box_width_multiplier = 0.6;
float kFace_box_height_multiplier = 0.8;

// Amount of data to process for one hr calc
int kWindow_size = 300; // Divide this by 30 for number of seconds
int kFrames_per_sec = 30;

// Acceptable heart rate ranges
float kMin_hr_bpm = 60.0;
float kMax_hr_bpm = 130.0;
int kSec_per_min = 60;


//--------------------------------------------------------------
void ofApp::setup() {
	
	web_cam.setDesiredFrameRate(kFrames_per_sec);
	web_cam.setup(kCam_width, kCam_height);
	finder.setup("haarcascade_frontalface_default.xml");
	//std::cout << web_cam.listDevices() << endl;

	plot = new ofxHistoryPlot(NULL, "green rgb value", 150, false);
	plot->setRange(0, 255); //hard range, will not adapt to values off-scale
	plot->addHorizontalGuide(255 / 2, ofColor(255, 0, 0)); //add custom reference guides
	plot->setColor(ofColor(0, 255, 0)); //color of the plot line
	plot->setShowNumericalInfo(true);  //show the current value and the scale in the plot
	plot->setRespectBorders(true);	   //dont let the plot draw on top of text
	plot->setLineWidth(1);				//plot line width
	plot->setBackgroundColor(ofColor(0, 220)); //custom bg color
	//custom grid setup
	plot->setDrawGrid(true);
	plot->setGridColor(ofColor(30)); //grid lines color
	plot->setGridUnit(14);
	plot->setCropToRect(true);

	//std::string face_cascade_name = "haarcascade_frontalface_default.xml";

	ofSetVerticalSync(true);
	ofSetFrameRate(kFrames_per_sec);
	//fft.setup(16384);

	//int bufferSize = 300;
	//int fftw_init_threads(void);
	pointColor = ofColor(255, 0, 0, 155);
	drawLines = true;
	circleResolution = 22;

	// Set the circle resolution
	ofSetCircleResolution(circleResolution);

	graph.setPos(640 * kCam_stretch, 0);
	graph.setOuterDim(700 * kCam_stretch, 700 * kCam_stretch);

	graph.getTitle().setRelativePos(0.4);
	graph.getTitle().setTextAlignment(GRAFICA_LEFT_ALIGN);

	graph.getXAxis().setAxisLabelText("x axis");
	graph.getYAxis().setAxisLabelText("y axis");

	/*
	vector<float> temp;
	temp.push_back(5.0);
	temp.push_back(1.0);
	temp.push_back(1.0);
	temp.push_back(1.0);
	temp.push_back(1.0);
	vector<float> rmv_temp = RemoveOutliers(temp, 2);
	std::cout << rmv_temp[0];
	*/
}

//--------------------------------------------------------------
void ofApp::update() {
	frame_counter++;
	web_cam.update();
	plot->update(green_avg);


	/*
	int length = 0;
	if (all_green_values.size() < 300){
		length = all_green_values.size();
	} else {
		length = 300;
	}
	*/
	if (web_cam.isFrameNew() && frame_counter > (kFrames_per_sec / 2)) {
		GreenValBounding(box_x, box_y, box_w, box_h);
		plot->setRange(plot->getLowestValue(), plot->getHighestValue());
		if (all_green_values.size() > kWindow_size && all_green_values.size() % 15 == 0) {
			points.clear();
			int length = all_green_values.size();
			std::vector<float> temp_green_values(&all_green_values[length - kWindow_size - 1], &all_green_values[length - 1]);
			window_green_values = temp_green_values;
			HrCalc(window_green_values);
			//std::thread thread_obj(&ofApp::HrCalc, this, webcam_values);
			//thread_obj.join();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	string msg = ofToString((int)ofGetFrameRate()) + " fps";
	ofFill();
	ofDrawBitmapString(msg, ofGetWidth() - 80, ofGetHeight() - 20);

	web_cam.draw(0, 0, kCam_width * kCam_stretch, kCam_height * kCam_stretch);
	plot->draw(0, kCam_height * kCam_stretch, kCam_width * kCam_stretch, 240 * kCam_stretch);

	ofSetLineWidth(3);
	ofNoFill();
	if (kRemove_eyes) {
		ofDrawRectangle(forehead_x * kCam_stretch, forehead_y * kCam_stretch, forehead_w * kCam_stretch, forehead_h * kCam_stretch);
		ofDrawRectangle(low_face_x * kCam_stretch, low_face_y * kCam_stretch, low_face_w * kCam_stretch, low_face_h * kCam_stretch);
	}
	else {
		ofDrawRectangle(box_x * kCam_stretch, box_y * kCam_stretch, box_w * kCam_stretch, box_h * kCam_stretch);
	}

	graph.beginDraw();
	graph.drawBox();
	graph.drawXAxis();
	graph.drawYAxis();
	graph.drawTitle();

	if (drawLines) {
		graph.drawLines();
	}

	graph.drawPoints(pointColor); // this is 3 times faster than drawPoints()
	graph.drawLabels();
	graph.endDraw();
}

void ofApp::GreenValBounding(int x_pos, int y_pos, int width, int height) {
	float red_sum = 0;
	float green_sum = 0;
	float blue_sum = 0;

	for (int i = 0; i < (kCam_width * kCam_height * 3); i += 3) {
		int cur_xpos = 0;
		int cur_height = i / 1920;
		if (cur_height > 1) {
			cur_xpos = i - (cur_height * 1920);
		}
		if (cur_xpos >= x_pos * 3 && cur_xpos <= (x_pos + width) * 3) {
			if (cur_height >= y_pos && cur_height <= (y_pos + height)) {
				//red_sum = red_sum + (float)(web_cam.getPixels()[i]);
				if (kRemove_eyes && check_eyes_removed) {
					if (cur_height >= y_pos + box_h + eyes_h || cur_height <= y_pos + box_h) {
						green_sum = green_sum + (float)(web_cam.getPixels()[i + 1]);
						//std::cout << cur_height << " " << cur_xpos << " " << green_sum << endl;
						//blue_sum = blue_sum + (float)(web_cam.getPixels()[i+2]);
					}
				}
				else {
					green_sum = green_sum + (float)(web_cam.getPixels()[i + 1]);
					//std::cout << cur_height << " " << cur_xpos << " " << green_sum << endl;
					//blue_sum = blue_sum + (float)(web_cam.getPixels()[i+2]);
				}

			}
		}
	}
	//red_avg = red_sum / (width * height);
	green_avg = green_sum / (width * height);
	//blue_avg = blue_sum / (width * height);
	//std::cout << (red_sum/307200) << std::endl;

	//std::cout << green_avg << std::endl;
	all_green_values.push_back(green_avg);
	//std::cout << all_green_values.size();
}

void ofApp::HrCalc(vector<float> values)
{
	// Find power Spectrum
	fftw_complex *in, *out;
	//fftw_complex *out;
	fftw_plan p;

	std_values = StandardizeVector(values);

	in = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * kWindow_size);

	out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * kWindow_size);

	p = fftw_plan_dft_1d(kWindow_size, in, out, FFTW_FORWARD, FFTW_MEASURE);

	for (int i = 0; i < kWindow_size; i++) {
		in[i][0] = std_values[i];
		in[i][1] = 0;
	}

	fftw_execute(p);

	for (int i = 0; i < kWindow_size; i++) {
		float temp = abs(pow(out[i][0], 2));
		out_values.push_back(temp);
	}

	FftFfreq(kWindow_size, (1.0 / kFrames_per_sec));

	for (int i = 0; i < kWindow_size; i++) {
		if ((fft_freq[i] >= kMin_hr_bpm / kSec_per_min) && (fft_freq[i] <= kMax_hr_bpm / kSec_per_min)) {
			valid_index.push_back(i);
		}
	}

	for (int i = 0; i < valid_index.size(); i++) {
		valid_power.push_back(out_values[valid_index[i]]);
		valid_freqs.push_back(fft_freq[valid_index[i]]);
	}
	float max_power = 0.0;
	int max_power_index = 0;
	for (int i = 0; i < valid_power.size(); i++) {
		if (valid_power[i] > max_power) {
			max_power = valid_power[i];
			max_power_index = i;
		}
	}

	heart_rate = valid_freqs[max_power_index] * kSec_per_min;

	if (max_power > 100000) {
		max_power = 1000;
	}
	graph.setYLim(0.0, (double)max_power + 10);

	heart_rates.push_back(heart_rate);
	vector<float> clean_heart_rates = RemoveOutliers(heart_rates, 1.5);

	if (heart_rates.size() >= 10) {
		std::cout << heart_rate << " From freq: " << valid_freqs[max_power_index] << " " << endl;
		std::cout << "Average Heart Rate: " << VecMeanEndRange(clean_heart_rates, 20) << endl << endl;
	}

	fftw_destroy_plan(p);
	//fftw_free(in); 
	fftw_free(out);
	std::cout << "HrCalc runthrough" << endl;

	for (int i = 0; i < valid_freqs.size(); i++) {
		points.emplace_back(valid_freqs[i], valid_power[i]);
	}

	// Add the points
	graph.setPoints(points);
	graph.setXLim(kMin_hr_bpm / kSec_per_min, kMax_hr_bpm / kSec_per_min);

	fft_freq.clear();
	out_values.clear();
	std_values.clear();
	valid_index.clear();
	valid_power.clear();
	valid_freqs.clear();
}

void ofApp::FftFfreq(int size, float sample_spacing) {
	// reworked from the np.fft.fftfreq algorithm
	float val = 1.0 / (size * sample_spacing);
	int N = ((size - 1) / 2) + 1;
	for (int i = 0; i < N; i++) {
		float temp = i * val;
		fft_freq.push_back(temp);
	}
	for (int i = -(size / 2); i < 0; i++) {
		float temp = i * val;
		fft_freq.push_back(temp);
	}
}

vector<float> ofApp::StandardizeVector(vector<float> values) {
	vector<float> build_vector;
	float raw_total = 0;
	float raw_standard_deviation = 0;
	float mean = VecMean(values);
	float std = VecStdDev(values, mean);

	// Normalize the data
	for (int i = 0; i < values.size(); i++) {
		build_vector.push_back((values[i] - mean) / std);
	}
	return build_vector;
}

float ofApp::VecMean(vector<float> values) {
	float raw_total = 0;

	for (int i = 0; i < values.size(); i++) {
		raw_total += values[i];
	}
	float mean = raw_total / values.size();
	return mean;
}

float ofApp::VecMeanEndRange(vector<float> values, int num_last_vals) {
	float raw_total = 0;

	if (values.size() < num_last_vals) {
		return VecMean(values);
	}

	for (int i = (values.size() - num_last_vals); i < values.size(); i++) {
		raw_total += values[i];
	}
	float mean = raw_total / num_last_vals;
	return mean;
}

float ofApp::VecStdDev(vector<float> values, float mean) {
	float raw_std_dev = 0;

	for (int i = 0; i < values.size(); i++) {
		raw_std_dev += pow(values[i] - mean, 2);
	}
	float std_dev = sqrt(raw_std_dev / values.size());
	return std_dev;
}

vector<float> ofApp::RemoveOutliers(vector<float> values, float k_value) {
	vector<float> outliers_removed = values;
	float mean = VecMean(values);
	float std_dev = VecStdDev(values, mean);

	for (int i = values.size() - 1; i >= 0; i--) {
		if (values[i] >= mean + (std_dev * k_value) || values[i] <= mean - (std_dev * k_value)) {
			outliers_removed.erase(outliers_removed.begin() + i);
		}
	}
	return outliers_removed;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	if (key == 'p')
	{
		face.setFromPixels(web_cam.getPixels());
		reboundBox();
	}
}

void ofApp::reboundBox() {
	face.setFromPixels(web_cam.getPixels());
	finder.findHaarObjects(face);
	try {
		if (finder.blobs.size() > 0) {
			ofRectangle cur = finder.blobs[0].boundingRect;
			box_w = cur.width * kFace_box_width_multiplier;
			box_h = cur.height * kFace_box_height_multiplier;
			box_x = cur.x + (.5 * cur.width - (.5 * box_w));
			box_y = cur.y + (.5 *  cur.height - (.5 * box_h));
			if (kRemove_eyes) {
				boundingBoxNoEyes();
				check_eyes_removed = true;
			}
		}
		else {
			std::cout << "No bounding rectangles";
		}
	}
	catch (...) {
		std::cout << "failed";
	}
}

void ofApp::boundingBoxNoEyes() {
	eyes_h = box_h * (1 - forehead_porportion - lower_face_porportion); // 2
	forehead_x = box_x;
	forehead_y = box_y;
	forehead_w = box_w;
	forehead_h = box_h * forehead_porportion;

	low_face_x = box_x;
	low_face_y = box_y + eyes_h + forehead_h;
	low_face_w = box_w;
	low_face_h = box_h * lower_face_porportion;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::exit() {

}