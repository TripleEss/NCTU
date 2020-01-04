#ifndef _INDIVIDUAL_H_
#define _INDIVIDUAL_H_

#include <thrust/random.h>

template<typename T>
struct Individual_t
{
	typedef T ValueType;

	int genes_size;
	ValueType *genes;
	double fitness;

	__host__ __device__ static void crossover(const Individual_t &p1, const Individual_t &p2, Individual_t &c1, Individual_t &c2);
	__host__ __device__ static void mutation(Individual_t &indiv);

	__host__ __device__
		T& operator[](size_t idx) {
		return genes[idx];
	}

	__host__ __device__
		const T& operator[](size_t idx) const {
		return genes[idx];
	}

	__host__ __device__
		bool operator<(const Individual_t indiv) const {
		return (fitness < indiv.fitness);
	}

	__host__ __device__
		bool operator>(const Individual_t indiv) const {
		return (fitness > indiv.fitness);
	}

	
	thrust::default_random_engine rng;

	__host__ __device__
		void init_rand() {
		rng.discard((long)this);
	}

	__host__ __device__
		int rand() {
		thrust::uniform_int_distribution<int> dist;
		return dist(rng);
	}

	__host__ __device__
		double drand(double from = 0, double to = 1) {
		thrust::uniform_real_distribution<double> dist(from, to);
		return dist(rng);
	}
};


// crossover int
template<>
__host__ __device__
void Individual_t<int>::crossover(const Individual_t &p1, const Individual_t &p2, Individual_t &c1, Individual_t &c2) {
	// uniform crossover
	for (int g = 0; g < p1.genes_size; ++g) {
		bool xover = c1.rand() % 2;
		if (xover) {
			c1[g] = p2[g];
			c2[g] = p1[g];
		}
		else {
			c1[g] = p1[g];
			c2[g] = p2[g];
		}
	}
}

// mutation int
template<>
__host__ __device__
void Individual_t<int>::mutation(Individual_t &indiv) {
	int g = indiv.rand() % indiv.genes_size;
	indiv[g] = !(indiv[g]);
}

// crossover double
template<>
__host__ __device__
void Individual_t<double>::crossover(const Individual_t &p1, const Individual_t &p2, Individual_t &c1, Individual_t &c2) {
	// blend corssover a<b
	// [a-p*(b-a),b+p*(b-a)]
	double p = c1.drand(0.3, 0.7);
	for (int g = 0; g < p1.genes_size; ++g) {
		double a = min(p1[g], p2[g]);
		double b = max(p1[g], p2[g]);
		double diff = b - a;
		c1[g] = c1.drand(a - p*diff, b + p*diff);
		c2[g] = c2.drand(a - p*diff, b + p*diff);
	}
}

// mutation double
template<>
__host__ __device__
void Individual_t<double>::mutation(Individual_t &indiv) {
	double p = indiv.drand(-0.05, 0.05);
	for (int g = 0; g < indiv.genes_size; g++) {
		indiv[g] += indiv[g] * p;
	}
}

#endif /* _INDIVIDUAL_H_ */
