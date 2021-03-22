#include <vector>
#include <Eigen/Dense>
#include<algorithm>
#include <iostream>
#include<iomanip>
Eigen::IOFormat fmt(Eigen::StreamPrecision, 0, ", ", "\n", "", "", "", "");
namespace gmp {
	namespace hw1 {
		void buildSampling(
			const  std::vector<float>& point_x,
			const  std::vector<float>& point_y,
			std::vector<float>& xs,
			std::vector<float>& ys,
			unsigned int sample_num) {
			xs.clear(); ys.clear(); xs.reserve(sample_num); ys.reserve(sample_num);
			auto min_max = std::minmax_element(point_x.begin(), point_x.end());
			auto dt = (*min_max.second - *min_max.first) / float(sample_num);
			for (int i = 0; i < sample_num; i++) {
				float val = *min_max.first + float(i) * dt;
				xs.push_back(val);
			}
			xs.push_back(*min_max.second);
		}
		void lagrange_polynomial_base(
			const std::vector<float>& point_x, 
			const std::vector<float>& point_y, 
			std::vector<float>& xs, 
			std::vector<float>& ys, 
			unsigned int sample_num) {
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
		void lagrange_polynomial(
			const  std::vector<float>& point_x, 
			const  std::vector<float>& point_y, 
			std::vector<float>& xs, 
			std::vector<float>& ys, 
			unsigned int sample_num) {
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
		void gauss_base(
			const  std::vector<float>& point_x,
			const  std::vector<float>& point_y,
			std::vector<float>& xs,
			std::vector<float>& ys,
			unsigned int sample_num,
			double theta = 2.5)
		{
			buildSampling(point_x, point_y, xs, ys, sample_num);
			int length = point_x.size();
		 
			Eigen::MatrixXf A(length, length);
			Eigen::VectorXf b(length);
			Eigen::VectorXf x(length);
			float inv_sigma_sq = 1.0 / pow(theta, 2);
			if (length >= 2)
			{
				for (int i = 0; i < length; i++)
				{
					for (int j = 0; j < length; j++)
						if (i == j) A(i, j) = 1;
						else A(i, j) = exp(-0.5 * pow(point_x[j]- point_x[i], 2) * inv_sigma_sq);
					b(i) = point_y[i];
				}
				x = A.colPivHouseholderQr().solve(b);

				for (auto xcoord : xs)
				{
					float y = 0.0f;
					for (int i = 0; i < length; i++)
						y += x(i) * exp(-0.5 * pow(xcoord - point_x[i], 2) * inv_sigma_sq);
					ys.push_back(y);
				}
			}
			 
		};
		void gauss(
			const  std::vector<float>& point_x,
			const  std::vector<float>& point_y,
			std::vector<float>& xs,
			std::vector<float>& ys,
			unsigned int sample_num,
			double theta = 10.0)
		{
			using namespace Eigen;



			//build sampling
			buildSampling(point_x, point_y, xs, ys, sample_num);
			int length = point_x.size();
			Eigen::MatrixXd A(length, length);
			Eigen::VectorXd Y(length);
			Eigen::MatrixXd b(length,1);
			 
			double _p = -1.0 / (2 * pow(theta, 2.0));
			auto g = [&](double i, double j)->double {
				auto diff_s = (i - j) * (i - j);
				return exp(diff_s * _p);
			};
			/*auto border_x = (point_x[point_x.size() - 1] + point_x[point_x.size() - 2]) / 2.0;
			auto border_y = (point_y[point_y.size()-1] + point_y[point_y.size() - 2]) / 2.0;*/
	
			for (int i = 0; i < length; i++) {
				Y(i) = point_y[i];
				for (int j= 0;j < length ;j++) {
					if (j ==i) {
						A(i, j) = 1;
					}
					else {
						A(i, j) = g(point_x[i], point_x[j]);
					
					}
				
				}
			}


			b = A.colPivHouseholderQr().solve(Y);
			//std::cout << b.format(fmt) << std::endl;
			int a = 666;
			for (auto x : xs) {
				MatrixXd _g( 1,length);
				for (int j = 0; j < length ; j++) {
					_g(0, j) = g(x, point_x[j]);
				}
				//std::cout << _g.format(fmt) << std::endl;
				auto y = _g * b;
				//std::cout << y.format(fmt) << std::endl;
				ys.push_back(y(0, 0));
			}
		};
		void LSM(
			const  std::vector<float>& point_x,
			const  std::vector<float>& point_y,
			std::vector<float>& xs,
			std::vector<float>& ys,
			unsigned int sample_num,
			int highest = 5) {
			using namespace Eigen;
			//build sampling
			buildSampling(point_x, point_y, xs, ys, sample_num);
			int length = point_x.size();
			highest = std::min(length - 2, highest);

			Eigen::MatrixXf B(length, highest + 1);
			Eigen::MatrixXf Y(length,1) ;
			for (int i = 0; i < length; i++) {
				for (int j = 0; j < highest + 1; j++) {
					B(i, j) = pow(point_x[i],(double) j);
				}
				Y(i, 0) = point_y[i];
			}
			auto BT = B.transpose();
			auto BTB = BT * B;
			auto a = BTB.inverse() * BT * Y;
		
		   auto _f = [&](double x)->double {
				Eigen::MatrixXf _x(highest + 1, 1);
				for (int i = 0; i < highest + 1; i++) _x(i, 0) = pow(x, (double)i);
				auto val = _x.transpose() * a;
				return val(0, 0);
			};
		   
		   for (int i = 0; i < xs.size(); i++) {
			   ys.push_back(_f(xs[i]));
		   }
		};
		void LSM_base(
			const  std::vector<float>& point_x,
			const  std::vector<float>& point_y,
			std::vector<float>& xs,
			std::vector<float>& ys,
			unsigned int sample_num,
			 int highest =5)
		{
			using namespace Eigen;



			//build sampling
			buildSampling(point_x, point_y, xs, ys, sample_num);
			int length = point_x.size();
			highest = std::min(length - 2, highest);
		 
			Eigen::MatrixXf A(length, highest + 1);
			Eigen::MatrixXf ATA(highest + 1, highest + 1);
			Eigen::VectorXf b(length);
			Eigen::VectorXf ATb(highest + 1);
			Eigen::VectorXf x(highest + 1);
			if (length > 2)
			{
				for (int i = 0; i < length; i++)
				{
					for (int j = 0; j < highest + 1; j++)
						A(i, j) = pow(point_x[i], j);
					b(i) = point_y[i];
				}
				ATA = A.transpose() * A;
				ATb = A.transpose() * b;
				x = ATA.inverse() * ATb;
			}

			for (auto xcoord : xs)
			{
				float y = x(0);
				for (int i = 1; i < highest + 1; i++)
					y += x(i) * pow(xcoord, i);
				ys.push_back(y);
			}
		};
	}
}