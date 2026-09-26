import { useState } from 'react'
import { dealHand, type CardOut } from '../api'
import { PlayingCard } from './Card'

export function HandDisplay() {
  const [cards, setCards] = useState<CardOut[]>([])
  const [handRank, setHandRank] = useState<string | null>(null)
  const [loading, setLoading] = useState(false)
  const [error, setError] = useState<string | null>(null)

  async function handleDeal() {
    setLoading(true)
    setError(null)
    try {
      const res = await dealHand()
      setCards(res.cards)
      setHandRank(res.hand_rank)
    } catch {
      setError('Could not reach the backend. Is the API running?')
    } finally {
      setLoading(false)
    }
  }

  return (
    <section className="panel">
      <div className="panel-header">
        <h2>Deal a Hand</h2>
        <button onClick={handleDeal} disabled={loading}>
          {loading ? 'Dealing…' : 'Deal 5 Cards'}
        </button>
      </div>
      {error && <p className="error">{error}</p>}
      {cards.length > 0 && (
        <>
          <div className="card-row">
            {cards.map((c, i) => (
              <PlayingCard key={i} suit={c.suit} rank={c.rank} />
            ))}
          </div>
          <span className="hand-rank-badge">{handRank}</span>
        </>
      )}
    </section>
  )
}
