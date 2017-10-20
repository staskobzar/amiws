# web-root-vue

> Web root example with Vue.js

## Install

Install node.js (lots of info in google).

In the "web_root_vue" folder install dependencies:
```bash
npm install
```

Build static files in "dist" folder:
```bash
npm run build
```

Point web_root parameter to "dist" folder in "amiws.yaml" configuration file. For example, if the projec folder is in "/var/www/amiws_webroot", then parameter should look like:
```yaml
web_root: /var/www/amiws_webroot/dist
```


## Available commands

``` bash
# install dependencies
npm install

# serve with hot reload at localhost:8080
npm run dev

# build for production with minification
npm run build

# build for production and view the bundle analyzer report
npm run build --report

# run unit tests
npm run unit

# run e2e tests
npm run e2e

# run all tests
npm test
```

For detailed explanation on how things work, checkout the [guide](http://vuejs-templates.github.io/webpack/) and [docs for vue-loader](http://vuejs.github.io/vue-loader).
