import React, { useState } from 'react';
import Header from './components/Header';
import SearchBar from './components/SearchBar';
import SearchResults from './components/SearchResults';
import FilterOptions from './components/FilterOptions';
import Footer from './components/Footer';
import { SearchFilters, SearchResult } from './types';

function App() {
  const [query, setQuery] = useState('');
  const [results, setResults] = useState<SearchResult[]>([]);
  const [isSearching, setIsSearching] = useState(false);
  const [showFilters, setShowFilters] = useState(false);
  const [filters, setFilters] = useState<SearchFilters>({
    type: 'all',
    status: 'all',
    category: null,
    sortBy: 'relevance'
  });

  const handleSearch = (searchQuery: string) => {
    setQuery(searchQuery);
    setIsSearching(true);

    fetch(`/api/search?query=${encodeURIComponent(searchQuery)}`)
      .then(res => res.json())
      .then(data => {
        setResults(data);
        setIsSearching(false);
      })
      .catch(error => {
        console.error('Search failed:', error);
        setIsSearching(false);
      });
  };

  return (
    <div className="min-h-screen flex flex-col">
      <Header />
      
      <main className="flex-1">
        <div className="w-full max-w-5xl mx-auto px-4 pt-12 pb-16">
          <div className="text-center mb-10">
            <h2 className="text-2xl font-medium text-white mb-2">
              Explore the Invisible Web
            </h2>
            <p className="text-white/60">
              Search and discover hidden services on the Tor network and SN domains
            </p>
          </div>
          
          <SearchBar onSearch={handleSearch} onToggleFilters={() => setShowFilters(true)} />
          
          <SearchResults 
            results={results} 
            query={query} 
            filters={filters}
            isSearching={isSearching}
          />
        </div>
      </main>
      
      <FilterOptions 
        filters={filters} 
        setFilters={setFilters} 
        isOpen={showFilters} 
        onClose={() => setShowFilters(false)} 
      />
      <Footer />
    </div>
  );
}

export default App;