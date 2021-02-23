#include "comp_geo_common.hpp"
#include <vector>
#include <list>
namespace lyh_cg {
	void convex_hull_extreme_edges(std::vector<float>& input_x, std::vector<float>& input_y, std::vector<float>& output_x, std::vector<float>& output_y) {
		output_x.clear(); output_y.clear();
		if (input_x.size() < 3)return;
		std::vector<lyh_cg::vec3>point_set; point_set.resize(input_x.size());
		for (int i = 0; i < input_x.size(); i++) {
			point_set[i][0] = input_x[i]; point_set[i][1] = input_y[i]; point_set[i][2] = 1.0f;
		}
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


}