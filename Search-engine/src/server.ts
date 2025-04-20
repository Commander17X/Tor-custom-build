import express from 'express';
import { Crawler } from './crawler/index.js';
import { Indexer } from './indexer/index.js';
import { QueryProcessor } from './queryProcessor/index.js';
import path from 'path';

const app = express();
const port = process.env.PORT || 3000;

app.use(express.json());
app.use(express.static('dist'));

const crawler = new Crawler();
const indexer = new Indexer();
const queryProcessor = new QueryProcessor();

// Start crawler process
async function startCrawler() {
  while (true) {
    const url = await crawler.getNextUrlToCrawl();
    if (url) {
      await crawler.crawl(url);
      await indexer.indexPage(url);
    }
    await new Promise(resolve => setTimeout(resolve, 5000));
  }
}

// Search endpoint
app.get('/api/search', async (req, res) => {
  try {
    const { query, ...filters } = req.query;
    const results = await queryProcessor.search(query as string, filters);
    res.json(results);
  } catch (error) {
    res.status(500).json({ error: 'Search failed' });
  }
});

// Crawl endpoint (protected)
app.post('/api/crawl', async (req, res) => {
  try {
    const { url } = req.body;
    await crawler.crawl(url);
    await indexer.indexPage(url);
    res.json({ status: 'success' });
  } catch (error) {
    res.status(500).json({ error: 'Crawl failed' });
  }
});

// Serve the frontend
app.get('*', (_req, res) => {
  res.sendFile(path.join(process.cwd(), 'dist', 'index.html'));
});

app.listen(port, () => {
  console.log(`Server running on port ${port}`);
  startCrawler().catch(console.error);
});