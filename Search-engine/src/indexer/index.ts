import { redis } from '../utils/redis.js';

export class Indexer {
  async indexPage(url: string): Promise<void> {
    const pageData = await redis.hGetAll(`page:${url}`);
    if (!pageData) return;

    const document = {
      url,
      title: pageData.title,
      description: pageData.description,
      content: pageData.content,
      lastIndexed: new Date().toISOString()
    };

    // Store in search index
    await redis.hSet(`index:${url}`, document);
    
    // Update inverted index for search terms
    const terms = this.extractSearchTerms(document);
    for (const term of terms) {
      await redis.sAdd(`term:${term}`, url);
    }
  }

  private extractSearchTerms(document: any): string[] {
    const text = `${document.title} ${document.description} ${document.content}`;
    return text
      .toLowerCase()
      .split(/\W+/)
      .filter(term => term.length > 2);
  }
}