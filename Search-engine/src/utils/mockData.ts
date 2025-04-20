import { SearchResult } from '../types';

export const mockResults: SearchResult[] = [
  {
    id: '1',
    url: 'examplesite.onion',
    title: 'Example Hidden Service',
    description: 'This is an example of a hidden service on the Tor network with various information and resources.',
    lastSeen: '2025-07-01T14:32:00Z',
    status: 'online',
    type: 'onion',
    category: 'Information',
    screenshotUrl: 'https://images.pexels.com/photos/4792731/pexels-photo-4792731.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=1'
  },
  {
    id: '2',
    url: 'privacytools.sn',
    title: 'Privacy Tools - SN Network',
    description: 'A collection of privacy tools, resources, and information for maintaining anonymity online.',
    lastSeen: '2025-07-02T08:15:00Z',
    status: 'online',
    type: 'sn',
    category: 'Privacy',
    screenshotUrl: 'https://images.pexels.com/photos/5380642/pexels-photo-5380642.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=1'
  },
  {
    id: '3',
    url: 'searchengine.onion',
    title: 'Torch - Tor Search Engine',
    description: 'A search engine for the Tor network that indexes .onion sites and provides search functionality.',
    lastSeen: '2025-06-29T21:45:00Z',
    status: 'offline',
    type: 'onion',
    category: 'Search',
    screenshotUrl: 'https://images.pexels.com/photos/33970/pexels-photo.jpg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=1'
  },
  {
    id: '4',
    url: 'cryptoforum.sn',
    title: 'Crypto Discussion Forum',
    description: 'A forum for discussing cryptocurrencies, blockchain technology, and related topics in a secure environment.',
    lastSeen: '2025-07-01T12:10:00Z',
    status: 'online',
    type: 'sn',
    category: 'Forum',
    screenshotUrl: 'https://images.pexels.com/photos/844124/pexels-photo-844124.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=1'
  },
  {
    id: '5',
    url: 'securemail.onion',
    title: 'Secure Mail Service',
    description: 'An encrypted email service that prioritizes user privacy and security for communications.',
    lastSeen: '2025-07-02T10:30:00Z',
    status: 'online',
    type: 'onion',
    category: 'Communication',
    screenshotUrl: 'https://images.pexels.com/photos/6190326/pexels-photo-6190326.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=1'
  },
  {
    id: '6',
    url: 'newsportal.sn',
    title: 'Independent News Portal',
    description: 'An independent news portal covering global events with a focus on uncensored reporting.',
    lastSeen: '2025-06-30T18:22:00Z',
    status: 'unknown',
    type: 'sn',
    category: 'News',
    screenshotUrl: 'https://images.pexels.com/photos/518543/pexels-photo-518543.jpeg?auto=compress&cs=tinysrgb&w=1260&h=750&dpr=1'
  }
];

export const categories = [
  'All',
  'Information',
  'Privacy',
  'Search',
  'Forum',
  'Communication',
  'Marketplace',
  'News',
  'Social',
  'Technical',
  'Other'
];