#ifndef _PROBLEM_H_
#define _PROBLEM_H_

#include "individual.cuh"

template <typename T>
class Problem
{
public:
	typedef Individual_t<T> Individual;

	int genes_size;

	int type; // -1: MIN, 0:OPT, 1: MAX
	double fitness_threshold;
	double upper_bound, lower_bound;

	__host__ __device__
		Problem(int genes_size_, double fitness_threshold_)
		: genes_size(genes_size_),
		fitness_threshold(fitness_threshold_),
		upper_bound(0),
		lower_bound(0) {}

	__host__ __device__ virtual ~Problem() {};
	__host__ __device__ virtual void initIndividual(Individual &indiv) = 0;
	__host__ __device__ virtual void computeIndividualFitness(Individual &indiv) = 0;
	__host__ __device__ virtual void compareFitness(double f1, double f2, int *result) = 0;

	__host__ __device__
		virtual void normalize(Individual &indiv) {
		for (int g = 0; g < genes_size; ++g) {
			while (indiv[g] > upper_bound || indiv[g] < lower_bound) {
				if (indiv[g] > upper_bound) {
					indiv[g] = upper_bound - (indiv[g] - upper_bound)*indiv.drand();
				}
				else if (indiv[g] < lower_bound) {
					indiv[g] = lower_bound - (indiv[g] - lower_bound)*indiv.drand();
				}
			}
		}
	}
};

class OneMaxProblem : public Problem<int> 
{
public:
	__host__ __device__
		OneMaxProblem(int genes_size_, double itness_threshold_)
		: Problem<int>(genes_size_, itness_threshold_) {
			type = 1;
			upper_bound = 1;
			lower_bound = 0;
		}

	__host__ __device__
		void initIndividual(Individual &indiv) {
		for (int g = 0; g < genes_size; ++g) {
			indiv[g] = indiv.rand() % 2;
		}
		//computeIndividualFitness(indiv);
	}

	// f(x) = sum_i(xi)
	__host__ __device__
		void computeIndividualFitness(Individual &indiv) {
		indiv.fitness = 0;
		for (int g = 0; g < genes_size; ++g) {
			indiv.fitness += indiv[g];
		}
	}

	// global maximum: f(x)=genes_size at x=(1,1,..)
	__host__ __device__
		void compareFitness(double f1, double f2, int *result) {
			*result = (f1 > f2 ? 1 : (f1 == f2 ? 0 : -1));
		}

	__host__ __device__
		void normalize(Individual &indiv) {}
};

class AckleyFunction : public Problem<double>
{
public:
	__host__ __device__
		AckleyFunction(int genes_size_, double itness_threshold_)
		: Problem<double>(genes_size_, itness_threshold_){
			type = -1;
			upper_bound = 32.768;
			lower_bound = -32.768;
		}

	__host__ __device__
		void initIndividual(Individual &indiv) {
		for (int g = 0; g < genes_size; ++g) {
			indiv[g] = indiv.drand(lower_bound, upper_bound);
		}
		//computeIndividualFitness(indiv);
	}

	// a=20, b=0.2, c=2*PI
	// f(x) = -a*exp(-b*sqrt((1/d)*sum_i(xi*xi)))-exp((1/d)*sum_i(cos(c*xi)))+a+exp(1)
	__host__ __device__
		void computeIndividualFitness(Individual &indiv) {
		int offset = 25;
		double *x = indiv.genes;
		int d = genes_size;
		double a = 0, b = 0;
		for (int i = 0; i < d; ++i) {
			a += (x[i] - offset)*(x[i] - offset);
			b += cos(2 * M_PI*(x[i] - offset));
		}
		indiv.fitness = -20 * exp((-0.2)*sqrt(a / d)) - exp(b / d) + 20 + M_E;
	}

	// global minimum: f(x)=0 at x=(0,0,..)
	__host__ __device__
		void compareFitness(double f1, double f2, int *result) {
			*result = (f1 < f2 ? 1 : (f1 == f2 ? 0 : -1));
		}
};

class SchwefelFunction : public Problem<double>
{
public:
	__host__ __device__
		SchwefelFunction(int genes_size_, double itness_threshold_)
		: Problem<double>(genes_size_, itness_threshold_){
			type = -1;
			upper_bound = 500;
			lower_bound = -500;
		}

	__host__ __device__
		void initIndividual(Individual &indiv) {
		for (int g = 0; g < genes_size; ++g) {
			indiv[g] = indiv.drand(lower_bound, upper_bound);
		}
		//computeIndividualFitness(indiv);
	}

	// f(x) = 418.9829*d-sum_i(xi*sin(sqrt(|xi|)))
	__host__ __device__
		void computeIndividualFitness(Individual &indiv) {
		double *x = indiv.genes;
		int d = genes_size;
		double a = 0;
		for (int i = 0; i < d; ++i) {
			a += x[i] * sin(sqrt(abs(x[i])));
		}
		indiv.fitness = 418.9829*d - a;
	}

	// global minimum: f(x)=0 at x=(420.9687,420.9687,...)
	__host__ __device__
		void compareFitness(double f1, double f2, int *result) {
			*result = (f1 < f2 ? 1 : (f1 == f2 ? 0 : -1));
		}
};

#endif /* _PROBLEM_H_ */
