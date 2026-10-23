"""Desktop-only threshold decision demonstration; never controls an actuator."""
from dataclasses import dataclass
from enum import Enum
from math import isfinite

class BrakeAction(str, Enum):
    NONE = "none"
    PARTIAL = "partial"
    HARD = "hard"
    INVALID = "invalid_sensor"

@dataclass(frozen=True)
class DecisionInput:
    gap_units: float
    requested_speed_units: float

def decide(request: DecisionInput) -> BrakeAction:
    """Project-unit thresholds; not a vehicle safety algorithm."""
    if not (isfinite(request.gap_units) and isfinite(request.requested_speed_units)):
        return BrakeAction.INVALID
    if request.gap_units < 0 or not 0 <= request.requested_speed_units <= 120:
        return BrakeAction.INVALID
    if request.requested_speed_units == 0:
        return BrakeAction.NONE
    if request.gap_units <= 5:
        return BrakeAction.HARD
    if request.gap_units <= 10:
        return BrakeAction.PARTIAL
    return BrakeAction.NONE

if __name__ == "__main__":
    for gap in [30, 10, 7, 5, 2, -1]:
        print(f"gap={gap:>3} => {decide(DecisionInput(gap, 40)).value}")
