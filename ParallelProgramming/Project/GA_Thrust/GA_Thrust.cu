#define _USE_MATH_DEFINES

#include <cassert>

#include <iostream>
#include <iomanip>
#include <vector>
#include <ctime>
#include <sys/time.h>

#include <thrust/device_vector.h>

#include "problem.cuh"
#include "individual.cuh"

using std::cout;
using std::endl;

#define DEFAULT_SELECTION_SIZE 2
#define DEFAULT_MAX_ITERATION 10000
#define DEFAULT_CROSSOVER_PROB 1
#define DEFAULT_MUTATIOM_PROB 0.01

template<typename P>
class GeneticAlgorithm
{
private:
	typedef typename P::Individual Individual;
	typedef typename Individual::ValueType ValueType;

	P problem;

	Individual *indivs, *new_indivs;
	Individual best_indiv;

	thrust::device_vector<Individual> indivs_dv, new_indivs_dv;
	thrust::device_vector<ValueType> data_dv, new_data_dv; // indivs.genes

	thrust::host_vector<Individual> indivs_h;
	thrust::host_vector<ValueType> data_h;

	int population_size;
	int max_iteration;
	int selection_size;
	double crossover_probability, mutation_probability;


	void move_dv_to_host() {
		indivs_h.clear();
		data_h.clear();

		indivs_h.resize(indivs_dv.size());
		data_h.resize(data_dv.size());

		thrust::copy(indivs_dv.begin(), indivs_dv.end(), indivs_h.begin());
		thrust::copy(data_dv.begin(), data_dv.end(), data_h.begin());
	}

	void print_indivs() {
		move_dv_to_host();

		for(int i = 0; i < population_size; ++i) {
			for(int g = 0; g < problem.genes_size; ++g) {
				cout << data_h[i * problem.genes_size + g];
			}
			cout << " " << indivs_h[i].fitness << endl;
		}
	}

public:
	int stopit;

	GeneticAlgorithm(
		P problem_,
		int population_size_,
		int max_iteration_ = DEFAULT_MAX_ITERATION,
		int selection_size_ = DEFAULT_SELECTION_SIZE,
		double crossover_probability_ = DEFAULT_CROSSOVER_PROB,
		double mutation_probability_ = DEFAULT_MUTATIOM_PROB) :
		problem(problem_),
		population_size(population_size_),
		max_iteration(max_iteration_),
		selection_size(selection_size_),
		crossover_probability(crossover_probability_),
		mutation_probability(mutation_probability_),
		stopit(-1) {}

	/* initialization */
	class InitPopulation {
	private:
		P problem;
		Individual *indivs;
		ValueType *data;
	public:
		InitPopulation(P problem_, Individual *indivs_, ValueType *data_)
			: problem(problem_),
			indivs(indivs_),
			data(data_) {}

		__host__ __device__
			void operator()(int i) {
			Individual &indiv = indivs[i];
			indiv.init_rand();
			indiv.genes = data + i*problem.genes_size;
			indiv.genes_size = problem.genes_size;
			problem.initIndividual(indiv);
		}
	};

	void initPopulation() {
		data_dv.resize(population_size*problem.genes_size);
		indivs_dv.resize(population_size);
		indivs = thrust::raw_pointer_cast(indivs_dv.data());

		new_data_dv.resize(population_size*problem.genes_size);
		new_indivs_dv.resize(population_size);
		new_indivs = thrust::raw_pointer_cast(new_indivs_dv.data());

		thrust::for_each(
			thrust::make_counting_iterator(0),
			thrust::make_counting_iterator(population_size),
			InitPopulation(problem, indivs, thrust::raw_pointer_cast(data_dv.data())));

		thrust::for_each(
			thrust::make_counting_iterator(0),
			thrust::make_counting_iterator(population_size),
			InitPopulation(problem, new_indivs, thrust::raw_pointer_cast(new_data_dv.data())));
	}

	/* compute fitness of population */
	class ComputePopulationFitness {
	private:
		P problem;
		Individual *indivs;
	public:
		ComputePopulationFitness(P problem_)
			: problem(problem_) {}

		__host__ __device__
			void operator()(Individual &indiv) {
			problem.computeIndividualFitness(indiv);
		}
	};

	void computePopulationFitness() {
		thrust::for_each(
			thrust::device_pointer_cast(indivs),
			thrust::device_pointer_cast(indivs + population_size),
			ComputePopulationFitness(problem));
	}

	/* selection -> crossover -> mutaion*/
	class SelectAndRecombine {
	private:
		P problem;
		Individual *indivs, *new_indivs;

		int population_size;
		int selection_size;
		double crossover_probability, mutation_probability;

		__host__ __device__
			void selectParent(int *p, int idx) {
			int best = indivs[idx].rand() % population_size;
			double best_fitness = indivs[best].fitness;
			for (int i = 1; i < selection_size; ++i) {
				int r = indivs[idx].rand() % population_size;

				int result;
				problem.compareFitness(indivs[r].fitness, best_fitness, &result);
				if (result == 1) {
					best = r;
					best_fitness = indivs[best].fitness;
				}
			}
			*p = best;
		}
	public:
		SelectAndRecombine(P problem_, Individual *indivs_, Individual *new_indivs_, int population_size_, int selection_size_, double crossover_probability_, double mutation_probability_)
			: problem(problem_),
			indivs(indivs_),
			new_indivs(new_indivs_),
			population_size(population_size_),
			selection_size(selection_size_),
			crossover_probability(crossover_probability_),
			mutation_probability(mutation_probability_) {}

