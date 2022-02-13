import numpy as np
np.set_printoptions(precision=3)

EDGE_CUMULATIVE_PROB = 0.9
SENSOR_PROB = 0.85

def create_transition_list(m, n, obstacles):
    transitions = [[[] for j in range(m)] for i in range(n)]
    for i in range(n):
        for j in range(m):
            edge_moves = []
            non_edge_moves = []
            possible_edge_moves = [(i - 1, j), (i + 1, j), (i, j - 1), (i, j + 1)]
            for (x, y) in possible_edge_moves:
                if x >= 0 and x < n and y >= 0 and y < m and (x, y) not in obstacles:
                    edge_moves.append((x, y))
            possible_non_edge_moves = [(i, j), (i - 1, j - 1), (i - 1, j + 1), (i + 1, j - 1), (i + 1, j + 1)]
            for (x, y) in possible_non_edge_moves:
                if x >= 0 and x < n and y >= 0 and y < m and (x, y) not in obstacles:
                    non_edge_moves.append((x, y))
            
            edge_prob = EDGE_CUMULATIVE_PROB / len(edge_moves) if len(edge_moves) else 0
            non_edge_prob = (1 - EDGE_CUMULATIVE_PROB) / len(non_edge_moves) if len(edge_moves) else 1 / len(non_edge_moves)
            for (x, y) in edge_moves:
                transitions[x][y].append((i, j, edge_prob))
            for (x, y) in non_edge_moves:
                transitions[x][y].append((i, j, non_edge_prob))
    return transitions

def main():
    # input row, col and number of obstacles
    n, m, k = [int(x) for x in input().split()]

    # setting obstacles
    obstacles = set()
    for x in range(k):
        i, j = [int(y) for y in input().split()]
        obstacles.add((i, j))
    
    # initializing belief state
    belief_state = np.full((n, m), 100 / (n * m - k))
    for (i, j) in obstacles:
        belief_state[i][j] = 0
    print(belief_state, '\n')

    # calculating transition probabilities
    # the function returns a list of triples: (source cell row, source cell col, probability)
    transitions = create_transition_list(m, n, obstacles)
    # print(transitions)

    partial_belief_state = np.empty((n, m))
    emissions = np.empty((n, m))   
    while True:
        inputs = input().split()
        if inputs[0] == 'R':
            i, j = int(inputs[1]), int(inputs[2])

            # setting emission probabilities
            sensor_prob = SENSOR_PROB if inputs[3] == '1' else (1 - SENSOR_PROB)
            emissions.fill(1 - sensor_prob)
            for x in range(i - 1, i + 2):
                for y in range(j - 1, j + 2):
                    if x >= 0 and x < n and y >= 0 and y < m:
                        emissions[x][y] = sensor_prob
            # print(emissions)

            # calculating partial belief state
            partial_belief_state.fill(0)
            for i in range(n):
                for j in range(m):
                    for entry in transitions[i][j]:
                        partial_belief_state[i][j] +=  entry[2] * belief_state[entry[0]][entry[1]]
            # print(partial_belief_state)

            # calculating belief state
            for i in range(n):
                for j in range(m):
                    belief_state[i][j] = emissions[i][j] * partial_belief_state[i][j]
            
            # normalizing
            prob_sum = np.sum(belief_state)
            for i in range(n):
                for j in range(m):
                    belief_state[i][j] = belief_state[i][j] * 100 / prob_sum

            print(belief_state, '\n')

        elif inputs[0] == 'C':
            print('Casper is probably at ', np.unravel_index(belief_state.argmax(), belief_state.shape), '\n')
            
        elif inputs[0] == 'Q':
            break

if __name__ == "__main__":
    main()