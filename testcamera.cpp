// ����

/*
library include
library link
*/
#include "../Common/Common.h"
#include <filesystem>
namespace fs = std::filesystem;
int main()
{


	//rectangle(input_img, Rect(10, 10, 300, 300), Scalar(255, 255, 255), LineTypes::FILLED);


	//image path
	string fileDir = "D:\\camera module\\2018121014312634_Lens_6_2_0_PASS.jpg";
	//vector<string> filelist;			// ���̺귯�� ������ vector
	//for (const auto& entry : fs::directory_iterator(fileDir)) {		// & : ���۷��� �����̴�. �ּҰ� �ƴϴ�
	//	std::cout << entry.path().string() << std::endl;
	//	filelist.push_back(entry.path().string());	// fileDir�� �ִ� ������ �ϳ��� filelist��� ���Ϳ� �ִ´�.(push_back)
	//}

	//for (const std::string& filePath : filelist) {	// ���۷��� : �޸𸮿� �ִ� �������ߵ�
	//	std::cout << filePath << std::endl;
		Mat input_img = cv::imread(fileDir, cv::ImreadModes::IMREAD_ANYCOLOR);





		//to do (�⺻���� ���� ����)
		int width = input_img.cols;
		int height = input_img.rows / 4;
		int channel = input_img.channels();
		//get roi	roi : Ư�� ������ ���ؼ��� ���� ó���� �����Ҷ� �����ϴ� ����	(����ڰ� ���Ƿ� ������)
		// �簢���� �ƴ� �ٸ� ������� ROI ���� �� ����ũ ������ ����
		Rect roi = Rect(1200, 400, 3400 - 1200, 2600 - 400);
		// (1200, 400) ��ǥ���� (3400 - 1200) x (2600 - 400) ũ�⸸ŭ�� �簢�� �κ� ������ �����ؼ� roi�� ����
		// 3400�� ���ϴ� �̹��� �κ��� x�� ����, 2600�� ���ϴ� �̹��� �κ��� y��

		Mat img_1 = input_img(Rect(0, 0, width, height));
		Mat obj_img = img_1(roi).clone();
		Mat hole_img = ~obj_img;
		Mat bin_hole_img;
		double thres_min = 200;	// ù��° ��Ŀ��  ��(Circle), ���� ���� 200�̴�.
		double thres_max = 255;
		threshold(hole_img, bin_hole_img, thres_min, thres_max, ThresholdTypes::THRESH_BINARY);
		// 200 �Ʒ��� 0�� �ǰ�, 200�̻��̸� 255�� �ٲٰ� �̹��� ��ü�� ����ȭ�Ѵ�.

//erosion	// ħ�� ����
		erode(bin_hole_img, bin_hole_img, Mat::ones(Size(3, 3), CV_8UC1), Point(-1, -1), 2);
		// ������ �׸���
		std::vector<std::vector<cv::Point>> contours;
		std::vector<cv::Vec4i> hierarchy;
		cv::findContours(bin_hole_img, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

		int max_index = -1;
		double max_area = -1;
		for (size_t i = 0; i < contours.size(); i++)
		{
			double area = cv::contourArea(contours[i]);
			if (max_area < area)
			{
				max_area = area;
				max_index = i;
			}
		}
		//make mask

		cv::Mat mask = cv::Mat::zeros(bin_hole_img.size(), CV_8UC1);
		if (max_index >= 0)
		{
			drawContours(mask, contours, max_index, cv::Scalar(255), CV_FILLED);
		}
		//reduce image
		Mat reduced_obj_img = obj_img & mask;


		//cv::Mat mask_small = cv::Mat::zeros(Size(500, 500), CV_8UC1);
		//cv::Mat mask_large = cv::Mat::zeros(Size(500, 500), CV_8UC1);
		//cv::Mat mask_donut = cv::Mat::zeros(Size(500, 500), CV_8UC1);

		//circle(mask_small, Point(250, 250), 50, Scalar(255, 255, 255), LineTypes::FILLED);
		//circle(mask_large, Point(250, 250), 200, Scalar(255, 255, 255), LineTypes::FILLED);

		//mask_donut = mask_large - mask_small;
		cv::Mat mask_small = cv::Mat::zeros(bin_hole_img.size(), CV_8UC1);
		cv::Mat mask_large = cv::Mat::zeros(bin_hole_img.size(), CV_8UC1);
		cv::Mat mask_donut = cv::Mat::zeros(bin_hole_img.size(), CV_8UC1);

		int cx = 1090;		// �̷��� ��ġ���� �̹����� ���콺 �뼭 ���� ��� ��	������ x	1100
		int cy = 1090;		// �Լ� ���ų� �׷��� ���ϴ� �� �ƴ�					������ y	1100
		int radius_small = 1090 - 710;		// ���� ����ũ			1100-730
		int radius_large = 1090 - 370;		// ū ����ũ				1100-360
		circle(mask_small, Point(cx, cy), radius_small, Scalar(255, 255, 255), LineTypes::FILLED);
		circle(mask_large, Point(cx, cy), radius_large, Scalar(255, 255, 255), LineTypes::FILLED);

		mask_donut = mask_large - mask_small;	// ū ����ũ���� ���� ����ũ�� ���� ���� ����� ����ũ


		//imwrite("mask_small.jpg", mask_small);	// ���� �۵� ��
		/*imwrite("mask_small.bmp", mask_small);
		imwrite("mask_large.bmp", mask_large);
		imwrite("mask_donut.bmp", mask_donut);

		imwrite("input_img.jpg", input_img);
		imwrite("hole_img.jpg", hole_img);
		imwrite("bin_hole_img.jpg", bin_hole_img);*/

		//get sub images
		vector<Mat> vObj_Imgs;			// ������ ����
		for (size_t i = 0; i < 4; i++)
		{
			Mat img_sub = input_img(Rect(0, i * height, width, height));
			// i * height : input_img ���� ������ i��° �κ��� �̹����� ����Ų��.
			Mat obj_sub = img_sub(roi) & mask_donut; //AND ������ Ȱ���ؼ� 0�� �ƴ� �κи� ��� ���ϴ� �κи� ����
			vObj_Imgs.push_back(obj_sub);
			// input_img���� �� ������ ù��°, �ι�°,..,�׹�° �κ� �̹����� vObj_Imgs ���Ϳ� ������� ����

		/*string desc = "";
		desc = std::format("img{}.bmp", i);
		imwrite(desc, vObj_Imgs[i]);
		imwrite("img_sub.bmp", img_sub);*/
		}// 24~76���� ��ó��



		Mat obj2_img = obj_img.clone();
		Mat bin_obj2_img;
		double scratch = 50;	// 50 �̻��̸� ��ũ��ġ
		double clean = 5;
		threshold(obj2_img, bin_obj2_img, scratch, clean, ThresholdTypes::THRESH_BINARY);

		Mat result_mask ;
		result_mask=obj2_img & mask_donut;


		std::vector<std::vector<cv::Point>> contoursresult_mask;
		std::vector<cv::Vec4i> hierarchyresult_mask;
		cv::findContours(result_mask, contoursresult_mask, hierarchyresult_mask, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_NONE);

		Mat result_img;
		cvtColor(result_mask, result_img, ColorConversionCodes::COLOR_GRAY2BGR);
		drawContours(result_img, contoursresult_mask, -1, Scalar(255, 0, 255), 6, LINE_8, hierarchyresult_mask, 4);

		cv::waitKey(0);


		int result = 1;






	

	return 1;

}
