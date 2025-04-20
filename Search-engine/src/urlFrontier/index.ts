import { redis } from '../utils/redis.js';

export class URLFrontier {
  private readonly queueKey = 'url:queue';
  private readonly visitedKey = 'url:visited';

  async addURL(url: string): Promise<void> {
    const isVisited = await redis.sIsMember(this.visitedKey, url);
    if (!isVisited) {
      await redis.sAdd(this.queueKey, url);
    }
  }

  async getNextURL(): Promise<string | null> {
    const url = await redis.sPop(this.queueKey) as unknown as string | null;
    if (url) {
      await redis.sAdd(this.visitedKey, url);
      return url;
    } else {
      return null;
    }
  }

  async isVisited(url: string): Promise<boolean> {
    return redis.sIsMember(this.visitedKey, url);
  }

  async getQueueSize(): Promise<number> {
    return redis.sCard(this.queueKey);
  }
}