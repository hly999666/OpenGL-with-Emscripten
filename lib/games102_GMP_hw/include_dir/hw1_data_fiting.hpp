#include <vector>
#include <Eigen/Dense>
#include<algorithm>
#include <iostream>
#include<iomanip>
namespace gmp {
	namespace hw1 {
		void lagrange_polynomial_base(const std::vector<float>& point_x, const std::vector<float>& point_y, std::vector<float>& xs, std::vector<float>& ys, unsigned int sample_num) {
			using namespace Eigen;



			//build sampling
			xs.clear(); ys.clear(); xs.reserve(sample_num); ys.reserve(sample_num);
			auto min_max = std::minmax_element(point_x.begin(), point_x.end());
			auto dt = (*min_max.second - *min_max.first) / float(sample_num);
			for (int i = 0; i < sample_num; i++) {
				float val = *min_max.first + float(i) * dt;
				xs.push_back(val);
			}
			xs.push_back(*min_max.second);
			int length = point_x.size();

		 
			if (length >= 2)
			{
				for (auto x : xs)
				{
					float y = 0.0f;
					for (int i = 0; i < length; i++)
					{
						float temp = point_y[i];
						for (int j = 0; j < length; j++)
						{
							if (i != j)
							{
								temp *= x - point_x[j];
								temp *= 1 / (point_x[i] - point_x[j]);
							}
						}
						y += temp;
					}
					ys.push_back(y);
				}
			}
 
		}
		void lagrange_polynomial(const  std::vector<float>& point_x, const  std::vector<float>& point_y, std::vector<float>& xs, std::vector<float>& ys,unsigned int sample_num) {
			using namespace Eigen;


		 
	     //build sampling
			 xs.clear(); ys.clear(); xs.reserve(sample_num); ys.reserve(sample_num);
			 auto min_max = std::minmax_element(point_x.begin(), point_x.end());
			 auto dt = (*min_max.second - *min_max.first)/float(sample_num);
			 for (int i = 0; i < sample_num; i++) {
				 float val = *min_max.first +float(i) * dt;
				 xs.push_back(val);
			 }
			 xs.push_back(*min_max.second);
			 int length = point_x.size();
			 
		 {
			
				 MatrixXd y_mat(length, 1); MatrixXd x_mat(length, 1);
				 for (int i = 0; i < length; i++) {
					 x_mat(i, 0) = point_x[i];
					 y_mat(i, 0) = point_y[i];
				 }


				 MatrixXd other_mat(length, length - 1);
				 for (int i = 0; i < length; i++) {
					 int count = 0;
					 for (int j = 0; j < length; j++) {
						 if (i != j) {
							 other_mat(i, count) = point_x[j];
							 count++;
						 }
					 }
				 }
				 auto diff_x_mat = x_mat.replicate(1, length - 1) - other_mat;
				 auto prod_denominator = diff_x_mat.rowwise().prod();
			 
				 auto _L = [&](int i, float x)->double {
					 auto xm = MatrixXd::Ones(1, length - 1) * x - other_mat.row(i);
					 //std::cout << xm.format(OctaveFmt) << std::endl;
					 return xm.rowwise().prod()(0, 0) / prod_denominator(i, 0);
				 };
             
				 for (auto x_sample : xs) {
					 MatrixXd L_i(length, 1);
					 for (int i = 0; i < length; i++) {
						 L_i(i, 0) = _L(i, x_sample);
					 }
					 auto res_mat = y_mat.transpose() * L_i;
					 ys.push_back(res_mat(0, 0));
				 }
			 }
		 
		}
		
	}
}