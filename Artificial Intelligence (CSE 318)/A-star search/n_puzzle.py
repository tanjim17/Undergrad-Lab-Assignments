from queue import PriorityQueue
import numpy as np


class Node:
    heuristic = None

    def __init__(self, board, parent_node=None, cost_from_start=0):
        self.board = board
        self.parent_node = parent_node
        self.cost_from_start = cost_from_start
        self.expected_cost = cost_from_start + self.heuristic()

    def __lt__(self, other):
        return self.expected_cost < other.expected_cost

    def create_neighbor_nodes(self, explored_boards):
        x, y = get_blank_pos(self.board)
        new_positions = [(x - 1, y), (x + 1, y), (x, y - 1), (x, y + 1)]
        nodes = []
        size = len(self.board)
        for new_x, new_y in new_positions:
            if 0 <= new_x < size and 0 <= new_y < size:
                new_board = np.copy(self.board)
                new_board[x][y], new_board[new_x][new_y] = new_board[new_x][new_y], new_board[x][y]
                if new_board.tobytes() in explored_boards:
                    continue
                nodes.append(Node(new_board, self, self.cost_from_start + 1))
        return nodes

    def get_hamming_dist(self):
        dist = 0
        size = len(self.board)
        for i in range(size):
            for j in range(size):
                if self.board[i][j] - 1 != i * size + j:
                    dist += 1
        dist -= 1  # blank space doesn't count
        return dist

    def get_manhattan_dist(self):
        dist = 0
        size = len(self.board)
        for i in range(size):
            for j in range(size):
                if self.board[i][j] == 0:
                    continue
                row, col = (self.board[i][j] - 1) // size, (self.board[i][j] - 1) % size
                dist += abs(row - i) + abs(col - j)
        return dist

    def get_manhattan_dist_with_linear_conflict(self):
        no_of_conflicts = 0
        size = len(self.board)
        for i in range(size):
            for j in range(size):
                if self.board[i][j] and i == (self.board[i][j] - 1) // size:
                    for k in range(j + 1, size):
                        if self.board[i][k] and i == (self.board[i][k] - 1) // size \
                                and self.board[i][k] < self.board[i][j]:
                            no_of_conflicts += 1
        return self.get_manhattan_dist() + 2 * no_of_conflicts

    def __str__(self):
        size = len(self.board)
        s = ''
        for i in range(size):
            for j in range(size):
                s += '*' if self.board[i][j] == 0 else str(self.board[i][j])
                s += ' '
            s += '\n'
        return s


def get_blank_pos(board):
    pos = np.where(board == 0)
    return pos[0][0], pos[1][0]


def count_inversions(arr):
    count = 0
    for i in range(len(arr)):
        for j in range(i + 1, len(arr)):
            if arr[j] and arr[j] < arr[i]:
                count += 1
    return count


def is_solvable(board):
    size = len(board)
    inversion_count = count_inversions(board.flatten())
    if size % 2:
        return True if inversion_count % 2 == 0 else False
    else:
        blank_pos_row = get_blank_pos(board)[0]
        if (blank_pos_row % 2 == 0 and inversion_count % 2) or (blank_pos_row % 2 and inversion_count % 2 == 0):
            return True
        return False


def print_path(node):
    if node is None:
        return
    else:
        print_path(node.parent_node)
        print(node)


heuristics = {"hamming distance": Node.get_hamming_dist,
              "manhattan distance": Node.get_manhattan_dist,
              "manhattan distance with linear conflict": Node.get_manhattan_dist_with_linear_conflict}


def solve_puzzle(board, heuristic):
    Node.heuristic = heuristic
    size = len(board)
    goal_board = np.append(np.arange(1, size * size), 0).reshape(size, size)

    pq = PriorityQueue()
    pq.put(Node(board))
    expanded_nodes_count = 1

    explored_boards = set()
    while True:
        node = pq.get()
        if np.array_equal(node.board, goal_board):
            print_path(node)
            print("# of explored nodes: " + str(len(explored_boards)))
            print("# of expanded nodes: " + str(expanded_nodes_count))
            print("Optimal cost: " + str(node.cost_from_start))
            break
        for neighbor_node in node.create_neighbor_nodes(explored_boards):
            pq.put(neighbor_node)
            expanded_nodes_count += 1
        explored_boards.add(node.board.tobytes())


def main():
    size = int(input("Enter board size: "))
    print("Enter board entries:")
    board = np.empty((size, size), int)
    for i in range(size):
        row = input().split()
        for j in range(size):
            board[i][j] = 0 if row[j] == '*' else int(row[j])

    if not is_solvable(board):
        print("not solvable!")
        return

    for heuristic in heuristics.items():
        print("\n" + heuristic[0] + ":")
        solve_puzzle(board, heuristic[1])


if __name__ == "__main__":
    main()
