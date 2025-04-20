export interface SearchResult {
  id: string;
  url: string;
  title: string;
  description: string;
  lastSeen: string;
  status: 'online' | 'offline' | 'unknown';
  type: 'onion' | 'sn';
  category?: string;
  screenshotUrl?: string;
}

export interface SearchFilters {
  type: 'all' | 'onion' | 'sn';
  status: 'all' | 'online' | 'offline';
  category: string | null;
  sortBy: 'relevance' | 'lastSeen' | 'popularity';
}