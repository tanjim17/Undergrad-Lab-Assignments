import numpy as np

EDGE_PROB = 0.9
SENSOR_PROB = 0.85

def create_transition_list(m, n, obstacles):
    transitions = [[[] for j in range(m)] for i in range(n)]
    for i in range(n):
        for j in range(m):
            edge_moves = []
            non_edge_moves = []
            possible_edge_moves = [(i-1, j), (i+1, j), (i, j-1), (i, j+1)]
            for (x, y) in possible_edge_moves:
                if x >= 0 and x < n and y >= 0 and y < m and (x, y) not in obstacles:
                    edge_moves.append((x, y))
            possible_non_edge_moves = [(i, j), (i-1, j-1), (i-1, j+1), (i+1, j-1), (i+1, j+1)]
            for (x, y) in possible_non_edge_moves:
                if x >= 0 and x < n and y >= 0 and y < m and (x, y) not in obstacles:
                    non_edge_moves.append((x, y))
            
            edge_prob = (EDGE_PROB / len(edge_moves)) if len(edge_moves) else 0
            non_edge_prob = (1 - EDGE_PROB) / len(non_edge_moves)
            for (x, y) in edge_moves:
                transitions[x][y].append((i, j, edge_prob))
            for (x, y) in non_edge_moves:
                transitions[x][y].append((i, j, non_edge_prob))
    return transitions

def main():
    n, m, k = [int(x) for x in input().split()]
    # n, m, k = 1, 3, 0# 6, 6, 6
    arr = [(2, 1), (4, 1), (2, 0), (0, 5), (1, 3), (4, 5)]
    obstacles = set()
    for x in range(k):
        i, j = [int(y) for y in input().split()]
        # i, j = arr[x]
        obstacles.add((i, j))
    
    belief_state = np.full((n, m), 1/(n*m - k))
    for (i, j) in obstacles:
        belief_state[i][j] = 0
    print(belief_state)
    partial_belief_state = np.zeros((n, m))

    transitions = create_transition_list(m, n, obstacles)
    # print(transitions)

    emissions = np.zeros((n, m))   
    while True:
        inputs = input().split()
        if inputs[0] == 'r':
            i, j = int(inputs[1]), int(inputs[2])
            sensor_prob = SENSOR_PROB if inputs[3] == '1' else (1 - SENSOR_PROB)
            emissions.fill(1 - sensor_prob)
            for x in range(i - 1, i + 2):
                for y in range(j - 1, j + 2):
                    if x >= 0 and x < n and y >= 0 and y < m:
                        emissions[x][y] = sensor_prob
            # print(emissions)
            for i in range(n):
                for j in range(m):
                    for entry in transitions[i][j]:
                        partial_belief_state[i][j] +=  entry[2] * belief_state[entry[0]][entry[1]]
            # print(partial_belief_state)
            for i in range(n):
                for j in range(m):
                    belief_state[i][j] = emissions[i][j] * partial_belief_state[i][j]
            
            prob_sum = np.sum(belief_state)
            for i in range(n):
                for j in range(m):
                    belief_state[i][j] = belief_state[i][j] * 100 / prob_sum
            print(belief_state)
        elif inputs[0] == 'c':
            print('Casper is probably at ', np.unravel_index(belief_state.argmax(), belief_state.shape))
        elif inputs[0] == 'q':
            break

if __name__ == "__main__":
    main()