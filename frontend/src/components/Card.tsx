const SUIT_SYMBOLS = ['♠', '♥', '♦', '♣'] // spade, heart, diamond, club
const SUIT_IS_RED = [false, true, true, false]
const RANK_LABELS: Record<number, string> = { 1: 'A', 11: 'J', 12: 'Q', 13: 'K' }

function rankLabel(rank: number): string {
  return RANK_LABELS[rank] ?? String(rank)
}

export function PlayingCard({ suit, rank }: { suit: number; rank: number }) {
  const red = SUIT_IS_RED[suit]
  const symbol = SUIT_SYMBOLS[suit]
  const label = rankLabel(rank)
  return (
    <div className={`playing-card ${red ? 'red' : 'black'}`} aria-label={`${label} of suit ${suit}`}>
      <div className="corner top">
        {label}
        {symbol}
      </div>
      <div className="pip">{symbol}</div>
      <div className="corner bottom">
        {label}
        {symbol}
      </div>
    </div>
  )
}
