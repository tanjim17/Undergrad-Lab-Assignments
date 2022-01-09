import numpy as np
import copy
import sys
import time

P1_STORAGE_IDX = 6
P2_STORAGE_IDX = 13
# MAX_DEPTH = 4
INF = sys.maxsize

MESSAGES = {1: 'bonus turn!', 2: 'captured!', 3: 'bin is empty!'}


class State:
    heuristic = None
    MAX_DEPTH = 0

    def __init__(self, board, is_p1_turn):
        self.board = board
        self.is_p1_turn = is_p1_turn

    def move_stones(self, idx):  # returns message code i.e. key for MESSAGES
        if self.board[idx] == 0:
            return 3
        else:
            size = len(self.board)
            curr_indices = self.get_curr_indices()
            storage_idx = curr_indices[-1]
            curr_idx = idx
            while self.board[idx] > 0:
                curr_idx = (curr_idx + 1) % size
                if not (curr_idx == ((storage_idx + 7) % size)):  # skip opponent's storage
                    self.board[curr_idx] += 1
                    self.board[idx] -= 1

            if curr_idx == storage_idx:
                return 1
            else:
                self.is_p1_turn = not self.is_p1_turn
                # attempt to capture
                opposite_idx = (curr_idx + ((storage_idx - curr_idx) * 2)) % size
                if curr_idx in curr_indices[:-1] and self.board[curr_idx] == 1 and self.board[opposite_idx]:
                    self.board[storage_idx] += self.board[curr_idx] + self.board[opposite_idx]
                    self.board[curr_idx] = 0
                    self.board[opposite_idx] = 0
                    return 2
                return 0

    def is_game_over(self):
        for i in range(0, P1_STORAGE_IDX):
            if self.board[i] != 0:
                for j in range(P1_STORAGE_IDX + 1, P2_STORAGE_IDX):
                    if self.board[j] != 0:
                        return False
                return True
        return True

    def show_result(self):
        # adding the remaining stones to corresponding storage
        self.board[P1_STORAGE_IDX] += sum(self.board[0: P1_STORAGE_IDX])
        self.board[P2_STORAGE_IDX] += sum(self.board[P1_STORAGE_IDX + 1: P2_STORAGE_IDX])
        self.board[0: P1_STORAGE_IDX] = 0
        self.board[P1_STORAGE_IDX + 1: P2_STORAGE_IDX] = 0

        print(self)
        if self.board[P1_STORAGE_IDX] > self.board[P2_STORAGE_IDX]:
            print("Player 1 has won!")
        elif self.board[P1_STORAGE_IDX] < self.board[P2_STORAGE_IDX]:
            print("Player 2 has won!")
        else:
            print("Game has been tied!")

    def get_curr_indices(self):
        return list(range(0, P1_STORAGE_IDX + 1)) if self.is_p1_turn else list(
            range(P1_STORAGE_IDX + 1, P2_STORAGE_IDX + 1))

    def heuristic1(self):
        return self.board[P1_STORAGE_IDX] - self.board[P2_STORAGE_IDX]

    def heuristic2(self):
        return 8 * self.heuristic1() + 1 * (sum(self.board[0: P1_STORAGE_IDX]) - sum(self.board[P1_STORAGE_IDX + 1:
                                                                                                P2_STORAGE_IDX]))

    def heuristic3(self):
        bonus_move_count = 0
        curr_indices = self.get_curr_indices()
        storage_idx = curr_indices[-1]
        for i in curr_indices[:-1]:
            bonus_move_count += 1 if self.board[i] == storage_idx - i else 0
        # print(bonus_move_count, 'bonus')
        if not self.is_p1_turn:
            bonus_move_count *= -1
        return self.heuristic2() + 1 * bonus_move_count

    def heuristic4(self):
        max_capture = 0
        size = len(self.board)
        curr_indices = self.get_curr_indices()
        storage_idx = curr_indices[-1]
        for idx in curr_indices[:-1]:
            stone_count = self.board[idx]
            if stone_count > 0:
                curr_idx = idx
                while stone_count > 0:
                    curr_idx = (curr_idx + 1) % size
                    if not (curr_idx == ((storage_idx + 7) % size)):
                        stone_count -= 1
                if curr_idx != storage_idx:
                    opposite_idx = (curr_idx + ((storage_idx - curr_idx) * 2)) % size
                    if curr_idx in curr_indices[:-1] and self.board[curr_idx] == 0 and self.board[opposite_idx]:
                        max_capture = max(max_capture, 1 + self.board[opposite_idx])
        if not self.is_p1_turn:
            max_capture *= -1
        return self.heuristic1() + 1 * max_capture

    def heuristic5(self):
        return self.heuristic1() + self.board[0] - self.board[P1_STORAGE_IDX + 1]

    def heuristic6(self):
        if self.board[P2_STORAGE_IDX] >= 5:
            return self.heuristic1() - (self.board[P2_STORAGE_IDX] * 1.5 - self.board[P1_STORAGE_IDX])
        return self.heuristic1()

    def heuristic7(self):
        bonus_move_count = 0
        curr_indices = self.get_curr_indices()
        storage_idx = curr_indices[-1]
        for i in curr_indices[:-1]:
            bonus_move_count += 1 if self.board[i] == storage_idx - i else 0
        # print(bonus_move_count, 'bonus')
        if not self.is_p1_turn:
            bonus_move_count *= -1
        return self.heuristic2() + 2 * bonus_move_count

    def __str__(self):
        s = '+----' * 8 + '+\n'
        s += '|    '
        for i in range(P2_STORAGE_IDX - 1, P1_STORAGE_IDX, -1):
            s += '| ' + '{:2}'.format(self.board[i]) + ' '
        s += '|    |\n'
        s += '| ' + '{:2}'.format(self.board[P2_STORAGE_IDX]) + ' |----' + '+----' * 5 \
             + '| ' + '{:2}'.format(self.board[P1_STORAGE_IDX]) + ' |\n'
        s += '|    '
        for i in range(0, P1_STORAGE_IDX):
            s += '| ' + '{:2}'.format(self.board[i]) + ' '
        s += '|    |\n'
        s += '+----' * 8 + '+\n'
        s += ' ' * 5
        for c in ['a', 'b', 'c', 'd', 'e', 'f']:
            s += '   ' + c + ' '
        s += '\nPlayer ' + str(1 if self.is_p1_turn else 2) + '\'s turn\n'
        return s


