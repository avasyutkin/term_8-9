import unittest
import main

data_input = [7, 9]
data_output = {0, 1, 2, 4}


class MyTestCase(unittest.TestCase):
    def test(self):
        result = main.quadratic_residues(data_input[0])
        self.assertEqual(result, data_output)


if __name__ == '__main__':
    unittest.main()
