import { createClient } from 'redis';

// Create Redis client with connection handling
export const redis = createClient({
  url: process.env.REDIS_URL || 'redis://localhost:6379'
});

// In-memory fallback when Redis is unavailable
const fallbackCache = new Map();

redis.on('error', err => {
  console.warn('Redis connection failed, using in-memory fallback:', err.message);
});

// Wrap Redis operations with fallback
export const redisWrapper = {
  async get(key: string): Promise<string | null> {
    try {
      if (!redis.isOpen) {
        return fallbackCache.get(key) || null;
      }
      return await redis.get(key);
    } catch (err) {
      return fallbackCache.get(key) || null;
    }
  },

  async set(key: string, value: string): Promise<void> {
    try {
      if (!redis.isOpen) {
        fallbackCache.set(key, value);
        return;
      }
      await redis.set(key, value);
    } catch (err) {
      fallbackCache.set(key, value);
    }
  },

  async del(key: string): Promise<void> {
    try {
      if (!redis.isOpen) {
        fallbackCache.delete(key);
        return;
      }
      await redis.del(key);
    } catch (err) {
      fallbackCache.delete(key);
    }
  }
};

// Attempt to connect but don't throw if it fails
redis.connect().catch(() => {
  console.warn('Redis connection failed, using in-memory fallback storage');
});