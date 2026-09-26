import { useMemo, useState } from 'react'
import {
  CartesianGrid,
  Line,
  LineChart,
  ReferenceLine,
  ResponsiveContainer,
  Tooltip,
  XAxis,
  YAxis,
} from 'recharts'
import { runSimulation, type SimulateResult } from '../api'

const CATEGORY_ORDER = [
  'High Card',
  'One Pair',
  'Two Pair',
  'Three of a Kind',
  'Straight',
  'Flush',
  'Full House',
  'Four of a Kind',
  'Straight Flush',
  'Royal Flush',
]

// Ten hand categories span six orders of magnitude (High Card ~50% down to
// Royal Flush ~0.00015%), so plotting them together on one linear y-axis
// would flatten every rare category to a line hugging zero. Rather than
// force a shared axis (or add a second one -- never a good idea), the chart
// shows one category's convergence at a time, chosen from the dropdown,
// while the table below covers all ten side by side, which is what tables
// are for.
export function SimulationPanel() {
  const [trials, setTrials] = useState(200_000)
  const [category, setCategory] = useState('Flush')
  const [history, setHistory] = useState<SimulateResult[]>([])
  const [running, setRunning] = useState(false)
  const [error, setError] = useState<string | null>(null)

  async function handleRun() {
    setRunning(true)
    setError(null)
    setHistory([])
    try {
      await runSimulation(trials, (msg) => {
        setHistory((prev) => [...prev, msg])
      })
    } catch {
      setError('Could not reach the backend. Is the API running?')
    } finally {
      setRunning(false)
    }
  }

  const chartData = useMemo(
    () =>
      history.map((h) => ({
        trials: h.trials,
        frequency: h.frequencies[category] * 100,
      })),
    [history, category],
  )

  const finalResult = history.length > 0 ? history[history.length - 1] : null
  const theoreticalPct = finalResult ? finalResult.theoretical[category] * 100 : null

  return (
    <section className="panel">
      <div className="panel-header">
        <h2>Monte Carlo Convergence</h2>
      </div>

      <div className="controls-row">
        <label>
          Trials
          <input
            type="number"
            min={1000}
            max={5_000_000}
            step={1000}
            value={trials}
            onChange={(e) => setTrials(Number(e.target.value))}
          />
        </label>
        <label>
          Category
          <select value={category} onChange={(e) => setCategory(e.target.value)}>
            {CATEGORY_ORDER.map((c) => (
              <option key={c} value={c}>
                {c}
              </option>
            ))}
          </select>
        </label>
        <button onClick={handleRun} disabled={running}>
          {running ? 'Running…' : 'Run Simulation'}
        </button>
      </div>

      {error && <p className="error">{error}</p>}

      <div className="chart-wrap">
        <ResponsiveContainer width="100%" height={280}>
          <LineChart data={chartData} margin={{ top: 8, right: 16, left: 0, bottom: 0 }}>
            <CartesianGrid stroke="var(--gridline)" vertical={false} />
            <XAxis
              dataKey="trials"
              stroke="var(--muted)"
              tick={{ fill: 'var(--muted)', fontSize: 12 }}
              tickFormatter={(v: number) => (v >= 1000 ? `${Math.round(v / 1000)}k` : String(v))}
            />
            <YAxis
              stroke="var(--muted)"
              tick={{ fill: 'var(--muted)', fontSize: 12 }}
              width={64}
              tickFormatter={(v: number) => `${v.toFixed(v < 1 ? 3 : 1)}%`}
            />
            <Tooltip
              formatter={(value: number) => [`${value.toFixed(4)}%`, 'Empirical']}
              labelFormatter={(v: number) => `${v.toLocaleString()} trials`}
              contentStyle={{
                background: 'var(--surface)',
                border: '1px solid var(--border)',
                borderRadius: 8,
                color: 'var(--text-primary)',
              }}
            />
            {theoreticalPct !== null && (
              <ReferenceLine
                y={theoreticalPct}
                stroke="var(--muted)"
                strokeDasharray="4 4"
                label={{
                  value: `theoretical ${theoreticalPct.toFixed(4)}%`,
                  position: 'insideTopRight',
                  fill: 'var(--text-secondary)',
                  fontSize: 12,
                }}
              />
            )}
            <Line
              type="monotone"
              dataKey="frequency"
              stroke="var(--series-1)"
              strokeWidth={2}
              dot={false}
              isAnimationActive={false}
            />
          </LineChart>
        </ResponsiveContainer>
      </div>

      {finalResult && (
        <table className="results-table">
          <thead>
            <tr>
              <th>Hand</th>
              <th>Count</th>
              <th>Empirical</th>
              <th>Theoretical</th>
            </tr>
          </thead>
          <tbody>
            {CATEGORY_ORDER.map((c) => (
              <tr key={c} className={c === category ? 'highlighted' : undefined}>
                <td>{c}</td>
                <td>{finalResult.counts[c].toLocaleString()}</td>
                <td>{(finalResult.frequencies[c] * 100).toFixed(4)}%</td>
                <td>{(finalResult.theoretical[c] * 100).toFixed(4)}%</td>
              </tr>
            ))}
          </tbody>
        </table>
      )}
    </section>
  )
}
