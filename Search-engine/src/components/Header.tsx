import React from 'react';
import { Globe, Shield } from 'lucide-react';

const Header: React.FC = () => {
  return (
    <header className="sticky top-0 z-50 glass-effect">
      <div className="container mx-auto px-4 py-3 flex items-center justify-between">
        <div className="flex items-center gap-3">
          <div className="text-white/80 bg-white/5 p-2 rounded-lg">
            <Globe className="h-5 w-5" />
          </div>
          <div>
            <h1 className="text-lg font-medium text-white flex items-center gap-1">
              <span>SN</span>
              <span className="text-xs text-white/40">/</span>
              <span className="text-indigo-400">Onion</span>
            </h1>
            <p className="text-xs text-white/60 -mt-0.5">Dark Web Search Engine</p>
          </div>
        </div>
        
        <div className="flex items-center gap-3">
          <div className="flex items-center gap-1.5 text-sm text-green-400 bg-green-400/10 px-3 py-1.5 rounded-lg">
            <Shield size={14} />
            <span>Tor Connected</span>
          </div>
        </div>
      </div>
    </header>
  );
};

export default Header;