def minimax(state, depth, alpha, beta):
    if state.is_game_over():
        return state.board[P1_STORAGE_IDX] - state.board[P2_STORAGE_IDX], -1

    if depth == State.MAX_DEPTH:
        return state.heuristic(), -1

    curr_indices = state.get_curr_indices()
    if state.is_p1_turn:
        best_util_val = -INF
    else:
        best_util_val = INF
    best_idx = -1

    for idx in curr_indices[-2::-1]:
        if state.board[idx]:
            new_state = copy.deepcopy(state)
            new_state.move_stones(idx)
            util_val, i = minimax(new_state, depth + 1, alpha, beta)
            # print('depth ', depth + 1)
            # print('idx ', idx)
            # print('util_val ', util_val)
            # print(new_state)
            if state.is_p1_turn and util_val > best_util_val:
                best_util_val = util_val
                best_idx = idx
                alpha = max(alpha, best_util_val)
                # print(alpha, ' ', beta)
                if alpha >= beta:
                    # print("alpha pruned")
                    break
            elif not state.is_p1_turn and util_val < best_util_val:
                best_util_val = util_val
                best_idx = idx
                beta = min(beta, best_util_val)
                # print(alpha, ' ', beta)
                if beta <= alpha:
                    # print("beta pruned")
                    break
            # best_util_val = best(minimax(new_state, level + 1)[0], best_util_val)
    return best_util_val, best_idx


def main():
    start = time.time()
    for d in range(2, 11):
        State.MAX_DEPTH = d
        print('Depth: ', State.MAX_DEPTH)
        board = np.array([4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0])
        print('Choose player 1 type:  1. Human   2. AI')
        is_p1_human = True if input() == '1' else False
        # is_p1_human = False
        print('Choose who will make the first move: 1. Player 1   2. Player 2')
        is_p1_turn_1st = True if input() == '1' else False
        # is_p1_turn_1st = True
        state = State(board, is_p1_turn_1st)
        while True:
            if state.is_game_over():
                state.show_result()
                break
            print(state)
            if is_p1_human and state.is_p1_turn:
                idx = ord(input()) - ord('a')
                message_code = state.move_stones(idx)
            else:
                State.heuristic = State.heuristic5 if state.is_p1_turn else State.heuristic6
                message_code = state.move_stones(minimax(state, 0, -INF, INF)[1])
            if message_code:
                print(MESSAGES[message_code])
    stop = time.time()
    print('Time: ', stop - start)


if __name__ == "__main__":
    main()
