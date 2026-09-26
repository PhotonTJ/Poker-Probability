"""FastAPI backend for Poker-Probability.

Thin wrapper around the compiled `poker_engine` pybind11 module (built from
`engine/`, see engine/build.sh). Every route delegates the actual card
logic to that C++ engine -- this file's job is HTTP/WebSocket plumbing:
request validation, offloading the CPU-bound simulation onto a worker
thread so the asyncio event loop is never blocked, and streaming progress
back to the frontend.
"""
import asyncio
from typing import Optional

from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware

from . import poker_engine as pe  # compiled pybind11 extension module
from .schemas import DealResponse, SimulateRequest, SimulateResponse

app = FastAPI(
    title="Poker-Probability API",
    description=(
        "Deals and classifies 5-card stud hands, and runs Monte Carlo "
        "simulations over the C++ poker engine, streaming convergence "
        "toward the closed-form combinatorial probabilities."
    ),
    version="1.0.0",
)

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)


@app.get("/api/health")
def health() -> dict:
    return {"status": "ok"}


@app.post("/api/deal", response_model=DealResponse)
def deal() -> DealResponse:
    deck = pe.Deck()
    deck.shuffle()
    cards = deck.deal(5)
    hand = pe.Hand([(c["suit"], c["rank"]) for c in cards])
    return DealResponse(cards=cards, hand_rank=hand.classify_name())


@app.post("/api/simulate", response_model=SimulateResponse)
async def simulate(req: SimulateRequest) -> SimulateResponse:
    sim = pe.MonteCarloSimulator() if req.seed is None else pe.MonteCarloSimulator(seed=req.seed)
    # Offload to a worker thread: the pybind11 binding releases the GIL for
    # the C++ loop, so this does not stall other requests while it runs.
    result = await asyncio.to_thread(sim.run, req.trials, 0, None)
    return SimulateResponse(**result)


@app.websocket("/ws/simulate")
async def ws_simulate(websocket: WebSocket) -> None:
    await websocket.accept()
    try:
        params = await websocket.receive_json()
        trials = int(params.get("trials", 100_000))
        trials = max(1, min(trials, 20_000_000))
        seed: Optional[int] = params.get("seed")
        # ~200 progress updates over the run, regardless of trial count.
        batch_size = max(1, trials // 200)

        loop = asyncio.get_running_loop()
        queue: asyncio.Queue = asyncio.Queue()

        def on_batch(result: dict) -> None:
            # Runs on the worker thread; hand off to the event loop safely.
            loop.call_soon_threadsafe(queue.put_nowait, {"final": False, **result})

        async def run_simulation() -> None:
            sim = pe.MonteCarloSimulator() if seed is None else pe.MonteCarloSimulator(seed=seed)
            result = await asyncio.to_thread(sim.run, trials, batch_size, on_batch)
            loop.call_soon_threadsafe(queue.put_nowait, {"final": True, **result})

        sim_task = asyncio.create_task(run_simulation())

        while True:
            message = await queue.get()
            await websocket.send_json(message)
            if message.get("final"):
                break

        await sim_task
    except WebSocketDisconnect:
        pass
