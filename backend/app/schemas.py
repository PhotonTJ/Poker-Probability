from typing import Dict, List

from pydantic import BaseModel, Field


class CardOut(BaseModel):
    suit: int
    rank: int
    label: str


class DealResponse(BaseModel):
    cards: List[CardOut]
    hand_rank: str


class SimulateRequest(BaseModel):
    trials: int = Field(default=100_000, ge=1, le=20_000_000)
    seed: int | None = Field(default=None, description="Optional RNG seed for reproducibility")


class SimulateResponse(BaseModel):
    trials: int
    counts: Dict[str, int]
    frequencies: Dict[str, float]
    theoretical: Dict[str, float]
