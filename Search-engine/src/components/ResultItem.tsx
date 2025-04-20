import React, { useState } from 'react';
import { SearchResult } from '../types';
import { ExternalLink, Clock, Bookmark, Share2, Info } from 'lucide-react';

interface ResultItemProps {
  result: SearchResult;
}

const ResultItem: React.FC<ResultItemProps> = ({ result }) => {
  const [isBookmarked, setIsBookmarked] = useState(false);
  const [showPreview, setShowPreview] = useState(false);
  
  const formatDate = (dateString: string) => {
    const date = new Date(dateString);
    return date.toLocaleDateString('en-US', { 
      month: 'short', 
      day: 'numeric', 
      year: 'numeric',
      hour: '2-digit',
      minute: '2-digit'
    });
  };
  
  const getStatusColor = (status: string) => {
    switch(status) {
      case 'online': return 'text-green-400 bg-green-400/10';
      case 'offline': return 'text-red-400 bg-red-400/10';
      default: return 'text-yellow-400 bg-yellow-400/10';
    }
  };
  
  const getDomainTypeColor = (type: string) => {
    return type === 'onion' 
      ? 'text-purple-400 bg-purple-400/10' 
      : 'text-indigo-400 bg-indigo-400/10';
  };
  
  return (
    <div 
      className={`card p-4 hover:border-white/10 transition-all duration-200 ${
        showPreview ? 'lg:grid lg:grid-cols-3 lg:gap-4' : ''
      }`}
      onMouseEnter={() => setShowPreview(true)}
      onMouseLeave={() => setShowPreview(false)}
    >
      <div className={showPreview ? 'lg:col-span-2' : ''}>
        <div className="flex items-start justify-between">
          <h3 className="text-lg font-medium text-white">{result.title}</h3>
          <div className="flex gap-1">
            <button 
              onClick={() => setIsBookmarked(!isBookmarked)} 
              className={`p-1.5 rounded-lg ${isBookmarked ? 'text-yellow-400 bg-yellow-400/10' : 'text-white/40 hover:text-white hover:bg-white/5'} transition-colors`}
            >
              <Bookmark className="h-4 w-4" />
            </button>
            <button className="p-1.5 rounded-lg text-white/40 hover:text-white hover:bg-white/5 transition-colors">
              <Share2 className="h-4 w-4" />
            </button>
            <button className="p-1.5 rounded-lg text-white/40 hover:text-white hover:bg-white/5 transition-colors">
              <Info className="h-4 w-4" />
            </button>
          </div>
        </div>
        
        <div className="flex items-center gap-1.5 mt-1 text-sm">
          <span className="text-white/60">{result.url}</span>
          <button className="text-white/40 hover:text-white transition-colors">
            <ExternalLink className="h-3.5 w-3.5" />
          </button>
        </div>
        
        <div className="flex items-center gap-2 mt-2">
          <span className={`text-xs px-2 py-0.5 rounded-md ${getStatusColor(result.status)}`}>
            {result.status}
          </span>
          <span className={`text-xs px-2 py-0.5 rounded-md ${getDomainTypeColor(result.type)}`}>
            .{result.type}
          </span>
          {result.category && (
            <span className="text-xs px-2 py-0.5 rounded-md text-blue-400 bg-blue-400/10">
              {result.category}
            </span>
          )}
        </div>
        
        <p className="mt-3 text-white/60 text-sm leading-relaxed">{result.description}</p>
        
        <div className="flex items-center mt-3 text-xs text-white/40">
          <Clock className="h-3.5 w-3.5 mr-1" />
          <span>Last seen: {formatDate(result.lastSeen)}</span>
        </div>
      </div>
      
      {showPreview && result.screenshotUrl && (
        <div className="hidden lg:block mt-4 lg:mt-0">
          <div className="relative h-full w-full rounded-lg overflow-hidden border border-white/5">
            <img 
              src={result.screenshotUrl} 
              alt={result.title}
              className="h-full w-full object-cover object-center brightness-75 hover:brightness-90 transition-all"
            />
            <div className="absolute inset-0 bg-gradient-to-t from-black/50 to-transparent pointer-events-none"></div>
          </div>
        </div>
      )}
    </div>
  );
};

export default ResultItem;