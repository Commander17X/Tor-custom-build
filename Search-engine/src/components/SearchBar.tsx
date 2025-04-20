import React, { useState } from 'react';
import { Search, X, Settings } from 'lucide-react';

interface SearchBarProps {
  onSearch: (query: string) => void;
  onToggleFilters: () => void;
}

const SearchBar: React.FC<SearchBarProps> = ({ onSearch, onToggleFilters }) => {
  const [query, setQuery] = useState('');
  const [focused, setFocused] = useState(false);
  
  const handleSearch = (e: React.FormEvent) => {
    e.preventDefault();
    if (query.trim()) {
      onSearch(query);
    }
  };

  return (
    <div className={`w-full max-w-3xl mx-auto transition-transform duration-200 ${focused ? 'scale-[1.02]' : 'scale-100'}`}>
      <form onSubmit={handleSearch} className="relative">
        <div className="relative flex items-center">
          <div className="absolute left-4 text-white/40">
            <Search className="h-5 w-5" />
          </div>
          
          <input
            type="text"
            value={query}
            onChange={(e) => setQuery(e.target.value)}
            onFocus={() => setFocused(true)}
            onBlur={() => setFocused(false)}
            placeholder="Search .onion and .sn sites..."
            className="w-full h-12 pl-12 pr-32 rounded-xl bg-white/[0.03] border border-white/[0.05] focus:border-indigo-500/50 text-white placeholder-white/40 focus:ring-1 focus:ring-indigo-500/25 outline-none transition-all"
          />
          
          <div className="absolute right-2 flex items-center gap-2">
            {query && (
              <button 
                type="button" 
                onClick={() => setQuery('')}
                className="p-2 text-white/40 hover:text-white transition-colors"
              >
                <X className="h-4 w-4" />
              </button>
            )}
            
            <button 
              type="button" 
              onClick={onToggleFilters}
              className="p-2 text-white/40 hover:text-white transition-colors"
            >
              <Settings className="h-4 w-4" />
            </button>
            
            <button 
              type="submit" 
              className="button-primary"
            >
              Search
            </button>
          </div>
        </div>
      </form>
      
      <div className="mt-3 flex gap-3 px-1 text-sm">
        <span className="text-white/40">Popular:</span>
        {['forums', 'markets', 'news'].map(term => (
          <button 
            key={term}
            onClick={() => {setQuery(term); onSearch(term);}}
            className="text-white/60 hover:text-white transition-colors"
          >
            {term}
          </button>
        ))}
      </div>
    </div>
  );
};

export default SearchBar;