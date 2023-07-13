// 복붙

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
	//vector<string> filelist;			// 라이브러리 개념의 vector
	//for (const auto& entry : fs::directory_iterator(fileDir)) {		// & : 래퍼런스 개념이다. 주소가 아니다
	//	std::cout << entry.path().string() << std::endl;
	//	filelist.push_back(entry.path().string());	// fileDir에 있는 파일을 하나씩 filelist라는 벡터에 넣는다.(push_back)
	//}

	//for (const std::string& filePath : filelist) {	// 래퍼런스 : 메모리에 있는 변수여야됨
	//	std::cout << filePath << std::endl;
		Mat input_img = cv::imread(fileDir, cv::ImreadModes::IMREAD_ANYCOLOR);





		//to do (기본적인 정보 얻어옴)
		int width = input_img.cols;
		int height = input_img.rows / 4;
		int channel = input_img.channels();
		//get roi	roi : 특정 영역에 대해서만 영상 처리를 수행할때 설정하는 영역	(사용자가 임의로 정해줌)
		// 사각형이 아닌 다른 모양으로 ROI 설정 시 마스크 연산을 응용
		Rect roi = Rect(1200, 400, 3400 - 1200, 2600 - 400);
		// (1200, 400) 좌표부터 (3400 - 1200) x (2600 - 400) 크기만큼의 사각형 부분 영상을 추출해서 roi에 저장
		// 3400은 원하는 이미지 부분의 x의 끝값, 2600은 원하는 이미지 부분의 y값

		Mat img_1 = input_img(Rect(0, 0, width, height));
		Mat obj_img = img_1(roi).clone();
		Mat hole_img = ~obj_img;
		Mat bin_hole_img;
		double thres_min = 200;	// 첫번째 포커스  원(Circle), 색의 값이 200이다.
		double thres_max = 255;
		threshold(hole_img, bin_hole_img, thres_min, thres_max, ThresholdTypes::THRESH_BINARY);
		// 200 아래면 0이 되고, 200이상이면 255로 바꾸고 이미지 전체를 이진화한다.

//erosion	// 침식 연산
		erode(bin_hole_img, bin_hole_img, Mat::ones(Size(3, 3), CV_8UC1), Point(-1, -1), 2);
		// 윤곽선 그리기
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

		int cx = 1090;		// 이러한 수치들은 이미지에 마우스 대서 직접 재야 함	센터의 x	1100
		int cy = 1090;		// 함수 쓰거나 그렇게 구하는 거 아님					센터의 y	1100
		int radius_small = 1090 - 710;		// 작은 마스크			1100-730
		int radius_large = 1090 - 370;		// 큰 마스크				1100-360
		circle(mask_small, Point(cx, cy), radius_small, Scalar(255, 255, 255), LineTypes::FILLED);
		circle(mask_large, Point(cx, cy), radius_large, Scalar(255, 255, 255), LineTypes::FILLED);

		mask_donut = mask_large - mask_small;	// 큰 마스크에서 작은 마스크를 빼면 도넛 모양의 마스크


		//imwrite("mask_small.jpg", mask_small);	// 정상 작동 됨
		/*imwrite("mask_small.bmp", mask_small);
		imwrite("mask_large.bmp", mask_large);
		imwrite("mask_donut.bmp", mask_donut);

		imwrite("input_img.jpg", input_img);
		imwrite("hole_img.jpg", hole_img);
		imwrite("bin_hole_img.jpg", bin_hole_img);*/

		//get sub images
		vector<Mat> vObj_Imgs;			// 수학적 벡터
		for (size_t i = 0; i < 4; i++)
		{
			Mat img_sub = input_img(Rect(0, i * height, width, height));
			// i * height : input_img 파일 내에서 i번째 부분의 이미지를 가리킨다.
			Mat obj_sub = img_sub(roi) & mask_donut; //AND 연산을 활용해서 0이 아닌 부분만 살려 원하는 부분만 추출
			vObj_Imgs.push_back(obj_sub);
			// input_img에서 맨 위부터 첫번째, 두번째,..,네번째 부분 이미지를 vObj_Imgs 벡터에 순서대로 저장

		/*string desc = "";
		desc = std::format("img{}.bmp", i);
		imwrite(desc, vObj_Imgs[i]);
		imwrite("img_sub.bmp", img_sub);*/
		}// 24~76까지 전처리



		Mat obj2_img = obj_img.clone();
		Mat bin_obj2_img;
		double scratch = 50;	// 50 이상이면 스크래치
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
