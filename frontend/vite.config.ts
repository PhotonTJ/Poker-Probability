import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// In dev, proxy API/WebSocket calls to the FastAPI backend so the frontend
// can just call relative paths ('/api/...', '/ws/...') in both `npm run dev`
// and the production nginx build (see frontend/nginx.conf).
export default defineConfig({
  plugins: [react()],
  server: {
    proxy: {
      '/api': { target: 'http://localhost:8000', changeOrigin: true },
      '/ws': { target: 'ws://localhost:8000', ws: true },
    },
  },
})
