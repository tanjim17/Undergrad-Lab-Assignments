import numpy as np
import copy
import sys

P1_STORAGE_IND = 6
P2_STORAGE_IND = 13
# MAX_DEPTH = 4
INF = sys.maxsize

MESSAGES = {1: 'bonus turn!', 2: 'captured!', 3: 'bin is empty!'}


class State:
    heuristic = None
    MAX_DEPTH = 0

    def __init__(self, board, is_p1_turn):
        self.board = board
        self.is_p1_turn = is_p1_turn

    def move_stones(self, ind):  # returns message code i.e. key for MESSAGES
        if self.board[ind] == 0:
            return 3
        else:
            size = len(self.board)
            curr_indices = self.get_curr_indices()
            storage_ind = curr_indices[-1]
            curr_ind = ind
            while self.board[ind] > 0:
                curr_ind = (curr_ind + 1) % size
                if not (curr_ind == ((storage_ind + 7) % size)):  # skip opponent's storage
                    self.board[curr_ind] += 1
                    self.board[ind] -= 1

            if curr_ind == storage_ind:
                return 1
            else:
                self.is_p1_turn = not self.is_p1_turn
                # attempt to capture
                opposite_ind = (curr_ind + ((storage_ind - curr_ind) * 2)) % size
                if curr_ind in curr_indices[:-1] and self.board[curr_ind] == 1 and self.board[opposite_ind]:
                    self.board[storage_ind] += self.board[curr_ind] + self.board[opposite_ind]
                    self.board[curr_ind] = 0
                    self.board[opposite_ind] = 0
                    return 2
                return 0

    def is_game_over(self):
        for i in range(0, P1_STORAGE_IND):
            if self.board[i] != 0:
                for j in range(P1_STORAGE_IND + 1, P2_STORAGE_IND):
                    if self.board[j] != 0:
                        return False
                return True
        return True

    def show_result(self):
        # adding the remaining stones to corresponding storage
        self.board[P1_STORAGE_IND] += sum(self.board[0: P1_STORAGE_IND])
        self.board[P2_STORAGE_IND] += sum(self.board[P1_STORAGE_IND + 1: P2_STORAGE_IND])
        self.board[0: P1_STORAGE_IND] = 0
        self.board[P1_STORAGE_IND + 1: P2_STORAGE_IND] = 0

        print(self)
        if self.board[P1_STORAGE_IND] > self.board[P2_STORAGE_IND]:
            print("Player 1 has won!")
        elif self.board[P1_STORAGE_IND] < self.board[P2_STORAGE_IND]:
            print("Player 2 has won!")
        else:
            print("Game has been tied!")

    def get_curr_indices(self):
        return list(range(0, P1_STORAGE_IND + 1)) if self.is_p1_turn else list(
            range(P1_STORAGE_IND + 1, P2_STORAGE_IND + 1))

    def heuristic1(self):
        return self.board[P1_STORAGE_IND] - self.board[P2_STORAGE_IND]

    def heuristic2(self):
        return 40 * self.heuristic1() + 5 * (sum(self.board[0: P1_STORAGE_IND]) - sum(self.board[P1_STORAGE_IND + 1:
                                                                                                P2_STORAGE_IND]))

    def heuristic3(self):
        bonus_move_count = 0
        curr_indices = self.get_curr_indices()
        storage_ind = curr_indices[-1]
        for i in curr_indices[:-1]:
            bonus_move_count += 1 if self.board[i] == storage_ind - i else 0
        # print(bonus_move_count, 'bonus')
        if not self.is_p1_turn:
            bonus_move_count *= -1
        return self.heuristic2() + 10 * bonus_move_count

    def heuristic4(self):
        capture_count = 0
        curr_indices = self.get_curr_indices()
        for ind in curr_indices[:-1]:
            state = copy.deepcopy(self)
            if state.move_stones(ind) == 2:
                capture_count += 1
        if not self.is_p1_turn:
            capture_count *= -1
        return self.heuristic2() + 10 * capture_count

    def heuristic5(self):
        bonus_move_count = 0
        curr_indices = self.get_curr_indices()
        storage_ind = curr_indices[-1]
        for i in curr_indices[:-1]:
            bonus_move_count += 1 if self.board[i] == storage_ind - i else 0
        # print(bonus_move_count, 'bonus')
        if not self.is_p1_turn:
            bonus_move_count *= -1
        return self.heuristic2() + 20 * bonus_move_count

    def heuristic6(self):
        pass

    def __str__(self):
        s = '+----' * 8 + '+\n'
        s += '|    '
        for i in range(P2_STORAGE_IND - 1, P1_STORAGE_IND, -1):
            s += '| ' + '{:2}'.format(self.board[i]) + ' '
        s += '|    |\n'
        s += '| ' + '{:2}'.format(self.board[P2_STORAGE_IND]) + ' |----' + '+----' * 5 \
             + '| ' + '{:2}'.format(self.board[P1_STORAGE_IND]) + ' |\n'
        s += '|    '
        for i in range(0, P1_STORAGE_IND):
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
        return state.board[P1_STORAGE_IND] - state.board[P2_STORAGE_IND], -1

    if depth == State.MAX_DEPTH:
        return state.heuristic(), -1

    curr_indices = state.get_curr_indices()
    if state.is_p1_turn:
        best_util_val = -INF
    else:
        best_util_val = INF
    best_ind = -1

    for ind in curr_indices[:-1]:
        if state.board[ind]:
            new_state = copy.deepcopy(state)
            new_state.move_stones(ind)
            util_val, i = minimax(new_state, depth + 1, alpha, beta)
            # print('level ', depth + 1)
            # print('ind ', ind)
            # print('util_val ', util_val)
            # print(new_state)
            if state.is_p1_turn and util_val > best_util_val:
                best_util_val = util_val
                best_ind = ind
                alpha = max(alpha, best_util_val)
                # print(alpha, ' ', beta)
                if alpha >= beta:
                    # print("alpha pruned")
                    break
            elif not state.is_p1_turn and util_val < best_util_val:
                best_util_val = util_val
                best_ind = ind
                beta = min(beta, best_util_val)
                # print(alpha, ' ', beta)
                if beta <= alpha:
                    # print("beta pruned")
                    break
            # best_util_val = best(minimax(new_state, level + 1)[0], best_util_val)
    return best_util_val, best_ind


def main():
    for d in range(2, 11):
        State.MAX_DEPTH = d
        print(State.MAX_DEPTH)
        board = np.array([4, 4, 4, 4, 4, 4, 0, 4, 4, 4, 4, 4, 4, 0])
        # print('Choose player 1 type:  1. Human   2. AI')
        # is_p1_human = True if input() == '1' else False
        is_p1_human = False
        # print('Choose who will make the first move: 1. Player 1   2. Player 2')
        # is_p1_turn_1st = True if input() == '1' else False
        is_p1_turn_1st = True
        state = State(board, is_p1_turn_1st)
        while True:
            if state.is_game_over():
                state.show_result()
                break
            # print(state)
            if is_p1_human and state.is_p1_turn:
                ind = ord(input()) - ord('a')
                message_code = state.move_stones(ind)
            else:
                State.heuristic = State.heuristic3 if state.is_p1_turn else State.heuristic1
                # print("yo", minimax(state, 0)[1])
                message_code = state.move_stones(minimax(state, 0, -INF, INF)[1])
            # if message_code:
            #     print(MESSAGES[message_code])


if __name__ == "__main__":
    main()
