import math
import sys
import unittest
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "simulation"))
from braking_demo import BrakeAction, DecisionInput, decide

class BrakingDecisionTests(unittest.TestCase):
    def test_clear_path(self): self.assertEqual(decide(DecisionInput(11, 30)), BrakeAction.NONE)
    def test_partial_boundary(self): self.assertEqual(decide(DecisionInput(10, 30)), BrakeAction.PARTIAL)
    def test_partial_between(self): self.assertEqual(decide(DecisionInput(8, 30)), BrakeAction.PARTIAL)
    def test_hard_boundary(self): self.assertEqual(decide(DecisionInput(5, 30)), BrakeAction.HARD)
    def test_hard_below_threshold(self): self.assertEqual(decide(DecisionInput(1, 30)), BrakeAction.HARD)
    def test_stopped(self): self.assertEqual(decide(DecisionInput(1, 0)), BrakeAction.NONE)
    def test_bad_gap(self): self.assertEqual(decide(DecisionInput(-2, 50)), BrakeAction.INVALID)
    def test_nan(self): self.assertEqual(decide(DecisionInput(math.nan, 50)), BrakeAction.INVALID)
    def test_bad_speed(self): self.assertEqual(decide(DecisionInput(30, 200)), BrakeAction.INVALID)
if __name__ == "__main__":
    unittest.main()
