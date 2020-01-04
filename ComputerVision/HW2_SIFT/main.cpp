#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <cmath>

#include <string>
#include <fstream>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/xfeatures2d/nonfree.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace std;
using namespace cv;

float dist(Mat_<float> A, Mat_<float> B) {
	Mat_<float> diff = A - B;
	return sqrt(diff.dot(diff));
}

class KDTree {

private:
	struct KDNode {
		Mat_<float> data;
		int label;
		int splitdim;
		KDNode* lchild;
		KDNode* rchild;
	};

	KDNode* build(Mat_<float> data, Mat_<int> label) {

		KDNode* node = new KDNode[1];

		if (data.rows == 0) {
			node = NULL;
		}
		else if (data.rows == 1) {
			node->data = data;
			node->label = label.at<int>(0);
			node->splitdim = -1;
			node->lchild = NULL;
			node->rchild = NULL;
		}
		else {
			Mat mean, stddev;

			int spdim = 0;
			meanStdDev(data.col(0), mean, stddev);
			float maxstddev = stddev.at<double>(0);

			for (int i = 1; i < data.cols; i++) {
				meanStdDev(data.col(i), mean, stddev);
				if (stddev.at<double>(0) > maxstddev) {
					maxstddev = stddev.at<double>(0);
					spdim = i;
				}
			}

			Mat_<int> sortidx;
			sortIdx(data.col(spdim), sortidx, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

			node->data = data.row(sortidx.at<int>(data.rows / 2));
			node->label = label.at<int>(sortidx.at<int>(data.rows / 2));
			node->splitdim = spdim;

			Mat_<float> ldata;
			Mat_<int> llabel;
			for (int i = 0; i < int(data.rows / 2); i++) {
				ldata.push_back(data.row(sortidx.at<int>(i)));
				llabel.push_back(label.at<int>(sortidx.at<int>(i)));
			}

			Mat_<float> rdata;
			Mat_<int> rlabel;
			for (int i = int(data.rows / 2) + 1; i < data.rows; i++) {
				if (data.at<float>(sortidx.at<int>(i), spdim) != node->data.at<float>(0, spdim)) {
					rdata.push_back(data.row(sortidx.at<int>(i)));
					rlabel.push_back(label.at<int>(sortidx.at<int>(i)));
				}
				else {
					ldata.push_back(data.row(sortidx.at<int>(i)));
					llabel.push_back(label.at<int>(sortidx.at<int>(i)));
				}
			}
			node->lchild = build(ldata, llabel);
			node->rchild = build(rdata, rlabel);
		}

		return node;
	}

	KDNode* root;

public:
	KDTree(Mat_<float> data, Mat_<int> label) {
		root = new KDNode[1];
		root = build(data, label);
	}

	int findNearest(Mat_<float> target) {
		KDNode* nearest;

		stack<KDNode*> searchpath;
		KDNode* searchnode = root;
		while (searchnode != NULL) {
			searchpath.push(searchnode);

			int spdim = searchnode->splitdim;
			if (spdim != -1) {
				if (target.at<float>(0, spdim) <= searchnode->data.at<float>(0, spdim))
					searchnode = searchnode->lchild;
				else
					searchnode = searchnode->rchild;
			}
			else
				break;
		}

		nearest = searchpath.top();
		searchpath.pop();

		while (!searchpath.empty()) {

			KDNode* backnode = searchpath.top();
			searchpath.pop();

			int spdim = backnode->splitdim;
			if (spdim != -1) {
				if (target.at<float>(0, spdim) - backnode->data.at<float>(0, spdim) < dist(nearest->data, target)) {
					if (target.at<float>(0, spdim) <= backnode->data.at<float>(0, spdim)) {
						if (backnode->rchild != NULL)
							searchpath.push(backnode->rchild);
					}
					else {
						if (backnode->lchild != NULL)
							searchpath.push(backnode->lchild);
					}
				}
			}

			if (dist(backnode->data, target) < dist(nearest->data, target)) {
				nearest = backnode;
			}
		}

		//cout << "nearest(" << target << ") = " << nearest->data << endl;
		return nearest->label;
	}

	void levelorderTraversal() {
		queue<KDNode*> q;
		q.push(root);
		while (!q.empty()) {
			KDNode* p = q.front();
			q.pop();
			cout << "(" << p->label << "):" << p->data << " @ " << p->splitdim << endl;

			if (p->lchild)
				q.push(p->lchild);
			if (p->rchild)
				q.push(p->rchild);
		}
	}
};

int main()
{
	Mat img1 = imread("..\\data\\object_11.bmp");
	Mat img2 = imread("..\\data\\target.bmp");

	// ===== SIFT
	Ptr<Feature2D> f2d = xfeatures2d::SIFT::create();

	vector<KeyPoint> keypoints1, keypoints2;
	f2d->detect(img1, keypoints1);
	f2d->detect(img2, keypoints2);

	Mat descriptors1, descriptors2;
	f2d->compute(img1, keypoints1, descriptors1);
	f2d->compute(img2, keypoints2, descriptors2);

	Mat feat1, feat2;
	drawKeypoints(img1, keypoints1, feat1);
	drawKeypoints(img2, keypoints2, feat2);

	imwrite("..\\data\\Object_KeyPoint.jpg", feat1);
	imwrite("..\\data\\Target_KeyPoint.jpg", feat2);
	//imshow("Object_KeyPoint.jpg", feat1);
	//imshow("Target_KeyPoint.jpg", feat2);
	cout << "keypoint1 = " << keypoints1.size() << endl;
	cout << "keypoint2 = " << keypoints2.size() << endl;
	cout << "--- SIFT done" << endl;

	// ===== kNN
	// bruteforce kNN
	// distance matrix
	Mat_<float> disMat(keypoints1.size(), keypoints2.size());
	for (int i = 0; i < keypoints1.size(); i++) {
		for (int j = 0; j < keypoints2.size(); j++) {
			disMat.at<float>(i, j) = dist(descriptors1.row(i), descriptors2.row(j));
		}
	}
	cout << "--- distance matrix done" << endl;

	const int knnK = 3;
	Mat_<int> knnidxMat(keypoints1.size(), knnK, -1);
	Mat_<KeyPoint> knnkeyptsMat(keypoints1.size(), knnK);

	for (int i = 0; i < keypoints1.size(); i++) {
		for (int j = 0; j < keypoints2.size(); j++) {
			for (int k = 0; k < knnK; k++) {
				int keyidx = knnidxMat.at<int>(i, k);
				if (keyidx >= 0) {
					if (disMat.at<float>(i, j) < disMat.at<float>(i, keyidx)) {
						for (int tmpk = knnK - 1; tmpk > k; tmpk--)
							knnidxMat.at<int>(i, tmpk) = knnidxMat.at<int>(i, tmpk - 1);
						knnidxMat.at<int>(i, k) = j;
						break;
					}
				}
				else {
					knnidxMat.at<int>(i, k) = j;
					break;
				}
			}
		}

		for (int k = 0; k < knnK; k++) {
			knnkeyptsMat.at<KeyPoint>(i, k) = keypoints2[knnidxMat.at<int>(i, k)];
		}
	}
	cout << "--- kNN done" << endl;

	/*
	// k-d-tree 1NN
	// k-d-tree
	Mat_<int> indexlabel(keypoints2.size(), 1);
	for (int i = 0; i < keypoints2.size(); i++)
		indexlabel.at<int>(i) = i;
	KDTree kdTree(descriptors2, indexlabel);
	//kdTree.levelorderTraversal();

	for (int i = 0; i < keypoints1.size(); i++) {
		knnidxMat.at<int>(i) = kdTree.findNearest(descriptors1.row(i));
		knnkeyptsMat.at<KeyPoint>(i) = keypoints2[knnidxMat.at<int>(i)];
	}
	cout << "--- k-d-tree kNN done" << endl;
	*/


	// ===== RANSAC
	const int ransacN = 4;
	const float errth = 30;
	const int maxinlierth = 0.4*keypoints1.size();
	const int itthmin = 5000 / pow(knnK, ransacN);
	const int itthmax = 10000 / pow(knnK, ransacN);

	const int runth = 1;
	//int max[runth];
	for (int r = 0; r < runth; r++) {

		Mat_<float> mappingMat(3, 3);
		int maxinlier = 0;
		int it = 0;
		RNG rng(getTickCount());

		while (it < itthmin || (maxinlier < maxinlierth && it < itthmax)) {
			it++;

			Mat_<int> x1idxMat(ransacN, 1);
			Mat_<KeyPoint> x1keyptsMat(ransacN, 1);
			for (int i = 0; i < ransacN; i++) {
				x1idxMat.at<int>(i) = rng.uniform(0, keypoints1.size());
				for (int j = 0; j < i; j++) {
					if (x1idxMat.at<int>(i) == x1idxMat.at<int>(j)) {
						x1idxMat.at<int>(i) = rng.uniform(0, keypoints1.size());
						j = -1;
					}
				}
				x1keyptsMat.at<KeyPoint>(i) = keypoints1[x1idxMat.at<int>(i)];
			}

            // persepective mapping
            // x2 = Hx1
            // x1=(xi,yi) -> x2=(ui,vi) for i=0,1,2,3
			for (int p0 = 0; p0 < knnK; p0++) {
				for (int p1 = 0; p1 < knnK; p1++) {
					for (int p2 = 0; p2 < knnK; p2++) {
						for (int p3 = 0; p3 < knnK; p3++) {

							Mat_<int> x2idxMat(ransacN, 1);
							x2idxMat.at<int>(0) = knnidxMat.at<int>(x1idxMat.at<int>(0), p0);
							x2idxMat.at<int>(1) = knnidxMat.at<int>(x1idxMat.at<int>(1), p1);
							x2idxMat.at<int>(2) = knnidxMat.at<int>(x1idxMat.at<int>(2), p2);
							x2idxMat.at<int>(3) = knnidxMat.at<int>(x1idxMat.at<int>(3), p3);

							Mat_<KeyPoint> x2keyptsMat(ransacN, 1);
							x2keyptsMat.at<KeyPoint>(0) = keypoints2[x2idxMat.at<int>(0)];
							x2keyptsMat.at<KeyPoint>(1) = keypoints2[x2idxMat.at<int>(1)];
							x2keyptsMat.at<KeyPoint>(2) = keypoints2[x2idxMat.at<int>(2)];
							x2keyptsMat.at<KeyPoint>(3) = keypoints2[x2idxMat.at<int>(3)];

							// solve A[8*9] * H[9*1] = 0[8*1]
							//                                         | h00 |
							// | x0 y0  1  0  0  0 -u0*x0 -u0*y0 -u0 | | h01 |   | 0 |
							// |  0  0  0 x0 y0  1 -v0*x0 -v0*y0 -v0 | | h02 |   | 0 |
							// | x1 y1  1  0  0  0 -u1*x1 -u1*y1 -u1 | | h10 |   | 0 |
							// |  0  0  0 x1 y1  1 -v1*x1 -v1*y1 -v1 | | h11 | = | 0 |
							// | x2 y2  1  0  0  0 -u2*x2 -u2*y2 -u2 | | h12 |   | 0 |
							// |  0  0  0 x2 y2  1 -v2*x2 -v2*y2 -v2 | | h20 |   | 0 |
							// | x3 y3  1  0  0  0 -u3*x3 -u3*y3 -u3 | | h21 |   | 0 |
							// |  0  0  0 x3 y3  1 -v3*x3 -v3*y3 -v3 | | h22 |   | 0 |

							Mat_<float> A(2 * ransacN, 9);
							for (int i = 0; i < ransacN; i++) {
								for (int j = 0; j < 2; j++) {
									KeyPoint x1 = x1keyptsMat.at<KeyPoint>(i);
									KeyPoint x2 = x2keyptsMat.at<KeyPoint>(i);
									if (j == 0) {
										A.at<float>(2 * i + j, 0) = x1.pt.x;
										A.at<float>(2 * i + j, 1) = x1.pt.y;
										A.at<float>(2 * i + j, 2) = 1;
										A.at<float>(2 * i + j, 3) = 0;
										A.at<float>(2 * i + j, 4) = 0;
										A.at<float>(2 * i + j, 5) = 0;
										A.at<float>(2 * i + j, 6) = -1 * x2.pt.x*x1.pt.x;
										A.at<float>(2 * i + j, 7) = -1 * x2.pt.x*x1.pt.y;
										A.at<float>(2 * i + j, 8) = -1 * x2.pt.x;
									}
									else {
										A.at<float>(2 * i + j, 0) = 0;
										A.at<float>(2 * i + j, 1) = 0;
										A.at<float>(2 * i + j, 2) = 0;
										A.at<float>(2 * i + j, 3) = x1.pt.x;
										A.at<float>(2 * i + j, 4) = x1.pt.y;
										A.at<float>(2 * i + j, 5) = 1;
										A.at<float>(2 * i + j, 6) = -1 * x2.pt.y*x1.pt.x;
										A.at<float>(2 * i + j, 7) = -1 * x2.pt.y*x1.pt.y;
										A.at<float>(2 * i + j, 8) = -1 * x2.pt.y;
									}
								}
							}

							// solution 1.1
							// SVD::solveZ(A,H)
							Mat_<float> H(3,3);
							SVD::solveZ(A, H);
							H = H.reshape(1, 3);
							//cout << "SVD::solveZ(A,H) - (" << p0 << "," << p1 << "," << p2 << "," << p3 << ")" << endl << H << endl << endl;

							/*
							// solution 1.2
							// eigen(A.t()*A) the eigenvector with smallest eigenvalue
							Mat_<float> eigenvalue;
							Mat_<float> eigenvector;
							eigen(A.t()*A, eigenvalue, eigenvector);
							H = eigenvector.row(eigenvector.size().height).reshape(1, 3);
							//cout << "eigen(A.t()*A) the eigenvector with smallest eigenvalue" << endl << H << endl << endl;
							*/

							// solve B[8*8] * H[8*1] = C[8*1]
							// h22 = 1
							// | x0 y0  1  0  0  0 -u0*x0 -u0*y0 | | h00 |   | u0 |
							// |  0  0  0 x0 y0  1 -v0*x0 -v0*y0 | | h01 |   | v0 |
							// | x1 y1  1  0  0  0 -u1*x1 -u1*y1 | | h02 |   | u1 |
							// |  0  0  0 x1 y1  1 -v1*x1 -v1*y1 | | h10 |   | v1 |
							// | x2 y2  1  0  0  0 -u2*x2 -u2*y2 | | h11 | = | u2 |
							// |  0  0  0 x2 y2  1 -v2*x2 -v2*y2 | | h12 |   | v2 |
							// | x3 y3  1  0  0  0 -u3*x3 -u3*y3 | | h20 |   | u3 |
							// |  0  0  0 x3 y3  1 -v3*x3 -v3*y3 | | h21 |   | v3 |
							/*
							Mat_<float> B(2 * ransacN, 8);
							Mat_<float> C(2 * ransacN, 1);
							for (int i = 0; i < ransacN; i++) {
								for (int j = 0; j < 2; j++) {
									KeyPoint x1 = x1keyptsMat.at<KeyPoint>(i);
									KeyPoint x2 = x2keyptsMat.at<KeyPoint>(i);
									if (j == 0) {
										B.at<float>(2 * i + j, 0) = x1.pt.x;
										B.at<float>(2 * i + j, 1) = x1.pt.y;
										B.at<float>(2 * i + j, 2) = 1;
										B.at<float>(2 * i + j, 3) = 0;
										B.at<float>(2 * i + j, 4) = 0;
										B.at<float>(2 * i + j, 5) = 0;
										B.at<float>(2 * i + j, 6) = -1 * x2.pt.x*x1.pt.x;
										B.at<float>(2 * i + j, 7) = -1 * x2.pt.x*x1.pt.y;
										C.at<float>(2 * i + j, 0) = x2.pt.x;
									}
									else {
										B.at<float>(2 * i + j, 0) = 0;
										B.at<float>(2 * i + j, 1) = 0;
										B.at<float>(2 * i + j, 2) = 0;
										B.at<float>(2 * i + j, 3) = x1.pt.x;
										B.at<float>(2 * i + j, 4) = x1.pt.y;
										B.at<float>(2 * i + j, 5) = 1;
										B.at<float>(2 * i + j, 6) = -1 * x2.pt.y*x1.pt.x;
										B.at<float>(2 * i + j, 7) = -1 * x2.pt.y*x1.pt.y;
										C.at<float>(2 * i + j, 0) = x2.pt.y;
									}
								}
							}

							// solution 2
							// solve(B,C,H)
							Mat_<float> _H;
							solve(B, C, _H);
							for (int i = 0; i < 8; i++)
								H.at<float>(i / 3, i % 3) = _H.at<float>(i);
							H.at<float>(2, 2) = 1;
							//cout << "solve(B,C,H)" << endl << H << endl << endl;
							*/

							// solution 3
							// imgproc.hpp : getPerspectiveTransform(src,dst)
							/*
							Mat_<Point2f> src(1, 4);
							Mat_<Point2f> dst(1, 4);
							for (int i = 0; i < 4; i++) {
								src.at<Point2f>(0, i) = Point2f(x1keyptsMat.at<KeyPoint>(i).pt.x, x1keyptsMat.at<KeyPoint>(i).pt.y);
								dst.at<Point2f>(0, i) = Point2f(x2keyptsMat.at<KeyPoint>(i).pt.x, x2keyptsMat.at<KeyPoint>(i).pt.y);
							}
							H = getPerspectiveTransform(src, dst);
							//cout << "getPerspectiveTransform" << endl << H << endl << endl;
							*/

							int inlier = 0;
							for (int i = 0; i < keypoints1.size(); i++) {
								if (i == x1idxMat.at<int>(0) || i == x1idxMat.at<int>(1) || i == x1idxMat.at<int>(2) || i == x1idxMat.at<int>(3)) {
									inlier++;
									continue;
								}

								Mat_<float> x1(3, 1);
								x1.at<float>(0) = keypoints1[i].pt.x;
								x1.at<float>(1) = keypoints1[i].pt.y;
								x1.at<float>(2) = 1;

								Mat_<float> Hx1 = H*x1;
								Hx1.at<float>(0) = Hx1.at<float>(0) / Hx1.at<float>(2);
								Hx1.at<float>(1) = Hx1.at<float>(1) / Hx1.at<float>(2);
								Hx1.at<float>(2) = 1;

								Mat_<float> x2(3, 1);
								x2.at<float>(0) = knnkeyptsMat.at<KeyPoint>(i, 0).pt.x;
								x2.at<float>(1) = knnkeyptsMat.at<KeyPoint>(i, 0).pt.y;
								x2.at<float>(2) = 1;

								float err = dist(Hx1, x2);
								if (err < errth)
									inlier++;

								//cout << "pt" << i << " : (" << Hx1.at<float>(0) << "," << Hx1.at<float>(1) << ") - (" << x2.at<float>(0) << "," << x2.at<float>(1) << ")" << endl;
								//cout << "pt" << i << " : " << err;
							}

							if (inlier > maxinlier) {
								maxinlier = inlier;
								mappingMat = H.clone();

								cout << maxinlier << endl;
								//cout << mappingMat << endl;
							}
						}
					}
				}
			}

            /*
            // affine mapping
            // x2 = Hx1
			// x1=(xi,yi) -> x2=(ui,vi) for i=0,1,2
			for (int p0 = 0; p0 < knnK; p0++) {
				for (int p1 = 0; p1 < knnK; p1++) {
					for (int p2 = 0; p2 < knnK; p2++) {

                        Mat_<int> x2idxMat(3, 1);
                        x2idxMat.at<int>(0) = knnidxMat.at<int>(x1idxMat.at<int>(0), p0);
						x2idxMat.at<int>(1) = knnidxMat.at<int>(x1idxMat.at<int>(1), p1);
						x2idxMat.at<int>(2) = knnidxMat.at<int>(x1idxMat.at<int>(2), p2);

						Mat_<KeyPoint> x2keyptsMat(3, 1);
						x2keyptsMat.at<KeyPoint>(0) = keypoints2[x2idxMat.at<int>(0)];
						x2keyptsMat.at<KeyPoint>(1) = keypoints2[x2idxMat.at<int>(1)];
						x2keyptsMat.at<KeyPoint>(2) = keypoints2[x2idxMat.at<int>(2)];

                        // solve B[6*6] * H[6*1] = C[6*1]
                        // h20=0; h21=0; h22=1;
                        // | x0 y0  1  0  0  0 | | h00 |   | u0 |
                        // |  0  0  0 x0 y0  1 | | h01 |   | v0 |
                        // | x1 y1  1  0  0  0 | | h02 |   | u1 |
                        // |  0  0  0 x1 y1  1 | | h10 |   | v1 |
                        // | x2 y2  1  0  0  0 | | h11 | = | u2 |
                        // |  0  0  0 x2 y2  1 | | h12 |   | v2 |

                        Mat_<float> B(2 * 3, 6);
                        Mat_<float> C(2 * 3, 1);
                        for (int i = 0; i < 3; i++) {
                            for (int j = 0; j < 2; j++) {
                                KeyPoint x1 = x1keyptsMat.at<KeyPoint>(i);
                                KeyPoint x2 = x2keyptsMat.at<KeyPoint>(i);
                                if (j == 0) {
                                    B.at<float>(2 * i + j, 0) = x1.pt.x;
                                    B.at<float>(2 * i + j, 1) = x1.pt.y;
                                    B.at<float>(2 * i + j, 2) = 1;
                                    B.at<float>(2 * i + j, 3) = 0;
                                    B.at<float>(2 * i + j, 4) = 0;
                                    B.at<float>(2 * i + j, 5) = 0;
                                    C.at<float>(2 * i + j, 0) = x2.pt.x;
                                }
                                else {
                                    B.at<float>(2 * i + j, 0) = 0;
                                    B.at<float>(2 * i + j, 1) = 0;
                                    B.at<float>(2 * i + j, 2) = 0;
                                    B.at<float>(2 * i + j, 3) = x1.pt.x;
                                    B.at<float>(2 * i + j, 4) = x1.pt.y;
                                    B.at<float>(2 * i + j, 5) = 1;
                                    C.at<float>(2 * i + j, 0) = x2.pt.y;
                                }
                            }
                        }

                        Mat_<float> H(3,3);
                        Mat_<float> _H;
                        solve(B, C, _H, DECOMP_SVD);
                        for (int i = 0; i < 6; i++)
                            H.at<float>(i / 3, i % 3) = _H.at<float>(i);
                        H.at<float>(2, 0) = 0;
                        H.at<float>(2, 1) = 0;
                        H.at<float>(2, 2) = 1;
                        //cout << "solve(B,C,H)" << endl << H << endl << endl;

                        // imgproc.hpp : getAffineTransform(src,dst)
                        Mat_<Point2f> src(1, 3);
                        Mat_<Point2f> dst(1, 3);
                        for (int i = 0; i < 3; i++) {
                            src.at<Point2f>(0, i) = Point2f(x1keyptsMat.at<KeyPoint>(i).pt.x, x1keyptsMat.at<KeyPoint>(i).pt.y);
                            dst.at<Point2f>(0, i) = Point2f(x2keyptsMat.at<KeyPoint>(i).pt.x, x2keyptsMat.at<KeyPoint>(i).pt.y);
                        }
                        H = getAffineTransform(src, dst);
                        Mat_<float> h = (Mat_<float>(1, 3) << 0, 0, 1);
                        H.push_back(h);
                        //cout << "getAffineTransform" << endl << H << endl << endl;


                        int inlier = 0;
                        for (int i = 0; i < keypoints1.size(); i++) {
                            if (i == x1idxMat.at<int>(0) || i == x1idxMat.at<int>(1) || i == x1idxMat.at<int>(2) || i == x1idxMat.at<int>(3)) {
                                inlier++;
                                continue;
                            }

                            Mat_<float> x1(3, 1);
                            x1.at<float>(0) = keypoints1[i].pt.x;
                            x1.at<float>(1) = keypoints1[i].pt.y;
                            x1.at<float>(2) = 1;

                            Mat_<float> Hx1 = H*x1;
                            Hx1.at<float>(0) = Hx1.at<float>(0) / Hx1.at<float>(2);
                            Hx1.at<float>(1) = Hx1.at<float>(1) / Hx1.at<float>(2);
                            Hx1.at<float>(2) = 1;

                            Mat_<float> x2(3, 1);
                            x2.at<float>(0) = knnkeyptsMat.at<KeyPoint>(i, 0).pt.x;
                            x2.at<float>(1) = knnkeyptsMat.at<KeyPoint>(i, 0).pt.y;
                            x2.at<float>(2) = 1;

                            float err = dist(Hx1, x2);
                            if (err < errth)
                                inlier++;

                            //cout << "pt" << i << " : (" << Hx1.at<float>(0) << "," << Hx1.at<float>(1) << ") - (" << x2.at<float>(0) << "," << x2.at<float>(1) << ")" << endl;
                            //cout << "pt" << i << " : " << err;
                        }

                        if (inlier > maxinlier) {
                            maxinlier = inlier;
                            mappingMat = H.clone();

                            cout << maxinlier << endl;
                            //cout << mappingMat << endl;
                        }
					}
				}
			}
			*/
		}
		cout << "--- RANSAC done" << endl;


		// ===== image wrapping
		// forward
		Mat output = img2.clone();
		for (int i = 0; i < img1.rows; i++) {
			for (int j = 0; j < img1.cols; j++) {

				if (img1.at<Vec3b>(i, j) != Vec3b(255, 255, 255)) {

					Mat_<float> x1(3, 1);
					x1.at<float>(0) = j;
					x1.at<float>(1) = i;
					x1.at<float>(2) = 1;

					Mat_<float> Hx1 = mappingMat*x1;
					Hx1.at<float>(0) = Hx1.at<float>(0) / Hx1.at<float>(2);
					Hx1.at<float>(1) = Hx1.at<float>(1) / Hx1.at<float>(2);
					Hx1.at<float>(2) = 1;

					Mat_<int> x2(2, 1);
					x2.at<int>(0) = int(Hx1.at<float>(0));
					x2.at<int>(1) = int(Hx1.at<float>(1));

					if (x2.at<int>(0) >= 0 && x2.at<int>(1) >= 0 && x2.at<int>(0) < img2.cols && x2.at<int>(1) < img2.rows)
						output.at<Vec3b>(x2.at<int>(1), x2.at<int>(0)) = img1.at<Vec3b>(i, j);
				}
			}
		}
		imwrite("..\\data\\output_f.jpg", output);
		imshow("forward wrapping", output);
		//imwrite("..\\data\\output_11\\11_" + to_string(r) + "f.jpg", output);

		// backward
		output = img2.clone();
		for (int i = 0; i < img2.rows; i++) {
			for (int j = 0; j < img2.cols; j++) {

				Mat_<float> x2(3, 1);
				x2.at<float>(0) = j;
				x2.at<float>(1) = i;
				x2.at<float>(2) = 1;

				Mat_<float> invHx2 = mappingMat.inv()*x2;
				invHx2.at<float>(0) = invHx2.at<float>(0) / invHx2.at<float>(2);
				invHx2.at<float>(1) = invHx2.at<float>(1) / invHx2.at<float>(2);
				invHx2.at<float>(2) = 1;

				Mat_<int> x1(2, 1);
				x1.at<int>(0) = int(invHx2.at<float>(0));
				x1.at<int>(1) = int(invHx2.at<float>(1));

				if (x1.at<int>(0) >= 0 && x1.at<int>(1) >= 0 && x1.at<int>(0) < img1.cols && x1.at<int>(1) < img1.rows)
					if (img1.at<Vec3b>(x1.at<int>(1), x1.at<int>(0)) != Vec3b(255, 255, 255))
						output.at<Vec3b>(i, j) = img1.at<Vec3b>(x1.at<int>(1), x1.at<int>(0));
			}
		}
		imwrite("..\\data\\output_b.jpg", output);
		imshow("backward wrapping", output);
		//imwrite("..\\data\\output_11\\11_" + to_string(r) + "b.jpg", output);
		cout << "--- image wrapping done" << endl;

		//max[r] = maxinlier;
	}

	//ofstream fout("..\\data\\output_11\\11.txt");
	//for (int r = 0; r < runth; r++)
	//	fout << max[r] << endl;
	//fout.close();

	waitKey(0);
	return 0;
}
