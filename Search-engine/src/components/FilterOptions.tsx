import React from 'react';
import { SearchFilters } from '../types';
import { categories } from '../utils/mockData';
import { Check, X } from 'lucide-react';

interface FilterOptionsProps {
  filters: SearchFilters;
  setFilters: React.Dispatch<React.SetStateAction<SearchFilters>>;
  isOpen: boolean;
  onClose: () => void;
}

const FilterOptions: React.FC<FilterOptionsProps> = ({ 
  filters, 
  setFilters, 
  isOpen, 
  onClose 
}) => {
  if (!isOpen) return null;
  
  return (
    <div className="fixed inset-0 bg-black/80 backdrop-blur-sm z-50 flex items-center justify-center">
      <div className="w-full max-w-md card p-6 animate-fade-in">
        <div className="flex items-center justify-between mb-6">
          <h3 className="text-lg font-medium text-white">Search Filters</h3>
          <button 
            onClick={onClose}
            className="text-white/40 hover:text-white transition-colors"
          >
            <X className="h-5 w-5" />
          </button>
        </div>
        
        <div className="space-y-6">
          {/* Domain Type */}
          <div>
            <label className="block text-sm font-medium text-white/60 mb-3">Domain Type</label>
            <div className="flex gap-2">
              {['all', 'onion', 'sn'].map((type) => (
                <button
                  key={type}
                  onClick={() => setFilters(prev => ({ ...prev, type: type as any }))}
                  className={`px-3 py-1.5 rounded-lg flex items-center gap-1.5 ${
                    filters.type === type 
                      ? 'bg-indigo-600 text-white' 
                      : 'bg-white/5 text-white/60 hover:bg-white/10 hover:text-white'
                  } transition-colors`}
                >
                  {filters.type === type && <Check className="h-3.5 w-3.5" />}
                  {type === 'all' ? 'All Domains' : `.${type}`}
                </button>
              ))}
            </div>
          </div>
          
          {/* Status */}
          <div>
            <label className="block text-sm font-medium text-white/60 mb-3">Site Status</label>
            <div className="flex gap-2">
              {['all', 'online', 'offline'].map((status) => (
                <button
                  key={status}
                  onClick={() => setFilters(prev => ({ ...prev, status: status as any }))}
                  className={`px-3 py-1.5 rounded-lg flex items-center gap-1.5 ${
                    filters.status === status 
                      ? 'bg-indigo-600 text-white' 
                      : 'bg-white/5 text-white/60 hover:bg-white/10 hover:text-white'
                  } transition-colors`}
                >
                  {filters.status === status && <Check className="h-3.5 w-3.5" />}
                  {status === 'all' ? 'All Status' : status}
                </button>
              ))}
            </div>
          </div>
          
          {/* Categories */}
          <div>
            <label className="block text-sm font-medium text-white/60 mb-3">Category</label>
            <div className="grid grid-cols-3 gap-2 max-h-48 overflow-y-auto pr-2 custom-scrollbar">
              {categories.map((category) => (
                <button
                  key={category}
                  onClick={() => setFilters(prev => ({ 
                    ...prev, 
                    category: category === 'All' ? null : category 
                  }))}
                  className={`px-3 py-1.5 rounded-lg flex items-center gap-1.5 ${
                    (category === 'All' && filters.category === null) || 
                    filters.category === category
                      ? 'bg-indigo-600 text-white' 
                      : 'bg-white/5 text-white/60 hover:bg-white/10 hover:text-white'
                  } transition-colors`}
                >
                  {((category === 'All' && filters.category === null) || 
                    filters.category === category) && <Check className="h-3.5 w-3.5" />}
                  {category}
                </button>
              ))}
            </div>
          </div>
          
          {/* Sort By */}
          <div>
            <label className="block text-sm font-medium text-white/60 mb-3">Sort Results By</label>
            <div className="flex gap-2">
              {[
                { value: 'relevance', label: 'Relevance' },
                { value: 'lastSeen', label: 'Last Seen' },
                { value: 'popularity', label: 'Popularity' }
              ].map((option) => (
                <button
                  key={option.value}
                  onClick={() => setFilters(prev => ({ ...prev, sortBy: option.value as any }))}
                  className={`px-3 py-1.5 rounded-lg flex items-center gap-1.5 ${
                    filters.sortBy === option.value 
                      ? 'bg-indigo-600 text-white' 
                      : 'bg-white/5 text-white/60 hover:bg-white/10 hover:text-white'
                  } transition-colors`}
                >
                  {filters.sortBy === option.value && <Check className="h-3.5 w-3.5" />}
                  {option.label}
                </button>
              ))}
            </div>
          </div>
        </div>
        
        <div className="mt-8 flex justify-end gap-3">
          <button
            onClick={() => setFilters({
              type: 'all',
              status: 'all',
              category: null,
              sortBy: 'relevance'
            })}
            className="button-secondary"
          >
            Reset
          </button>
          <button
            onClick={onClose}
            className="button-primary"
          >
            Apply Filters
          </button>
        </div>
      </div>
    </div>
  );
};

export default FilterOptions;