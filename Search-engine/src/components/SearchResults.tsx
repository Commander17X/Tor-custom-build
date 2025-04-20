import React from 'react';
import { SearchResult, SearchFilters } from '../types';
import ResultItem from './ResultItem';
import { AlertTriangle } from 'lucide-react';

interface SearchResultsProps {
  results: SearchResult[];
  query: string;
  filters: SearchFilters;
  isSearching: boolean;
}

const SearchResults: React.FC<SearchResultsProps> = ({ 
  results, 
  query, 
  filters,
  isSearching 
}) => {
  if (isSearching) {
    return (
      <div className="w-full max-w-3xl mx-auto mt-8 p-8 flex flex-col items-center">
        <div className="animate-pulse flex flex-col items-center">
          <div className="h-6 w-64 bg-purple-900/50 rounded mb-8"></div>
          <div className="space-y-6 w-full">
            {[1, 2, 3].map(i => (
              <div key={i} className="bg-purple-900/30 rounded-xl p-6 h-40 w-full"></div>
            ))}
          </div>
        </div>
      </div>
    );
  }
  
  if (!query) {
    return (
      <div className="w-full max-w-3xl mx-auto mt-16 p-8 text-center">
        <p className="text-purple-300 text-lg">
          Enter a search query to find .onion and .sn sites
        </p>
      </div>
    );
  }
  
  if (results.length === 0) {
    return (
      <div className="w-full max-w-3xl mx-auto mt-8 p-8 flex flex-col items-center text-center">
        <AlertTriangle className="h-12 w-12 text-yellow-400 mb-4" />
        <h3 className="text-xl font-semibold text-white mb-2">No results found</h3>
        <p className="text-purple-300">
          We couldn't find any sites matching "{query}" with your current filters.
        </p>
        <div className="mt-4 bg-purple-900/20 rounded-lg p-4 text-sm text-purple-300 max-w-lg">
          <p className="font-medium text-indigo-300 mb-1">Suggestions:</p>
          <ul className="list-disc pl-5 space-y-1">
            <li>Check for spelling errors</li>
            <li>Try more general keywords</li>
            <li>Adjust your filters</li>
            <li>Try a different search term</li>
          </ul>
        </div>
      </div>
    );
  }
  
  // Apply filters to results
  const filteredResults = results.filter(result => {
    if (filters.type !== 'all' && result.type !== filters.type) return false;
    if (filters.status !== 'all' && result.status !== filters.status) return false;
    if (filters.category && result.category !== filters.category) return false;
    return true;
  });
  
  // Sort results
  const sortedResults = [...filteredResults].sort((a, b) => {
    if (filters.sortBy === 'lastSeen') {
      return new Date(b.lastSeen).getTime() - new Date(a.lastSeen).getTime();
    }
    return 0; // Default to the order they come in (relevance)
  });
  
  return (
    <div className="w-full max-w-3xl mx-auto mt-8">
      <div className="text-sm text-purple-400 px-4 mb-4">
        Found {filteredResults.length} results for "{query}"
        {(filters.type !== 'all' || filters.status !== 'all' || filters.category) && (
          <span> (filtered)</span>
        )}
      </div>
      
      <div className="space-y-4">
        {sortedResults.map((result) => (
          <ResultItem key={result.id} result={result} />
        ))}
      </div>
    </div>
  );
};

export default SearchResults;