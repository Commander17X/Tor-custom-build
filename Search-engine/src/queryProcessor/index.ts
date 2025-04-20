import { redis } from '../utils/redis.js';
import { SearchResult } from '../types';

export class QueryProcessor {
  async search(query: string, filters: any = {}): Promise<SearchResult[]> {
    const terms = query.toLowerCase().split(/\W+/).filter(term => term.length > 2);
    const results = new Map<string, number>();

    // Search each term
    for (const term of terms) {
      const urls = await redis.sMembers(`term:${term}`);
      for (const url of urls) {
        results.set(url, (results.get(url) || 0) + 1);
      }
    }

    // Get full documents and sort by relevance
    const documents = await Promise.all(
      Array.from(results.entries()).map(async ([url]) => {
        const doc = await redis.hGetAll(`index:${url}`);
        return {
          id: url,
          url,
          title: doc.title,
          description: doc.description,
          lastSeen: doc.lastIndexed,
          status: 'online',
          type: url.endsWith('.onion') ? 'onion' : 'sn'
        } as SearchResult;
      })
    );

    // Apply filters
    return this.applyFilters(documents, filters);
  }

  private applyFilters(results: SearchResult[], filters: any): SearchResult[] {
    return results.filter(result => {
      if (filters.type && filters.type !== 'all' && result.type !== filters.type) {
        return false;
      }
      if (filters.status && filters.status !== 'all' && result.status !== filters.status) {
        return false;
      }
      return true;
    });
  }
}