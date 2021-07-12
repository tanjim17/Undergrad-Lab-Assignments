import unittest
import random
from sort import sort

LIST_MAX_SIZE = 100
TEST_VAL_UPPER_BOUND = 10000
TEST_VAL_LOWER_BOUND = -10000


def has_same_elements(a, b):
    temp = list(b)
    for element in a:
        try:
            temp.remove(element)
        except ValueError:
            return False
    return not temp


def is_sorted(a):
    for i in range(len(a) - 1):
        if a[i] > a[i + 1]:
            return False
    return True


def is_valid(a, sorted_a):
    return has_same_elements(a, sorted_a) and is_sorted(sorted_a)


def generate_list(size=random.randint(0, LIST_MAX_SIZE)):
    random_list = []
    for i in range(size):
        random_list.append(random.uniform(TEST_VAL_LOWER_BOUND, TEST_VAL_UPPER_BOUND))
    return random_list


def generate_sorted_list(is_descending=0):
    random_list = []
    size = random.randint(0, LIST_MAX_SIZE)
    bound_diff = (TEST_VAL_UPPER_BOUND - TEST_VAL_LOWER_BOUND) / size
    lower_bound = TEST_VAL_LOWER_BOUND
    for i in range(size):
        random_list.append(random.uniform(lower_bound, lower_bound + bound_diff))
        lower_bound += bound_diff
    if is_descending:
        random_list.reverse()
    return random_list


def generate_same_element_list():
    return [random.uniform(TEST_VAL_LOWER_BOUND, TEST_VAL_UPPER_BOUND)] * random.randint(0, LIST_MAX_SIZE)


class TestSort(unittest.TestCase):
    def setUp(self):
        self.test_inputs = [[], generate_list(size=1), generate_list(size=2), generate_list(), generate_sorted_list(),
                            generate_sorted_list(is_descending=1), generate_same_element_list()]
        print("Test cases:")
        for i, test_input in enumerate(self.test_inputs, start=1):
            print(str(i) + '.', test_input)

    def test_sort(self):
        for i, test_input in enumerate(self.test_inputs, start=1):
            with self.subTest(msg=i):
                sorted_list = sort(test_input)
                self.assertTrue(is_valid(test_input, sorted_list))
