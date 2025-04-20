import axios from 'axios';
import { SocksProxyAgent } from 'socks-proxy-agent';
import * as cheerio from 'cheerio';
import { redis } from '../utils/redis.js';
import { URLFrontier } from '../urlFrontier/index.js';

export class Crawler {
  private agent: SocksProxyAgent;
  private urlFrontier: URLFrontier;

  constructor() {
    // Configure Tor SOCKS proxy
    this.agent = new SocksProxyAgent('socks5h://127.0.0.1:9050');
    this.urlFrontier = new URLFrontier();
  }

  // Public method to safely access the next URL from the frontier
  public async getNextUrlToCrawl(): Promise<string | null> {
    return this.urlFrontier.getNextURL();
  }

  async crawl(url: string): Promise<void> {
    try {
      const response = await axios.get(url, {
        httpAgent: this.agent,
        httpsAgent: this.agent,
        timeout: 30000
      });

      const $ = cheerio.load(response.data);
      const pageData = {
        url,
        title: $('title').text(),
        description: $('meta[name="description"]').attr('content') || '',
        content: $('body').text(),
        links: [] as string[],
        lastCrawled: new Date().toISOString()
      };

      // Extract links
      $('a').each((_, element) => {
        const href = $(element).attr('href');
        if (href && (href.endsWith('.onion') || href.endsWith('.sn'))) {
          pageData.links.push(href);
          this.urlFrontier.addURL(href);
        }
      });

      // Store in Redis
      // Convert the pageData object to a structure suitable for hSet
      // A simple approach is to store the whole object as a JSON string in one field.
      // Or store primitives directly and stringify complex fields like arrays.
      // Let's store the whole object as a JSON string field named 'data'.
      const dataToStore = { data: JSON.stringify(pageData) };
      await redis.hSet(`page:${url}`, dataToStore);
      
    } catch (error) {
      console.error(`Error crawling ${url}:`, error);
      await redis.hSet(`page:${url}`, 'status', 'error');
    }
  }
}