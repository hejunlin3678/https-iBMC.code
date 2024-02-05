const fs = require('fs');
const path = require('path');
const unzipFileRegex = /.*(?<!\.gz)$/;
const zipSourceRegex = /\.(js|css|scss|ts)?$/;
const zipTargetRegex = /\.(js|css|scss|ts)\.gz$/;
const distPath = './dist/webapp/';

function travel(dir, callback) {
  fs.readdirSync(dir).forEach((file) => {
    const pathname = path.join(dir, file);
    if (fs.statSync(pathname).isDirectory()) {
      travel(pathname, callback);
    } else {
      callback(pathname);
    }
  })
}

travel(distPath, function (pathname) {

  if (unzipFileRegex.test(pathname)) {
    if (zipSourceRegex.test(pathname)) {
      fs.unlinkSync(pathname);
    }
  } else {
    if (!zipTargetRegex.test(pathname)) {
      fs.unlinkSync(pathname);
    }
  }
});