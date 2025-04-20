/** @type {import('tailwindcss').Config} */
export default {
  content: ['./index.html', './src/**/*.{js,ts,jsx,tsx}'],
  theme: {
    extend: {
      colors: {
        primary: {
          50: '#eeeaff',
          100: '#d9d0ff',
          200: '#b8a4ff',
          300: '#9572ff',
          400: '#7a45ff',
          500: '#6d28d9',
          600: '#5b21b6',
          700: '#4c1d95',
          800: '#38197a',
          900: '#2D1B69',
          950: '#1A103E',
        },
      },
      boxShadow: {
        'glow': '0 0 15px rgba(99, 102, 241, 0.4)',
      },
    },
  },
  plugins: [],
};