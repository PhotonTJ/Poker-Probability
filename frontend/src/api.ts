export type CardOut = { suit: number; rank: number; label: string }

export type DealResponse = { cards: CardOut[]; hand_rank: string }

export type SimulateResult = {
  trials: number
  counts: Record<string, number>
  frequencies: Record<string, number>
  theoretical: Record<string, number>
  final?: boolean
}

export async function dealHand(): Promise<DealResponse> {
  const res = await fetch('/api/deal', { method: 'POST' })
  if (!res.ok) throw new Error(`deal failed: ${res.status}`)
  return res.json() as Promise<DealResponse>
}

function wsUrl(path: string): string {
  const proto = window.location.protocol === 'https:' ? 'wss' : 'ws'
  return `${proto}://${window.location.host}${path}`
}

// Opens a WebSocket to the backend, streams periodic progress via
// `onProgress`, and resolves with the final result once the backend marks a
// message `final: true`.
export function runSimulation(
  trials: number,
  onProgress: (r: SimulateResult) => void,
): Promise<SimulateResult> {
  return new Promise((resolve, reject) => {
    const ws = new WebSocket(wsUrl('/ws/simulate'))
    ws.onopen = () => ws.send(JSON.stringify({ trials }))
    ws.onerror = () => reject(new Error('WebSocket error'))
    ws.onmessage = (ev) => {
      const msg = JSON.parse(ev.data) as SimulateResult
      onProgress(msg)
      if (msg.final) {
        resolve(msg)
        ws.close()
      }
    }
  })
}
