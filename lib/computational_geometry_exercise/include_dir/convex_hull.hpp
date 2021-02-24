#include "comp_geo_common.hpp"
#include <vector>
#include <list>
#include <algorithm>
#include <queue> 
namespace lyh_cg {
	void convex_hull_extreme_edges(std::vector<float>& input_x, std::vector<float>& input_y, std::vector<float>& output_x, std::vector<float>& output_y) {
		output_x.clear(); output_y.clear();
		if (input_x.size() < 3)return;
		std::vector<lyh_cg::vec3>point_set; point_set.resize(input_x.size());
		for (int i = 0; i < input_x.size(); i++) {
			point_set[i][0] = input_x[i]; point_set[i][1] = input_y[i]; point_set[i][2] = 1.0f;
		}
		//
		std::vector<int>next; next.resize(input_x.size());
		 
		for (auto& i : next)i = -1;
		for (int i = 0; i < point_set.size(); i++) {
			for (int j = 0; j< point_set.size();j++) {
				if (i == j)continue;
				bool flag = true;
				for (int k = 0;k < point_set.size();k++) {
					if (i==k||j==k)continue;
					if (to_left(point_set[i], point_set[j], point_set[k]) == false) {
						flag = false; break;
					}
				
				}
				if (flag)next[i] = j;
			}
		
		}
		int first = -1;
		for (int i = 0; i < point_set.size(); i++) {
			if (next[i] != -1) {
				first = i; break;
			}
		}
		if (first == -1)return;
		output_x.push_back(point_set[first].x());
		output_y.push_back(point_set[first].y());
		int now = next[first];
		while (now != first) {
			if (now == -1)break;
			if (output_x.size() == point_set.size())break;
			output_x.push_back(point_set[now].x());
			output_y.push_back(point_set[now].y());
			now = next[now];
		
		}
		output_x.push_back(point_set[first].x());
		output_y.push_back(point_set[first].y());
	}
	void convex_hull_Granham_Scan(std::vector<float>& input_x, std::vector<float>& input_y, std::vector<float>& output_x, std::vector<float>& output_y) {
		output_x.clear(); output_y.clear();
		if (input_x.size() < 3)return;
		std::vector<lyh_cg::vec3>point_set; point_set.resize(input_x.size());
		for (int i = 0; i < input_x.size(); i++) {
			point_set[i][0] = input_x[i]; point_set[i][1] = input_y[i]; point_set[i][2] = 1.0f;
		};
		//find llt
		auto comp_llt = [](const vec3& v1, const vec3& v2)->bool {
			if (v1.y() < v2.y())return true;
			if (v1.y() > v2.y())return false;
			if (v1.x() < v2.x())return true;
			if (v1.x() > v2.x())return false;
			return false;
		};
		auto llt_itr =   std::min_element(point_set.begin(), point_set.end(), comp_llt);
		const auto const_llt = *llt_itr;
		auto back_itr = point_set.end() - 1;
		std::swap(*llt_itr, *back_itr);
		point_set.pop_back();

		//pre-sorting
		auto comp_to_left_llt = [&const_llt](const vec3& v1, const vec3& v2)->bool {
            if (lyh_cg::to_left(const_llt, v1, v2))return false;
			else return true;
		};
		std::sort(point_set.begin(), point_set.end(), comp_to_left_llt);
		std::vector<vec3>current_result;  
		current_result.push_back(const_llt); current_result.push_back(point_set.back());
		point_set.pop_back();
		while (!point_set.empty()) {
			auto back = point_set.back(); point_set.pop_back();
			while (!lyh_cg::to_left(*(current_result.end() - 2), *(current_result.end() - 1), back)) {
				current_result.pop_back();
			}
			current_result.push_back(back);
		
		}
		//outpur
		for (auto& v : current_result) {
			output_x.push_back(v.x());
			output_y.push_back(v.y());
		}
		output_x.push_back(current_result[0].x());
		output_y.push_back(current_result[0].y());
	}

}