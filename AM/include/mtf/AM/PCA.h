#ifndef MTF_PCA_H
#define MTF_PCA_H

#include "SSDBase.h"

_MTF_BEGIN_NAMESPACE

struct PCAParams : AMParams{
	//! The number of eigen basis
	int n_eigenvec;
	//! batch size for the eigen basis update
	int batch_size;
	//! forgetting factor
	double f_factor;
	//! show all patches in the eigen basis
	bool show_basis;
	//! value constructor
	PCAParams(const AMParams *am_params,
		int _n_eigenvec,
		int _batch_size,
		double _f_factor,
		bool _show_basis);
	//! default/copy constructor
	PCAParams(const PCAParams *params = nullptr);
};

class PCA : public SSDBase{
public:

	typedef PCAParams ParamType;

	PCA(const ParamType *pca_params = nullptr, const int _n_channels = 1);

	// Returns a normalized version of the similarity that lies between 0 and 1
	double getLikelihood() const override;

	// Initialize the features
	void initializeSimilarity() override;

	// Update the similarity
	void updateSimilarity(bool prereq_only = true) override;
	void updateModel(const Matrix2Xd& curr_pts) override;

	// Defined in Appearance Model,
	// It is called before performing the first iteration on a new image
	// to indicate that the image has changed since the last time the
	// update functions were called
	void setFirstIter() override;

	// It is called after performing the last iteration on a new image
	void clearFirstIter() override;

protected:

	ParamType params;

	// *****************************
	//! additional patches B, d x m
	MatrixXd addi_patches;
	//! mean of prev patches, miu_A
	VectorXd mean_prev_patches;
	//! the patch in max similarity of particle each frame
	VectorXd max_patch_eachframe;
	//! number of prev patches, n
	int n_prev_patches;
	//! number of additional patches, m. constant in this case
	unsigned int batch_size;

	//! eigen basis U, initialized to be empty
	MatrixXd U;
	//! Sigma
	VectorXd sigma;
	//! flag to indicate whether the basis U is available
	bool U_available;

	int cv_img_type_uchar, cv_img_type_float;

	// update the basis U and eigen values sigma
	void updateBasis();

	// One step of incremental PCA
	void incrementalPCA();

	// the core algorithm in ivt tracker:
	// sklm(Sequential Karhunen-Loeve Transform with Mean update)
	void sklm(MatrixXd &U, VectorXd &sigma, VectorXd &mu_A, 
		MatrixXd &B, int &n, double ff, int max_n_eig);

	// display the basis and the reconstructed image
	void display_images(const VectorXd &curr_image,
		const VectorXdM &error_image);
	void display_basis();

};

_MTF_END_NAMESPACE

#endif
