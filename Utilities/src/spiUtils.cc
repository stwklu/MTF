#include "mtf/Utilities/spiUtils.h"
#include "mtf/Utilities/imgUtils.h"
#include "mtf/Utilities/miscUtils.h"
#include "mtf/Utilities/excpUtils.h"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

#define SPI_PIX_DIFF_THRESH 10
#define SPI_GRAD_THRESH 0.005
#define SPI_GRAD_USE_UNION false
#define SPI_GFTT_MAX_CORNERS
#define SPI_GFTT_QUALITY_LEVEL
#define SPI_GFTT_MIN_DISTANCE
#define SPI_GFTT_BLOCK_SIZE 3
#define SPI_GFTT_USE_HARRIS_DETECTOR false
#define SPI_GFTT_K 0.04
#define SPI_GFTT_USE_UNION false
#define SPI_GFTT_NEIGH_OFFSET 0

#define parse_spi_param(name, data_type, param, spi_type)\
try{\
	name = boost::any_cast<data_type>(param);\
} catch(const boost::bad_any_cast &){\
	throw InvalidArgument(cv::format("spi::%s :: Invalid parameter type provided for %s", #spi_type, #name));\
}

_MTF_BEGIN_NAMESPACE
namespace utils{
	namespace spi{
		PixDiff::PixDiff(VectorXb &_mask, const ParamsType &params) :
			Base(_mask), pix_diff_thresh(SPI_PIX_DIFF_THRESH){
			if(!params.empty()){
				if(params.size() != 1){
					throw InvalidArgument("spi::PixDiff needs exactly one input argument");
				}

				parse_spi_param(pix_diff_thresh, double, params[0], PixDiff);
			}
			printf("\n");
			printf("Using Pixel Difference SPI model with:\n");
			printf("pix_diff_thresh: %f\n", pix_diff_thresh);
			printf("\n");
		}
		void PixDiff::initialize(const PixValT &init_pix_vals){
			max_pix_diff = init_pix_vals.maxCoeff() - init_pix_vals.minCoeff();
			rel_pix_diff.resize(init_pix_vals.size());
			mask.fill(true);			
		}
		void PixDiff::update(const PixValT &init_pix_vals, const PixValT &curr_pix_vals){
			//rel_pix_diff = (init_pix_vals - curr_pix_vals) / max_pix_diff;
			mask = (init_pix_vals - curr_pix_vals).cwiseAbs().array() < pix_diff_thresh*max_pix_diff;
		}
		Gradient::Gradient(VectorXb &_mask, const ParamsType &params) :
			Base(_mask), grad_thresh(SPI_GRAD_THRESH),
			use_union(SPI_GRAD_USE_UNION){
			if(!params.empty()){
				if(params.size() != 2){
					throw InvalidArgument("spi::Gradient needs exactly 2 input arguments");
				}
				parse_spi_param(grad_thresh, double, params[0], Gradient);
				parse_spi_param(use_union, bool, params[1], Gradient);
			}
			printf("\n");
			printf("Using Gradient Magnitude SPI model with:\n");
			printf("grad_thresh: %f\n", grad_thresh);
			printf("use_union: %d\n", use_union);
			printf("\n");
		}
		void Gradient::initialize(const PixGradT &init_pix_grad){
			n_pix = init_pix_grad.rows();
			init_mask = init_pix_grad.rowwise().norm().array() > grad_thresh;
			mask.fill(true);
		}
		void Gradient::update(const PixGradT &curr_pix_grad){			
			assert(curr_pix_grad.rows() == n_pix);
			pix_grad_norm = curr_pix_grad.rowwise().norm();
			// printMatrixToFile(pix_grad_norm, "pix_grad_norm", "log/pix_grad_norm.txt");
			mask = pix_grad_norm.array() > grad_thresh;
			if(use_union){
				for(unsigned int pix_id = 0; pix_id < n_pix; ++pix_id){
					mask[pix_id] = mask[pix_id] || init_mask[pix_id];
				}
			}
		}
		GFTT::GFTT(VectorXb &_mask, const ParamsType &params) :
			Base(_mask), max_corners(SPI_GFTT_MAX_CORNERS),
			quality_level(SPI_GFTT_QUALITY_LEVEL), min_distance(SPI_GFTT_MIN_DISTANCE),
			block_size(SPI_GFTT_BLOCK_SIZE),
			use_harris_detector(SPI_GFTT_USE_HARRIS_DETECTOR), k(SPI_GFTT_K),
			use_union(SPI_GFTT_USE_UNION), neigh_offset(SPI_GFTT_NEIGH_OFFSET){
			if(!params.empty()){
				if(params.size() != 8){
					throw InvalidArgument("spi::GFTT needs exactly 8 input arguments");
				}
				parse_spi_param(max_corners, int, params[0], GFTT);
				parse_spi_param(quality_level, double, params[1], GFTT);
				parse_spi_param(min_distance, double, params[2], GFTT);
				parse_spi_param(block_size, int, params[3], GFTT);
				parse_spi_param(use_harris_detector, bool, params[4], GFTT);
				parse_spi_param(k, double, params[5], GFTT);
				parse_spi_param(use_union, bool, params[6], GFTT);
				parse_spi_param(neigh_offset, int, params[7], GFTT);
			}
			printf("\n");
			printf("Using Good Features To Track SPI model with:\n");
			printf("max_corners: %d\n", max_corners);
			printf("quality_level: %f\n", quality_level);
			printf("min_distance: %f\n", min_distance);
			printf("block_size: %d\n", block_size);
			printf("use_harris_detector: %d\n", use_harris_detector);
			printf("k: %f\n", k);
			printf("use_union: %d\n", use_union);
			printf("neigh_offset: %d\n", neigh_offset);
			printf("\n");
		}
		void GFTT::initialize(const PixValT &init_pix_vals, unsigned int _resx, unsigned int _resy){
			resx = _resx;
			resy = _resy;
			n_pix = resx*resy;
			curr_patch_32f.create(resy, resx, CV_32FC1);
			init_mask.resize(n_pix);
			getMask(init_pix_vals, init_mask);
			mask.fill(true);
		}
		void GFTT::update(const PixValT &curr_pix_vals){
			assert(curr_pix_vals.size() == n_pix);
			getMask(curr_pix_vals, mask);
			if(use_union){
				for(unsigned int pix_id = 0; pix_id < n_pix; ++pix_id){
					mask[pix_id] = mask[pix_id] || init_mask[pix_id];
				}
			}
		}
		void GFTT::getMask(const PixValT &curr_pix_vals, VectorXb &_mask){
			assert(curr_pix_vals.size() == resx*resy);
			cv::Mat(resy, resx, CV_64FC1, const_cast<double*>(curr_pix_vals.data())).convertTo(
				curr_patch_32f, curr_patch_32f.type());
			std::vector< cv::Point2f > good_locations_vec;
			cv::goodFeaturesToTrack(curr_patch_32f, good_locations_vec, max_corners, quality_level, min_distance,
				cv::noArray(), block_size, use_harris_detector, k);
			//std::cout << "\n" << good_locations << "\n";
			//printf("good_locations type: %d :: %s\n", good_locations.type(), getType(good_locations));
			_mask.setZero();
			//int n_good_features = good_locations.rows;
			int n_good_features = good_locations_vec.size();
			printf("n_good_features: %d\n", n_good_features);
			for(int feat_id = 0; feat_id < n_good_features; ++feat_id){
				//cv::Vec2f pix_loc = good_locations.at<cv::Vec2f>(feat_id);
				//int feat_x = static_cast<int>(pix_loc[0]);
				//int feat_y = static_cast<int>(pix_loc[1]);
				//printf("x: %d\t y=%d\n", feat_x, feat_y);
				unsigned int feat_x = static_cast<unsigned int>(good_locations_vec[feat_id].x);
				unsigned int feat_y = static_cast<unsigned int>(good_locations_vec[feat_id].y);
				for(unsigned int x = feat_x - neigh_offset; x <= feat_x + neigh_offset; ++x){
					if(x < 0 || x >= resx){	continue; }
					for(unsigned int y = feat_y - neigh_offset; y <= feat_y + neigh_offset; ++y){
						if(y < 0 || y >= resy){	continue; }
						int id = y*resx + x;
						_mask[id] = true;
					}
				}
			}
		}
	}

	double getMean(const bool *spi_mask, const VectorXd &vec,
		int vec_size){
		double mean = 0;
		int valid_vec_size = 0;
		for(int vec_id = 0; vec_id < vec_size; vec_id++){
			if(!spi_mask[vec_id]){ continue; }
			++valid_vec_size;
			mean += vec[vec_id];
		}
		mean /= valid_vec_size;
		return mean;
	}
	void getMean(RowVectorXd &mean_vec, const bool *spi_mask,
		const MatrixXd &mat, int n_rows){
		assert(mat.rows() == n_rows && mat.cols() == mean_vec.size());
		mean_vec.setZero();
		int valid_vec_size = 0;
		for(int row_id = 0; row_id < n_rows; row_id++){
			if(!spi_mask[row_id]){ continue; }
			++valid_vec_size;
			mean_vec += mat.row(row_id);
		}
		mean_vec /= valid_vec_size;
	}
	void getMean(VectorXd &mean_vec, const bool *spi_mask,
		const MatrixXd &mat, int n_cols){
		assert(mat.cols() == n_cols && mat.rows() == mean_vec.size());

		mean_vec.setZero();
		int valid_vec_size = 0;
		for(int col_id = 0; col_id < n_cols; col_id++){
			if(!spi_mask[col_id]){ continue; }
			++valid_vec_size;
			mean_vec += mat.col(col_id);
		}
		mean_vec /= valid_vec_size;
	}
	void getProd(RowVectorXd &df_dp, const bool *spi_mask,
		const RowVectorXd &df_dI, const MatrixXd &dI_dp,
		int n_pix, int n_channels){
		assert(dI_dp.rows() == n_pix*n_channels && dI_dp.rows() == df_dp.size());
		df_dp.setZero();
		int ch_pix_id = 0;
		for(int pix_id = 0; pix_id < n_pix; pix_id++){
			if(!spi_mask[pix_id]){ ch_pix_id += n_channels;  continue; }
			for(int channel_id = 0; channel_id < n_channels; ++channel_id){
				df_dp += df_dI[ch_pix_id] * dI_dp.row(ch_pix_id);
				++ch_pix_id;
			}
		}
	}
	void getDiffOfProd(RowVectorXd &df_dp, const bool *spi_mask,
		const RowVectorXd &df_dIt, const MatrixXd &dIt_dp,
		const RowVectorXd &df_dI0, const MatrixXd &dI0_dp,
		int n_pix, int n_channels){
		assert(dIt_dp.rows() == n_pix*n_channels && dIt_dp.rows() == df_dp.size());
		assert(dI0_dp.rows() == n_pix*n_channels && dI0_dp.rows() == df_dp.size());
		df_dp.setZero();
		int ch_pix_id = 0;
		//printMatrix(df_dIt, "df_dIt");
		//printMatrix(dIt_dp, "dIt_dp");
		//printMatrix(df_dI0, "df_dI0");
		//printMatrix(dI0_dp, "dI0_dp");
		//printMatrix(Map<const VectorXb>(spi_mask, n_pix), "spi_mask", "%d");
		for(int pix_id = 0; pix_id < n_pix; ++pix_id){
			if(!spi_mask[pix_id]){ ch_pix_id += n_channels;  continue; }
			for(int channel_id = 0; channel_id < n_channels; ++channel_id){
				df_dp += df_dIt[ch_pix_id] * dIt_dp.row(ch_pix_id) -
					df_dI0[ch_pix_id] * dI0_dp.row(ch_pix_id);
				++ch_pix_id;
			}
		}
		//printMatrix(df_dp, "df_dp");
	}
	void expandMask(bool *out_mask, const bool *in_mask, int res_ratio_x,
		int res_ratio_y, int in_resx, int in_resy, int out_resx, int out_resy){
		assert(out_resx == in_resx*res_ratio_x);
		assert(out_resy == in_resy*res_ratio_y);
		for(int y_id = 0; y_id < in_resy; y_id++){
			int out_start_y = y_id*res_ratio_y;
			for(int x_id = 0; x_id < in_resx; x_id++){
				int in_id = y_id*in_resx + x_id;
				int out_start_x = x_id*res_ratio_x;
				for(int y = 0; y < res_ratio_y; y++){
					for(int x = 0; x < res_ratio_x; x++){
						int out_id = (out_start_y + y)*out_resx + (out_start_x + x);
						out_mask[out_id] = in_mask[in_id];
					}
				}
			}
		}
	}

}
_MTF_END_NAMESPACE