		__host__ __device__
			void operator()(int i) {
			i *= 2; // 2-individual based
			int p1, p2;
			selectParent(&p1, i);
			selectParent(&p2, i + 1);

			if (indivs[p1].drand() < crossover_probability) {
				Individual::crossover(indivs[p1], indivs[p2], new_indivs[i], new_indivs[i + 1]);

				problem.normalize(new_indivs[i]);
				problem.normalize(new_indivs[i + 1]);
			}

			if (indivs[p2].drand() < mutation_probability) {
				Individual::mutation(new_indivs[i]);
				Individual::mutation(new_indivs[i + 1]);

				problem.normalize(new_indivs[i]);
				problem.normalize(new_indivs[i + 1]);
			}
		}
	};

	void selectAndRecombine() {
		thrust::for_each(
			thrust::make_counting_iterator(0),
			thrust::make_counting_iterator(population_size / 2),
			SelectAndRecombine(
				problem,
				indivs,
				new_indivs,
				population_size,
				selection_size,
				crossover_probability,
				mutation_probability
			)
		);
		std::swap(indivs, new_indivs);
	}

	void updateBestIndividual() {
		if (problem.type == 1) {
			best_indiv = *(thrust::max_element(
				thrust::device_pointer_cast(indivs),
				thrust::device_pointer_cast(indivs + population_size)));
		}
		else if (problem.type == -1) {
			best_indiv = *(thrust::min_element(
				thrust::device_pointer_cast(indivs),
				thrust::device_pointer_cast(indivs + population_size)));
		}
	}

	const Individual& getBestIndividual() {
		return best_indiv;
	}

	void evolve() {

		bool stop = false;
		for (int it = 0; it < max_iteration; ++it) {
			struct timeval start, start1, start2, end;

			gettimeofday(&start, NULL);
			selectAndRecombine();
			gettimeofday(&end, NULL);
			double delta1 = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

			gettimeofday(&start1, NULL);
			computePopulationFitness();
			gettimeofday(&end, NULL);
			double delta2 = ((end.tv_sec - start1.tv_sec) * 1000000u + end.tv_usec - start1.tv_usec) / 1.e6;

			gettimeofday(&start2, NULL);
			updateBestIndividual();
			gettimeofday(&end, NULL);
			double delta3 = ((end.tv_sec - start1.tv_sec) * 1000000u + end.tv_usec - start1.tv_usec) / 1.e6;			
			double delta4 = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

			cout << it << " " << best_indiv.fitness << " " << delta1 << " " << delta2 << " " << delta3 << " " << delta4 << endl;

			if (!stop) {
				int result;
				problem.compareFitness(best_indiv.fitness, problem.fitness_threshold, &result);
				if (result != -1) {
					stopit = it;
					stop = true;
				}
			}
		}
	}
};

template<typename P>
void runGA(
	int prob_genes_size,
	double prob_fitness_threshold,
	int ga_population_size,
	int ga_max_iteration = DEFAULT_MAX_ITERATION,
	int ga_selection_size = DEFAULT_SELECTION_SIZE,
	double ga_crossover_probability = DEFAULT_CROSSOVER_PROB,
	double ga_mutation_probability = DEFAULT_MUTATIOM_PROB) {

	assert(ga_population_size % 2 == 0);

	P problem(prob_genes_size, prob_fitness_threshold);

	GeneticAlgorithm<P> ga(
		problem,
		ga_population_size,
		ga_max_iteration,
		ga_selection_size,
		ga_crossover_probability,
		ga_mutation_probability);

	cout << "genes_size: " << prob_genes_size << endl;
	cout << "population_size: " << ga_population_size << endl;

	struct timeval start, start1, end;

	gettimeofday(&start, NULL);
	ga.initPopulation();
	gettimeofday(&end, NULL);
	double delta1 = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

	gettimeofday(&start1, NULL);
	ga.evolve();
	gettimeofday(&end, NULL);
	double delta2 = ((end.tv_sec - start1.tv_sec) * 1000000u + end.tv_usec - start1.tv_usec) / 1.e6;
	double delta3 = ((end.tv_sec - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e6;

	cout << ga.stopit << " finished " << prob_genes_size << "*" << ga_population_size << " " << delta1 << " " << delta2 << " " << delta3 << endl;
}

int main(int argv, char **argc)
{
	srand(time(NULL));

	if(argv < 5) {
		cout << "arg: problem genes_size fitness_threshold population_size (max_iteration) (selection_size) (crossover_probability) (mutation_probability)";
		return 0;
	}

	int genes_size = atoi(argc[2]);
	double fitness_threshold = atof(argc[3]);
	int population_size = atoi(argc[4]);

	int max_iteration = DEFAULT_MAX_ITERATION;
	if (argv > 5)
		max_iteration = atoi(argc[5]);

	int selection_size = DEFAULT_SELECTION_SIZE;
	if (argv > 6)
		selection_size = atoi(argc[6]);
	
	double crossover_probability = DEFAULT_CROSSOVER_PROB;
	if (argv > 7)
		crossover_probability = atof(argc[7]);
	
	double mutation_probability = DEFAULT_MUTATIOM_PROB;
	if (argv > 8)
		mutation_probability = atof(argc[8]);

	switch (argc[1][0])
	{
	case 'O':
		runGA<OneMaxProblem>(genes_size, fitness_threshold, population_size, max_iteration, selection_size, crossover_probability, mutation_probability);
		break;
	case 'A':
		runGA<AckleyFunction>(genes_size, fitness_threshold, population_size, max_iteration, selection_size, crossover_probability, mutation_probability);
		break;
	case 'S':
		runGA<SchwefelFunction>(genes_size, fitness_threshold, population_size, max_iteration, selection_size, crossover_probability, mutation_probability);
		break;
	}
}
