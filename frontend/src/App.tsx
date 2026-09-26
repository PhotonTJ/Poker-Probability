import { HandDisplay } from './components/HandDisplay'
import { SimulationPanel } from './components/SimulationPanel'

export default function App() {
  return (
    <div className="app">
      <header className="app-header">
        <h1>Poker Probability</h1>
        <p className="subtitle">
          5-card stud hand classification and Monte Carlo simulation, backed by a C++ engine
          exposed through pybind11.
        </p>
      </header>
      <main className="app-main">
        <HandDisplay />
        <SimulationPanel />
      </main>
    </div>
  )
}
