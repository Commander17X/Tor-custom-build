import React from 'react';
import { Globe, Shield, AlertTriangle } from 'lucide-react';

const Footer: React.FC = () => {
  return (
    <footer className="mt-auto border-t border-white/5">
      <div className="container mx-auto px-4 py-12">
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-8">
          <div>
            <div className="flex items-center gap-2 mb-4">
              <Globe className="h-5 w-5 text-indigo-400" />
              <h3 className="text-lg font-medium text-white">About SN/Onion</h3>
            </div>
            <p className="text-sm text-white/60 leading-relaxed">
              A next-generation search engine designed for the decentralized web, 
              focusing on .onion and .sn domains to enhance privacy and accessibility 
              in the digital age.
            </p>
          </div>
          
          <div>
            <h3 className="text-lg font-medium text-white mb-4">Technology</h3>
            <p className="text-sm text-white/60 leading-relaxed">
              Built on advanced crawling technology and the Tor network, we provide 
              secure access to hidden services while maintaining user privacy and 
              anonymity at every step.
            </p>
          </div>
          
          <div>
            <h3 className="text-lg font-medium text-white mb-4">Our Mission</h3>
            <p className="text-sm text-white/60 leading-relaxed">
              Empowering users with tools for privacy-focused communication and 
              access to information. We believe in a decentralized internet that 
              prioritizes user autonomy and security.
            </p>
          </div>
          
          <div>
            <h3 className="text-lg font-medium text-white mb-4">Security</h3>
            <div className="flex items-center gap-2 text-sm text-green-400 bg-green-400/10 px-3 py-2 rounded-lg mb-3">
              <Shield className="h-4 w-4" />
              <span>End-to-End Encryption</span>
            </div>
            <p className="text-sm text-white/60 leading-relaxed">
              Your searches are protected by industry-standard encryption and routed 
              through the Tor network for maximum privacy.
            </p>
          </div>
        </div>
        
        <div className="mt-12 pt-8 border-t border-white/5">
          <div className="text-center">
            <p className="text-sm text-white/40">
              © 2025 SN/Onion Search Engine. All rights reserved.
            </p>
          </div>
        </div>
        
        <div className="mt-8 p-4 rounded-xl bg-yellow-400/10 border border-yellow-400/20">
          <div className="flex items-start gap-3">
            <AlertTriangle className="h-5 w-5 text-yellow-400 flex-shrink-0 mt-0.5" />
            <div>
              <h4 className="text-sm font-medium text-yellow-400 mb-1">Legal Disclaimer</h4>
              <p className="text-sm text-white/60 leading-relaxed">
                This search engine indexes content from the Tor network and SN domains. 
                Users are responsible for their actions and compliance with applicable laws. 
                We do not endorse or promote any illegal activities.
              </p>
            </div>
          </div>
        </div>
      </div>
    </footer>
  );
};

export default Footer;