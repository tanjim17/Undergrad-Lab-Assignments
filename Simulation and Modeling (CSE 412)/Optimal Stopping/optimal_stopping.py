import sys
import random
import matplotlib.pyplot as plt

if __name__ == '__main__':
    n = int(sys.argv[1]) # population size
    s = int(sys.argv[2]) # success criterion
    max_itr = int(sys.argv[3])

    success_rates = []
    sample_sizes = range(1, n)
    for m in sample_sizes:
        success_count = 0
        for _ in range(max_itr):
            # generate n uniform random numbers
            population = [random.random() for i in range(n)]

            # find the top s numbers
            sorted_population = sorted(population)
            top_s_candidates = sorted_population[-s:]

            # generate m samples from the population and remove them
            new_population = population[:]
            samples = [new_population.pop(random.randrange(len(new_population))) for i in range(m)]

            # find the largest sample
            largest_sample = max(samples)

            # if no candidate is larger than the largest sample, then the last one will be selected
            selected_candidate = new_population[-1]

            # selecting candidate
            for x in new_population:
                if x > largest_sample:
                    selected_candidate = x
                    break
            
            # determining success
            if selected_candidate in top_s_candidates:
                success_count += 1
        
        success_rates.append((success_count / max_itr) * 100)
    
    # plot the success rates
    plt.plot(sample_sizes, success_rates)
    plt.title(f'Success rate vs Sample size (s = {s})')
    plt.xlabel("Sample size")
    plt.ylabel("Success rate %")
    plt.grid()
    plt.plot()
    plt.savefig(f"s_{s}.png")
    plt.close()
             

