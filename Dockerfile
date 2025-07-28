FROM node:20

# Install build deps

WORKDIR /app

# Copy everything but ignore node_modules via .dockerignore
COPY . .

# 1. Install root deps (if any)
RUN npm install

# 2. Build C++ addon — split this up to guarantee install
WORKDIR /app/binding
RUN apt-get update && apt-get install -y \
  build-essential \
  cmake \
  python3 \
  libssl-dev \
  libpthread-stubs0-dev
RUN find /usr/include -name pthread.h || (echo "pthread.h not found!" && exit 1)
RUN npm install node-addon-api node-api-headers
RUN npx node-gyp configure && npx node-gyp build

# 3. Build frontend
WORKDIR /app/client
RUN npm install && npm run build

# 4. Back to root
WORKDIR /app
EXPOSE 3000
CMD ["node", "app.js"]
