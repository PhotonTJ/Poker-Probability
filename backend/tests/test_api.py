from fastapi.testclient import TestClient

from app.main import app

client = TestClient(app)


def test_health():
    resp = client.get("/api/health")
    assert resp.status_code == 200
    assert resp.json() == {"status": "ok"}


def test_deal_returns_five_valid_cards():
    resp = client.post("/api/deal")
    assert resp.status_code == 200
    body = resp.json()
    assert len(body["cards"]) == 5
    ranks = {(c["suit"], c["rank"]) for c in body["cards"]}
    assert len(ranks) == 5  # no duplicate cards
    assert body["hand_rank"] in {
        "High Card", "One Pair", "Two Pair", "Three of a Kind", "Straight",
        "Flush", "Full House", "Four of a Kind", "Straight Flush", "Royal Flush",
    }


def test_simulate_matches_trial_count_and_sums_correctly():
    resp = client.post("/api/simulate", json={"trials": 50_000, "seed": 123})
    assert resp.status_code == 200
    body = resp.json()
    assert body["trials"] == 50_000
    assert sum(body["counts"].values()) == 50_000
    # Every observed frequency should be in the same ballpark as its
    # theoretical counterpart -- a loose sanity bound, not a statistical
    # proof (that lives in the C++ test suite with its sigma-based bound).
    for category, theoretical_p in body["theoretical"].items():
        observed = body["frequencies"][category]
        assert abs(observed - theoretical_p) < max(0.01, theoretical_p * 3)


def test_simulate_rejects_absurd_trial_counts():
    resp = client.post("/api/simulate", json={"trials": 0})
    assert resp.status_code == 422

    resp = client.post("/api/simulate", json={"trials": 100_000_000})
    assert resp.status_code == 422


def test_websocket_streams_batches_then_a_final_message():
    with client.websocket_connect("/ws/simulate") as ws:
        ws.send_json({"trials": 10_000, "seed": 7})
        messages = []
        while True:
            msg = ws.receive_json()
            messages.append(msg)
            if msg.get("final"):
                break

        assert len(messages) >= 2  # at least one progress update plus the final
        assert all(m["trials"] <= 10_000 for m in messages)
        # trial counts should be non-decreasing across the stream
        trial_counts = [m["trials"] for m in messages]
        assert trial_counts == sorted(trial_counts)
        assert messages[-1]["trials"] == 10_000
        assert messages[-1]["final"] is True
